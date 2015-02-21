#pragma once

#include "status.hpp"
#include "header.hpp"
#include "version.hpp"

namespace http {

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
            static const version version { 1, 1 };

            *this << content_length[content_->size()];
            std::ostream os { &(*header_) };
            os << version << " " << status_ << CRLF;
            for (const auto & header: headers_)
                os << header << CRLF;
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
