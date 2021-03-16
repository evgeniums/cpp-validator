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
#include <dracosha/validator/utils/unwrap_object.hpp>
#include <dracosha/validator/basic_property.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename T>
struct is_bool : public std::is_same<T,bool>
{
};

template <typename T>
struct is_string_view : public std::is_same<T,string_view>
{
};

template <typename T>
struct is_string : public std::is_same<T,std::string>
{
};

//-------------------------------------------------------------

/**
 * @brief Safe equal to if operation is not possible.
 */
template <typename LeftT, typename RightT, typename=void>
struct safe_eq
{
    using comparable=std::false_type;

    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1&, const RightT1&) noexcept
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
                &&
                decltype(hana::equal(
                    hana::is_a<property_tag,RightT>,
                    hana::is_a<property_tag,LeftT>
                ))::value
            >
        >
{
    using comparable=std::true_type;

    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1& left, const RightT1& right) noexcept
    {
        return left == right;
    }
};

namespace detail {

/**
 * @brief Safe less than or equal to if operation is not possible.
 */
template <typename LeftT, typename RightT, typename=void>
struct safe_lte
{
    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1&, const RightT1&) noexcept
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
    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1& left, const RightT1& right) noexcept
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
    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1&, const RightT1&) noexcept
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
    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1& left, const RightT1& right) noexcept
    {
        return left < right;
    }
};

/**
 * @brief Safe not equal to if operation is not possible.
 */
template <typename LeftT, typename RightT, typename=void>
struct safe_ne
{
    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1&, const RightT1&) noexcept
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
    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1& left, const RightT1& right) noexcept
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
    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1&, const RightT1&) noexcept
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
    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1& left, const RightT1& right) noexcept
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
    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1&, const RightT1&) noexcept
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
    template <typename LeftT1, typename RightT1>
    constexpr static bool f(const LeftT1& left, const RightT1& right) noexcept
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
    template <typename LeftT1, typename RightT1>
    constexpr static bool less(const LeftT1& left, const RightT1& right) noexcept
    {
        return safe_lt<LeftT,RightT>::f(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool less_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return safe_lte<LeftT,RightT>::f(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return safe_eq<LeftT,RightT>::f(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool not_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return safe_ne<LeftT,RightT>::f(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater(const LeftT1& left, const RightT1& right) noexcept
    {
        return safe_gt<LeftT,RightT>::f(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater_equal(const LeftT1& left, const RightT1& right) noexcept
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
                            !is_bool<RightT>::value
                        >
                    >
{
    template <typename LeftT1, typename RightT1>
    constexpr static bool less(const LeftT1& left, const RightT1& right) noexcept
    {
        if (left < 0)
        {
            return true;
        }
        return static_cast<std::make_unsigned_t<LeftT>>(left) < right;
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return static_cast<std::make_unsigned_t<LeftT>>(left) == right;
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool not_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return !equal(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool less_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        if (left < 0)
        {
            return true;
        }
        return static_cast<std::make_unsigned_t<LeftT>>(left) <= right;
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater(const LeftT1& left, const RightT1& right) noexcept
    {
        return !less_equal(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater_equal(const LeftT1& left, const RightT1& right) noexcept
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
            !is_bool<LeftT>::value>
    >
{
    template <typename LeftT1, typename RightT1>
    constexpr static bool less(const LeftT1& left, const RightT1& right) noexcept
    {
        if (right < 0)
        {
            return false;
        }
        return left < static_cast<std::make_unsigned_t<RightT>>(right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return left == static_cast<std::make_unsigned_t<RightT>>(right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool not_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return !equal(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool less_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        if (right < 0)
        {
            return false;
        }
        return left <= static_cast<std::make_unsigned_t<RightT>>(right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater(const LeftT1& left, const RightT1& right) noexcept
    {
        return !less_equal(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater_equal(const LeftT1& left, const RightT1& right) noexcept
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
                    std::enable_if_t<
                            is_bool<RightT>::value
                            &&
                            !is_bool<LeftT>::value
                            &&
                            std::is_convertible<LeftT, bool>::value
                        >
                >
{
    template <typename LeftT1, typename RightT1>
    constexpr static bool less(const LeftT1& left, const RightT1& right) noexcept
    {
        return static_cast<bool>(left) < right;
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return static_cast<bool>(left)==right;
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool not_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return !equal(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool less_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return static_cast<bool>(left) <= right;
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater(const LeftT1& left, const RightT1& right) noexcept
    {
        return !less_equal(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater_equal(const LeftT1& left, const RightT1& right) noexcept
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
        std::enable_if_t<
                is_bool<LeftT>::value
                &&
                !is_bool<RightT>::value
                &&
                std::is_convertible<RightT, bool>::value
            >
    >
{
    template <typename LeftT1, typename RightT1>
    constexpr static bool less(const LeftT1& left, const RightT1& right) noexcept
    {
        return left < static_cast<bool>(right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return left==static_cast<bool>(right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool not_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return !equal(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool less_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return left <= static_cast<bool>(right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater(const LeftT1& left, const RightT1& right) noexcept
    {
        return !less_equal(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater_equal(const LeftT1& left, const RightT1& right) noexcept
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
            (is_string_view<LeftT>::value && is_string<RightT>::value)
            ||
            (is_string<LeftT>::value && is_string_view<RightT>::value)
        >
    >
{
    template <typename LeftT1, typename RightT1>
    constexpr static bool less(const LeftT1& left, const RightT1& right) noexcept
    {
        return string_view(left) < string_view(right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return string_view(left) == string_view(right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool not_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return !equal(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool less_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return string_view(left) <= string_view(right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater(const LeftT1& left, const RightT1& right) noexcept
    {
        return !less_equal(left,right);
    }
    template <typename LeftT1, typename RightT1>
    constexpr static bool greater_equal(const LeftT1& left, const RightT1& right) noexcept
    {
        return !less(left,right);
    }
};

}

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
    return detail::safe_compare<unwrap_object_t<LeftT>,unwrap_object_t<RightT>>
            ::less(unwrap_object(a),unwrap_object(b));
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
    return detail::safe_compare<unwrap_object_t<LeftT>,unwrap_object_t<RightT>>
            ::greater(unwrap_object(a),unwrap_object(b));
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
    return detail::safe_compare<unwrap_object_t<LeftT>,unwrap_object_t<RightT>>
            ::equal(unwrap_object(a),unwrap_object(b));
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
    return detail::safe_compare<unwrap_object_t<LeftT>,unwrap_object_t<RightT>>
            ::not_equal(unwrap_object(a),unwrap_object(b));
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
    return detail::safe_compare<unwrap_object_t<LeftT>,unwrap_object_t<RightT>>
            ::less(unwrap_object(a),unwrap_object(b));
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
    return detail::safe_compare<unwrap_object_t<LeftT>,unwrap_object_t<RightT>>
            ::greater_equal(unwrap_object(a),unwrap_object(b));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_SAFE_COMPARE_HPP
