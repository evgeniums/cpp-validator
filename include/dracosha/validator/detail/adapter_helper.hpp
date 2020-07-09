/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/adapter_helper.hpp
*
*  Defines adapter helper.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ADAPTER_HELPER_HPP
#define DRACOSHA_VALIDATOR_ADAPTER_HELPER_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
struct adapter_tag;

namespace detail
{
/**
 * @brief Adapter helper when object is not of adapter type
 */
template <typename T, typename =hana::when<true>>
struct adapter_helper_t
{
    template <typename T1>
    auto operator() (T1&& val) const -> decltype(auto)
    {
        return hana::id(std::forward<T1>(val));
    }
};
/**
 * @brief Adapter helper for extracting wrapped object from adapter
 */
template <typename T>
struct adapter_helper_t<T,hana::when<hana::is_a<adapter_tag,T>>>
{
    template <typename T1>
    auto operator() (T1&& val) const -> decltype(auto)
    {
        return val.object();
    }
};
/**
 * @brief Adapter helper for extracting wrapped object from adapter
 */
template <typename T>
constexpr adapter_helper_t<T> adapter_helper{};
}
//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADAPTER_HELPER_HPP
