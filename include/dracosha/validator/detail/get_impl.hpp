/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/get_impl.hpp
*
*  Defines helpers for getting member from object.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_GET_IMPL_HPP
#define DRACOSHA_VALIDATOR_GET_IMPL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/can_get.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

namespace detail
{

namespace get_helpers
{
    enum class getter : int
    {
        property = 1,
        at = 2,
        brackets = 3,
        iterator = 4,
        find = 5,

        none = -1
    };

    template <typename T1, typename T2>
    struct selector
    {
        constexpr static const auto value = hana::if_(
            can_get<T1, T2>.property(),
            getter::property,
            hana::if_(
                can_get<T1, T2>.at(),
                getter::at,
                hana::if_(
                    can_get<T1, T2>.brackets(),
                    getter::brackets,
                    hana::if_(
                        can_get<T1, T2>.iterator(),
                        getter::iterator,
                        hana::if_(
                            can_get<T1, T2>.find(),
                            getter::find,
                            getter::none
                        )
                    )
                )
            )
        );
    };
}

template <typename T1, typename T2, typename=hana::when<true>>
struct get_t
{
};

/**
 * @brief Get with iterator.
 */
template <typename T1, typename T2>
struct get_t<T1,T2,
            hana::when<get_helpers::selector<T1,T2>::value == get_helpers::getter::iterator>>
{
    auto operator () (T1&&, T2&& k) const -> decltype(auto)
    {
        return k.get();
    }
};

/**
 * @brief Get as property.
 */
template <typename T1, typename T2>
struct get_t<T1,T2,
            hana::when<get_helpers::selector<T1,T2>::value == get_helpers::getter::property>>
{
    auto operator () (T1&& v, T2&& k) const -> decltype(auto)
    {
        return property(std::forward<T1>(v),std::forward<T2>(k));
    }
};

/**
 * @brief Get using at(key) method.
 */
template <typename T1, typename T2>
struct get_t<T1,T2,
    hana::when<get_helpers::selector<T1, T2>::value == get_helpers::getter::at>>
{
    auto operator () (T1&& v, T2&& k) const -> decltype(auto)
    {
      return v.at(std::forward<T2>(k));
    }
};

/**
 * @brief Get using [key] operator.
 */
template <typename T1, typename T2>
struct get_t<T1,T2,
    hana::when<get_helpers::selector<T1, T2>::value == get_helpers::getter::brackets>>
{
    auto operator () (T1&& v, T2&& k) const -> decltype(auto)
    {
      return v[std::forward<T2>(k)];
    }
};
/**
 * @brief Get using find(key) method.
 */
template <typename T1, typename T2>
struct get_t<T1,T2,
    hana::when<get_helpers::selector<T1, T2>::value == get_helpers::getter::find>>
{
    auto operator () (T1&& v, T2&& k) const -> decltype(auto)
    {
      return *(v.find(std::forward<T2>(k)));
    }
};

/**
 * @brief Helper for getting member from object of type T1 using key of type T2.
 */
template <typename T1, typename T2>
constexpr get_t<T1,T2> get_impl{};
}

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_GET_IMPL_HPP
