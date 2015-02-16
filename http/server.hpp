#pragma once

#include "response.hpp"

#include "tools/traits.hpp"

namespace http {

    struct server {

        using port_t = unsigned short;

        server(port_t port,
               unsigned thread_count = std::thread::hardware_concurrency()):
            endpoint_ { asio::ip::tcp::v4(), port },
            acceptor_ { service_, endpoint_ },
            thread_count_ { thread_count }
        { }

        template <class Expr, class F>
        void add_route(const Expr & e, const F & f) {
            using R = typename tools::traits::attribute_of_parser<Expr>::type;
            auto call = [f](const R & t) {
                using std::placeholders::_1;
                return std::bind(f, _1, t);
            };
            qi::rule<spirit::istream_iterator, R()> sub_expr = e;
            add_route(sub_expr.copy()[qi::_val = phoenix::bind(call, qi::_1)]);
        }

        void serve() {
            std::vector<std::thread> threads;

            accept_next();

            for (unsigned i = 1; i < thread_count_; ++i)
                threads.emplace_back([this]() { service_.run(); });

            std::cout << "listening on port " << endpoint_.port()
                      << " using up to " << thread_count_ << " threads"
                      << std::endl;

            service_.run();

            for (auto & thread: threads)
                thread.join();
        }

    private:

        using route_handler = std::function<void (response &)>;
        using route_rule = qi::rule<spirit::istream_iterator, route_handler()>;

        void add_route(const route_rule & rule) {
            routes_ = routes_.copy() | rule.copy();
        }

        using acceptor_t = asio::ip::tcp::acceptor;
        using endpoint_t = asio::ip::tcp::endpoint;
        using socket_t = asio::ip::tcp::socket;
        using service_t = asio::io_service;
        using err_code_t = boost::system::error_code;

        struct connection {
            connection(service_t & service):
                socket { service }
            { }

            socket_t socket;
            asio::streambuf buff;
            response resp;
        };

        using shared_connection_t = std::shared_ptr<connection>;

        void accept_next() {
            using std::placeholders::_1;

            shared_connection_t conn { new connection { service_ } };
            auto on_accepted = std::bind(&server::on_accepted, this, conn, _1);
            acceptor_.async_accept(conn->socket, on_accepted);
        }

        void on_accepted(shared_connection_t conn, const err_code_t & code) {
            using std::placeholders::_1;
            using std::placeholders::_2;

            accept_next();
            if (!code) {
                auto on_read = std::bind(&server::on_read, this, conn, _1, _2);
                asio::async_read_until(conn->socket, conn->buff, CRLF + CRLF, on_read);
            }
            else
                std::cerr << "accept: " << code << std::endl;
        }

        void on_read(shared_connection_t conn, const err_code_t & code, size_t) {
            if (!code) {
                auto handler = find_route(conn->buff);
                if (handler)
                    (*handler)(conn->resp);
                else
                    conn->resp << not_found;
                write_response(conn);
            }
            else
                std::cerr << "read: " << code << std::endl;
        }

        void write_response(shared_connection_t conn) {
            using std::placeholders::_1;
            using std::placeholders::_2;

            auto on_written = std::bind(&server::on_written, this, conn, _1, _2);
            conn->resp.build();
            asio::async_write(conn->socket, conn->resp.buff(), on_written);
        }

        void on_written(shared_connection_t conn, const err_code_t & code,
                        size_t transferred) {
            if (!code)
               std::cout << "written " << transferred << " bytes" << std::endl;
            else
                std::cerr << "write: " << code << std::endl;
            conn->socket.shutdown(asio::ip::tcp::socket::shutdown_both);
        }

        boost::optional<route_handler> find_route(asio::streambuf & buff) {
            std::istream stream { &buff };
            spirit::istream_iterator first { stream };
            spirit::istream_iterator last;
            route_handler handler;

            stream.unsetf(std::ios::skipws);
            if (qi::parse(first, last, qi::lit("GET ") >> routes_, handler))
                return handler;
            return boost::none;
        }

        service_t service_;
        endpoint_t endpoint_;
        acceptor_t acceptor_;

        unsigned thread_count_;

        route_rule routes_;

    };

}
