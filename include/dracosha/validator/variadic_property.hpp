/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/variadic_property.hpp
*
* Defines helpers to work with properties with any number of arguments.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_HPP
#define DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/class_method_args.hpp>
#include <dracosha/validator/property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename FnT, typename AccumulatedArgsT, typename FnArgTypes>
struct variadic_property_closure
{
    template <typename T>
    constexpr static bool has_c()
    {
        auto current_type=hana::at(
                        FnArgTypes{},
                        hana::size(AccumulatedArgsT{})
                    );
        return std::is_convertible<T,typename decltype(current_type)::type>::value;
    }

    template <typename T>
    auto at(T&& arg,
            std::enable_if_t<variadic_property_closure::has_c<T>(), void*> =nullptr
        ) const -> decltype(auto)
    {
        return hana::if_(
            hana::equal(hana::plus(hana::size(_args),hana::size_c<1>),hana::size(_arg_types)),
            [](auto&& self, auto&& arg) -> decltype(auto)
            {
                return hana::unpack(self->_args,hana::reverse_partial(self->_fn,std::forward<decltype(arg)>(arg)));
            },
            [](auto&& self, auto&& arg)
            {
                auto next_args=hana::append(std::move(self->_args),std::forward<decltype(arg)>(arg));
                return variadic_property_closure<FnT,decltype(next_args),FnArgTypes>{
                    std::move(self->_fn),
                    std::move(next_args),
                    std::move(self->_arg_types)
                };
            }
        )(this,std::forward<T>(arg));
    }

    template <typename T>
    constexpr static bool has(
            T,
            std::enable_if_t<
                variadic_property_closure::has_c<T>()
            , void*> =nullptr
        )
    {
        return true;
    }

    FnT _fn;
    AccumulatedArgsT _args;
    FnArgTypes _arg_types;
};

#define DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_FLAG(prop,flag_dscr,n_flag_dscr) \
struct might_have_variadic_t_##prop \
{ \
    template <typename T> \
    constexpr auto operator() (T*) -> decltype(&T::prop) \
    { \
        return nullptr; \
    } \
}; \
constexpr might_have_variadic_t_##prop might_have_variadic_##prop{}; \
struct type_variadic_p_##prop \
{ \
    using hana_tag=DRACOSHA_VALIDATOR_NAMESPACE::property_tag; \
    \
    template <typename T> \
    static auto create_closure(T&& v) \
    { \
        auto apply_prop_to_v=[&v](auto&&... args) -> decltype(auto) \
        { \
            return v.prop(std::forward<decltype(args)>(args)...); \
        }; \
        auto init_args=hana::tuple<>{}; \
        auto arg_types=class_method_args<decltype(&std::decay_t<decltype(v)>::prop)>::types(); \
        return variadic_property_closure<decltype(apply_prop_to_v),decltype(init_args),decltype(arg_types)>{ \
                    std::move(apply_prop_to_v), \
                    std::move(init_args), \
                    std::move(arg_types) \
                }; \
    } \
    \
    template <typename T> \
    static auto get(T&& v) \
    { \
        return create_closure(std::forward<T>(v)); \
    } \
    \
    template <typename T> \
    constexpr static bool has() \
    { \
        return hana::sfinae(might_have_variadic_##prop)(std::add_pointer_t<T>(nullptr)) \
                != hana::nothing; \
    } \
    constexpr static const char* name() \
    {\
        return #prop; \
    }\
    constexpr static const char* flag_str(bool b) \
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
    constexpr bool operator == (const type_variadic_p_##prop&) const \
    { \
        return true; \
    } \
    \
    constexpr bool operator != (const type_variadic_p_##prop&) const \
    { \
        return false; \
    } \
    \
}; \
constexpr type_variadic_p_##prop prop{};

#define DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(prop) DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_FLAG(prop,nullptr,nullptr)

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VARIADIC_PROPERTY_HPP
