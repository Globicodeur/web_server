#pragma once

namespace http {

    struct header {

        struct hash {

            auto operator()(const header & header) const {
                static const std::hash<std::string> hasher;
                return hasher(header.name);
            }

        };

        header(const std::string & name): name(name) {}
        header(const std::string & name, const std::string & value):
            name(name), value(value)
        { }

        template <class T>
        header(const std::string & name, const T & t):
            name(name) {
            std::ostringstream oss;
            oss << t;
            value = oss.str();
        }

        const std::string name;
        std::string value;

        template <class T>
        header operator[](const T & value) const {
            std::ostringstream oss;
            oss << value;
            return { name, oss.str() };
        }

        friend std::ostream & operator<<(std::ostream & os, const header & h) {
            return os << h.name << ":" << h.value;
        }

    };

    bool operator==(const header & lhs, const header & rhs) {
        return lhs.name == rhs.name;
    }

    static const header content_type   { "Content-Type"   };
    static const header content_length { "Content-Length" };
    // ...
}
