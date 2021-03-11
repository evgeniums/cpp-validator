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

struct format_join_grammar_cats_t
{
    template <typename DstT, typename FormatterTs, typename ... Args>
    void operator () (DstT& dst, FormatterTs&& formatters, Args&&... args) const
    {
        auto pairs=hana::zip(
            std::forward<FormatterTs>(formatters),
            make_cref_tuple(std::forward<Args>(args)...)
        );
        auto parts=hana::fold(
            pairs,
            hana::make_tuple(),
            [](auto&& prev, auto&& current)
            {
                return hana::append(std::forward<decltype(prev)>(prev),apply_ref(hana::front(current),hana::back(current),last_grammar_categories(prev,0)));
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
