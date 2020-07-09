/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/scalar_compare.hpp
*
*  Defines compiler friendly operators to compare scalar values
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_COMPARE_HPP
#define DRACOSHA_VALIDATOR_COMPARE_HPP

#include <type_traits>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 *  @brief Compare two values of matching types
 *
 * "Matching types" means that compiler does not emit warnings when values of these types are compared.
 */
template <typename LeftT, typename RightT, typename=void>
struct scalar_compare
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        return left < right;
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return left == right;
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return left <= right;
    }
};

/**
 *  @brief Compare signed and unsigned values except for floating and boolean types
 *
 */
template <typename LeftT, typename RightT>
struct scalar_compare<LeftT,RightT,
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
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        if (left < 0)
        {
            return true;
        }
        return static_cast<std::make_unsigned_t<LeftT>>(left) <= right;
    }
};

/**
 *  @brief Compare unsigned and signed values except for floating and boolean types
 *
 */
template <typename LeftT, typename RightT>
struct scalar_compare<LeftT, RightT,
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
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        if (right < 0)
        {
            return false;
        }
        return left <= static_cast<std::make_unsigned_t<RightT>>(right);
    }
};

/**
 *  @brief Compare non-boolean and boolean values
 *
 */
template <typename LeftT,typename RightT>
struct scalar_compare<LeftT,RightT,
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
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return static_cast<bool>(left) <= right;
    }
};

/**
 *  @brief Compare boolean and non-boolean values
 *
 */
template <typename LeftT, typename RightT>
struct scalar_compare<LeftT, RightT,
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
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return left <= static_cast<bool>(right);
    }
};

//-------------------------------------------------------------

template <typename LeftT, typename RightT, typename=void>
struct compare
{
};

/**
 * @brief Compare non scalar values
 */
template <typename LeftT, typename RightT>
struct compare<LeftT, RightT,
        std::enable_if_t<!std::is_scalar<LeftT>::value && !std::is_scalar<RightT>::value>>
{
    constexpr static bool less(const LeftT& left, const RightT& right) noexcept
    {
        return left < right;
    }
    constexpr static bool equal(const LeftT& left, const RightT& right) noexcept
    {
        return left==right;
    }
    constexpr static bool less_equal(const LeftT& left, const RightT& right) noexcept
    {
        return left <= right;
    }
};

/**
 * @brief Compare two values when at least one of them is of scalar type
 */
template <typename LeftT, typename RightT>
struct compare<LeftT, RightT,
        std::enable_if_t<std::is_scalar<LeftT>::value || std::is_scalar<RightT>::value>
    > : public scalar_compare<LeftT,RightT>
{
};

template <typename LeftT, typename RightT>
constexpr bool compare_less(const LeftT& a, const RightT& b)
{
    return compare<LeftT,RightT>::less(a,b);
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_COMPARE_HPP
