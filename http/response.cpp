#include "response.hpp"

namespace http {

    response::response(const std::string & content, code code):
        buff_ { new asio::streambuf },
        stream_ { new std::ostream { &(*buff_) } },
        code_ { code } {
            *stream_ << version << " "
                     << code << " "
                     << code_strings[code] << CRLF
                     << "Content-Length: " << content.size() << CRLF << CRLF
                     << content;
    }

    asio::streambuf & response::buff() const {
        return *buff_;
    }

}
