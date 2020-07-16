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
#include <dracosha/validator/utils/safe_compare.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
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
#include <dracosha/validator/adapters/adapter.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>
#include <dracosha/validator/prepare_dispatcher.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/detail/validator_impl.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/master_sample.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/operators.hpp>
#include <dracosha/validator/validate.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Helper for generating members and master sample wrappers
 */
struct _t
{
    /**
     * @brief Create member
     * @param key Key of the member
     * @return Member
     */
    template <typename T>
    constexpr auto operator [] (T&& key) const -> decltype(auto)
    {
        return member<std::decay_t<T>>(std::forward<T>(key));
    }

    /**
     * @brief Create master sample wrapper
     * @param masterRefObj Object to use as a master sample
     * @return Wrapper of master sample object
     */
    template <typename T>
    constexpr auto operator () (const T& masterRefObj) const -> decltype(auto)
    {
        return master_sample<T>(masterRefObj);
    }
};
constexpr _t _{};

//-------------------------------------------------------------

/**
 * @brief Helper class to wrap validators
 */
struct validator_t
{
    /**
     * @brief Wrap list of validators or validation operators into logical AND
     */
    template <typename ... Args>
    constexpr auto operator () (Args&& ...args) const -> decltype(auto)
    {
        return AND(std::forward<Args>(args)...);
    }

    /**
     * @brief Just syntax sugar, return input validator or validation operator as is
     */
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
