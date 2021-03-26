/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/member.hpp
*
*  Defines generic descriptor of a member to be validated in objects.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_MEMBER_HPP
#define DRACOSHA_VALIDATOR_MEMBER_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/utils/hana_to_std_tuple.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>
#include <dracosha/validator/detail/member_helper.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct member_tag;

template <typename Ts>
auto make_member(Ts&& path);

template <typename Ts, typename T>
auto inherit_member(Ts&& path, T&& member);

//-------------------------------------------------------------

/**
 *  @brief Generic descriptor of a member to be validated.
 *
 * Member descriptor defines a path for extracting specific member/subobject from the object under validation.
 */
template <typename T, typename ...ParentPathT>
class member
{
    public:

        using hana_tag=member_tag;

        using type=T;
        using path_type=hana::tuple<ParentPathT...,type>;

        using is_aggregated=decltype(detail::is_member_aggregated<ParentPathT...,type>::value);
        using is_with_any=decltype(detail::is_member_with_any<ParentPathT...,type>::value);
        using is_key_any=std::is_same<unwrap_object_t<type>,std::decay_t<decltype(ANY)>>;
        using is_with_varg=decltype(detail::is_member_with_varg<ParentPathT...,type>::value);

        constexpr static bool has_any() noexcept
        {
            return is_with_any::value;
        }
        constexpr static bool key_is_any() noexcept
        {
            return is_key_any::value;
        }
        constexpr static bool has_varg() noexcept
        {
            return is_with_varg::value;
        }

        constexpr static const bool is_nested=sizeof...(ParentPathT)!=0;
        constexpr static const size_t path_size=sizeof...(ParentPathT)+1;

        template <template <typename ...> class T1,
                  typename T2, typename PathT>
        static auto create_derived(T2&& name, PathT&& path)
        {
            return T1<T2,T,ParentPathT...>(std::forward<PathT>(path),std::forward<T2>(name));
        }

        member()=default;

        /**
         * @brief Constructor of nested member.
         * @param key Key of current member.
         * @param parent_path Path to parent member which is of previous (upper) level.
         */
        template <typename T1, typename ParentPathTs>
        member(T1&& key, ParentPathTs&& parent_path,
               std::enable_if_t<!std::is_constructible<std::string,ParentPathTs>::value,void*> =nullptr)
             : _path(hana::append(std::forward<ParentPathTs>(parent_path),std::forward<T1>(key)))
        {}

        /**
         * @brief Constructor of nested member.
         * @param key Key of current member.
         * @param parent_path Path to parent member which is of previous (upper) level.
         */
        template <typename ParentPathTs>
        member(type&& key, ParentPathTs&& parent_path,
               std::enable_if_t<!std::is_constructible<std::string,ParentPathTs>::value,void*> =nullptr)
             : _path(hana::append(std::forward<ParentPathTs>(parent_path),std::move(key)))
        {}

        /**
         * @brief Ctor.
         * @param key Key of current member.
         */
        template <typename T1>
        member(T1&& key)
             : _path(hana::make_tuple(T(std::forward<T1>(key))))
        {}

        /**
         * @brief Constructor from path.
         * @param path Member's path.
         */
        member(path_type path)
             : _path(std::move(path))
        {}

        /**
         * @brief Constructor from key of string type.
         * @param str Key of current member.
         */
        member(std::string str)
             : _path(hana::make_tuple(std::move(str)))
        {}

        /**
         * @brief Check if path of this member is equal to path of other member.
         * @param other Other member.
         */
        template <typename T1>
        bool equals(const T1& other) const
        {
            return paths_equal(_path,other.path());
        }

        /**
         * @brief Callable operator in case member is rvalue.
         */
        template <typename ... Args>
        auto operator() (Args&&... args) &&
        {
            return detail::member_helper<Args...>(std::move(*this),std::forward<Args>(args)...);
        }

        /*
        * @brief Callable operator in case member is lvalue.
        */
       template <typename ... Args>
       auto operator() (Args&&... args) const &
       {
           return detail::member_helper<Args...>(*this,std::forward<Args>(args)...);
       }

        /**
         * @brief Create member of parent type.
         */
        auto parent() const
        {
            return inherit_member(parent_path(),*this);
        }

        /**
         * @brief Make super member prepending new key to the path.
         * @param key First key of super member.
         * @return Member of super type.
         */
        template <typename KeyT>
        auto make_super(KeyT&& first_key) const
        {
            using stype=typename adjust_storable_type<KeyT>::type;
            return member<T,stype,ParentPathT...>(hana::prepend(_path,stype(std::forward<KeyT>(first_key))));
        }

        /**
         * @brief Get the last key in the path corresponding to the member at current level.
         * @return Key of current member.
         */
        auto key() const -> decltype(auto)
        {
            return unwrap_object(hana::back(_path));
        }

        /**
          * @brief Get reference to the last path element holding the member's key.
          */
        auto last_path_item() const -> decltype(auto)
        {
            return hana::back(_path);
        }

        /**
         * @brief Get path depth of this member.
         * @return Number of elements in path.
         */
        constexpr static size_t path_depth()
        {
            return path_size;
        }

        /**
         * @brief Get path.
         * @return Path.
         */
        const auto& path() const
        {
            return _path;
        }

        /**
         * @brief Get parent path.
         * @return Parent path.
         */
        auto parent_path() const
        {
            return hana::drop_back(_path);
        }

        /**
         * @brief Append next level to member.
         * @param key Member key.
         */
        template <typename T1>
        constexpr auto operator [] (T1&& key) const
        {
            using type=typename adjust_storable_type<T1>::type;
            auto path_types=hana::transform(_path,hana::make_type);
            auto key_and_path_types=hana::prepend(path_types,hana::type_c<type>);
            auto next_member_tmpl=hana::unpack(key_and_path_types,hana::template_<member>);
            return typename decltype(next_member_tmpl)::type(type(std::forward<T1>(key)),_path);
        }

        /**
         * @brief member does not have explicit name.
         */
        constexpr static bool has_name = false;

    private:

        path_type _path;

        template <typename MemberT, typename T1>
        friend auto make_member_with_name(MemberT&& member, T1&& name);

        template <typename MemberT, typename T1>
        friend auto make_member_with_name_list(MemberT&& member, T1&& name);
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_MEMBER_HPP
