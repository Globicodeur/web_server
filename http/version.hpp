#pragma once

namespace http {

    struct version {

        const uint major;
        const uint minor;

        friend std::ostream & operator<<(std::ostream & os, const version & v) {
            return os << "HTTP/" << v.major << '.' << v.minor;
        }

    };

}
