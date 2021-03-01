/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/safe_compare.hpp
*
*  Defines type safe operators to compare values.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_SAFE_COMPARE_HPP
#define DRACOSHA_VALIDATOR_SAFE_COMPARE_HPP

#include <type_traits>
#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/string_view.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Safe less than or equal to if operation is not possible.
 */
template <typename LeftT, typename RightT, typename=void>
struct safe_lte
{
    constexpr static bool f(const LeftT&, const RightT&) noexcept
    {
        return false;
    }
};
/**
 * @brief Safe less or equal to if operation is possible.
 */
template <typename LeftT, typename RightT>
struct safe_lte<LeftT,RightT,
            std::enable_if_t
            <
                std::is_same<
                       bool,
                       decltype(
                           std::declval<LeftT>()<=std::declval<RightT>()
                        )
                   >::value
            >
        >
{
    constexpr static bool f(const LeftT& left, const RightT& right) noexcept
    {
        return left <= right;
    }
};

/**
 * @brief Safe less than if operation is not possible.
 */
template <typename LeftT, typename RightT, typename=void>
struct safe_lt
{
    constexpr static bool f(const LeftT&, const RightT&) noexcept
    {
        return false;
    }
};

/**
 * @brief Safe less than if operation is possible.
 */
template <typename LeftT, typename RightT>
struct safe_lt<LeftT,RightT,
            std::enable_if_t
            <
                std::is_same<
                       bool,
                       decltype(
                           std::declval<LeftT>()<std::declval<RightT>()
                        )
                   >::value
            >
        >
{
    constexpr static bool f(const LeftT& left, const RightT& right) noexcept
    {
        return left < right;
    }
};

/**
 * @brief Safe equal to if operation is not possible.
 */
template <typename LeftT, typename RightT, typename=void>
struct safe_eq
{
    constexpr static bool f(const LeftT&, const RightT&) noexcept
    {
        return false;
    }
};

/**
 * @brief Safe equal to if operation is possible.
 */
template <typename LeftT, typename RightT>
struct safe_eq<LeftT,RightT,
            std::enable_if_t
            <
                std::is_same<
                       bool,
                       decltype(
                           std::declval<LeftT>()==std::declval<RightT>()
                        )
                   >::value
            >
        >
{
    constexpr static bool f(const LeftT& left, const RightT& right) noexcept
    {
        return left == right;
    }
};

/**
 * @brief Safe not equal to if operation is not possible.
 */
template <typename LeftT, typename RightT, typename=void>
struct safe_ne
{
    constexpr static bool f(const LeftT&, const RightT&) noexcept
    {
        return true;
    }
};

/**
 * @brief Safe not equal to if operation is possible.
 */
template <typename LeftT, typename RightT>
struct safe_ne<LeftT,RightT,
            std::enable_if_t
            <
                std::is_same<
                       bool,
                       decltype(
                           std::declval<LeftT>()!=std::declval<RightT>()
                        )
                   >::value
            >
        >
{
    constexpr static bool f(const LeftT& left, const RightT& right) noexcept
    {
        return left != right;
    }
};

/**
 * @brief Safe greater than or equal to if operation is not possible.
 */
template <typename LeftT, typename RightT, typename=void>
struct safe_gte
{
    constexpr static bool f(const LeftT&, const RightT&) noexcept
    {
        return false;
    }
};
/**
 * @brief Safe less or equal to if operation is possible.
 */
template <typename LeftT, typename RightT>
struct safe_gte<LeftT,RightT,
            std::enable_if_t
            <
                std::is_same<
                       bool,
                       decltype(
                           std::declval<LeftT>()>=std::declval<RightT>()
                        )
                   >::value
            >
        >
{
    constexpr static bool f(const LeftT& left, const RightT& right) noexcept
    {
        return left >= right;
    }
};

/**
 * @brief Safe greater than if operation is not possible.
 */
template <typename LeftT, typename RightT, typename=void>
struct safe_gt
{
    constexpr static bool f(const LeftT&, const RightT&) noexcept
    {
        return false;
    }
};

/**
 * @brief Safe greater than if operation is possible.
 */
template <typename LeftT, typename RightT>
struct safe_gt<LeftT,RightT,
            std::enable_if_t
            <
                std::is_same<
                       bool,
                       decltype(
                           std::declval<LeftT>()>std::declval<RightT>()
                        )
                   >::value
            >
        >
{
    constexpr static bool f(const LeftT& left, const RightT& right) noexcept
    {
        return left > right;
    }
};

/**
 *  @brief Helper for comparing two values of comparable or not comparable types.
 *
 * If types are not comparable then comparison result is always false.
 */
template <typename LeftT, typename RightT, typename=void>
struct safe_compare
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        return safe_lt<LeftT,RightT>::f(left,right);
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return safe_lte<LeftT,RightT>::f(left,right);
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return safe_eq<LeftT,RightT>::f(left,right);
    }
    constexpr static bool not_equal(const LeftT& left, const RightT& right) noexcept
    {
        return safe_ne<LeftT,RightT>::f(left,right);
    }
    constexpr static bool greater(const LeftT& left, const RightT& right) noexcept
    {
        return safe_gt<LeftT,RightT>::f(left,right);
    }
    constexpr static bool greater_equal(const LeftT& left, const RightT& right) noexcept
    {
        return safe_gte<LeftT,RightT>::f(left,right);
    }
};

/**
 *  @brief Helper for comparing signed and unsigned values except for floating and boolean types.
 *
 */
template <typename LeftT, typename RightT>
struct safe_compare<LeftT,RightT,
                        std::enable_if_t<
                            std::is_signed<LeftT>::value &&
                            !std::is_floating_point<LeftT>::value &&
                            std::is_unsigned<RightT>::value &&
                            !std::is_same<RightT, bool>::value
                        >
                    >
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        if (left < 0)
        {
            return true;
        }
        return static_cast<std::make_unsigned_t<LeftT>>(left) < right;
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return static_cast<std::make_unsigned_t<LeftT>>(left) == right;
    }
    constexpr static bool not_equal(const LeftT& left, const RightT& right) noexcept
    {
        return !equal(left,right);
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        if (left < 0)
        {
            return true;
        }
        return static_cast<std::make_unsigned_t<LeftT>>(left) <= right;
    }
    constexpr static bool greater(const LeftT& left, const RightT& right) noexcept
    {
        return !less_equal(left,right);
    }
    constexpr static bool greater_equal(const LeftT& left, const RightT& right) noexcept
    {
        return !less(left,right);
    }
};

/**
 *  @brief Helper for comparing unsigned and signed values except for floating and boolean types.
 *
 */
template <typename LeftT, typename RightT>
struct safe_compare<LeftT, RightT,
        std::enable_if_t<
            std::is_unsigned<LeftT>::value &&
            std::is_signed<RightT>::value &&
            !std::is_floating_point<RightT>::value &&
            !std::is_same<LeftT, bool>::value>
    >
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        if (right < 0)
        {
            return false;
        }
        return left < static_cast<std::make_unsigned_t<RightT>>(right);
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return left == static_cast<std::make_unsigned_t<RightT>>(right);
    }
    constexpr static bool not_equal(const LeftT& left, const RightT& right) noexcept
    {
        return !equal(left,right);
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        if (right < 0)
        {
            return false;
        }
        return left <= static_cast<std::make_unsigned_t<RightT>>(right);
    }
    constexpr static bool greater(const LeftT& left, const RightT& right) noexcept
    {
        return !less_equal(left,right);
    }
    constexpr static bool greater_equal(const LeftT& left, const RightT& right) noexcept
    {
        return !less(left,right);
    }
};

/**
 *  @brief Helper for comparing non-boolean and boolean values.
 *
 */
template <typename LeftT,typename RightT>
struct safe_compare<LeftT,RightT,
                    std::enable_if_t<!std::is_same<LeftT, bool>::value && std::is_same<RightT, bool>::value>
                >
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        return static_cast<bool>(left) < right;
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return static_cast<bool>(left)==right;
    }
    constexpr static bool not_equal(const LeftT& left, const RightT& right) noexcept
    {
        return !equal(left,right);
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return static_cast<bool>(left) <= right;
    }
    constexpr static bool greater(const LeftT& left, const RightT& right) noexcept
    {
        return !less_equal(left,right);
    }
    constexpr static bool greater_equal(const LeftT& left, const RightT& right) noexcept
    {
        return !less(left,right);
    }
};

/**
 *  @brief Helper for comparing boolean and non-boolean values.
 *
 */
template <typename LeftT, typename RightT>
struct safe_compare<LeftT, RightT,
        std::enable_if_t<std::is_same<LeftT, bool>::value && !std::is_same<RightT, bool>::value>
    >
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        return left < static_cast<bool>(right);
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return left==static_cast<bool>(right);
    }
    constexpr static bool not_equal(const LeftT& left, const RightT& right) noexcept
    {
        return !equal(left,right);
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return left <= static_cast<bool>(right);
    }
    constexpr static bool greater(const LeftT& left, const RightT& right) noexcept
    {
        return !less_equal(left,right);
    }
    constexpr static bool greater_equal(const LeftT& left, const RightT& right) noexcept
    {
        return !less(left,right);
    }
};

/**
 *  @brief Helper for comparing string and string_view.
 *
 */
template <typename LeftT, typename RightT>
struct safe_compare<LeftT, RightT,
        std::enable_if_t<
            (std::is_same<LeftT,string_view>::value && std::is_same<RightT,std::string>::value)
            ||
            (std::is_same<LeftT,std::string>::value && std::is_same<RightT,string_view>::value)
        >
    >
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        return string_view(left) < string_view(right);
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return string_view(left) == string_view(right);
    }
    constexpr static bool not_equal(const LeftT& left, const RightT& right) noexcept
    {
        return !equal(left,right);
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return string_view(left) <= string_view(right);
    }
    constexpr static bool greater(const LeftT& left, const RightT& right) noexcept
    {
        return !less_equal(left,right);
    }
    constexpr static bool greater_equal(const LeftT& left, const RightT& right) noexcept
    {
        return !less(left,right);
    }
};

//-------------------------------------------------------------

/**
 * @brief Compare if left value is less than right value.
 * @param a First value to compare.
 * @param b Second valut to compare.
 * @return Operation result.
 */
template <typename LeftT, typename RightT>
constexpr bool safe_compare_less(const LeftT& a, const RightT& b)
{
    return safe_compare<LeftT,RightT>::less(a,b);
}

/**
 * @brief Compare if left value is greater than right value.
 * @param a First value to compare.
 * @param b Second valut to compare.
 * @return Operation result.
 */
template <typename LeftT, typename RightT>
constexpr bool safe_compare_greater(const LeftT& a, const RightT& b)
{
    return safe_compare<LeftT,RightT>::greater(a,b);
}

/**
 * @brief Compare if left value is equal to right value.
 * @param a First value to compare.
 * @param b Second valut to compare.
 * @return Operation result.
 */
template <typename LeftT, typename RightT>
constexpr bool safe_compare_equal(const LeftT& a, const RightT& b)
{
    return safe_compare<LeftT,RightT>::equal(a,b);
}

/**
 * @brief Compare if left value is not equal to right value.
 * @param a First value to compare.
 * @param b Second valut to compare.
 * @return Operation result.
 */
template <typename LeftT, typename RightT>
constexpr bool safe_compare_not_equal(const LeftT& a, const RightT& b)
{
    return safe_compare<LeftT,RightT>::not_equal(a,b);
}

/**
 * @brief Compare if left value is less than or equal to right value.
 * @param a First value to compare.
 * @param b Second valut to compare.
 * @return Operation result.
 */
template <typename LeftT, typename RightT>
constexpr bool safe_compare_less_equal(const LeftT& a, const RightT& b)
{
    return safe_compare<LeftT,RightT>::less(a,b);
}

/**
 * @brief Compare if left value is greater than or equal to right value.
 * @param a First value to compare.
 * @param b Second valut to compare.
 * @return Operation result.
 */
template <typename LeftT, typename RightT>
constexpr bool safe_compare_greater_equal(const LeftT& a, const RightT& b)
{
    return safe_compare<LeftT,RightT>::greater_equal(a,b);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SAFE_COMPARE_HPP
