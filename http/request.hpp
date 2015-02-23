#pragma once

#include "version.hpp"
#include "header.hpp"

namespace http {

    struct request {

        using parameters_t = std::unordered_map<std::string, std::string>;
        using headers_t = std::unordered_set<header, header::hash>;

        struct grammar:
            qi::grammar<spirit::istream_iterator, request(), qi::blank_type> {

            grammar(): grammar::base_type(rule) {
                using qi::char_;
                using qi::lit;
                using qi::uint_;
                using qi::lexeme;
                using qi::omit;
                using qi::as_string;
                using qi::_val;
                using qi::_1;
                using qi::_2;

                using phoenix::insert;
                using phoenix::construct;
                using phoenix::bind;

                key = lexeme[+char_("a-zA-Z-")];
                pair_rule = key >> '=' >> key;
                params_rule = omit[char_('?')] >> pair_rule % '&';

                version_rule = lit("HTTP/") >> uint_ >> '.' >> uint_;

                headers_rule =
                    *(
                        (
                                key
                            >>  ": "
                            >> as_string[+(char_ - '\r')]
                            >> CRLF
                        )[insert(_val, construct<header>(_1, _2))]
                    );

                rule =
                        -params_rule[bind(&request::params_, _val) = _1]
                    >>  version_rule[bind(&request::version_, _val) = _1]
                    >>  CRLF
                    >>  headers_rule[bind(&request::headers_, _val) = _1]
                    >>  CRLF
                    >>  as_string[*char_][bind(&request::body_, _val) = _1]
                    ;

            }

            qi::rule<spirit::istream_iterator, version(), qi::blank_type> version_rule;

            qi::rule<spirit::istream_iterator, std::string(), qi::blank_type> key;
            qi::rule<spirit::istream_iterator, std::pair<std::string, std::string>(), qi::blank_type> pair_rule;
            qi::rule<spirit::istream_iterator, parameters_t(), qi::blank_type> params_rule;

            qi::rule<spirit::istream_iterator, headers_t(), qi::blank_type> headers_rule;

            qi::rule<spirit::istream_iterator, request(), qi::blank_type> rule;

        };

        const auto & params() const {
            return params_;
        }

        const auto & headers() const {
            return headers_;
        }

        const auto & body() const {
            return body_;
        }

    private:

        headers_t headers_;
        parameters_t params_;
        version version_;
        std::string body_;

    };

}
