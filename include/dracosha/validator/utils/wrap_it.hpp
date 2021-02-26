/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/wrap_it.hpp
*
*  Defines wrapper of container's iterator.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_WRAP_IT_HPP
#define DRACOSHA_VALIDATOR_WRAP_IT_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/get_it.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct wrap_iterator_tag;

/**
 * @brief Wrapper of container's iterator paired with explicit name or description.
 */
template <typename T>
struct wrap_it_t
{
    using hana_tag=wrap_iterator_tag;

    /**
      @brief Get value from wrapped iterator.
      */
    auto get() -> decltype(auto)
    {
        return get_it(_it);
    }

    /**
      @brief Get constant value from wrapped iterator.
      */
    auto get() const -> decltype(auto)
    {
        return get_it(_it);
    }

    /**
     * @brief Get iterator name.
     * @return Iterator name to use in report.
     */
    std::string name() const
    {
        return _name;
    }

    T _it;
    std::string _name;
};

/**
  @brief Wrap iterator and pair it with explicit name or description.
  @param it Iterator.
  @param aggregation Aggregation descriptor to take iterator name or desctiption from.
  @return Wrapped iterator.
  */
template <typename Ti, typename Ta>
auto wrap_it(Ti&& it, Ta&& aggregation)
{
    return wrap_it_t<decltype(it)>{std::forward<decltype(it)>(it),aggregation};
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_WRAP_IT_HPP
