/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/heterogeneous_property.hpp
*
* Defines classes to work with heterogeneous properties.
* Heterogeneous property corresponds to tuple's element or similar object field
* whose index is known at compilation time.
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY_HPP
#define DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/basic_property.hpp>
#include <dracosha/validator/utils/heterogeneous_size.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Base tag of heterogeneous properties.
 */
struct heterogeneous_property_tag{};

//-------------------------------------------------------------

namespace detail
{
struct heterogeneous_property_index_impl
{
    template <typename T>
    constexpr auto operator() (T&& prop) const
    {
        return typename std::decay_t<decltype(prop)>::index{};
    }
};
constexpr heterogeneous_property_index_impl heterogeneous_property_index{};
}

/**
 * @brief Helper for extracting integral constant index from heterogeneous property.
 *
 * Default implementation handles two kinds of such properties:
 * - if property is of heterogeneous_property_tag then index{} of that property is used;
 * - property is considered being integral constant itself and is used as is.
 *
 * For custom types custom specializations of this helper can be defined.
 */
template <typename PropertyT, typename Enable=hana::when<true>>
struct heterogeneous_property_index_t
{
    template <typename PropertyT1>
    constexpr auto operator () (PropertyT1&& prop) const -> decltype(auto)
    {
        return hana::if_(
            std::is_base_of<heterogeneous_property_tag,std::decay_t<PropertyT1>>{},
            detail::heterogeneous_property_index,
            hana::id
        )(std::forward<PropertyT1>(prop));
    }
};
/**
 * Instance of heterogeneous_property_index_t helper.
 */
template <typename PropertyT>
constexpr heterogeneous_property_index_t<PropertyT> heterogeneous_property_index_inst{};

/**
 * @brief Implementation of heterogeneous_property_index().
 */
struct heterogeneous_property_index_impl
{
    template <typename PropertyT>
    constexpr auto operator () (PropertyT&& prop) const -> decltype(auto)
    {
        return heterogeneous_property_index_inst<std::decay_t<PropertyT>>(std::forward<PropertyT>(prop));
    }
};
/**
 * @brief Extract integral constant index from heterogeneous property.
 * @param prop Heterogeneous property.
 * @return Integral constant index of the property.
 *
 * If property is of heterogeneous_property_tag then index{} of that property is used.
 * Otherwise, the property is considered being integral constant itself and is returned as is.
 * Besides, specialization of heterogeneous_property_index_t can be defined for a custom type to
 * extract heterogeneous index from that type.
 */
constexpr heterogeneous_property_index_impl heterogeneous_property_index{};

//-------------------------------------------------------------

/**
 * @brief Helper for getting heterogeneous property from an object.
 *
 * Default implementation uses hana::at() to get the value by heterogeneous_property_index(prop).
 * For custom types custom specializations of get_heterogeneous_property_t can be defined.
 */
template <typename T, typename PropertyT, typename Enable=hana::when<true>>
struct get_heterogeneous_property_t
{
    template <typename T1, typename PropertyT1>
    constexpr auto operator () (T1&& obj, PropertyT1&& prop) const -> decltype(auto)
    {
        return hana::at(obj,heterogeneous_property_index(prop));
    }
};
/**
 * Instance of get_heterogeneous_property_t helper.
 */
template <typename T, typename PropertyT>
constexpr get_heterogeneous_property_t<T,PropertyT> get_heterogeneous_property_inst{};

/**
 * @brief Implementation of get_heterogeneous_property().
 */
struct get_heterogeneous_property_impl
{
    template <typename T, typename PropertyT>
    constexpr auto operator () (T&& obj, PropertyT&& prop) const -> decltype(auto)
    {
        return get_heterogeneous_property_inst<std::decay_t<T>,std::decay_t<PropertyT>>(
                    std::forward<T>(obj),
                    std::forward<PropertyT>(prop)
                );
    }
};
/**
 * @brief Get heterogeneous property form an object.
 * @param obj Object, typically a kind of tuple.
 * @param prop Heterogeneous property or integral constant of size_t type.
 * @return Element of object corresponding to heterogeneous property.
 */
constexpr get_heterogeneous_property_impl get_heterogeneous_property{};

//-------------------------------------------------------------

/**
 * @brief Base struct for heterogeneous properties.
 *
 * Heterogeneous property usually correspond to a tuple's element.
 * Thus, the property just wraps an integral constant for the element's index.
 */
template <size_t Index>
struct heterogeneous_property_t : public heterogeneous_property_tag,
                                  public basic_property
{
    using index=hana::size_t<Index>;

    heterogeneous_property_t()=default;

    //! Constructor from any value used as a stub.
    template <typename T>
    heterogeneous_property_t(T&&)
    {}

    /**
     * @brief Get value of heterogeneous property from object.
     * @param v Object.
     * @result Object's element corresponding to this property.
     */
    template <typename T>
    constexpr static auto get(T&& v) -> decltype(auto)
    {
        return get_heterogeneous_property(std::forward<T>(v),index{});
    }

    /**
     * @brief Check if object has a property.
     * @return If property index is less than size of heterogeneous object then true, otherwise false.
     */
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

/**
 * @brief Base struct for heterogeneous properties with configuration holding property's strings
 *        such as name and flag descriptions.
 */
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

/**
 * @brief Config of heterogeneous property made of integral constant index.
 */
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
        return false;
    }
};

/**
 * @brief Base tag struct for heterogeneous properties made of integral constant indexes.
 */
struct heterogeneous_property_just_index_tag{};

/**
 * @brief Heterogeneous property made of integral constant index.
 */
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
        return DRACOSHA_VALIDATOR_NAMESPACE::make_property_validator(prop,std::forward<Args>(args)...); \
    }

#define DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY(prop,index) DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY_FLAG(prop,index,nullptr,nullptr)

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY_HPP
