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

        template <typename Expr, typename Iterator = spirit::unused_type>
        struct attribute_of_parser
        {
            typedef typename spirit::result_of::compile<
                spirit::qi::domain, Expr
            >::type parser_expression_type;

            typedef typename spirit::traits::attribute_of<
                parser_expression_type, spirit::unused_type, Iterator
            >::type type;
        };

        template <class... Params> struct parameter_pack {

            // Find the type with the given index in the pack
            template <size_t i, class T1, class... Ts>
            struct type_at_t {
                using type = typename type_at_t<i - 1, Ts...>::type;
            };

            template <class T, class... Ts>
            struct type_at_t<0, T, Ts...> {
                using type = T;
            };

            template <size_t i> struct type_at {
                static_assert(
                    i < sizeof...(Params),
                    "The parameter pack is smaller than the given index"
                );
                using type = typename type_at_t<i, Params...>::type;
            };

        };

        template <class T>
        struct is_fusion_vector {
            static constexpr bool value = std::is_base_of<
                fusion::sequence_base<T>,
                T
            >::value;
        };

        // builds an integer sequence of <start, start + 1, ..., end - 1>
        template <size_t start, size_t end, class enabler = void, size_t... seq>
        struct make_range_index_sequence:
            make_range_index_sequence<start + 1, end, void, seq..., start> { };

        template <size_t start, size_t end, size_t... seq>
        struct make_range_index_sequence<
            start,
            end,
            typename std::enable_if<start == end>::type,
            seq...
        >: std::integer_sequence<size_t, seq...> { };

    }
}
