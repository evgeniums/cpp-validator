/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/string_view.hpp
*
*  Defines alias for string_view: boost::string_view is used for C++14 standard and std::string_view is used for C++17 and newer stansards.
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

struct adjust_view_ignore{};

namespace detail
{

/**
 * @brief Default helper to adjust value to string view if applicable.
 */
template <typename T, typename=hana::when<true>>
struct adjust_view_type_t
{
};

/**
 * @brief Helper to adjust value to string view if string_view is constructible from the type.
 */
template <typename T>
struct adjust_view_type_t<T,
                    hana::when<
                            std::is_constructible<string_view,T>::value
                            &&
                            !std::is_base_of<adjust_view_ignore,std::decay_t<T>>::value
                        >
                    >
{
    using type=string_view;

    /**
     * @brief Construct string_view from argument.
     * @param v Input variable.
     * @return string_view of input variable.
     */
    template <typename T1>
    constexpr auto operator() (T1&& v) const
    {
        return string_view(std::forward<T1>(v));
    }
};

/**
 * @brief Helper to adjust value to string view if string_view is not constructible from the type.
 */
template <typename T>
struct adjust_view_type_t<T,
                    hana::when<
                            !std::is_constructible<string_view,T>::value
                            ||
                            std::is_base_of<adjust_view_ignore,std::decay_t<T>>::value
                        >
                    >
{
    using type=std::decay_t<T>;

    /**
     * @brief Bypass input.
     * @param v Input argument.
     * @return Bypassed input argument.
     */
    template <typename T1>
    constexpr auto operator() (T1&& v) const -> decltype(auto)
    {
        return hana::id(std::forward<T1>(v));
    }
};

/**
 * @brief Instance of helper for adjusting value to string view.
 */
template <typename T>
constexpr adjust_view_type_t<T> adjust_view_type{};
}

/**
 * @brief Convert value to string_view type if applicable.
 * @param v Value.
 * @return String view of value if applicable or value itself otherwise.
 */
template <typename T>
constexpr auto adjust_view_type(T&& v) -> decltype(auto)
{
    return detail::adjust_view_type<T>(std::forward<T>(v));
}

template <typename T2, typename T1>
constexpr auto unadjust_view_type(T1&& v) -> decltype(auto)
{
    return hana::if_(
        hana::and_(
            std::is_same<std::decay_t<T2>,std::string>{},
            std::is_same<std::decay_t<T1>,string_view>{}
        ),
        [](auto&& v)
        {
            return std::string(v.data(),v.size());
        },
        [](auto&& v) -> decltype(auto)
        {
            return hana::id(std::forward<decltype(v)>(v));
        }
    )(std::forward<T1>(v));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_STRING_VIEW_HPP
