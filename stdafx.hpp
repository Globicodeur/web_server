// STL
#include <unordered_map>

// Boost specific
#include <boost/asio.hpp>
namespace asio = boost::asio;

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/phoenix.hpp>
namespace qi = boost::spirit::qi;
namespace sp = boost::spirit;
namespace ph = boost::phoenix;

static const std::string CRLF = "\r\n";
