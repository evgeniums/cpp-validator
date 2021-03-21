/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/format_join_grammar_cats.hpp
*
*  Defines helpers to format and join parts of reporting phrase with taking into account grammar categories.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMAT_JOIN_GRAMMAR_CATS_HPP
#define DRACOSHA_VALIDATOR_FORMAT_JOIN_GRAMMAR_CATS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/reporting/phrase_grammar_cats.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct fmt_pair_impl
{
    template <typename FormatterT, typename T>
    auto operator() (FormatterT&& formatter, T&& value) const
    {
        return hana::make_tuple(hana::make_pair(std::forward<FormatterT>(formatter),cref(std::forward<T>(value))));
    }
};
constexpr fmt_pair_impl fmt_pair{};

//-------------------------------------------------------------

struct format_join_grammar_cats_t
{
    template <typename DstT, typename ... Args>
    void operator () (DstT& dst, Args&&... args) const
    {
        auto pairs=hana::flatten(hana::make_tuple(args...));
        auto parts=hana::fold(
            pairs,
            hana::make_tuple(),
            [](auto&& prev, auto&& current)
            {
                return hana::append(std::forward<decltype(prev)>(prev),apply_ref(hana::first(current),hana::second(current),last_grammar_categories(prev,0)));
            }
        );
        backend_formatter.append_join(
           dst,
           " ",
           parts
        );
    }
};
constexpr format_join_grammar_cats_t format_join_grammar_cats{};

//-------------------------------------------------------------
DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMAT_JOIN_GRAMMAR_CATS_HPP
