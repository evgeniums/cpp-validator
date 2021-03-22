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
#include <dracosha/validator/aggregation/element_aggregation.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct wrap_iterator_tag;

/**
 * @brief Wrapper of container's iterator paired with explicit name or description.
 */
template <typename T, typename AggregationT, typename ModifierT>
struct wrap_it_t : public AggregationT::type,
                   public adjust_storable_ignore
{
    using hana_tag=wrap_iterator_tag;

    /**
      @brief Constructor.
      @param it Iterator.
      @param aggregation Aggregation descriptor to take iterator name or desctiption from.
      @param modifier Aggregation modifier.
      @return Wrapped iterator.
    */
    template <typename Ti, typename Ta, typename Tm>
    wrap_it_t(Ti&& it, Ta&& aggregation, Tm&& modifier)
            : _it(std::forward<Ti>(it)),
              _aggregation_type(std::forward<Ta>(aggregation)),
              _modifier(std::forward<Tm>(modifier))
    {}

    /**
      @brief Get constant value from wrapped iterator.
      */
    auto get() const -> decltype(auto)
    {
        return hana::eval_if(
            std::is_same<ModifierT,keys_t>{},
            [&](auto&& _) -> decltype(auto)
            {
                return key_it(_(_it));
            },
            [&](auto&&) -> decltype(auto)
            {
                return hana::eval_if(
                    std::is_same<ModifierT,iterators_t>{},
                    [&](auto&& _) -> decltype(auto)
                    {
                        return *_(_it);
                    },
                    [&](auto&& _) -> decltype(auto)
                    {
                        return get_it(_(_it));
                    }
                );
            }
        );
    }

    /**
     * @brief Get iterator name.
     * @return Iterator name to use in report.
     */
    std::string name() const
    {
        return _aggregation_type(_modifier);
    }

    template <typename T1>
    constexpr bool operator == (T1) const noexcept
    {
        return true;
    }
    template <typename T1>
    constexpr bool operator != (T1) const noexcept
    {
        return false;
    }
    template <typename T1>
    constexpr friend bool operator == (const T1&, const wrap_it_t&) noexcept
    {
        return true;
    }
    template <typename T1>
    constexpr friend bool operator != (const T1&, const wrap_it_t&) noexcept
    {
        return false;
    }

    private:

        T _it;
        AggregationT _aggregation_type;
        ModifierT _modifier;
};

/**
  @brief Wrap iterator and pair it with explicit name or description.
  @param it Iterator.
  @param aggregation Aggregation descriptor to take iterator name or desctiption from.
  @param modifier Aggregation modifier.
  @return Wrapped iterator.
  */
template <typename Ti, typename Ta, typename Tm>
auto wrap_it(Ti&& it, Ta&& aggregation, Tm&& modifier)
{
    return wrap_it_t<std::decay_t<Ti>,std::decay_t<Ta>,std::decay_t<Tm>>{
        std::forward<Ti>(it),
        std::forward<Ta>(aggregation),
        std::forward<Tm>(modifier)
    };
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_WRAP_IT_HPP
