#pragma once

namespace http {

    struct version {

        uint major;
        uint minor;

        friend std::ostream & operator<<(std::ostream & os, const version & v) {
            return os << "HTTP/" << v.major << '.' << v.minor;
        }

    };

}

BOOST_FUSION_ADAPT_STRUCT(
    http::version,
    (uint, major)
    (uint, minor)
)
