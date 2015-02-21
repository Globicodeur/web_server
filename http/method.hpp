#pragma once

namespace http {

    struct method {

        const std::string name;

    };

    static const method get  { "GET"  };
    static const method post { "POST" };

}
