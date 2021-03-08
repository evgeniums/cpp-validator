/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/is_container.hpp
*
*  Defines helper to check if object is if container type.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_IS_CONTAINER_HPP
#define DRACOSHA_VALIDATOR_IS_CONTAINER_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/string_view.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Default helper for checking if type has begin() method.
 */
template <typename T, typename=void>
struct has_begin
{
    constexpr static const bool value=false;
};

DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_begin_c = hana::is_valid([](auto&& v) -> decltype(
                                                            (void)hana::traits::declval(v).begin()
                                                           )
                                                        {});
/**
 * @brief Helper for checking if type has begin() method for case when it has.
 */
template <typename T>
struct has_begin<T,
        std::enable_if_t<has_begin_c(hana::type_c<std::decay_t<T>>)>
    >
{
    constexpr static const bool value=true;
};

/**
 * @brief Default helper for checking if type has end() method.
 */
template <typename T, typename=void>
struct has_end
{
    constexpr static const bool value=false;
};

DRACOSHA_VALIDATOR_INLINE_LAMBDA auto has_end_c = hana::is_valid([](auto&& v) -> decltype(
                                                            (void)hana::traits::declval(v).begin()
                                                           )
                                                        {});
/**
 * @brief Helper for checking if type has end() method for case when it has.
 */
template <typename T>
struct has_end<T,
        std::enable_if_t<has_end_c(hana::type_c<std::decay_t<T>>)>
    >
{
    constexpr static const bool value=true;
};

/**
 * @brief Check if type is a container but not string or string_view.
**/
template <typename T>
using is_container_t=std::integral_constant<bool,
                                    has_begin<T>::value
                                    && has_end<T>::value
                                    && !std::is_same<std::decay_t<T>,std::string>::value
                                    && !std::is_same<std::decay_t<T>,string_view>::value
                                >;

/**
 * @brief Check if variable is a container but not string or string_view.
 * @param v Variable to check.
 * @return True if variable has begin() and end() methods.
 */
template <typename T>
constexpr auto is_container(T&& v)
{
    std::ignore=v;
    return is_container_t<T>::value;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_IS_CONTAINER_HPP
