/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/dispatcher_impl.hpp
*
*  Defines implementation of validation dispatcher.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_DISPATCHER_IMPL_HPP
#define DRACOSHA_VALIDATOR_DISPATCHER_IMPL_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/check_member.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/master_sample.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct adapter_tag;
struct member_tag;

//-------------------------------------------------------------
namespace detail
{

/**
 * @brief Default dispatcher implementation.
 */
template <typename T1, typename =hana::when<true>>
struct dispatcher_impl_t
{
};

/**
 * @brief Implementation of dispatcher that first wraps object into default adapter and then forwards it to actual dispatcher.
 */
template <typename T1>
struct dispatcher_impl_t<T1,hana::when<!hana::is_a<adapter_tag,T1>>>
{
    /**
     *  @brief Perform validation of object.
     *  @param a Object to validate.
     *  @param args Validation arguments.
     *  @return Validation status.
     */
    template <typename ...Args>
    status operator() (T1&& obj, Args&&... args) const;

    /**
     *  @brief Perform validation of object's property at one level without member nesting.
     *  @param prop Property to validate.
     *  @param a Object to validate.
     *  @param args Validation arguments.
     *  @return Validation status.
     */
    template <typename ...Args>
    static status invoke(T1&& obj, Args&&... args);

    /**
     * @brief Execute validators on object and aggregate their results using logical AND.
     * @return Logical AND of results of intermediate validators.
     */
    template <typename ... Args>
    static status validate_and(T1&& obj, Args&&... args);

    /**
     * @brief Execute validators on object and aggregate their results using logical OR.
     * @return Logical OR of results of intermediate validators.
     */
    template <typename ... Args>
    static status validate_or(T1&& obj, Args&&... args);

    /**
     * @brief Execute validator on object and negate its result using logical NOT.
     * @return Logical NOT of result of intermediate validators.
     */
    template <typename ... Args>
    static status validate_not(T1&& obj, Args&&... args);
};

/**
 * @brief Implementation of dispatcher that dispatches validating requests to corresponding methods of supplied adapter.
 */
template <typename T1>
struct dispatcher_impl_t<T1,hana::when<hana::is_a<adapter_tag,T1>>>
{
    /**
     *  @brief Perform validation of object.
     *  @param a Adapter with object to validate.
     *  @param op Operator for validation.
     *  @param b Sample argument for validation.
     *  @return Validation status.
     */
    template <typename T2, typename OpT>
    status operator() (T1&& a, OpT&& op, T2&& b) const
    {
        return traits_of(a).validate_operator(a,std::forward<OpT>(op),unwrap_object(std::forward<T2>(b)));
    }

    /**
     *  @brief Perform validation of object's property at one level without member nesting.
     *  @param prop Property to validate.
     *  @param a Adapter with object to validate.
     *  @param op Operator for validation.
     *  @param b Sample argument for validation.
     *  @return Validation status.
     */
    template <typename T2, typename OpT, typename PropT>
    status operator() (PropT&& prop, T1&& a, OpT&& op, T2&& b) const
    {
        return invoke(std::forward<T1>(a),std::forward<PropT>(prop),std::forward<OpT>(op),unwrap_object(std::forward<T2>(b)));
    }

    /**
     *  @brief Perform validation of object's property at one level without member nesting.
     *  @param prop Property to validate.
     *  @param a Adapter with object to validate.
     *  @param op Operator for validation.
     *  @param b Sample argument for validation.
     *  @return Validation status.
     */
    template <typename T2, typename OpT, typename PropT>
    static status invoke(T1&& a, PropT&& prop, OpT&& op, T2&& b,
                                     std::enable_if_t<
                                       !hana::is_a<member_tag,unwrap_object_t<T2>>,
                                       void*
                                     > =nullptr
                                 )
    {
        return traits_of(a).validate_property(a,std::forward<PropT>(prop),std::forward<OpT>(op),unwrap_object(std::forward<T2>(b)));
    }

    /**
     *  @brief Validate existance of a member.
     *  @param a Adapter with object to validate.
     *  @param member Member descriptor.
     *  @param op Operator for validation.
     *  @param b Boolean flag, when true check if member exists, when false check if member does not exist.
     *  @return Validation status.
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    static status invoke(T1&& a, MemberT&& member, PropT&&, OpT&& op, T2&& b,
                                 std::enable_if_t<
                                    (std::is_same<exists_t,std::decay_t<OpT>>::value
                                    ||
                                    std::is_same<exists_op_with_string_t,std::decay_t<OpT>>::value
                                    ),
                                   void*
                                 > =nullptr
                                )
    {
        using type=typename std::decay_t<T1>::type;
        return hana::if_(
            typename type::filter_if_not_exists{},
            [](auto&& ...)
            {
                // existence is checked in invoke_member_if_exists() when filtering member
                return status::code::ignore;
            },
            [](auto&& a, auto&& member, auto&& op, auto&& b)
            {
                return traits_of(a).validate_exists(a,std::forward<decltype(member)>(member),std::forward<decltype(op)>(op),unwrap_object(std::forward<decltype(b)>(b)));
            }
        )(std::forward<T1>(a),std::forward<MemberT>(member),std::forward<OpT>(op),unwrap_object(std::forward<T2>(b)));
    }

    /**
     *  @brief Normal validation of a member.
     *  @param a Adapter with object to validate.
     *  @param member Member descriptor.
     *  @param prop Property to validate.
     *  @param op Operator for validation.
     *  @param b Sample argument for validation.
     *  @return Validation status.
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    static status invoke(T1&& a, MemberT&& member, PropT&& prop, OpT&& op, T2&& b,
                                 std::enable_if_t<
                                   (!hana::is_a<member_tag,unwrap_object_t<T2>> &&
                                    !is_master_sample<unwrap_object_t<T2>>::value &&
                                    !std::is_same<exists_t,std::decay_t<OpT>>::value &&
                                    !std::is_same<exists_op_with_string_t,std::decay_t<OpT>>::value
                                    ),
                                   void*
                                 > =nullptr
                                )
    {
        return traits_of(a).validate(a,std::forward<MemberT>(member),std::forward<PropT>(prop),std::forward<OpT>(op),unwrap_object(std::forward<T2>(b)));
    }

    /**
     *  @brief Validate using other member of the same object as a reference argument for validation.
     *  @param a Object to validate.
     *  @param member Member descriptor.
     *  @param prop Property to validate.
     *  @param op Operator for validation.
     *  @param b Descriptor of sample member of the same object.
     *  @return Validation status.
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    static status invoke(T1&& a, MemberT&& member, PropT&& prop, OpT&& op, T2&& b,
                                 std::enable_if_t<
                                   hana::is_a<member_tag,unwrap_object_t<T2>>,
                                   void*
                                 > =nullptr
                                )
    {
        return traits_of(a).validate_with_other_member(a,std::forward<MemberT>(member),std::forward<PropT>(prop),std::forward<OpT>(op),unwrap_object(std::forward<T2>(b)));
    }

    /**
     *  @brief Validate using the same member of a Sample object.
     *  @param a Object to validate.
     *  @param member Member descriptor.
     *  @param prop Property to validate.
     *  @param op Operator for validation.
     *  @param b Sample object whose member must be used as argument passed to validation operator.
     *  @return Validation status.
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    static status invoke(T1&& a, MemberT&& member, PropT&& prop, OpT&& op, T2&& b,
                                 std::enable_if_t<
                                   is_master_sample<unwrap_object_t<T2>>::value,
                                   void*
                                 > =nullptr
                                )
    {
        return traits_of(a).validate_with_master_sample(a,std::forward<MemberT>(member),std::forward<PropT>(prop),std::forward<OpT>(op),unwrap_object(std::forward<T2>(b)));
    }

    /**
     * @brief Execute validators on object and aggregate their results using logical AND.
     * @return Logical AND of results of intermediate validators.
     */
    template <typename ... Args>
    static status validate_and(T1&& a, Args&&... args)
    {
        return traits_of(a).validate_and(a,std::forward<Args>(args)...);
    }

    /**
     * @brief Execute validators on object and aggregate their results using logical OR.
     * @return Logical OR of results of intermediate validators.
     */
    template <typename ... Args>
    static status validate_or(T1&& a, Args&&... args)
    {
        return traits_of(a).validate_or(a,std::forward<Args>(args)...);
    }

    /**
     * @brief Execute validator on object and negate its result using logical NOT.
     * @return Logical NOT of result of intermediate validators.
     */
    template <typename ... Args>
    static status validate_not(T1&& a, Args&&... args)
    {
        return traits_of(a).validate_not(a,std::forward<Args>(args)...);
    }
};

/**
 * Instance of dispatcher implementation object.
 */
template <typename T1>
constexpr dispatcher_impl_t<T1> dispatcher_impl{};

template <typename T1>
template <typename ...Args>
status dispatcher_impl_t<T1,hana::when<!hana::is_a<adapter_tag,T1>>>::operator() (T1&& obj, Args&&... args) const
{
    using type=decltype(make_default_adapter(std::forward<T1>(obj)));
    return dispatcher_impl<type>(make_default_adapter(std::forward<T1>(obj)),std::forward<Args>(args)...);
}

template <typename T1>
template <typename ...Args>
status dispatcher_impl_t<T1,hana::when<!hana::is_a<adapter_tag,T1>>>::invoke(T1&& obj, Args&&... args)
{
    using type=decltype(make_default_adapter(std::forward<T1>(obj)));
    return dispatcher_impl<type>.invoke(make_default_adapter(std::forward<T1>(obj)),std::forward<Args>(args)...);
}

template <typename T1>
template <typename ...Args>
status dispatcher_impl_t<T1,hana::when<!hana::is_a<adapter_tag,T1>>>::validate_and(T1&& obj, Args&&... args)
{
    using type=decltype(make_default_adapter(std::forward<T1>(obj)));
    return dispatcher_impl<type>.validate_and(make_default_adapter(std::forward<T1>(obj)),std::forward<Args>(args)...);
}

template <typename T1>
template <typename ...Args>
status dispatcher_impl_t<T1,hana::when<!hana::is_a<adapter_tag,T1>>>::validate_or(T1&& obj, Args&&... args)
{
    using type=decltype(make_default_adapter(std::forward<T1>(obj)));
    return dispatcher_impl<type>.validate_or(make_default_adapter(std::forward<T1>(obj)),std::forward<Args>(args)...);
}

template <typename T1>
template <typename ...Args>
status dispatcher_impl_t<T1,hana::when<!hana::is_a<adapter_tag,T1>>>::validate_not(T1&& obj, Args&&... args)
{
    using type=decltype(make_default_adapter(std::forward<T1>(obj)));
    return dispatcher_impl<type>.validate_not(make_default_adapter(std::forward<T1>(obj)),std::forward<Args>(args)...);
}

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_DISPATCHER_IMPL_HPP
