/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/validator.hpp
*
*  Top level header of dracosha::validator library
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_HPP
#define DRACOSHA_VALIDATOR_HPP

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
#include <dracosha/validator/validate.hpp>
#include <dracosha/validator/prepare_validate.hpp>
#include <dracosha/validator/properties/value.hpp>
#include <dracosha/validator/properties/empty.hpp>
#include <dracosha/validator/properties/size.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/detail/validator_impl.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/master_reference.hpp>
#include <dracosha/validator/and.hpp>
#include <dracosha/validator/or.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct _t
{
    template <typename T>
    constexpr auto operator [] (T&& key) const -> decltype(auto)
    {
        return member<typename std::decay<T>::type>(std::forward<T>(key));
    }

    template <typename T>
    constexpr auto operator () (const T& masterRefObj) const -> decltype(auto)
    {
        return master_reference<T>(masterRefObj);
    }
};
constexpr _t _{};

//-------------------------------------------------------------

struct validator_t
{
    template <typename ... Args>
    constexpr auto operator () (Args&& ...args) const -> decltype(auto)
    {
        return AND(std::forward<Args>(args)...);
    }

    template <typename T>
    constexpr auto operator () (T&& v) const -> decltype(auto)
    {
        return hana::id(std::forward<T>(v));
    }
};
constexpr validator_t validator{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HPP
