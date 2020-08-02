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
#include <dracosha/validator/property_validator.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/master_sample.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/operators.hpp>
#include <dracosha/validator/validate.hpp>
#include <dracosha/validator/detail/default_adapter_impl.ipp>
#include <dracosha/validator/operand.hpp>
#include <dracosha/validator/operators/invert_op.hpp>
#include <dracosha/validator/operators/wrap_op.hpp>

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
        return member<
                    typename adjust_storable_type<std::decay_t<T>>::type
                >(std::forward<T>(key));
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

    /**
     * @brief Wrap operand appending explicit description to the operand
     * @param val Operand
     * @param description Explicit description to be used in reporting
     * @return Wrapped operand
     */
    template <typename T1, typename T2>
    constexpr auto operator () (
            T1&& val,
            T2&& description,
            std::enable_if_t<!hana::is_a<operator_tag,T1>,void*> =nullptr
        ) const
    {
        return operand<T1>(std::forward<T1>(val),std::forward<T2>(description));
    }

    /**
     * @brief Wrap operator appending explicit description to the operator
     * @param op Operator
     * @param description Explicit description to be used in reporting
     * @return Wrapped operator
     */
    template <typename T1, typename T2>
    constexpr auto operator () (
            T1&& op,
            T2&& description,
            std::enable_if_t<
                (hana::is_a<operator_tag,T1>
                 &&
                 !std::is_same<exists_t,std::decay_t<T1>>::value
                 &&
                 !std::is_base_of<flag_t,std::decay_t<T1>>::value
                 )
                ,void*> =nullptr
        ) const
    {
        return wrap_op_with_string<T1,T2>(std::forward<T1>(op),std::forward<T2>(description));
    }

    /**
     * @brief Wrap flag operator appending explicit description
     * @param op Flag operator
     * @param description Explicit description to be used in reporting
     * @return Flag operator with description
     */
    template <typename T1, typename T2>
    constexpr auto operator () (
            T1&& op,
            T2&& description,
            std::enable_if_t<
                 std::is_same<flag_op,std::decay_t<T1>>::value
                ,void*> =nullptr
        ) const
    {
        return op(std::forward<T2>(description));
    }

    /**
     * @brief Wrap exists operator appending explicit description
     * @param op Exists operator
     * @param description Explicit description to be used in reporting
     * @return Exists operator with description
     */
    template <typename T1, typename T2>
    constexpr auto operator () (
            T1&&,
            T2&& description,
            std::enable_if_t<std::is_same<exists_t,std::decay_t<T1>>::value,void*> =nullptr
        ) const
    {
        return exists_op_with_string_t{std::forward<T2>(description)};
    }
};
constexpr _t _{};

//-------------------------------------------------------------

/**
 * @brief Helper class to wrap validators
 */
struct wrap_validator
{
    /**
     * @brief Wrap list of validators or validation operators into logical AND
     * @param args Arguments to forward to AND
     * @return Validator
     */
    template <typename ... Args>
    constexpr auto operator () (Args&& ...args) const -> decltype(auto)
    {
        return AND(std::forward<Args>(args)...);
    }

    /**
     * @brief Make validator from a property validator
     * @param v Property validator
     * @return Validator
     */
    template <typename T>
    constexpr auto operator () (T&& v,
                                std::enable_if_t<hana::is_a<property_validator_tag,T>,void*> =nullptr
            ) const -> decltype(auto)
    {
        return make_validator(std::forward<T>(v));
    }

    /**
     * @brief Just syntax sugar, return validator or validation operator as is
     * @param v Validator or validation operator
     * @return Input validator as is
     */
    template <typename T>
    constexpr auto operator () (T&& v,
                                std::enable_if_t<!hana::is_a<property_validator_tag,T>,void*> =nullptr
            ) const -> decltype(auto)
    {
        return hana::id(std::forward<T>(v));
    }
};
constexpr wrap_validator validator{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HPP
