/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/format_join_grammar_cats.hpp
*
*  Defines helpers to format and join parts of a report with taking into account grammar categories.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMAT_JOIN_GRAMMAR_CATS_HPP
#define DRACOSHA_VALIDATOR_FORMAT_JOIN_GRAMMAR_CATS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/wrap_object.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>
#include <dracosha/validator/reporting/phrase_grammar_cats.hpp>
#include <dracosha/validator/reporting/backend_formatter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Implementer of fmt_plain_pair.
 */
struct fmt_plain_pair_impl
{
    template <typename FormatterT, typename T>
    auto operator() (FormatterT&& formatter, T&& value) const
    {
        return hana::make_pair(std::forward<FormatterT>(formatter),wrap_object(std::forward<T>(value)));
    }
};
/**
 * @brief Make pair of formatter and value this formatter must be used to format.
 * @param formatter Formatter to use for value formatting.
 * @param value value to format.
 * @return hana::pair of formatter and value.
 */
constexpr fmt_plain_pair_impl fmt_plain_pair{};

/**
 * @brief Implementer of fmt_pair.
 */
struct fmt_pair_impl
{
    template <typename FormatterT, typename T>
    auto operator() (FormatterT&& formatter, T&& value) const
    {
        return fmt_plain_pair(std::forward<FormatterT>(formatter),std::forward<T>(value));
    }
};
/**
 * @brief Alias for fmt_plain_pair.
 */
constexpr fmt_pair_impl fmt_pair{};

//-------------------------------------------------------------

/**
 * @brief Implementer of format_join_grammar_cats.
 */
struct format_join_grammar_cats_t
{
    template <typename DstT, typename ... Args>
    void operator () (DstT& dst, Args&&... args) const
    {
        auto pairs=hana::make_tuple(std::forward<Args>(args)...);
        auto parts=hana::fold(
            pairs,
            hana::make_tuple(),
            [](auto&& prev, auto&& current)
            {
                return hana::append(
                                std::forward<decltype(prev)>(prev),
                                apply_ref(hana::first(current),unwrap_object(hana::second(current)),last_grammar_categories(prev,0))
                            );
            }
        );
        backend_formatter.append_join(
           dst,
           " ",
           parts
        );
    }
};
/**
 * @brief Format and join parts of reports with taking into account grammar categories.
 * @param dst Destination string to append report to.
 * @param args Parts of report to format in form of hana::pair objects made with fmt_pair().
 *             The first element of each pair is a formatter, and the second is a value to format using that formatter.
 */
constexpr format_join_grammar_cats_t format_join_grammar_cats{};

//-------------------------------------------------------------
DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMAT_JOIN_GRAMMAR_CATS_HPP
