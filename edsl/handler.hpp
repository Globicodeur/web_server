#pragma once

namespace http {

    template <class Expr> struct request_handler;
    struct response;
    struct request;

    struct request_handler_domain:
        proto::domain<proto::generator<request_handler>> {

        using base_t = proto::domain<proto::generator<request_handler>>;

        // Using value semantic for immediates
        template <class T>
        struct as_child: base_t::as_expr<T> { };

    };

    template <class Expr>
    struct request_handler:
        proto::extends<Expr, request_handler<Expr>, request_handler_domain> {

        using result_type = void;
        using base_t = proto::extends<
            Expr,
            request_handler<Expr>,
            request_handler_domain
        >;

        request_handler(const Expr & expr = Expr()):
            base_t { expr }
        { }

        template <class Parsed>
        void operator()(response & response, const request & request,
                        const Parsed & parsed) const {
            auto context = make_context(response, request, parsed);
            proto::eval(*this, context);
        }

    };

    // Terminals
    struct response_placeholder { };
    const request_handler<proto::terminal<response_placeholder>::type> _response;

    struct request_placeholder { };
    const request_handler<proto::terminal<request_placeholder>::type> _request;

    template <size_t i> struct route_placeholder { };
    const request_handler<proto::terminal<route_placeholder<0>>::type> _1;
    const request_handler<proto::terminal<route_placeholder<1>>::type> _2;
    const request_handler<proto::terminal<route_placeholder<2>>::type> _3;
    // ...

}
