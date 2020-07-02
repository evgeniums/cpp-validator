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

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <template <typename...> class V>
struct tuple_to_variadic
{
    template <typename T, typename ... Prefix>
    constexpr static auto to_template(T&& t, Prefix&&... prefix)
    {
        auto concat=hana::concat(hana::make_tuple(prefix...),t);
        auto types_tuple=hana::transform(concat,hana::make_type);
        return hana::unpack(types_tuple,hana::template_<V>);
    }
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VARIADIC_TMPL_HPP
