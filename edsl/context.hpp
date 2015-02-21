#pragma once

#include "tools/traits.hpp"

#include "handler.hpp"
#include "html.hpp"

namespace http {

    template <class... Args>
    struct handler_context: proto::callable_context<handler_context<Args...>> {

        using args_t = std::tuple<const Args &...>;

        template <class Call> struct result {
            using type = typename std::result_of<Call>::type;
        };

        handler_context(response & response, const request & request,
                        const Args &... args):
            response_ { response }, request_(request), args_ { args... }
        { }

        response & operator()(proto::tag::terminal, response_placeholder) {
            return response_;
        }

        const request & operator()(proto::tag::terminal, request_placeholder) const {
            return request_;
        }

        template <size_t i>
        const auto & operator()(proto::tag::terminal,
                                route_placeholder<i>) const {
            static_assert(
                i < sizeof...(Args),
                "no such placeholder for this expression"
            );
            return std::get<i>(args_);
        }

        template <const char * name>
        const auto & operator()(proto::tag::terminal,
                                html::tag_placeholder<name>) const {
            static const html::tag<name> tag_wrapper;
            return tag_wrapper;
        }

    private:

        response & response_;
        const request & request_;
        args_t args_;

   };

    template <
        class NotAVector,
        std::enable_if_t<!tools::traits::is_fusion_vector<NotAVector>::value>...
    >
    inline auto make_context(response & response, const request & request,
                             const NotAVector & value) {
        return handler_context<NotAVector> { response, request, value };
    }

    template <
        class Vector,
        std::enable_if_t<tools::traits::is_fusion_vector<Vector>::value>...
    >
    inline auto make_context(response & response, const request & request,
                             const Vector & vector) {
        return make_context_impl(response, request, vector);
    }

    template <template <class...> class Vector, class... Args>
    inline auto make_context_impl(response & response, const request & request,
                                  const Vector<Args...> & vector) {
        using sequence_t = tools::traits::make_range_index_sequence<
            0,
            sizeof...(Args)
        >;
        constexpr sequence_t sequence;

        return make_context_impl(response, request, vector, sequence);
    }

    template <template <class...> class Vector, class... Args, size_t... indexes>
    inline auto make_context_impl(response & response, const request & request,
                                  const Vector<Args...> & vector,
                                  std::index_sequence<indexes...>)
    {
        return handler_context<Args...> {
            response,
            request,
            fusion::at_c<indexes>(vector)...
        };
    }

}
