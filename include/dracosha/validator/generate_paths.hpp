/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/generate_paths.hpp
*
*  Defines "generate_paths".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_GENERATE_PATHS_HPP
#define DRACOSHA_VALIDATOR_GENERATE_PATHS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/status.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

template <typename KeyT, typename Enable=hana::when<true>>
struct generate_paths_t
{
    template <typename UsedPathSizeT, typename PathT, typename AdapterT, typename HandlerT>
    status operator () (UsedPathSizeT&&, PathT&& path, AdapterT&& adapter, HandlerT&& handler) const
    {
        return handler(std::forward<AdapterT>(adapter),std::forward<PathT>(path),hana::size(path));
    }
};
template <typename KeyT>
constexpr generate_paths_t<KeyT> generate_paths{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_GENERATE_PATHS_HPP
