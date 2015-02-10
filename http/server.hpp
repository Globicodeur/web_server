#pragma once

template <class F> struct first_arg_of {

    template <class R, class A>
    static constexpr auto helper(R (F::*)(A) const) {
        return A { };
    }

    using type = decltype(helper(&F::operator()));
};

namespace http {

    struct response;

    struct server {
        using port_t = unsigned short;

        server(port_t);

        using route_handler = std::function<response ()>;
        void add_exact_route(const std::string &, route_handler);
        using route_rule = qi::rule<sp::istream_iterator, route_handler()>;
        void add_route(const route_rule &);

        template <class Expr, class F>
        void add_route(const Expr & expr, const F & f) {
            using arg_type = typename first_arg_of<F>::type;

            auto call = [f](const arg_type & val) {
                return std::bind(f, val);
            };

            qi::rule<sp::istream_iterator, arg_type()> sub_expr = expr;
            add_route(sub_expr.copy()[qi::_val = ph::bind(call, qi::_1)]);
        };

        template <class Expr, class F>
        void add_route_auto(const Expr & expr, const F & f) {
            auto call = [&f](const auto & val) {
                return std::bind(f, val);
            };
            route_rule rule = expr[qi::_val = ph::bind(call, qi::_1)];
            add_route(rule);
        };

        void serve();

    private:

        using acceptor_t = asio::ip::tcp::acceptor;
        using endpoint_t = asio::ip::tcp::endpoint;
        using socket_t = asio::ip::tcp::socket;
        using service_t = asio::io_service;
        using err_code_t = boost::system::error_code;

        struct connection {
            connection(service_t &);

            socket_t socket;
            asio::streambuf buff;
        };

        using shared_connection_t = std::shared_ptr<connection>;

        void accept_next();
        void write_response(shared_connection_t, const response &);

        void on_accepted(shared_connection_t, const err_code_t &);
        void on_read(shared_connection_t, const err_code_t &, size_t);
        void on_written(shared_connection_t, response, const err_code_t &, size_t);

        boost::optional<route_handler> find_route(asio::streambuf & buff);

        service_t service_;
        endpoint_t endpoint_;
        acceptor_t acceptor_;

        route_rule routes_;

    };

}
