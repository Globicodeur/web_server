#pragma once

#include "handler.hpp"

namespace html {

    template <const char *name>
    struct tag {

        template <class T>
        std::string operator[](const T & t) const {
            std::ostringstream oss;
            oss << "<" << name << ">" << t << "</" << name << ">";
            return oss.str();
        }

    };

    template <const char *>
    struct tag_placeholder { };

}

namespace html {

    template <class Expr> struct html_handler;

    struct html_handler_domain:
        proto::domain<
            proto::generator<html_handler>,
            proto::_,
            http::request_handler_domain
        > {

        using base_t = proto::domain<
            proto::generator<html_handler>,
            proto::_,
            http::request_handler_domain
        >;

        // Using value semantic for immediates
        template <class T>
        struct as_child: base_t::as_expr<T> { };

    };

    template <class Expr>
    struct html_handler:
        proto::extends<Expr, html_handler<Expr>, html_handler_domain> {

        using result_type = void;
        using base_t = proto::extends<
            Expr,
            html_handler<Expr>,
            html_handler_domain
        >;

        html_handler(const Expr & expr = Expr()):
            base_t { expr }
        { }

    };

    static constexpr char H1[] = "h1";
    const html_handler<proto::terminal<tag_placeholder<H1>>::type> h1;
    static constexpr char H2[] = "h2";
    const html_handler<proto::terminal<tag_placeholder<H2>>::type> h2;
    static constexpr char H3[] = "h3";
    const html_handler<proto::terminal<tag_placeholder<H3>>::type> h3;
    static constexpr char H4[] = "h4";
    const html_handler<proto::terminal<tag_placeholder<H4>>::type> h4;
    static constexpr char H5[] = "h5";
    const html_handler<proto::terminal<tag_placeholder<H5>>::type> h5;
    static constexpr char H6[] = "h6";
    const html_handler<proto::terminal<tag_placeholder<H6>>::type> h6;

}
