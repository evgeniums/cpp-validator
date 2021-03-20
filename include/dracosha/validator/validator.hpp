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
#include <dracosha/validator/check_contains.hpp>
#include <dracosha/validator/check_exists.hpp>
#include <dracosha/validator/check_member.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/operators/comparison.hpp>
#include <dracosha/validator/lazy.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/dispatcher.hpp>
#include <dracosha/validator/adapter.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>
#include <dracosha/validator/property_validator.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/make_validator.hpp>
#include <dracosha/validator/member.hpp>
#include <dracosha/validator/master_sample.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/operators.hpp>
#include <dracosha/validator/operand.hpp>
#include <dracosha/validator/operators/invert_op.hpp>
#include <dracosha/validator/operators/wrap_op.hpp>
#include <dracosha/validator/member_with_name.hpp>
#include <dracosha/validator/member_with_name_list.hpp>
#include <dracosha/validator/make_member.hpp>

#include <dracosha/validator/aggregation/aggregation.ipp>
#include <dracosha/validator/aggregation/element_aggregation.ipp>
#include <dracosha/validator/detail/member_helper.ipp>
#include <dracosha/validator/prepend_super_member.ipp>
#include <dracosha/validator/apply_generated_paths.ipp>
#include <dracosha/validator/apply_member_path.ipp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

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

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HPP
