/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/range.hpp
*
*  Defines range wrapper
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_RANGE_HPP
#define DRACOSHA_VALIDATOR_RANGE_HPP

#include <vector>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/enable_to_string.hpp>
#include <dracosha/validator/reporting/format_operand.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct range_tag;

using sorted_t=std::true_type;
/**
 * Flag to use as parameter for sorted ranges.
 */
constexpr sorted_t sorted{};

/**
 * @brief Wrapper of searchable container and can be used in operators of "in" type.
 */
template <typename T, typename SortedT=std::false_type>
struct range_t
{
    using hana_tag=range_tag;
    using type=T;
    using is_sorted=SortedT;

    /**
     * @brief Constructor.
     * @param container Container to be wrapped into range.
     * @param max_report_elements Max number of range elements to be listed in report.
     */
    template <typename T1>
    range_t(
            T1&& container,
            size_t max_report_elements=(std::numeric_limits<size_t>::max)()
        ) : container(std::forward<T1>(container)),
            max_report_elements(max_report_elements)
    {}

    T container;
    size_t max_report_elements;
};

/**
 * @brief Helper for building ranges.
 */
struct range_helper
{
    /**
     * @brief Make range from container.
     * @param container Container to wrap in range object.
     * @return Range.
     */
    template <typename T>
    auto operator() (T&& container) const
    {
        return range_t<T>(std::forward<T>(container));
    }

    /**
     * @brief Make range from sorted container.
     * @param container Container to wrap in range object.
     * @param sorted Explicit sorted value to flag that container is sorted.
     * @return Sorted range.
     *
     * Sorted ranges can use use faster lookup methods than ordinary ranges.
     */
    template <typename T, typename T2>
    auto operator() (T&& container, T2 sorted,
                     std::enable_if_t<
                        std::is_same<std::decay_t<T2>,sorted_t>::value,
                        void*
                     > = nullptr
                     ) const
    {
        std::ignore=sorted;
        return range_t<T,sorted_t>(std::forward<T>(container));
    }

    /**
     * @brief Make range from container.
     * @param container Container to be wrapped into range.
     * @param max_report_elements Max number of range elements to be listed in report.
     * @return Range.
     */
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

    /**
     * @brief Make range from sorted container.
     * @param container Container to wrap in range object.
     * @param sorted Explicit sorted value to flag that container is sorted.
     * @param max_report_elements Max number of range elements to be listed in report
     * @return Sorted range.
     *
     * Sorted ranges can use use faster lookup methods than ordinary ranges.
     */
    template <typename T>
    auto operator() (T&& container, sorted_t sorted, size_t max_report_elements) const
    {
        std::ignore=sorted;
        return range_t<T,sorted_t>(std::forward<T>(container),max_report_elements);
    }

    /**
     * @brief Make range from initializer list.
     * @param init Initializer list.
     * @return Range.
     *
     * Initializer list is moved to embedded vector container of the range.
     */
    template <typename T>
    auto operator() (std::initializer_list<T> init) const
    {
        return range_t<std::vector<T>>(std::vector<T>{std::move(init)});
    }

    /**
     * @brief Make sorted range from sorted initializer list.
     * @param init Initializer list.
     * @param sorted Explicit sorted value to flag that container is sorted.
     * @return Sorted range.
     *
     * Initializer list is moved to embedded vector container of the range.
     */
    template <typename T, typename T2>
    auto operator() (std::initializer_list<T> init, T2 sorted,
                     std::enable_if_t<
                             std::is_same<std::decay_t<T2>,sorted_t>::value,
                             void*
                          > = nullptr
                     ) const
    {
        std::ignore=sorted;
        return range_t<std::vector<T>,sorted_t>(std::vector<T>{std::move(init)});
    }

    /**
     * @brief Make range from initializer list.
     * @param init Initializer list.
     * @param max_report_elements Max number of range elements to be listed in report.
     * @return Range.
     *
     * Initializer list is moved to embedded vector container of the range.
     */
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

    /**
     * @brief Make sorted range from sorted initializer list.
     * @param init Initializer list.
     * @param sorted Explicit sorted value to flag that container is sorted.
     * @param max_report_elements Max number of range elements to be listed in report.
     * @return Range.
     *
     * Initializer list is moved to embedded vector container of the range.
     */
    template <typename T>
    auto operator() (std::initializer_list<T> init, sorted_t sorted, size_t max_report_elements) const
    {
        std::ignore=sorted;
        return range_t<std::vector<T>,sorted_t>(std::vector<T>{std::move(init)},max_report_elements);
    }
};
constexpr range_helper range{};

/**
 * @brief String representation of range to be used in reporting.
 */
struct range_str_t : public enable_to_string<range_str_t>
{
    constexpr static const char* description="range";
};

/**
 * @brief Instance of string representation of range to be used in reporting.
 */
constexpr range_str_t range_str{};

/**
 * @brief Helper to format range operand.
 *
 * Range is formatted as "range [x[0], x[1], ... , x[N]]", where x[i] denotes i-th element of the container which is formatted using format_operand helper.
 * If max_report_elements number is set then at most max_report_elements are used for formatting and ", ... " is appended to the end of the list.
 *
 * Example:
 * @code
 * std::vector<size_t> vec{1,2,3,4,5,6,7,8,9,10};
 * // vector is formatted as "range [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]" if max_report_elements number is not set
 * // vector is formatted as "range [1, 2, 3, 4, 5, ... ]" if max_report_elements number is set to 5
 * @endcode
 *
 * If decorator is used then only the part within braces including the braces is decorated.
 */
template <typename T>
struct format_operand_t<T,hana::when<hana::is_a<range_tag,T>>>
{
    /**
     * @brief Format range operand.
     * @param traits Formatter traits.
     * @param val Operand value.
     * @param cats Grammatical categories of preceding phrase.
     * @return Formatted range.
     */
    template <typename TraitsT, typename T1>
    auto operator () (const TraitsT& traits, T1&& val, grammar_categories cats) const
    {
        // format each element up to max_report_elements
        auto count=(std::min)(val.max_report_elements,val.container.size());
        bool append_ellipsis=count<val.container.size();
        using value_type=typename std::decay_t<decltype(val.container)>::value_type;
        using element_type=decltype(format_operand<value_type>(std::declval<TraitsT>(),std::declval<value_type>(),std::declval<bool>()));
        std::vector<element_type> elements;
        elements.reserve(count);
        for (auto&& it:val.container)
        {
            if (count>0)
            {
                elements.push_back(
                    format_operand<element_type>(traits,it,false)
                );
            }
            else
            {
                break;
            }
            --count;
        }

        // join elements
        std::string dst;
        backend_formatter.append_join(
           dst,
           ", ",
            elements
        );

        // append ellipsis if contaner size exceeds max_report_elements
        if (append_ellipsis)
        {
            backend_formatter.append(
               dst,
               ", ... "
            );
        }

        // wrap into braces
        std::string dst1;
        backend_formatter.append(
           dst1,
           "[",
           dst,
           "]"
        );

        // decorate operand
        auto formatted_operand=decorate(traits,std::move(dst1));

        // prepend description
        auto descr=translate(traits,std::string(range_str),cats);
        std::string dst2;
        backend_formatter.append(
           dst2,
           descr,
           " ",
           formatted_operand
        );

        // done
        return dst2;
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_RANGE_HPP
