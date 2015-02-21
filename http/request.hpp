#pragma once

#include "version.hpp"
#include "header.hpp"

namespace http {

    struct request {

        using parameters_t = std::unordered_map<std::string, std::string>;
        using headers_t = std::unordered_set<header, header::hash>;

        request(asio::streambuf & buff): body_(buff) {}

        struct grammar:
            qi::grammar<spirit::istream_iterator, request(), qi::blank_type> {

            grammar(): grammar::base_type(rule) {

                using qi::_val;
                using qi::_1;
                using qi::_2;

                key = qi::lexeme[+qi::char_("a-zA-Z-")];
                pair_rule = key >> '=' >> key;
                params_rule = qi::omit[qi::char_('?')] >> pair_rule % '&';

                version_rule = qi::lit("HTTP/") >> qi::uint_ >> '.' >> qi::uint_;

                headers_rule =
                    *(
                        (
                                key
                            >>  ": "
                            >> qi::as_string[+(qi::char_ - '\r')]
                            >> CRLF
                        )[phoenix::insert(_val, phoenix::construct<header>(_1, _2))]
                    );

                rule =
                        -params_rule[phoenix::bind(&request::params_, _val) = _1]
                    >>  version_rule[phoenix::bind(&request::version_, _val) = _1]
                    >>  CRLF
                    >>  headers_rule[phoenix::bind(&request::headers_, _val) = _1]
                    >>  CRLF;

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

        const asio::streambuf & body() const {
            return body_;
        }

    private:

        headers_t headers_;
        parameters_t params_;
        version version_;
        asio::streambuf & body_;

    };

}
