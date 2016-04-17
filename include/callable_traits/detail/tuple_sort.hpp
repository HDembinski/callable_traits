/*!
@file

@copyright Louis Dionne 2013-2016
@coyright Modified Work Barrett Adair 2016
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
*/

#ifndef CALLABLE_TRAITS_DETAIL_TUPLE_SORT_HPP
#define CALLABLE_TRAITS_DETAIL_TUPLE_SORT_HPP


#include <callable_traits/config.hpp>
#include <tuple>
#include <type_traits>

//This is a slightly modified version of Boost.Hana's sort
namespace callable_traits { 

    namespace detail {

        using empty_seq = CALLABLE_TRAITS_IX_SEQ(CALLABLE_TRAITS_EMPTY);

        template <typename Pred, std::size_t Insert, bool IsInsertionPoint,
            typename Left, std::size_t ...Right>
        struct insert;

        // We did not find the insertion point; continue processing elements
        // recursively.
        template <typename Pred, std::size_t Insert, std::size_t ...Left,
            std::size_t Right1, std::size_t Right2, std::size_t ...Right>
        struct insert<Pred, Insert, false, CALLABLE_TRAITS_IX_SEQ(Left...),
            Right1, Right2, Right...> {
            using type = typename insert<
                Pred, Insert, Pred::template apply<Insert, Right2>::value,
                CALLABLE_TRAITS_IX_SEQ(Left..., Right1),
                Right2, Right...
            >::type;
        };

        // We did not find the insertion point, but there is only one element
        // left. We insert at the end of the list, and we're done.
        template <typename Pred, std::size_t Insert, std::size_t ...Left, std::size_t Last>
        struct insert<Pred, Insert, false, CALLABLE_TRAITS_IX_SEQ(Left...), Last> {
            using type = CALLABLE_TRAITS_IX_SEQ(Left..., Last, Insert);
        };

        // We found the insertion point, we're done.
        template <typename Pred, std::size_t Insert, std::size_t ...Left, std::size_t ...Right>
        struct insert<Pred, Insert, true, CALLABLE_TRAITS_IX_SEQ(Left...), Right...> {
            using type = CALLABLE_TRAITS_IX_SEQ(Left..., Insert, Right...);
        };

        template <typename Pred, typename Result, std::size_t ...T>
        struct insertion_sort;

        template <typename Pred, std::size_t Result1, std::size_t ...Results,
            std::size_t T, std::size_t ...Ts>
        struct insertion_sort<Pred, CALLABLE_TRAITS_IX_SEQ(Result1, Results...), T, Ts...> {
            static constexpr bool pred_result = Pred::template apply<T, Result1>::value;
            using insert_result = typename insert<
                Pred, T, pred_result, empty_seq, Result1, Results...
            >::type;
            using type = typename insertion_sort<Pred, insert_result, Ts...>::type;
        };

        template <typename Pred, std::size_t T, std::size_t ...Ts>
        struct insertion_sort<Pred, empty_seq, T, Ts...> {
            using type = typename insertion_sort<
                Pred, CALLABLE_TRAITS_IX_SEQ(T), Ts...
            >::type;
        };

        template <typename Pred, typename Result>
        struct insertion_sort<Pred, Result> {
            using type = Result;
        };

        template <typename Pred, typename Indices>
        struct sort_indices;

        template <typename Pred, std::size_t ...i>
        struct sort_indices<Pred, CALLABLE_TRAITS_IX_SEQ(i...)> {
            using type = typename insertion_sort<Pred, empty_seq, i...>::type;
        };

        template<typename Tup, typename Pred>
        struct sort_impl {
            static constexpr std::size_t len = std::tuple_size<Tup>::value;
            using indices = typename sort_indices<Pred, CALLABLE_TRAITS_MAKE_IX_SEQ(len)>::type;
            using type = typename sort_impl<Tup, indices>::type;
        };

        template <typename Tup, std::size_t ...I>
        struct sort_impl<Tup, CALLABLE_TRAITS_IX_SEQ(I...)> {
            using type = std::tuple<at<I, Tup>...>;
        };

        template <typename Tup, template<class, class> class Pred>
        struct predicate {
            template <std::size_t I, std::size_t J>
            using apply = Pred<at<I, Tup>, at<J, Tup>>;
        };

        template<typename Tup, template<class, class> class Pred>
        using tuple_sort = typename sort_impl<Tup, predicate<Tup, Pred>>::type;
    }
}

#endif
