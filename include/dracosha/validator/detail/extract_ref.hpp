/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/extract_reference_wrapper.hpp
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

struct ref_tag;

namespace detail
{
template <typename T, typename =hana::when<true>>
struct extract_ref_t
{
};

/**
 * @brief Return the value itself if it is not a reference wrapper
 */
template <typename T>
struct extract_ref_t<T,
                    hana::when<!hana::is_a<ref_tag,T>>>
{
    constexpr auto operator() (T&& v) const -> decltype(auto)
    {
        return hana::id(std::forward<T>(v));
    }
};

/**
 * @brief Extract reference if value is a reference wrapper
 */
template <typename T>
struct extract_ref_t<T,
                    hana::when<hana::is_a<ref_tag,T>>>
{
    constexpr auto operator() (T&& v) const -> decltype(auto)
    {
        return v.get();
    }
};
template <typename T>
constexpr extract_ref_t<T> extract_ref_impl{};
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_EXTRACT_CREF_HPP
