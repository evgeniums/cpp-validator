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

DRACOSHA_VALIDATOR_INLINE_LAMBDA auto check_has_begin=hana::is_valid([](auto&& v) -> decltype((void)hana::traits::declval(v).begin()){});
template <typename T>
using has_begin =
            std::integral_constant<
                bool,
                check_has_begin(hana::type_c<std::decay_t<T>>)
            >;

DRACOSHA_VALIDATOR_INLINE_LAMBDA auto check_has_end=hana::is_valid([](auto&& v) -> decltype((void)hana::traits::declval(v).end()){});
template <typename T>
using has_end =
            std::integral_constant<
                bool,
                check_has_end(hana::type_c<std::decay_t<T>>)
            >;

/**
 * @brief Check if type is a container.
**/
template <typename T>
using is_container_t=std::integral_constant<bool,
                                    has_begin<T>::value && has_end<T>::value
                                    && !std::is_same<std::decay_t<T>,std::string>::value
                                    && !std::is_same<std::decay_t<T>,string_view>::value
                                >;

/**
 * @brief Check if variable is a container.
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
