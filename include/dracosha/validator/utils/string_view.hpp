/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/utils/string_view.hpp
*
*  Defines wrapper for string_view
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_STRING_VIEW_HPP
#define DRACOSHA_VALIDATOR_STRING_VIEW_HPP

#if __cplusplus < 201703L
#include <boost/utility/string_view.hpp>
#else
#include <string_view>
#endif

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

#if __cplusplus < 201703L
using string_view=boost::string_view;
#else
using string_view=std::string_view;
#endif

namespace detail
{

/**
 * @brief Helper to adjust value to string view if applicable
 */
template <typename T, typename=hana::when<true>>
struct adjust_view_type_t
{
};

template <typename T>
struct adjust_view_type_t<T,
                    hana::when<std::is_constructible<string_view,T>::value>
                    >
{
    using type=string_view;
    template <typename T1>
    constexpr auto operator() (T1&& v) const
    {
        return string_view(std::forward<T1>(v));
    }
};

template <typename T>
struct adjust_view_type_t<T,
                    hana::when<!std::is_constructible<string_view,T>::value>
                    >
{
    using type=std::decay_t<T>;

    template <typename T1>
    constexpr auto operator() (T1&& v) const -> decltype(auto)
    {
        return hana::id(std::forward<T1>(v));
    }
};
template <typename T>
constexpr adjust_view_type_t<T> adjust_view_type{};
}

/**
 * @brief Convert value to string_view type if applicable.
 * @param v Value
 * @return string view of value if applicable or value itself otherwise
 */
template <typename T>
constexpr auto adjust_view_type (T&& v) -> decltype(auto)
{
    return detail::adjust_view_type<T>(std::forward<T>(v));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_STRING_VIEW_HPP
