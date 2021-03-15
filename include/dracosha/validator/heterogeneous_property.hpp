/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/heterogeneous_property.hpp
*
* Defines classes to work with heterogeneous properties.
* Heterogeneous property corrspond to tuple's element or similar object field
* whose index is known at compilation time.
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY_HPP
#define DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/heterogeneous_size.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct property_tag;
struct heterogeneous_property_tag{};

//-------------------------------------------------------------

template <typename PropertyT, typename Enable=hana::when<true>>
struct heterogeneous_property_index_t
{
    template <typename PropertyT1>
    auto operator () (PropertyT1&& prop) const -> decltype(auto)
    {
        return hana::if_(
            std::is_base_of<heterogeneous_property_tag,std::decay_t<PropertyT1>>{},
            [](auto&& prop)
            {
                return typename std::decay_t<decltype(prop)>::index{};
            },
            [](auto&& prop) -> decltype(auto)
            {
                return hana::id(std::forward<decltype(prop)>(prop));
            }
        )(std::forward<PropertyT1>(prop));
    }
};
template <typename PropertyT>
constexpr heterogeneous_property_index_t<PropertyT> heterogeneous_property_index_inst{};

struct heterogeneous_property_index_impl
{
    template <typename PropertyT>
    auto operator () (PropertyT&& prop) const -> decltype(auto)
    {
        return heterogeneous_property_index_inst<std::decay_t<PropertyT>>(std::forward<PropertyT>(prop));
    }
};
constexpr heterogeneous_property_index_impl heterogeneous_property_index{};

//-------------------------------------------------------------

template <typename T, typename PropertyT, typename Enable=hana::when<true>>
struct get_heterogeneous_property_t
{
    template <typename T1, typename PropertyT1>
    auto operator () (T1&& obj, PropertyT1&& prop) const -> decltype(auto)
    {
        return hana::at(obj,heterogeneous_property_index(prop));
    }
};
template <typename T, typename PropertyT>
constexpr get_heterogeneous_property_t<T,PropertyT> get_heterogeneous_property_inst{};

struct get_heterogeneous_property_impl
{
    template <typename T, typename PropertyT>
    auto operator () (T&& obj, PropertyT&& prop) const -> decltype(auto)
    {
        return get_heterogeneous_property_inst<std::decay_t<T>,std::decay_t<PropertyT>>(
                    std::forward<T>(obj),
                    std::forward<PropertyT>(prop)
                );
    }
};
constexpr get_heterogeneous_property_impl get_heterogeneous_property{};

//-------------------------------------------------------------

template <size_t Index>
struct heterogeneous_property_t : public heterogeneous_property_tag
{
    using hana_tag=property_tag;
    using index=hana::size_t<Index>;

    heterogeneous_property_t()=default;

    // Construct from any value
    template <typename T>
    heterogeneous_property_t(T&&)
    {}

    template <typename T>
    constexpr static auto get(T&& v) -> decltype(auto)
    {
        return get_heterogeneous_property(std::forward<T>(v),index{});
    }

    template <typename T>
    constexpr static bool has()
    {
        return hana::less(
                    index{},
                    heterogeneous_size_t<T>{}
                 );
    }

    template <typename T> constexpr bool operator == (T) const
    {
        return false;
    }

    template <typename T> constexpr bool operator != (T) const
    {
        return true;
    }

    constexpr bool operator == (const heterogeneous_property_t<Index>&) const \
    {
        return true;
    }

    constexpr bool operator != (const heterogeneous_property_t<Index>&) const \
    {
        return false;
    }
};

//-------------------------------------------------------------

template <size_t Index, typename ConfigT>
struct heterogeneous_property_with_config_t : public heterogeneous_property_t<Index>
{
    using heterogeneous_property_t<Index>::heterogeneous_property_t;

    constexpr static auto name()
    {
        return ConfigT::name();
    }

    template <typename FormatterT>
    constexpr static const char* flag_str(bool b, const FormatterT&, bool =false)
    {
        if (b)
        {
            return ConfigT::flag();
        }
        return ConfigT::n_flag();
    }

    constexpr static bool has_flag_str()
    {
        return ConfigT::flag()!=nullptr && ConfigT::n_flag()!=nullptr;
    }
};

//-------------------------------------------------------------

template <size_t Index>
struct heterogeneous_property_const_config_t
{
    constexpr static size_t name()
    {
        return Index;
    }

    template <typename FormatterT>
    constexpr static const char* flag_str(bool, const FormatterT&, bool =false)
    {
        return nullptr;
    }

    constexpr static bool has_flag_str()
    {
        return nullptr;
    }
};

struct heterogeneous_property_just_index_tag{};

template <size_t Index>
struct heterogeneous_property_just_index_t : public heterogeneous_property_with_config_t<Index,heterogeneous_property_const_config_t<Index>>,
                                             public heterogeneous_property_just_index_tag
{
    using heterogeneous_property_with_config_t<Index,heterogeneous_property_const_config_t<Index>>::heterogeneous_property_with_config_t;
};

//-------------------------------------------------------------

#define DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY_FLAG(prop,index,flag_dscr,n_flag_dscr) \
    struct heterogeneous_p_conf_##prop \
    { \
        constexpr static const char* name() \
        { \
            return #prop; \
        } \
        constexpr static const char* flag() \
        { \
            return flag_dscr; \
        } \
        constexpr static const char* n_flag() \
        { \
            return n_flag_dscr; \
        } \
    }; \
    struct type_p_##prop : public DRACOSHA_VALIDATOR_NAMESPACE::heterogeneous_property_with_config_t<index,heterogeneous_p_conf_##prop> \
    { \
        using DRACOSHA_VALIDATOR_NAMESPACE::heterogeneous_property_with_config_t<index,heterogeneous_p_conf_##prop> \
                ::heterogeneous_property_with_config_t; \
       \
        template <typename ... Args> \
        constexpr auto operator () (Args&&... args) const -> decltype(auto); \
    }; \
    constexpr type_p_##prop prop{}; \
    template <typename ...Args> \
    constexpr auto type_p_##prop::operator () (Args&&... args) const -> decltype(auto) \
    { \
        return make_property_validator(prop,std::forward<Args>(args)...); \
    }

#define DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY(prop,index) DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY_FLAG(prop,index,nullptr,nullptr)

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY_HPP
