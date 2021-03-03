/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/value_as_container.hpp
*
*  Defines wrapper of a value with simple container interface to be used in getters.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALUE_AS_CONTAINER_HPP
#define DRACOSHA_VALIDATOR_VALUE_AS_CONTAINER_HPP

#include <stdexcept>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/utils/safe_compare.hpp>
#include <dracosha/validator/utils/string_view.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct value_as_container_tag;

/**
 * @brief Wrapper of a value with simple container interface to be used in getters.
 */
template <typename T>
struct value_as_container_wrapper : public object_wrapper<T>
{
    using hana_tag=value_as_container_tag;
    using object_wrapper<T>::object_wrapper;

    /**
     * Get value with at() interface for using with getters.
     */
    template <typename T1>
    auto at(const T& value) const -> decltype(auto)
    {
        if (!contains(value))
        {
            throw std::out_of_range("Invalid value");
        }
        return this->get();
    }

    /**
     * Check if reference contains a value for using with getters.
     */
    template <typename T1>
    bool contains(const T1& value) const noexcept
    {
        return safe_compare_equal(value,this->get());
    }
};

/**
 * @brief Wrapper of a value with simple container interface to be used in getters with inversed result of contains().
 */
template <typename T>
struct value_as_container_inverse_wrapper : public value_as_container_wrapper<T>
{
    using value_as_container_wrapper<T>::value_as_container_wrapper;

    /**
     * Get value with at() interface for using with getters.
     */
    template <typename T1>
    auto at(T1&& value/*,
            std::enable_if_t<
                    std::is_constructible<
                            std::decay_t<T>,
                            T1
                        >::value
            , void*> =nullptr*/) const -> decltype(auto)
    {
        if (!contains(value))
        {
            throw std::out_of_range("Invalid value");
        }
        return this->get();
    }

    /**
     * Check if reference contains a value for using with getters.
     */
    template <typename T1>
    bool contains(const T1& value) const noexcept
    {
        return !safe_compare_equal(value,this->get());
    }
};

/**
  @brief Create value as container wrapper.
  @param v Object to wrap.
  @return Created wrapper.
*/
struct value_as_container_t
{
    template <typename T>
    auto operator () (T&& v) const
    {
        using type=decltype(adjust_view_type(std::forward<T>(v)));
        return value_as_container_wrapper<type>{adjust_view_type(std::forward<T>(v))};
    }
};
constexpr value_as_container_t value_as_container{};

/**
  @brief Create value as container wrapper with inversed result of contains().
  @param v Object to wrap.
  @return Created wrapper.
*/
struct value_as_container_inverse_t
{
    template <typename T>
    auto operator () (T&& v) const
    {
        using type=decltype(adjust_view_type(std::forward<T>(v)));
        return value_as_container_inverse_wrapper<type>{adjust_view_type(std::forward<T>(v))};
    }
};
constexpr value_as_container_inverse_t value_as_container_inverse{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALUE_AS_CONTAINER_HPP
