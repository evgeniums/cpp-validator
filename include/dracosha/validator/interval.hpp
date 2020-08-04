/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/interval.hpp
*
*  Defines interval
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_INTERVAL_HPP
#define DRACOSHA_VALIDATOR_INTERVAL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>
#include <dracosha/validator/detail/format_operand.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct interval_tag;

enum class interval_mode : int
{
    closed,
    open,
    open_from,
    open_to
};

/**
 * @brief Interval to be used in operators of "in" type
 */
template <typename T>
struct interval_t
{
    public:

        using hana_tag=interval_tag;

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

struct interval_helper
{
    template <typename T>
    auto operator() (T&& from, T&& to, interval_mode mode=interval_mode::closed) const
    {
        return interval_t<typename adjust_storable_type<T>::type>(std::forward<T>(from),std::forward<T>(to),mode);
    }

    constexpr static interval_mode closed()
    {
        return interval_mode::closed;
    }

    constexpr static interval_mode open()
    {
        return interval_mode::open;
    }

    constexpr static interval_mode open_from()
    {
        return interval_mode::open_from;
    }

    constexpr static interval_mode open_to()
    {
        return interval_mode::open_to;
    }

};
constexpr interval_helper interval{};

struct interval_str_t : public enable_to_string<interval_str_t>
{
    constexpr static const char* description="interval";
};
constexpr interval_str_t interval_str{};

namespace detail
{
template <typename T>
struct format_operand_t<T,hana::when<hana::is_a<interval_tag,T>>>
{
    template <typename TraitsT, typename T1>
    auto operator () (const TraitsT& traits, T1&& val, grammar_categories cats) const
    {
        auto from=detail::format_operand<decltype(val.from)>(traits,val.from,false);
        auto to=detail::format_operand<decltype(val.to)>(traits,val.to,false);

        auto left=(val.mode==interval_mode::open || val.mode==interval_mode::open_from)?"(":"[";
        auto right=(val.mode==interval_mode::open || val.mode==interval_mode::open_to)?")":"]";

        std::string dst;
        backend_formatter.append(
           dst,
           left,
           from,
           ",",
           to,
           right
        );
        auto formatted_operand=decorate(traits,std::move(dst));

        auto descr=translate(traits,std::string(interval_str),cats);
        std::string dst1;
        backend_formatter.append(
           dst1,
           descr,
           " ",
           formatted_operand
        );
        return dst1;
    }
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_INTERVAL_HPP
