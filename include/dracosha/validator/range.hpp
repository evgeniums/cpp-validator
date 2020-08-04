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
            T1&& container,
            size_t max_report_elements=std::numeric_limits<size_t>::max()
        ) : container(std::forward<T1>(container)),
            max_report_elements(max_report_elements)
    {}

    T container;
    size_t max_report_elements;
};

struct range_helper
{
    template <typename T>
    auto operator() (T&& container) const
    {
        return range_t<T>(std::forward<T>(container));
    }

    template <typename T, typename T2>
    auto operator() (T&& container, T2,
                     std::enable_if_t<
                        std::is_same<std::decay_t<T2>,sorted_t>::value,
                        void*
                     > = nullptr
                     ) const
    {
        return range_t<T,sorted_t>(std::forward<T>(container));
    }

    template <typename T, typename T2>
    auto operator() (T&& container, T2 max_report_elements,
                     std::enable_if_t<
                        !std::is_same<std::decay_t<T2>,sorted_t>::value,
                        void*
                     > = nullptr
                     ) const
    {
        return range_t<T>(std::forward<T>(container),max_report_elements);
    }

    template <typename T>
    auto operator() (T&& container, sorted_t, size_t max_report_elements) const
    {
        return range_t<T,sorted_t>(std::forward<T>(container),max_report_elements);
    }

    template <typename T>
    auto operator() (std::initializer_list<T> init) const
    {
        return range_t<std::vector<T>>(std::vector<T>{std::move(init)});
    }

    template <typename T, typename T2>
    auto operator() (std::initializer_list<T> init, T2,
                     std::enable_if_t<
                             std::is_same<std::decay_t<T2>,sorted_t>::value,
                             void*
                          > = nullptr
                     ) const
    {
        return range_t<std::vector<T>,sorted_t>(std::vector<T>{std::move(init)});
    }

    template <typename T, typename T2>
    auto operator() (std::initializer_list<T> init, T2 max_report_elements,
                     std::enable_if_t<
                             !std::is_same<std::decay_t<T2>,sorted_t>::value,
                             void*
                          > = nullptr
                     ) const
    {
        return range_t<std::vector<T>>(std::vector<T>{std::move(init)},max_report_elements);
    }

    template <typename T>
    auto operator() (std::initializer_list<T> init, sorted_t, size_t max_report_elements) const
    {
        return range_t<std::vector<T>,sorted_t>(std::vector<T>{std::move(init)},max_report_elements);
    }
};
constexpr range_helper range{};

struct range_str_t : public enable_to_string<range_str_t>
{
    constexpr static const char* description="range";
};
constexpr range_str_t range_str{};

namespace detail
{
template <typename T>
struct format_operand_t<T,hana::when<hana::is_a<range_tag,T>>>
{
    template <typename TraitsT, typename T1>
    auto operator () (const TraitsT& traits, T1&& val, grammar_categories cats) const
    {
        auto count=(std::min)(val.max_report_elements,val.container.size());
        bool append_ellipsis=count<val.container.size();
        using value_type=typename std::decay_t<decltype(val.container)>::value_type;
        using element_type=decltype(detail::format_operand<value_type>(std::declval<TraitsT>(),std::declval<value_type>(),std::declval<bool>()));
        std::vector<element_type> elements;
        elements.reserve(count);
        for (auto&& it:val.container)
        {
            if (count>0)
            {
                elements.push_back(
                    detail::format_operand<element_type>(traits,it,false)
                );
            }
            else
            {
                break;
            }
            --count;
        }

        std::string dst;
        backend_formatter.append_join(
           dst,
           ",",
            elements
        );
        auto formatted_operand=decorate(traits,std::move(dst));

        auto descr=translate(traits,std::string(range_str),cats);
        std::string dst1;
        backend_formatter.append(
           dst1,
           descr,
           " [",
           formatted_operand
        );
        if (append_ellipsis)
        {
            backend_formatter.append(
               dst1,
               ",..."
            );
        }
        backend_formatter.append(
           dst1,
           "]"
        );
        return dst1;
    }
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_RANGE_HPP
