/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/utils/heterogeneous_size.hpp
*
*  Defines helper got getting size of heterogeneous objects/containers.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_HETEROGENEOUS_SIZE_HPP
#define DRACOSHA_VALIDATOR_HETEROGENEOUS_SIZE_HPP

#include <boost/hana/ext/std/tuple.hpp>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/hana_to_std_tuple.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename T, typename Enable=hana::when<true>>
struct heterogeneous_size_t : public hana::size_t<0>
{
};
template <typename T>
struct heterogeneous_size_t<T,
            hana::when<hana::Foldable<T>::value>
        > : public hana_tuple_size<T>
{
};

struct heterogeneous_size_impl
{
    template <typename T>
    constexpr auto operator () (T&&) const
    {
        return heterogeneous_size_t<T>{};
    }
};
constexpr heterogeneous_size_impl heterogeneous_size{};

//-------------------------------------------------------------

struct is_heterogeneous_container_impl
{
    template <typename T>
    constexpr auto operator () (T&& v) const
    {
        return hana::not_equal(heterogeneous_size(v),hana::size_c<0>);
    }
};
constexpr is_heterogeneous_container_impl is_heterogeneous_container{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_HETEROGENEOUS_SIZE_HPP
