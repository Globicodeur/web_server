#pragma once

#include "status.hpp"
#include "header.hpp"

namespace http {

    static const int version_major = 1;
    static const int version_minor = 1;

    static const std::string version = "HTTP/" + std::to_string(version_major) +
                                       "." + std::to_string(version_minor);

    struct response {

        using buffers = std::vector<asio::streambuf::const_buffers_type>;

        response(const status & status = ok):
            header_ { new asio::streambuf },
            content_ { new asio::streambuf },
            status_ { status } { }

        response(const std::string & content, const status & status = ok):
            response { status } {
            *this << content;
        }

        response(const response &) = delete;
        response & operator=(const response &) = delete;

        void build() {
            *this << content_length[content_->size()];
            std::ostream os { &(*header_) };
            os << version << " " << status_ << CRLF;
            for (auto it = headers_.begin(); it != headers_.end(); ++it)
                os << it->name << ":" << it->value << CRLF;
            os << CRLF;
        }

        const buffers buff() const {
            return {
                header_->data(),
                content_->data()
            };
        }

        template <class T>
        response & operator<<(const T & content) {
            std::ostream os { &(*content_) };
            os << content;
            return *this;
        }

        response & operator<<(const status & status) {
            status_ = status;
            return *this;
        }

        response & operator<<(const header & header) {
            headers_.insert(header);
            return *this;
        }

    private:

        std::shared_ptr<asio::streambuf> header_, content_;
        std::reference_wrapper<const status> status_;
        std::unordered_set<header, header::hash> headers_;

    };

}
