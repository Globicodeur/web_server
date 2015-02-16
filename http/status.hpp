#pragma once

namespace http {

    // http statuses are just a 3 digit code asociated with a description
    struct status {

        enum class code {
            ok = 200,
            not_found = 404,
            // ...
        };

        const code c;
        const std::string s;

        // enabling streaming
        friend std::ostream & operator<<(std::ostream & os, const status & s) {
            using code_t = typename std::underlying_type<code>::type;

            return os << static_cast<code_t>(s.c) << " " << s.s;
        }

    };

    // defining the standard statuses here
    // users should never have to define new ones themselves
    static status ok        { status::code::ok,        "OK"        };
    static status not_found { status::code::not_found, "Not Found" };
    // ...

}
