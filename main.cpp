#include "http/server.hpp"
#include "http/response.hpp"

int main()
{
    http::server server { 1337 };

    server.add_exact_route("/test", []() {
        return http::response("Hello World");
    });

    server.add_route((qi::lit("/integer/") >> qi::int_), [](const int & i) {
        return http::response("<h1>" + std::to_string(i) + "</h1>");
    });

    server.add_route((qi::lit("/float/") >> qi::double_), [](const double & d) {
        return http::response("<h1>" + std::to_string(d) + "</h1>");
    });

    server.add_route((qi::lit("/alnum/") >> +qi::char_("a-zA-Z0-9")),
        [](const std::string & d) {
        return http::response("<h1>" + d + "</h1>");
    });

    server.serve();

    return 0;
}
