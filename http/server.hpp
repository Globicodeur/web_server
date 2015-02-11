#pragma once

#include "response.hpp"

#include "tools/traits.hpp"

namespace http {

    using std::bind;
    using namespace std::placeholders;

    struct server {
        using port_t = unsigned short;

        server(port_t port):
            endpoint_ { asio::ip::tcp::v4(), port },
            acceptor_ { service_, endpoint_ } {
            accept_next();
        }

        using route_handler = std::function<response ()>;
        using route_rule = qi::rule<sp::istream_iterator, route_handler()>;

        void add_exact_route(const std::string & path, route_handler handler) {
            add_route(qi::lit(path)[qi::_val = handler]);
        }

        void add_route(const route_rule & rule) {
            routes_ = routes_.copy() | rule.copy();
        }

        template <class Expr, class F>
        void add_route(const Expr & expr, const F & f) {
            using arg_type = typename tools::traits::first_arg_of<F>::type;

            auto call = [&f](const arg_type & val) {
                return bind(f, val);
            };

            qi::rule<sp::istream_iterator, arg_type()> sub_expr = expr;
            add_route(sub_expr.copy()[qi::_val = ph::bind(call, qi::_1)]);
        };

        template <class Expr, class F>
        void add_route_auto(const Expr & expr, const F & f) {
            auto call = [&f](const auto & val) {
                return bind(f, val);
            };
            route_rule rule = expr[qi::_val = ph::bind(call, qi::_1)];
            add_route(rule);
        };

        void serve() {
            service_.run();
        }

    private:

        using acceptor_t = asio::ip::tcp::acceptor;
        using endpoint_t = asio::ip::tcp::endpoint;
        using socket_t = asio::ip::tcp::socket;
        using service_t = asio::io_service;
        using err_code_t = boost::system::error_code;

        struct connection {
            connection(service_t & service):
                socket { service } { }

            socket_t socket;
            asio::streambuf buff;
        };

        using shared_connection_t = std::shared_ptr<connection>;

        void accept_next() {
            shared_connection_t conn { new connection { service_ } };
            auto on_accepted = bind(&server::on_accepted, this, conn, _1);
            acceptor_.async_accept(conn->socket, on_accepted);
        }

        void write_response(shared_connection_t conn, const response & resp) {
            auto on_written = bind(&server::on_written, this, conn, resp, _1, _2);
            resp.build();
            asio::async_write(conn->socket, resp.buff(), on_written);
        }

        void on_accepted(shared_connection_t conn, const err_code_t & code) {
            accept_next();

            if (!code) {
                auto on_read = bind(&server::on_read, this, conn, _1, _2);
                asio::async_read_until(conn->socket, conn->buff, CRLF + CRLF, on_read);
            }
            else
                std::cerr << "accept: " << code << std::endl;
        }

        void on_read(shared_connection_t conn, const err_code_t & code, size_t) {
            if (!code) {
                auto handler = find_route(conn->buff);
                if (handler)
                    write_response(conn, (*handler)());
                else {
                    response resp { "<h1>404 Not found lol</h1>", not_found };
                    write_response(conn, resp);
                }
            }
            else
                std::cerr << "read: " << code << std::endl;
        }

        void on_written(shared_connection_t conn, response,
                        const err_code_t & code, size_t transferred) {
            if (!code)
               std::cout << "written " << transferred << " bytes" << std::endl;
            else
                std::cerr << "write: " << code << std::endl;
            conn->socket.shutdown(asio::ip::tcp::socket::shutdown_both);
        }

        boost::optional<route_handler> find_route(asio::streambuf & buff) {
            std::istream stream { &buff };
            sp::istream_iterator first { stream };
            sp::istream_iterator last;
            route_handler handler;

            stream.unsetf(std::ios::skipws);
            if (qi::parse(first, last, qi::lit("GET ") >> routes_, handler))
                return handler;
            return boost::none;
        }

        service_t service_;
        endpoint_t endpoint_;
        acceptor_t acceptor_;

        route_rule routes_;

    };

}
