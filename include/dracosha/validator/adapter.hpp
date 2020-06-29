/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/adapter.hpp
*
*  Defines default adapter class.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_ADAPTER_HPP
#define DRACOSHA_VALIDATOR_ADAPTER_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/check_member.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/operators/exists.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct adapter_tag;

/**
 * @brief Default adapter that performs validation using predefined validation operators "as is".
 *
 * Custom adapters must satisfy concept of this adapter, i.e. a custom adapter must have hana_tag=adapter_tag and
 * methods with the same generic signatures.
 */
template <typename T>
struct adapter
{
    using hana_tag=adapter_tag;
    const T& obj;

    adapter(const T& a):obj(a)
    {}

    /**
     *  @brief Perform validation of object at one level without member nesting
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     *  @return Validation status
     */
    template <typename T2, typename OpT>
    bool validate_operator(OpT&& op, T2&& b) const
    {
        return op(
                    extract(obj),
                    extract(std::forward<T2>(b))
                );
    }

    /**
     *  @brief Perform validation of object's property at one level without member nesting
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT>
    bool validate_property(PropT&& prop, OpT&& op, T2&& b) const
    {
        return op(
                    property(extract(obj),std::forward<PropT>(prop)),
                    extract(std::forward<T2>(b))
                );
    }

    /**
     *  @brief Validate existance of a member
     *  @param a Object to validate
     *  @param member Member descriptor
     *  @param b Boolean flag, when true check if member exists, when false check if member does not exist
     *  @return Validation status
     */
    template <typename T2, typename MemberT>
    bool validate_exists(MemberT&& member, T2&& b) const
    {
        return hana::if_(check_member_path(obj,member.path),
            [&b](auto&&)
            {
                return b==false;
            },
            [this,&b](auto&& path)
            {
                auto&& ax=extract(obj);
                return exists(ax,std::forward<decltype(path)>(path))==b;
            }
        )(member.path);
    }

    /**
     *  @brief Normal validation of a member
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Reference argument for validation
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    bool validate(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
    {
        auto&& ax=extract(obj);
        return op(
                    property(get_member(ax,member.path),std::forward<PropT>(prop)),
                    extract(std::forward<T2>(b))
                );
    }

    /**
     *  @brief Validate using other member of the same object as a reference argument for validation
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Descriptor of reference member of the same object
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    bool validate_with_other_member(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
    {
        auto&& ax=extract(obj);
        return op(
                    property(get_member(ax,member.path),prop),
                    property(get_member(ax,b.path),prop)
                );
    }

    /**
     *  @brief Validate using the same member of a reference object
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Reference object whose member to use as argument passed to validation operator
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    bool validate_with_master_reference(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
    {
        auto&& ax=extract(obj);
        return op(
                    property(get_member(ax,member.path),prop),
                    property(get_member(b(),member.path),prop)
                );
    }

    //! \todo validate_and
    //! \todo validate_or
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADAPTER_HPP
