/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/operators/flag.hpp
*
*  Defines flag pseudo operator for boolean operations
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FLAG_HPP
#define DRACOSHA_VALIDATOR_FLAG_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/operator.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/flag_presets.hpp>
#include <dracosha/validator/properties/value.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct flag_t
{
    using hana_tag=operator_tag;

    constexpr bool operator() (const bool& a, const bool& b) const
    {
        return a==b;
    }

    template <typename PropT>
    std::string str(PropT&& prop,
                    const bool& b,
                    std::enable_if_t<
                            (hana::is_a<property_tag,PropT> && !std::is_same<std::decay_t<PropT>,type_p_value>::value),
                            void*
                        > = nullptr) const
    {
        return std::decay_t<PropT>::has_flag_str()?prop.flag_str(b):default_flag_preset(b);
    }

    template <typename PropT>
    std::string str(PropT&&,
                    const bool& b,
                    std::enable_if_t<
                            (!hana::is_a<property_tag,PropT> || std::is_same<std::decay_t<PropT>,type_p_value>::value),
                            void*
                        > = nullptr) const
    {
        return default_flag_preset(b);
    }

    template <typename PropT>
    constexpr static bool prepend_property(PropT&&)
    {
        return !std::is_same<std::decay_t<PropT>,type_p_value>::value && !std::decay_t<PropT>::has_flag_str();
    }
};

template <typename T>
struct flag_op_with_preset : public flag_t
{
    public:

        template <typename T1>
        flag_op_with_preset(T1&& v) : _preset(std::forward<T1>(v))
        {}

        template <typename PropT>
        std::string str(PropT&&,const bool& b) const
        {
            return _preset(b);
        }

        template <typename PropT>
        constexpr static bool prepend_property(PropT&&)
        {
            return !std::is_same<std::decay_t<PropT>,type_p_value>::value;
        }

    private:

        T _preset;
};

struct flag_op_with_string : public flag_t
{
    public:

        template <typename T1>
        flag_op_with_string(T1&& v) : _string(std::forward<T1>(v))
        {}

        template <typename PropT>
        std::string str(PropT&&,const bool&) const
        {
            return _string;
        }

        template <typename PropT>
        constexpr static bool prepend_property(PropT&&)
        {
            return !std::is_same<std::decay_t<PropT>,type_p_value>::value;
        }

    private:

        std::string _string;
};

struct flag_op : public flag_t
{
    using flag_t::operator ();

    template <typename T>
    auto operator() (T&& v,
                     std::enable_if_t<
                        hana::is_a<flag_preset_tag,T>,
                        void*
                     > =nullptr) const
    {
        return flag_op_with_preset<T>{std::forward<T>(v)};
    }

    template <typename T>
    auto operator() (T&& v,
                     std::enable_if_t<
                        std::is_constructible<std::string,T>::value,
                        void*
                     > =nullptr) const
    {
        return flag_op_with_string{std::forward<T>(v)};
    }
};
constexpr flag_op flag{};

/*
  empty(flag,true) -> must be empty
  empty(flag,false) -> must be not empty
  empty(flag(flag_on_off),true) -> must be on
  empty(flag(flag_on_off),false) -> must be off
  empty(flag("custom description"),true) -> custom description
*/

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FLAG_HPP
