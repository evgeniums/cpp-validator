/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/property.hpp
*
* Defines classes to work with properties.
* Property can be either a class member method or a class member variable.
* Special case of a property is a "value" to use as a wrapper of the object itself.
* See list of predefined properties in properties/ folder.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_PROPERTY_HPP
#define HATN_VALIDATOR_PROPERTY_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/utils/adjust_storable_ignore.hpp>
#include <hatn/validator/detail/has_method.hpp>
#include <hatn/validator/detail/has_property.hpp>
#include <hatn/validator/basic_property.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

/**
 *  Helper to check at compile time if object has a property.
 */
template <typename T,typename PropT>
constexpr detail::has_property_t<T,PropT> has_property{};

/**
  @brief Check at compile time if object has a property.
  @param a Object under test.
  @param b Property which must be pre-declared with HATN_VALIDATOR_PROPERTY().
  @return Validation status.
*/
template <typename Ta, typename Tb>
constexpr auto has_property_fn(Ta&& a, Tb&& b)
{
    return has_property<decltype(a),decltype(b)>();
}

/**
  @brief Extract object's property.
  @param val Object.
  @param prop Property which must be pre-declared with HATN_VALIDATOR_PROPERTY().
  @return Value of the property or object itself if no such property exists.
*/
template <typename Tv, typename Tp>
auto property(Tv&& val, Tp&& prop) -> decltype(auto)
{
    return prop.get(std::forward<decltype(val)>(val));
}

//-------------------------------------------------------------

#define HATN_VALIDATOR_HAS_PROPERTY_FN(val,prop) HATN_VALIDATOR_NAMESPACE::hana::is_valid([](auto&& v) -> decltype((void)v.prop()){})(val)
#define HATN_VALIDATOR_HAS_PROPERTY(val,prop) HATN_VALIDATOR_NAMESPACE::hana::is_valid([](auto&& v) -> decltype((void)v.prop){})(val)

/**
  @brief Define property that can be used with validator.
  @param prop Property name.
  @param flag_dscr Description of positive flag.
  @param flag_dscr Description of negative flag.
 */
#define HATN_VALIDATOR_PROPERTY_FLAG(prop,flag_dscr,n_flag_dscr) \
    template <typename Tv> \
    auto try_get_##prop (Tv&& v) -> decltype(auto) \
    { \
      return HATN_VALIDATOR_NAMESPACE::hana::if_(HATN_VALIDATOR_HAS_PROPERTY_FN(v,prop), \
        [](auto&& x) -> decltype(auto) { return x.prop(); }, \
        [](auto&& vv) -> decltype(auto)  { \
                return HATN_VALIDATOR_NAMESPACE::hana::if_(HATN_VALIDATOR_HAS_PROPERTY(vv,prop), \
                  [](auto&& x) -> decltype(auto) { return x.prop; }, \
                  [](auto&& x) -> decltype(auto) { return HATN_VALIDATOR_NAMESPACE::hana::id(std::forward<decltype(x)>(x)); } \
                )(std::forward<decltype(vv)>(vv)); \
            } \
      )(std::forward<decltype(v)>(v)); \
    }; \
    HATN_VALIDATOR_INLINE_LAMBDA auto has_fn_##prop = HATN_VALIDATOR_NAMESPACE::hana::is_valid([](auto v) -> decltype( \
                                                                        (void)HATN_VALIDATOR_NAMESPACE::hana::traits::declval(v).prop() \
                                                                    ) \
                                                                {}); \
    HATN_VALIDATOR_INLINE_LAMBDA auto has_##prop = HATN_VALIDATOR_NAMESPACE::hana::is_valid([](auto v) -> decltype( \
                                                                        (void)HATN_VALIDATOR_NAMESPACE::hana::traits::declval(v).prop \
                                                                    ) \
                                                                {}); \
    struct type_p_##prop : public HATN_VALIDATOR_NAMESPACE::basic_property \
    { \
        template <typename T> \
        constexpr static auto get(T&& v) -> decltype(auto) \
        { \
            return try_get_##prop(std::forward<T>(v)); \
        } \
        template <typename ...Args> \
        constexpr auto operator () (Args&&... args) const -> decltype(auto); \
        template <typename T> \
        constexpr static bool has() \
        { \
            return HATN_VALIDATOR_NAMESPACE::hana::if_( \
                        has_fn_##prop(HATN_VALIDATOR_NAMESPACE::hana::template type_c<T>), \
                            true, \
                            HATN_VALIDATOR_NAMESPACE::hana::if_( \
                                has_##prop(HATN_VALIDATOR_NAMESPACE::hana::template type_c<T>), \
                                    true, \
                                    false \
                        )); \
        } \
        constexpr static const char* name() \
        {\
            return #prop; \
        }\
        template <typename FormatterT> \
        constexpr static const char* flag_str(bool b, const FormatterT&, bool =false) \
        {\
            if (b) \
            { \
                return flag_dscr; \
            } \
            return n_flag_dscr; \
        }\
        constexpr static bool has_flag_str() \
        { \
            return flag_dscr!=nullptr && n_flag_dscr!=nullptr; \
        } \
        template <typename T> constexpr bool operator == (T) const \
        { \
            return false; \
        } \
        template <typename T> constexpr bool operator != (T) const \
        { \
            return true; \
        } \
        constexpr bool operator == (const type_p_##prop&) const \
        { \
            return true; \
        } \
        \
        constexpr bool operator != (const type_p_##prop&) const \
        { \
            return false; \
        } \
        \
    }; \
    constexpr type_p_##prop prop{}; \
    template <typename ...Args> \
    constexpr auto type_p_##prop::operator () (Args&&... args) const -> decltype(auto)\
    { \
        return make_property_validator(prop,std::forward<Args>(args)...); \
    }

#define HATN_VALIDATOR_PROPERTY(prop) HATN_VALIDATOR_PROPERTY_FLAG(prop,nullptr,nullptr)

#define HATN_VALIDATOR_PROPERTY_TYPE(prop) decltype(HATN_VALIDATOR_NAMESPACE::_[prop])

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_PROPERTY_HPP
