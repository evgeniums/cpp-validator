/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/extract_cref.hpp
*
*  Defines helper for extracting constant reference from a value
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_EXTRACT_CREF_HPP
#define DRACOSHA_VALIDATOR_EXTRACT_CREF_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct cref_tag;

namespace detail
{
template <typename T, typename =hana::when<true>>
struct extract_cref_t
{
};

/**
 * @brief Extract constant reference if value is not a cref
 */
template <typename T>
struct extract_cref_t<T,
                    hana::when<!hana::is_a<cref_tag,T>>>
{
    constexpr auto operator() (T&& v) const -> decltype(auto)
    {
        return hana::id(std::forward<T>(v));
    }
};

/**
 * @brief Extract constant reference if value is a cref
 */
template <typename T>
struct extract_cref_t<T,
                    hana::when<hana::is_a<cref_tag,T>>>
{
    constexpr auto operator() (T&& v) const -> decltype(auto)
    {
        return v.get();
    }
};
template <typename T>
constexpr extract_cref_t<T> extract_cref{};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_EXTRACT_CREF_HPP
