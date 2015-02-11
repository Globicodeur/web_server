#pragma once

namespace tools {
    namespace traits {

        // This is kind of ugly for now
        template <class F> struct first_arg_of {

            template <class R, class A>
            static constexpr auto helper(R (F::*)(A) const) {
                return A { };
            }

            using type = decltype(helper(&F::operator()));

        };

    }
}
