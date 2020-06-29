/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/tuple_to_variadic.hpp
*
*  Defines helper to create instance of variadic template with types pack taken from the tuple
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VARIADIC_TMPL_HPP
#define DRACOSHA_VALIDATOR_VARIADIC_TMPL_HPP

#include <string>
#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/scalar_compare.hpp>
#include <dracosha/validator/adjust_storable_type.hpp>
#include <dracosha/validator/detail/has_method.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/can_get.hpp>
#include <dracosha/validator/get.hpp>
#include <dracosha/validator/can_check_contains.hpp>
#include <dracosha/validator/contains.hpp>
#include <dracosha/validator/check_member.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/operators/comparison.hpp>
#include <dracosha/validator/lazy.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/dispatcher.hpp>
#include <dracosha/validator/prepare_dispatcher.hpp>
#include <dracosha/validator/properties/value.hpp>
#include <dracosha/validator/properties/empty.hpp>
#include <dracosha/validator/properties/size.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <template <typename...> class V>
struct tuple_to_variadic
{
    template <typename T, typename ... Prefix>
    constexpr static auto to_template(T&& t, Prefix&&... prefix)
    {
        auto concat=hana::concat(hana::make_tuple(prefix...),t);
        auto types_tuple=hana::transform(concat,hana::make<hana::type_tag>);
        return hana::unpack(types_tuple,hana::template_<V>);
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VARIADIC_TMPL_HPP
