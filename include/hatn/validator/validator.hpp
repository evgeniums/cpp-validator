/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/validator.hpp
*
*  Top level header of validator library.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_HPP
#define HATN_VALIDATOR_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/utils/safe_compare.hpp>
#include <hatn/validator/utils/adjust_storable_type.hpp>
#include <hatn/validator/detail/has_method.hpp>
#include <hatn/validator/property.hpp>
#include <hatn/validator/can_get.hpp>
#include <hatn/validator/get.hpp>
#include <hatn/validator/can_check_contains.hpp>
#include <hatn/validator/check_contains.hpp>
#include <hatn/validator/check_exists.hpp>
#include <hatn/validator/check_member.hpp>
#include <hatn/validator/operators/exists.hpp>
#include <hatn/validator/operators/comparison.hpp>
#include <hatn/validator/lazy.hpp>
#include <hatn/validator/extract.hpp>
#include <hatn/validator/get_member.hpp>
#include <hatn/validator/dispatcher.hpp>
#include <hatn/validator/adapter.hpp>
#include <hatn/validator/adapters/default_adapter.hpp>
#include <hatn/validator/property_validator.hpp>
#include <hatn/validator/apply.hpp>
#include <hatn/validator/make_validator.hpp>
#include <hatn/validator/member.hpp>
#include <hatn/validator/master_sample.hpp>
#include <hatn/validator/properties.hpp>
#include <hatn/validator/operators.hpp>
#include <hatn/validator/operand.hpp>
#include <hatn/validator/operators/invert_op.hpp>
#include <hatn/validator/operators/wrap_op.hpp>
#include <hatn/validator/member_with_name.hpp>
#include <hatn/validator/member_with_name_list.hpp>
#include <hatn/validator/make_member.hpp>

#include <hatn/validator/aggregation/aggregation.ipp>
#include <hatn/validator/aggregation/element_aggregation.ipp>
#include <hatn/validator/detail/member_helper.ipp>
#include <hatn/validator/prepend_super_member.ipp>
#include <hatn/validator/apply_generated_paths.ipp>
#include <hatn/validator/apply_member_path.ipp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Helper for generating members and validation wrappers.
 */
struct _t
{
    /**
     * @brief Create member.
     * @param key Key of the member.
     * @return Member.
     */
    template <typename T>
    constexpr auto operator [] (T&& key) const
    {
        return make_plain_member(std::forward<T>(key));
    }

    /**
     * @brief Wrap master sample object.
     * @param masterRefObj Object to use as a master sample.
     * @return Wrapped master sample object.
     */
    template <typename T>
    constexpr auto operator () (const T& masterRefObj) const -> decltype(auto)
    {
        return master_sample<T>(masterRefObj);
    }

    /**
     * @brief Wrap operand appending explicit description to the operand.
     * @param val Operand.
     * @param description Explicit description to be used in reporting.
     * @return Wrapped operand.
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
     * @brief Wrap operator appending explicit description to the operator.
     * @param op Operator.
     * @param description Explicit description to be used in reporting.
     * @return Wrapped operator.
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
     * @brief Wrap flag operator appending explicit description.
     * @param op Flag operator.
     * @param description Explicit description to be used in reporting.
     * @return Flag operator with description.
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
     * @brief Wrap exists operator appending explicit description.
     * @param op Exists operator.
     * @param description Explicit description to be used in reporting.
     * @return Exists operator with description.
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

/**
 * @brief Instance of helper for generating members and validation wrappers.
 */
constexpr _t _{};

//-------------------------------------------------------------

/**
 * @brief Helper for wrapping validators.
 */
struct wrap_validator
{
    /**
     * @brief Forward list of validators or validation operators to logical AND.
     * @param args Arguments to forward to AND.
     * @return Validator.
     */
    template <typename ... Args>
    constexpr auto operator () (Args&& ...args) const -> decltype(auto)
    {
        return AND(std::forward<Args>(args)...);
    }

    /**
     * @brief Make validator from a property validator.
     * @param v Property validator.
     * @return Validator.
     */
    template <typename T>
    constexpr auto operator () (T&& v,
                                std::enable_if_t<hana::is_a<property_validator_tag,T>,void*> =nullptr
            ) const -> decltype(auto)
    {
        return make_validator(std::forward<T>(v));
    }

    /**
     * @brief Just syntax sugar, return validator or validation operator as is.
     * @param v Validator or validation operator.
     * @return Input validator as is.
     */
    template <typename T>
    constexpr auto operator () (T&& v,
                                std::enable_if_t<!hana::is_a<property_validator_tag,T>,void*> =nullptr
            ) const -> decltype(auto)
    {
        return hana::id(std::forward<T>(v));
    }

    /**
     * @brief Create validator form operator and operand.
     * @param op Operator.
     * @param b Operand.
     * @return Validator.
     */
    template <typename OpT, typename T>
    constexpr auto operator () (OpT&& op,
                                T&& b,
                                std::enable_if_t<hana::is_a<operator_tag,OpT>,void*> =nullptr
            ) const
    {
        return make_validator(value(std::forward<OpT>(op),std::forward<T>(b)));
    }
};

/**
 * @brief Helper for wrapping validators on heap.
 */
struct wrap_validator_on_heap
{
    /**
     * @brief Forward list of validators or validation operators to logical AND.
     * @param args Arguments to forward to AND.
     * @return Validator.
     */
    template <typename ... Args>
    constexpr auto operator () (Args&& ...args) const -> decltype(auto)
    {
        return AND_on_heap(std::forward<Args>(args)...);
    }

    /**
     * @brief Make validator from a property validator.
     * @param v Property validator.
     * @return Validator.
     */
    template <typename T>
    constexpr auto operator () (T&& v,
                                std::enable_if_t<hana::is_a<property_validator_tag,T>,void*> =nullptr
            ) const -> decltype(auto)
    {
        return make_validator_on_heap(std::forward<T>(v));
    }

    /**
     * @brief Just syntax sugar, return validator or validation operator as is.
     * @param v Validator or validation operator.
     * @return Input validator as is.
     */
    template <typename T>
    constexpr auto operator () (T&& v,
                                std::enable_if_t<std::is_pointer<std::remove_reference_t<T>>::value
                                                &&
                                                hana::is_a<validator_tag,typename std::pointer_traits<std::remove_reference_t<T>>::element_type>
                                                ,
                                                void*> =nullptr
            ) const -> decltype(auto)
    {
        return hana::id(std::forward<T>(v));
    }

    /**
     * @brief Create validator form operator and operand.
     * @param op Operator.
     * @param b Operand.
     * @return Validator.
     */
    template <typename OpT, typename T>
    constexpr auto operator () (OpT&& op,
                                T&& b,
                                std::enable_if_t<hana::is_a<operator_tag,OpT>,void*> =nullptr
            ) const
    {
        return make_validator_on_heap(value(std::forward<OpT>(op),std::forward<T>(b)));
    }
};

/**
  @brief Callable object for creating or wrapping validators.
*/
constexpr wrap_validator validator{};

/**
  @brief Callable object for creating or wrapping validators on heap.
*/
constexpr wrap_validator_on_heap new_validator{};

/**
 * @brief Helper for wrapping validators in shared pointer.
 */
struct shared_validator_t
{
    template <typename ... Args>
    auto operator () (Args&& ...args) const
    {
        using pointer_type=decltype(new_validator(std::forward<Args>(args)...));
        using element_type=typename std::pointer_traits<pointer_type>::element_type;
        return std::shared_ptr<element_type>(new_validator(std::forward<Args>(args)...));
    }
};
/**
  @brief Callable object for wrapping validator in shared pointer.
*/
constexpr shared_validator_t shared_validator{};

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_HPP
