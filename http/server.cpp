#include "server.hpp"

#include "response.hpp"

#include <iostream>

namespace http {

    using namespace std::placeholders;

    server::server(port_t port):
        endpoint_ { asio::ip::tcp::v4(), port },
        acceptor_ { service_, endpoint_ } {
            accept_next();
    }

    server::connection::connection(service_t & service):
        socket { service } { }

    void server::add_exact_route(const std::string & path,
                                 route_handler handler) {
        add_route(qi::lit(path)[qi::_val = handler]);
    }

    void server::add_route(const route_rule & rule) {
        routes_ = routes_.copy() | rule.copy();
    }

    void server::serve() {
        service_.run();
    }

    void server::accept_next() {
        shared_connection_t conn { new connection { service_ } };
        auto on_accepted = std::bind(&server::on_accepted, this, conn, _1);
        acceptor_.async_accept(conn->socket, on_accepted);
    }

    void server::write_response(shared_connection_t conn, const response & resp) {
        auto on_written = std::bind(&server::on_written, this, conn, resp, _1, _2);
        asio::async_write(conn->socket, resp.buff(), on_written);
    }

    void server::on_accepted(shared_connection_t conn, const err_code_t & code) {
        accept_next();

        if (!code) {
            auto on_read = std::bind(&server::on_read, this, conn, _1, _2);
            asio::async_read_until(conn->socket, conn->buff, CRLF + CRLF, on_read);
        }
        else
            std::cerr << "accept: " << code << std::endl;
    }

    void server::on_read(shared_connection_t conn, const err_code_t & code, size_t) {
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

    void server::on_written(shared_connection_t conn, response, const err_code_t & code,
                            size_t transferred) {
        if (!code)
            std::cout << "written " << transferred << " bytes" << std::endl;
        else
            std::cerr << "write: " << code << std::endl;
        conn->socket.shutdown(asio::ip::tcp::socket::shutdown_both);
    }

    boost::optional<server::route_handler> server::find_route(asio::streambuf & buff) {
        std::istream stream { &buff };
        sp::istream_iterator first { stream };
        sp::istream_iterator last;
        route_handler handler;

        stream.unsetf(std::ios::skipws);
        if (qi::parse(first, last, qi::lit("GET ") >> routes_, handler))
            return handler;
        return boost::none;
    }

}
