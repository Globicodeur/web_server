#pragma once

namespace http {

    static const int version_major = 1;
    static const int version_minor = 1;

    static const std::string version = "HTTP/" + std::to_string(version_major) +
                                       "." + std::to_string(version_minor);

    enum code { ok = 200, not_found = 404 };

    static std::unordered_map<int, std::string> code_strings = {
        { ok, "OK" },
        { not_found, "Not Found" },
    };

    struct response {

        response(const std::string &, code=ok);

        asio::streambuf & buff() const;

    private:

        std::shared_ptr<asio::streambuf> buff_;
        std::shared_ptr<std::ostream> stream_;
        code code_;

    };

}
