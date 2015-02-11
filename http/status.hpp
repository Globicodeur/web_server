#pragma once

namespace http {

    struct status {

        enum class code {
            ok = 200,
            not_found = 404,
        };

        const code c;
        const std::string s;

    };

    static status ok        { status::code::ok,        "OK" };
    static status not_found { status::code::not_found, "Not Found" };

    static std::ostream & operator<<(std::ostream & os, const status & stat) {
        using code_t = typename std::underlying_type<status::code>::type;

        return os << static_cast<code_t>(stat.c) << " " << stat.s;
    }

}
