/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/embedded_object.hpp
*
*  Defines helper to extract embedded object from adapter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_EMBEDDED_OBJECT_HPP
#define DRACOSHA_VALIDATOR_EMBEDDED_OBJECT_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/extract.hpp>
#include <dracosha/validator/get_member.hpp>
#include <dracosha/validator/operators/exists.hpp>
#include <dracosha/validator/adapters/impl/intermediate_adapter_traits.hpp>
#include <dracosha/validator/member_path.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Implementer of embedded_object().
 */
struct embedded_object_impl
{
    template <typename AdapterT>
    auto operator() (const AdapterT& adapter) const -> decltype(auto)
    {
        using type=typename AdapterT::type;
        return hana::eval_if(
            std::is_base_of<intermediate_adapter_tag,type>{},
            [&](auto&& _) -> decltype(auto)
            {
                return _(adapter).traits().value();
            },
            [&](auto&& _) -> decltype(auto)
            {
                return extract(traits_of(_(adapter)).get());
            }
        );
    }
};
/**
 * @brief Get reference to embedded object from validation adapter.
 * @param adapter Validation adapter.
 * @return Reference to object wrapped into adapter.
 *
 * In case of intermediate adapter the intermediate value of that adapter is returned.
 */
constexpr embedded_object_impl embedded_object;

//-------------------------------------------------------------

struct original_embedded_object_impl
{
    template <typename AdapterT>
    auto operator() (const AdapterT& adapter) const -> decltype(auto)
    {
        return extract(traits_of(adapter).get());
    }
};
/**
 * @brief Get reference to original object from validation adapter.
 * @param adapter Validation adapter.
 * @return Reference to object wrapped into adapter.
 *
 * In case of intermediate adapter the original value of the first adapter original adapter is returned.
 */
constexpr original_embedded_object_impl original_embedded_object;

//-------------------------------------------------------------

/**
 * @brief Implementer of embedded_object_path_suffix().
 */
struct embedded_object_path_suffix_impl
{
    template <typename AdapterT, typename PathT>
    auto operator() (const AdapterT& adapter, PathT&& path) const -> decltype(auto)
    {
        using type=typename AdapterT::type;
        return hana::eval_if(
            std::is_base_of<intermediate_adapter_tag,type>{},
            [&](auto&& _)
            {
                return _(adapter).traits().path(_(path));
            },
            [&](auto&& _) -> decltype(auto)
            {
                return hana::id(std::forward<PathT>(_(path)));
            }
        );
    }
};
/**
 *  @brief Get remaining member's path skipping already used part of path.
 *  @param adapter Validation adapter.
 *  @param path Full member's path.
 *  @return Remaining path excluding already used prefix.
 */
constexpr embedded_object_path_suffix_impl embedded_object_path_suffix{};

//-------------------------------------------------------------

/**
 * @brief Implementer of is_embedded_object_path_valid().
 */
struct is_embedded_object_path_valid_impl
{
    template <typename Tadapter, typename Tpath>
    constexpr auto operator () (Tadapter&& adapter, Tpath&& path) const
    {
        return hana::or_(
                            hana::is_empty(embedded_object_path_suffix(adapter,path)),
                            hana::not_(hana::is_nothing(member_value_type(embedded_object(adapter),embedded_object_path_suffix(adapter,path))))
                        );
    }
};
/**
 *  @brief Check member's path is valid for object wrapped in adapter.
 *  @param adapter Validation adapter.
 *  @param path Full member's path.
 *  @return Remaining path excluding already used prefix.
 */
constexpr is_embedded_object_path_valid_impl is_embedded_object_path_valid{};

//-------------------------------------------------------------

/**
 * @brief Check if path exists for object embedded into adapter.
 * @param adapter Validation adapter.
 * @param path Member's path.
 * @param b Boolean integral constant to check for existence/not-existence.
 * @return Boolean result of checking.
 */
struct embedded_object_has_path_impl
{
    template <typename AdapterT, typename PathT, typename T2>
    bool operator() (const AdapterT& adapter, PathT&& path, T2&& b) const
    {
        auto check_path_exists=[&b](auto&& obj, auto&& path)
        {
            return hana::if_(
                is_member_path_valid(obj,path),
                [&b](auto&& obj, auto&& path)
                {
                    return exists(obj,std::forward<decltype(path)>(path))==b;
                },
                [&b](auto&&, auto&&)
                {
                    return b==false;
                }
            )(std::forward<decltype(obj)>(obj),std::forward<decltype(path)>(path));
        };

        using type=typename AdapterT::type;
        return hana::eval_if(
            std::is_base_of<intermediate_adapter_tag,type>{},
            [&](auto&& _)
            {
                return check_path_exists(
                                            _(adapter).traits().value(),
                                            _(adapter).traits().path(_(path))
                                        );
            },
            [&](auto&& _)
            {
                return check_path_exists(
                            extract(traits_of(_(adapter)).get()),
                            _(path)
                        );
            }
        );
    }

    template <typename AdapterT, typename PathT, typename ForceOriginalT>
    bool invoke(const AdapterT& adapter, PathT&& path, ForceOriginalT) const
    {
        auto check_path_exists=[&adapter,this](auto&& obj, auto&& path)
        {
            if (!hana::value(is_member_path_valid(obj,path)))
            {
                return false;
            }
            if (traits_of(adapter).is_check_member_exists_before_validation())
            {
                return (*this)(adapter,path,true);
            }
            return true;
        };

        using type=typename AdapterT::type;
        return hana::eval_if(
            hana::and_(
                        std::is_base_of<intermediate_adapter_tag,type>{},
                        hana::not_(ForceOriginalT{})
                      ),
            [&](auto&& _)
            {
                return check_path_exists(
                                            _(adapter).traits().value(),
                                            _(adapter).traits().path(_(path))
                                        );
            },
            [&](auto&& _)
            {
                return check_path_exists(
                            extract(traits_of(_(adapter)).get()),
                            _(path)
                        );
            }
        );
    }

    template <typename AdapterT, typename PathT>
    bool operator() (const AdapterT& adapter, PathT&& path) const
    {
        return invoke(adapter,path,std::false_type{});
    }

};
/**
 * @brief Check if path exists for object embedded into adapter.
 * @param adapter Validation adapter.
 * @param path Member's path.
 * @param b Boolean true/false to check for existence/not-existence.
 * @return Boolean result of checking.
 */
constexpr embedded_object_has_path_impl embedded_object_has_path;

//-------------------------------------------------------------

/**
 * @brief Implementer of original_embedded_object_has_path().
 */
struct original_embedded_object_has_path_impl
{
    template <typename AdapterT, typename PathT>
    bool operator() (const AdapterT& adapter, const PathT& path) const
    {
        return embedded_object_has_path.invoke(adapter,path,std::true_type{});
    }
};
/**
 * @brief Check if path exists for original object wrapped into first adapter.
 * @param adapter Validation adapter.
 * @param path Member's path.
 * @return Boolean result of checking.
 */
constexpr original_embedded_object_has_path_impl original_embedded_object_has_path;

//-------------------------------------------------------------

/**
 * @brief Implementer of embedded_object_has_member().
 */
struct embedded_object_has_member_impl
{
    template <typename AdapterT, typename MemberT>
    bool operator() (const AdapterT& adapter, const MemberT& member) const
    {
        return embedded_object_has_path(adapter,member.path());
    }
};
/**
 * @brief Check if object embedded into adapter has a member.
 * @param adapter Validation adapter.
 * @param member Member.
 * @return Boolean result of checking.
 */
constexpr embedded_object_has_member_impl embedded_object_has_member;

//-------------------------------------------------------------

/**
 * @brief Implementer of original_embedded_object_has_member().
 */
struct original_embedded_object_has_member_impl
{
    template <typename AdapterT, typename MemberT>
    bool operator() (const AdapterT& adapter, const MemberT& member) const
    {
        return original_embedded_object_has_path(adapter,member.path());
    }
};
/**
 * @brief Check if original object wrapped into first adapter has a member.
 * @param adapter Validation adapter.
 * @param member Member.
 * @return Boolean result of checking.
 */
constexpr original_embedded_object_has_member_impl original_embedded_object_has_member;

//-------------------------------------------------------------

/**
 * @brief Implementer of embedded_object_member().
 */
struct embedded_object_member_impl
{
    template <typename AdapterT, typename MemberT>
    auto operator() (const AdapterT& adapter, const MemberT& member) const -> decltype(auto)
    {
        return get_member(embedded_object(adapter),embedded_object_path_suffix(adapter,path_of(member)));
    }
};
/**
 * @brief Get member's value of object embedded into adapter.
 * @param adapter Validation adapter.
 * @param member Member.
 * @return Member's value.
 */
constexpr embedded_object_member_impl embedded_object_member;

//-------------------------------------------------------------

/**
 * @brief Impolementer of original_embedded_object_member().
 */
struct original_embedded_object_member_impl
{
    template <typename AdapterT, typename MemberT>
    auto operator() (const AdapterT& adapter, const MemberT& member) const -> decltype(auto)
    {
        return get_member(original_embedded_object(adapter),member.path());
    }
};
/**
 * @brief Get member's value of original object wrapped into first adapter.
 * @param adapter Validation adapter.
 * @param member Member.
 * @return Member's value.
 */
constexpr original_embedded_object_member_impl original_embedded_object_member;

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_EMBEDDED_OBJECT_HPP
