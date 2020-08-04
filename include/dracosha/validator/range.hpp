/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/range.hpp
*
*  Defines range wrapper
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_RANGE_HPP
#define DRACOSHA_VALIDATOR_RANGE_HPP

#include <vector>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>
#include <dracosha/validator/detail/format_operand.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct range_tag;

using sorted_t=std::true_type;
constexpr sorted_t sorted{};

/**
 * @brief Range wraps searchable container and can be used in operators of "in" type
 */
template <typename T, typename SortedT=std::false_type>
struct range_t
{
    using hana_tag=range_tag;
    using type=T;
    using is_sorted=SortedT;

    template <typename T1>
    range_t(
            T1&& container
        ) : container(std::forward<T1>(container))
    {}

    T container;
};

struct range_helper
{
    template <typename T>
    auto operator() (T&& container) const
    {
        return range_t<T>(std::forward<T>(container));
    }

    template <typename T>
    auto operator() (T&& container, sorted_t) const
    {
        return range_t<T,sorted_t>(std::forward<T>(container));
    }

    template <typename T>
    auto operator() (std::initializer_list<T> init) const
    {
        return range_t<std::vector<T>>(std::vector<T>{std::move(init)});
    }

    template <typename T>
    auto operator() (std::initializer_list<T> init, sorted_t) const
    {
        return range_t<std::vector<T>,sorted_t>(std::vector<T>{std::move(init)});
    }
};
constexpr range_helper range{};

//struct interval_str_t : public enable_to_string<interval_str_t>
//{
//    constexpr static const char* description="interval";
//};
//constexpr interval_str_t interval_str{};

//namespace detail
//{
//template <typename T>
//struct format_operand_t<T,hana::when<hana::is_a<interval_tag,T>>>
//{
//    template <typename TraitsT, typename T1>
//    auto operator () (const TraitsT& traits, T1&& val, grammar_categories cats) const
//    {
//        auto from=detail::format_operand<decltype(val.from)>(traits,val.from,false);
//        auto to=detail::format_operand<decltype(val.to)>(traits,val.to,false);

//        auto left=(val.mode==interval_mode::open || val.mode==interval_mode::open_from)?"(":"[";
//        auto right=(val.mode==interval_mode::open || val.mode==interval_mode::open_to)?")":"]";

//        std::string dst;
//        backend_formatter.append(
//           dst,
//           left,
//           from,
//           ",",
//           to,
//           right
//        );
//        auto formatted_operand=decorate(traits,std::move(dst));

//        auto descr=translate(traits,std::string(interval_str),cats);
//        std::string dst1;
//        backend_formatter.append(
//           dst1,
//           descr,
//           " ",
//           formatted_operand
//        );
//        return dst1;
//    }
//};

//}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_RANGE_HPP
