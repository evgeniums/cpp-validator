/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/wrap_it.hpp
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

template <typename T>
struct wrap_it_t
{
    using hana_tag=wrap_iterator_tag;

    auto get() -> decltype(auto)
    {
        return get_it(_it);
    }

    auto get() const -> decltype(auto)
    {
        return get_it(_it);
    }

    std::string name() const
    {
        return _name;
    }

    T _it;
    std::string _name;
};

BOOST_HANA_CONSTEXPR_LAMBDA auto wrap_it=[](auto&& it, auto&& aggregation)
{
    return wrap_it_t<decltype(it)>{std::forward<decltype(it)>(it),aggregation.description};
};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_WRAP_IT_HPP
