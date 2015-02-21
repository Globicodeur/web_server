#include "http/server.hpp"
#include "edsl/edsl.hpp"

std::string reversed(std::string str)
{
    std::reverse(str.begin(), str.end());
    return str;
}

template <class F, class... Args>
auto lazy(const F & f, const Args &... args)
{
    return proto::make_expr<proto::tag::function>(f, args...);
}

int main()
{
    using qi::lit;
    using qi::char_;
    using qi::int_;
    using qi::double_;
    using qi::as_string;

    using namespace http;
    using namespace html;

    http::server server { 1337 };

    server.add_route(
        // pattern
        lit("/awesome/")
            >>  int_    >> ','
            >>  double_ >> ','
            >>  as_string[+char_("a-zA-Z")]
        ,
        // action
        _response
            <<  content_type [ "text/html" ]
            <<  h1 [ _1 * _1 ]
            <<  h2 [ _2 - 4.2 ]
            <<  h4 [ lazy(reversed, _3) ]
    );

    server.add_route(
        lit("/post"),
        _response << h1["ok"],
        post
    );

    server.serve();

    return 0;
}
