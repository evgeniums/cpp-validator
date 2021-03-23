/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/heterogeneous_size.hpp
*
*  Defines helper got getting size of heterogeneous objects/containers.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_HETEROGENEOUS_SIZE_HPP
#define DRACOSHA_VALIDATOR_HETEROGENEOUS_SIZE_HPP

#include <boost/hana/ext/std/tuple.hpp>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/hana_to_std_tuple.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Implementation of is_constant_size.
 */
template <typename T, typename Enable=hana::when<true>>
struct is_constant_size_impl
{
    constexpr static const bool value=false;
};
template <typename T>
struct is_constant_size_impl<T,
            hana::when<
                (
                hana::is_a<hana::integral_constant_tag<size_t>,T>
                ||
                hana::is_a<hana::ext::std::integral_constant_tag<size_t>,T>
                )
            >
        >
{
    constexpr static const bool value=true;
};

/**
 * @brief Helper to figure out if a type is of integral constant size_t.
 */
template <typename T>
struct is_constant_size : public std::integral_constant<bool,is_constant_size_impl<T>::value>
{};

//-------------------------------------------------------------

/**
 * @brief Helper for getting heterogeneous size of a type.
 * By default size is hana::size_t<0>.
 */
template <typename T, typename Enable=hana::when<true>>
struct heterogeneous_size_t : public hana::size_t<0>
{
};
/**
 * @brief Helper for getting heterogeneous size of foldable type such as han::tuple and std::tuple.
 */
template <typename T>
struct heterogeneous_size_t<T,
            hana::when<hana::Foldable<T>::value>
        > : public hana_tuple_size<T>
{
};

/**
 * @brief Implementer of heterogeneous_size().
 */
struct heterogeneous_size_impl
{
    template <typename T>
    constexpr auto operator () (T&&) const
    {
        return heterogeneous_size_t<T>{};
    }
};
/**
 * @brief Get size of heterogeneous container.
 */
constexpr heterogeneous_size_impl heterogeneous_size{};

//-------------------------------------------------------------

/**
 * @brief Implementer of is_heterogeneous_container().
 */
struct is_heterogeneous_container_impl
{
    template <typename T>
    constexpr auto operator () (T&& v) const
    {
        return hana::not_equal(heterogeneous_size(v),hana::size_c<0>);
    }
};
/**
 * @brief Figure out if object is a heterogeneous container.
 * @param v Object.
 * @return Logical integral constant.
 */
constexpr is_heterogeneous_container_impl is_heterogeneous_container{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HETEROGENEOUS_SIZE_HPP
