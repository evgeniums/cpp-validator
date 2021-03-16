/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/properties/h_size.hpp
*
*  Defines special property "h_size" to get heterogeneous size of object.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_H_SIZE_HPP
#define DRACOSHA_VALIDATOR_H_SIZE_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/basic_property.hpp>
#include <dracosha/validator/property_validator.hpp>
#include <dracosha/validator/utils/heterogeneous_size.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Property for getting size of heterogeneous containers such as tuple size.
 */
struct type_h_size : public basic_property
{
    template <typename T>
    constexpr static size_t get(T&& v)
    {
        return hana::value(heterogeneous_size(v));
    }
    template <typename T>
    constexpr static bool has()
    {
        // applicable to any object because
        // size of non heterogeneous containers is zero
        return true;
    }
    template <typename ... Args>
    constexpr auto operator () (Args&&... args) const;

    constexpr static const char* name()
    {
        return "heterogeneous size";
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

    template <typename T> constexpr bool operator == (const T&&) const
    {
        return false;
    }
    template <typename T> constexpr bool operator != (const T&&) const
    {
        return true;
    }
    constexpr bool operator == (const type_h_size&) const
    {
        return true;
    }
    constexpr bool operator != (const type_h_size&) const
    {
        return false;
    }
};
constexpr type_h_size h_size{};
template <typename ... Args>
constexpr auto type_h_size::operator () (Args&&... args) const
{
    return make_property_validator(h_size,std::forward<Args>(args)...);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_H_SIZE_HPP
