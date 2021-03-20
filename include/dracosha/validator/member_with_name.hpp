/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/member_with_name.hpp
*
*  Defines member_with_name.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_WITH_NAME_HPP
#define DRACOSHA_VALIDATOR_MEMBER_WITH_NAME_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/member.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct member_with_name_tag{};

/**
 * @brief Member with excplicit name.
 */
template <typename T1, typename T2, typename ...ParentPathT>
struct member_with_name : public member<T2,ParentPathT...>,
                          public member_with_name_tag
{
    using base_type=member<T2,ParentPathT...>;

    /**
     * @brief Constructor with name.
     * @param path Member's path.
     * @name name member's name.
     */
    template <typename Tt1>
    member_with_name(typename base_type::path_type path,
           Tt1&& name)
         : base_type(std::move(path)),
           _name(std::forward<Tt1>(name))
    {}

    /**
     * @brief Get member's name.
     * @return Name.
     */
    auto name() const -> decltype(auto)
    {
        return _name;
    }

    /**
     * @brief member_with_name has explicit name.
     */
    constexpr static bool has_name = true;

    /**
     * @brief Bind compound validator to current member.
     * @param v Prepared partial validator.
     * @return Prepared partial validator bound to current member.
     */
    template <typename Tt1>
    constexpr auto operator () (Tt1&& v) const -> decltype(auto)
    {
        return make_member_validator(*this,std::forward<Tt1>(v));
    }

    /**
     * @brief Bind plain operator to current member.
     * @param op Operator.
     * @param b Argument to forward to operator.
     * @return Prepared partial validator of "value" property bound to current member.
     */
    template <typename OpT, typename Tt1>
    constexpr auto operator () (OpT&& op, Tt1&& b) const -> decltype(auto)
    {
        return (*this)(value(std::forward<OpT>(op),std::forward<Tt1>(b)));
    }

    T1 _name;
};

/**
 * @brief Create a member with name from basic member.
 * @param member Basic member.
 * @param name Member name.
 * @return Basic name wrapped with name into member with name.
 */
template <typename MemberT, typename T>
auto make_member_with_name(MemberT&& member, T&& name)
{
    return member.template create_derived<member_with_name>(std::forward<T>(name),std::move(member._path));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_WITH_NAME_HPP
