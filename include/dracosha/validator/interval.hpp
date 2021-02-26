/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/interval.hpp
*
*  Defines interval.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_INTERVAL_HPP
#define DRACOSHA_VALIDATOR_INTERVAL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>
#include <dracosha/validator/reporting/format_operand.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct interval_tag;

/**
 * @brief Mode of interval.
 */
enum class interval_mode : int
{
    closed, //!< Interval includes both endpoints.
    open, //!< Interval does not include both endpoints.
    open_from, //!< Interval includes only "to" endpoint and doesn't include "from" endpoint.
    open_to //!< Interval includes only "from" endpoint and doesn't include "to" endpoint.
};

/**
 * @brief Interval to be used in operators of "in" type.
 */
template <typename T>
struct interval_t
{
    using hana_tag=interval_tag;
    using type=T;

    /**
     * @brief Constructor.
     * @param from From endpoint.
     * @param to To endpoint.
     * @param mode Mode of endpoints.
     */
    template <typename T1, typename T2>
    interval_t(
            T1&& from,
            T2&& to,
            interval_mode mode=interval_mode::closed
        ) : from(std::forward<T1>(from)),
            to(std::forward<T2>(to)),
            mode(mode)
    {}

    T from;
    T to;
    interval_mode mode;
};

/**
 * @brief Helper to build intervals.
 */
struct interval_helper
{
    /**
     * @brief Create interval from endpoints and mode.
     * @param from From endpoint.
     * @param to To endpoint.
     * @param mode Mode of endpoints.
     * @return Interval.
     */
    template <typename T1, typename T2>
    auto operator() (T1&& from, T2&& to, interval_mode mode=interval_mode::closed) const
    {
        return interval_t<typename adjust_storable_type<T1>::type>(std::forward<T1>(from),std::forward<T2>(to),mode);
    }

    /**
     * @brief Get mode for closed interval.
     * @return Mode for closed interval.
     */
    constexpr static interval_mode closed()
    {
        return interval_mode::closed;
    }

    /**
     * @brief Get mode for open interval.
     * @return Mode for open interval.
     */
    constexpr static interval_mode open()
    {
        return interval_mode::open;
    }

    /**
     * @brief Get mode for left open interval.
     * @return Mode for left open interval.
     */
    constexpr static interval_mode open_from()
    {
        return interval_mode::open_from;
    }

    /**
     * @brief Get mode for right open interval.
     * @return Mode for right open interval.
     */
    constexpr static interval_mode open_to()
    {
        return interval_mode::open_to;
    }

};

/**
  Interval instance to be used in validators.
  */
constexpr interval_helper interval{};

/**
 * @brief String representation of interval to be used in reporting.
 */
struct interval_str_t : public enable_to_string<interval_str_t>
{
    constexpr static const char* description="interval";
};

/**
 * @brief Instance of string representation of interval to be used in reporting.
 */
constexpr interval_str_t interval_str{};

/**
 * @brief Helper to format interval operand.
 *
 * Interval is formatted as "interval [from,two]" where forms of the braces depend on interval's mode as follows:
 *  - "interval [from,two]" for closed intervals;
 *  - "interval (from,two)" for open intervals;
 *  - "interval (from,two]" for left open intervals;
 *  - "interval [from,two)" for right open intervals.
 *
 * If decorator is used then only the part within braces including the brases is decorated.
 */
template <typename T>
struct format_operand_t<T,hana::when<hana::is_a<interval_tag,T>>>
{
    /**
     * @brief Format interval operand.
     * @param traits Formatter traits.
     * @param val Operand value.
     * @param cats Grammatical categories of preceding phrase.
     * @return Formatted interval.
     */
    template <typename TraitsT, typename T1>
    auto operator () (const TraitsT& traits, T1&& val, grammar_categories cats) const
    {
        // format endpoints
        auto from=format_operand<decltype(val.from)>(traits,val.from,false);
        auto to=format_operand<decltype(val.to)>(traits,val.to,false);

        // format braces
        auto left=(val.mode==interval_mode::open || val.mode==interval_mode::open_from)?"(":"[";
        auto right=(val.mode==interval_mode::open || val.mode==interval_mode::open_to)?")":"]";

        // format interval definition
        std::string dst;
        backend_formatter.append(
           dst,
           left,
           from,
           ",",
           to,
           right
        );

        // decorate operand
        auto formatted_operand=decorate(traits,std::move(dst));

        // prepend description
        auto descr=translate(traits,std::string(interval_str),cats);
        std::string dst1;
        backend_formatter.append(
           dst1,
           descr,
           " ",
           formatted_operand
        );

        // done
        return dst1;
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_INTERVAL_HPP
