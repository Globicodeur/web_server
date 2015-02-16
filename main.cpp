#include "http/server.hpp"
#include "edsl/edsl.hpp"

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
            <<  h1 [ _1 * _1 ]
            <<  h2 [ _2 - 4.2 ]
            <<  h4 [ _3 + " == " + _3 ]
    );

    server.serve();

    return 0;
}
