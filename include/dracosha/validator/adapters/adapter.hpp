/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/adapters/adapter.hpp
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
#include <dracosha/validator/check_member_path.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/apply.hpp>
#include <dracosha/validator/detail/adapter_helper.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------
struct member_tag;
struct adapter_tag;

/**
 * @brief Mode of processing of not found members
 */
enum class if_member_not_found : int
{
    ignore, //!< Ignore not found members
    abort //!< Abort validating with false report if member is not found
};

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

    const T& _obj;
    if_member_not_found _unknown_member_mode;

    /**
     * @brief Constructor
     * @param a Object to wrap into adapter
     */
    adapter(const T& a):_obj(a),_unknown_member_mode(if_member_not_found::ignore)
    {}

    /**
     * @brief Set mode to use if member is not found
     * @param mode Mode
     */
    void set_unknown_member_mode(if_member_not_found mode) noexcept
    {
        _unknown_member_mode=mode;
    }
    /**
     * @brief Get mode used if a member is not found
     * @return Mode to use if a member not found
     */
    if_member_not_found unknown_member_mode() const noexcept
    {
        return _unknown_member_mode;
    }

    /**
     * @brief Get reference to wrapped object
     * @return Wrapped object under validation
     */
    const T& object() const noexcept
    {
        return _obj;
    }

    /**
     *  @brief Perform validation of embedded object at one level without member nesting
     *  @param op Operator for validation
     *  @param b Sample argument for validation
     *  @return Validation status
     */
    template <typename T2, typename OpT>
    bool validate_operator(OpT&& op, T2&& b) const
    {
        return op(
                    extract(_obj),
                    extract(std::forward<T2>(b))
                );
    }

    /**
     *  @brief Perform validation of embedded object's property at one level without member nesting
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Sample argument for validation
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT>
    bool validate_property(PropT&& prop, OpT&& op, T2&& b) const
    {
        return op(
                    property(extract(_obj),std::forward<PropT>(prop)),
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
        return hana::if_(check_member_path(_obj,member.path),
            [this,&b](auto&& path)
            {
                auto&& ax=extract(_obj);
                return exists(ax,std::forward<decltype(path)>(path))==b;
            },
            [&b](auto&&)
            {
                return b==false;
            }
        )(member.path);
    }

    /**
     *  @brief Normal validation of a member
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Sample argument for validation
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    bool validate(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
    {
        return hana::if_(check_member_path(_obj,member.path),
            [this,&prop,&op,&b](auto&& path)
            {
                auto&& ax=extract(_obj);
                return op(
                            property(get_member(ax,std::forward<decltype(path)>(path)),std::forward<PropT>(prop)),
                            extract(std::forward<T2>(b))
                        );
            },
            [this](auto&&)
            {
                return _unknown_member_mode==if_member_not_found::ignore;
            }
        )(member.path);
    }

    /**
     *  @brief Validate using other member of the same embedded object as a reference argument for validation
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Descriptor of sample member of the same embedded object
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    bool validate_with_other_member(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
    {
        auto&& ax=extract(_obj);
        return op(
                    property(get_member(ax,member.path),prop),
                    property(get_member(ax,b.path),prop)
                );
    }

    /**
     *  @brief Validate using the same member of a Sample embedded object
     *  @param member Member descriptor
     *  @param prop Property to validate
     *  @param op Operator for validation
     *  @param b Sample embedded object whose member to use as argument passed to validation operator
     *  @return Validation status
     */
    template <typename T2, typename OpT, typename PropT, typename MemberT>
    bool validate_with_master_sample(MemberT&& member, PropT&& prop, OpT&& op, T2&& b) const
    {
        auto&& ax=extract(_obj);
        return op(
                    property(get_member(ax,member.path),prop),
                    property(get_member(b(),member.path),prop)
                );
    }

    /**
     * @brief Execute validators on given object and aggregate their results using logical AND
     * @param obj Object to validate
     * @param ops List of intermediate validators or validation operators
     * @return Logical AND of results of intermediate validators
     */
    template <typename ObjT, typename OpsT>
    static bool validate_and(ObjT&& obj, OpsT&& ops,
                             std::enable_if_t<!hana::is_a<member_tag,ObjT>,void*> =nullptr)
    {
        return hana::fold(std::forward<decltype(ops)>(ops),true,
                    [&obj](bool prevResult, auto&& op)
                    {
                        if (!prevResult)
                        {
                            return false;
                        }
                        return apply(std::forward<ObjT>(obj),std::forward<decltype(op)>(op));
                    }
                );
    }

    /**
     * @brief Execute validators on embedded object and aggregate their results using logical AND
     * @param ops List of intermediate validators or validation operators
     * @return Logical AND of results of intermediate validators
     */
    template <typename OpsT>
    bool validate_and(OpsT&& ops) const
    {
        return validate_and(_obj,std::forward<OpsT>(ops));
    }

    /**
     * @brief Execute validators on the member of a given object and aggregate their results using logical AND
     * @param obj Object to validate
     * @param member Member to process with validators
     * @param ops List of intermediate validators or validation operators
     * @return Logical AND of results of intermediate validators
     */
    template <typename ObjT, typename MemberT, typename OpsT>
    bool validate_and(ObjT&& obj, MemberT&& member, OpsT&& ops) const
    {
        return hana::if_(check_member_path(detail::adapter_helper<ObjT>(obj),member.path),
            [&obj,&member,&ops](auto&&)
            {
                return hana::fold(std::forward<decltype(ops)>(ops),true,
                            [&member,&obj](bool prevResult, auto&& op)
                            {
                                if (!prevResult)
                                {
                                    return false;
                                }
                                return apply_member(std::forward<ObjT>(obj),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member));
                            }
                        );
            },
            [this](auto&&)
            {
                return _unknown_member_mode==if_member_not_found::ignore;
            }
        )(member.path);
    }

    /**
     * @brief Execute validators on the member of embedded object and aggregate their results using logical AND
     * @param member Member to process with validators
     * @param ops List of intermediate validators or validation operators
     * @return Logical AND of results of intermediate validators
     */
    template <typename MemberT, typename OpsT>
    bool validate_and(MemberT&& member, OpsT&& ops,
                      std::enable_if_t<hana::is_a<member_tag,MemberT>,void*> =nullptr) const
    {
        return validate_and(_obj,std::forward<MemberT>(member),std::forward<OpsT>(ops));
    }

    template <typename ObjT, typename OpsT>
    static bool validate_or(ObjT&& obj, OpsT&& ops,
                     std::enable_if_t<!hana::is_a<member_tag,ObjT>,void*> =nullptr)
    {
        return hana::value(hana::length(ops))==0
                ||
               hana::fold(std::forward<decltype(ops)>(ops),false,
                    [&obj](bool prevResult, auto&& op)
                    {
                        if (prevResult)
                        {
                            return true;
                        }
                        return apply(std::forward<ObjT>(obj),std::forward<decltype(op)>(op));
                    }
                );
    }

    /**
     * @brief Execute validators on embedded object and aggregate their results using logical OR
     * @param ops List of intermediate validators or validation operators
     * @return Logical OR of results of intermediate validators
     */
    template <typename OpsT>
    bool validate_or(OpsT&& ops) const
    {
        return validate_or(_obj,std::forward<OpsT>(ops));
    }

    /**
     * @brief Execute validators on the member of a given object and aggregate their results using logical OR
     * @param obj Object to validate
     * @param member Member to process with validators
     * @param ops List of intermediate validators or validation operators
     * @return Logical OR of results of intermediate validators
     */
    template <typename ObjT, typename MemberT, typename OpsT>
    bool validate_or(ObjT&& obj, MemberT&& member, OpsT&& ops) const
    {
        return hana::if_(check_member_path(detail::adapter_helper<ObjT>(obj),member.path),
            [&obj,&member,&ops](auto&&)
            {
                return hana::value(hana::length(ops))==0
                        ||
                       hana::fold(std::forward<decltype(ops)>(ops),false,
                            [&obj,&member](bool prevResult, auto&& op)
                            {
                                if (prevResult)
                                {
                                    return true;
                                }
                                return apply_member(std::forward<ObjT>(obj),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member));
                            }
                        );
            },
            [this](auto&&)
            {
                return _unknown_member_mode==if_member_not_found::ignore;
            }
        )(member.path);
    }

    /**
     * @brief Execute validators on the member of embedded object and aggregate their results using logical OR
     * @param member Member to process with validators
     * @param ops List of intermediate validators or validation operators
     * @return Logical OR of results of intermediate validators
     */
    template <typename MemberT, typename OpsT>
    bool validate_or(MemberT&& member, OpsT&& ops,
                     std::enable_if_t<hana::is_a<member_tag,MemberT>,void*> =nullptr) const
    {
        return validate_or(_obj,std::forward<decltype(member)>(member),std::forward<decltype(ops)>(ops));
    }

    /**
     * @brief Execute validator on given object and negate the result
     * @param op Intermediate validator or validation operator
     * @return Logical NOT of results of intermediate validator
     */
    template <typename ObjT, typename OpT>
    static bool validate_not(ObjT&& obj, OpT&& op,
                             std::enable_if_t<!hana::is_a<member_tag,ObjT>,void*> =nullptr)
    {
        return !apply(std::forward<ObjT>(obj),std::forward<decltype(op)>(op));
    }

    /**
     * @brief Execute validator on embedded object and negate the result
     * @param op Intermediate validator or validation operator
     * @return Logical NOT of results of intermediate validator
     */
    template <typename OpT>
    bool validate_not(OpT&& op) const
    {
        return validate_not(_obj,std::forward<decltype(op)>(op));
    }

    /**
     * @brief Execute validator on the member of a given object and negate the result
     * @param obj Object to validate
     * @param member Member to process with validator
     * @param op Intermediate validator or validation operator
     * @return Logical NOT of results of intermediate validator
     */
    template <typename ObjT, typename MemberT, typename OpT>
    bool validate_not(ObjT&& obj, MemberT&& member, OpT&& op) const
    {
        return hana::if_(check_member_path(detail::adapter_helper<ObjT>(obj),member.path),
            [&obj,&member,&op](auto&&)
            {
                return !apply_member(std::forward<ObjT>(obj),std::forward<decltype(op)>(op),std::forward<decltype(member)>(member));
            },
            [this](auto&&)
            {
                return _unknown_member_mode==if_member_not_found::ignore;
            }
        )(member.path);
    }

    /**
     * @brief Execute validator on the member of embedded object and negate the result
     * @param member Member to process with validator
     * @param op Intermediate validator or validation operator
     * @return Logical NOT of results of intermediate validator
     */
    template <typename MemberT, typename OpT>
    bool validate_not(MemberT&& member, OpT&& op,
                      std::enable_if_t<hana::is_a<member_tag,MemberT>,void*> =nullptr) const
    {
        return validate_not(_obj,std::forward<decltype(member)>(member),std::forward<decltype(op)>(op));
    }
};

/**
  @brief Make default validation adapter wrapping the embedded object
  @param v Object to wrap into adapter
  @return Validation adapter
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto make_adapter = [](auto&& v)
{
    return adapter<std::decay_t<decltype(v)>>(std::forward<decltype(v)>(v));
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_ADAPTER_HPP
