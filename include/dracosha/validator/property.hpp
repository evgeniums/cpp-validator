/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/property.hpp
*
* Defines classes to work with properties.
* Property can be either a class member method or a class member variable.
* Special case of a property is a "value" to use as a wrapper of the object itself.
* See list of predefined properties in properties/ folder.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_PROPERTY_HPP
#define DRACOSHA_VALIDATOR_PROPERTY_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/detail/has_method.hpp>
#include <dracosha/validator/detail/has_property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 *  Traits to check at compile time if object has a property
 */
template <typename T,typename PropT>
constexpr detail::has_property_t<T,PropT> has_property{};

/**
  @brief Check at compile time if object has a property
  @param a Object under test
  @param b Property which must be pre-declared with DRACOSHA_VALIDATOR_PROPERTY()
  @return Validation status
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto has_property_fn=[](auto a, auto b)
{
    return has_property<decltype(a),decltype(b)>();
};

/**
  @brief Extract object's property
  @param val Object
  @param prop Property which must be pre-declared with DRACOSHA_VALIDATOR_PROPERTY()
  @return Value of the property or object itself if no such property exists
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto property = [](auto&& val, auto&& prop) -> decltype(auto)
{
    return std::decay_t<decltype(prop)>::get(std::forward<decltype(val)>(val));
};

//-------------------------------------------------------------

#define DRACOSHA_VALIDATOR_HAS_PROPERTY_FN(val,prop) hana::is_valid([](auto&& v) -> decltype((void)v.prop()){})(val)
#define DRACOSHA_VALIDATOR_HAS_PROPERTY(val,prop) hana::is_valid([](auto&& v) -> decltype((void)v.prop){})(val)

/**
  Define property that can be used with validator
 */
#define DRACOSHA_VALIDATOR_PROPERTY(prop) \
    auto try_get_##prop =[](auto&& v) -> decltype(auto) \
    { \
      return hana::if_(DRACOSHA_VALIDATOR_HAS_PROPERTY_FN(v,prop), \
        [](auto&& x) -> decltype(auto) { return x.prop(); }, \
        [](auto&& vv) -> decltype(auto)  { \
                return hana::if_(DRACOSHA_VALIDATOR_HAS_PROPERTY(vv,prop), \
                  [](auto&& x) -> decltype(auto) { return x.prop; }, \
                  [](auto&& x) -> decltype(auto) { return hana::id(std::forward<decltype(x)>(x)); } \
                )(std::forward<decltype(vv)>(vv)); \
            } \
      )(std::forward<decltype(v)>(v)); \
    }; \
    BOOST_HANA_CONSTEXPR_LAMBDA auto has_fn_##prop = hana::is_valid([](auto v) -> decltype( \
                                                                        (void)hana::traits::declval(v).prop() \
                                                                    ) \
                                                                {}); \
    BOOST_HANA_CONSTEXPR_LAMBDA auto has_##prop = hana::is_valid([](auto v) -> decltype( \
                                                                        (void)hana::traits::declval(v).prop \
                                                                    ) \
                                                                {}); \
    struct type_p_##prop \
    { \
        using hana_tag=property_tag; \
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
            return hana::if_( \
                        has_fn_##prop(hana::template type_c<T>), \
                            true, \
                            hana::if_( \
                                has_##prop(hana::template type_c<T>), \
                                    true, \
                                    false \
                        )); \
        } \
        constexpr static const char* name() \
        {\
            return #prop; \
        }\
    }; \
    constexpr type_p_##prop prop{}; \
    template <typename ...Args> \
    constexpr auto type_p_##prop::operator () (Args&&... args) const -> decltype(auto)\
    { \
        return prepare_dispatcher(prop,std::forward<Args>(args)...); \
    }

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_PROPERTY_HPP
