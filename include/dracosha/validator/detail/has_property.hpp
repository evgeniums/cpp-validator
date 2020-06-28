/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/has_property.hpp
*
* Defines implementation of checking if object has a property.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_HAS_PROPERTY_HPP
#define DRACOSHA_VALIDATOR_HAS_PROPERTY_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/detail/has_method.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct property_tag;

//-------------------------------------------------------------

namespace detail
{

template <typename T,typename PropT, typename = hana::when<true>>
struct has_property_t
{
};
template <typename T,typename PropT>
struct has_property_t<T,PropT,hana::when<hana::is_a<property_tag,PropT>>>
{
    constexpr bool operator() () const
    {
        return std::decay<PropT>::type::template has<T>();
    }
};
template <typename T,typename PropT>
struct has_property_t<T,PropT,hana::when<!hana::is_a<property_tag,PropT>>>
{
    constexpr bool operator() () const
    {
        return false;
    }
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HAS_PROPERTY_HPP
