#pragma once

#include "status.hpp"

namespace http {

    static const int version_major = 1;
    static const int version_minor = 1;

    static const std::string version = "HTTP/" + std::to_string(version_major) +
                                       "." + std::to_string(version_minor);

    struct response {

        response(const std::string & content, const status & stat = ok):
            buff_ { new asio::streambuf },
            content_ { new std::ostringstream },
            status_ { stat } {
                *content_ << content;

        }

        void build() const {
            std::ostream os { &(*buff_) };
            os << version << " "
               << status_ << CRLF
               << "Content-Length: " << content_->tellp() << CRLF << CRLF
               << content_->str();
        }

        asio::streambuf & buff() const {
            return *buff_;
        }

    private:

        std::shared_ptr<asio::streambuf> buff_;
        std::shared_ptr<std::ostringstream> content_;
        std::reference_wrapper<const status> status_;

    };

}
