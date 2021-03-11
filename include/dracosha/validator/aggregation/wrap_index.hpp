/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/wrap_index.hpp
*
*  Defines wrapper of index for variadic properties.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_WRAP_INDEX_HPP
#define DRACOSHA_VALIDATOR_WRAP_INDEX_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/safe_compare.hpp>
#include <dracosha/validator/variadic_arg.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct wrap_index_tag;

/**
 * @brief Wrapper of index for variadic properties.
 */
template <typename T, typename AggregationT>
struct wrap_index_t : public AggregationT::type
{
    using hana_tag=wrap_index_tag;

    template <typename Ti, typename Ta>
    wrap_index_t(Ti index, Ta&& aggregation)
            : _index(std::move(index)),
              _aggregation_type(std::forward<Ta>(aggregation))
    {}

    auto aggregation() const -> decltype(auto)
    {
        return _aggregation_type;
    }

    std::string name() const
    {
        return _aggregation_type.name;
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
    constexpr friend bool operator == (const T1&, const wrap_index_t&) noexcept
    {
        return true;
    }
    template <typename T1>
    constexpr friend bool operator != (const T1&, const wrap_index_t&) noexcept
    {
        return false;
    }

    operator T() const
    {
        return _index;
    }

    private:

        T _index;
        AggregationT _aggregation_type;
};

template <typename Ti, typename Ta>
auto wrap_index(Ti&& it, Ta&& aggregation)
{
    return wrap_index_t<std::decay_t<Ti>,std::decay_t<Ta>>{
        std::forward<Ti>(it),
        std::forward<Ta>(aggregation)
    };
}

struct is_wrap_index_t
{
    template <typename T>
    constexpr auto operator() (T&&) const
    {
        return hana::is_a<wrap_index_tag,T>;
    }
};
constexpr is_wrap_index_t is_wrap_index{};

template <typename T, typename AggregationT>
struct is_bool<wrap_index_t<T,AggregationT>> : public is_bool<T>
{};

template <typename T, typename AggregationT>
struct is_string_view<wrap_index_t<T,AggregationT>> : public is_string_view<T>
{};

template <typename T, typename AggregationT>
struct is_string<wrap_index_t<T,AggregationT>> : public is_string<T>
{};

DRACOSHA_VALIDATOR_NAMESPACE_END

namespace std {

template <typename T, typename AggregationT>
struct is_floating_point<DRACOSHA_VALIDATOR_NAMESPACE::wrap_index_t<T,AggregationT>> : public is_floating_point<T>
{};

template <typename T, typename AggregationT>
struct is_signed<DRACOSHA_VALIDATOR_NAMESPACE::wrap_index_t<T,AggregationT>> : public is_signed<T>
{};

template <typename T, typename AggregationT>
struct make_unsigned<DRACOSHA_VALIDATOR_NAMESPACE::wrap_index_t<T,AggregationT>> : public make_unsigned<T>
{};

}

#endif // DRACOSHA_VALIDATOR_WRAP_INDEX_HPP
