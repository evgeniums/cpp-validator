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

/**
 * @brief Helper for generating member paths.
 */
template <typename KeyT, typename Enable=hana::when<true>>
struct generate_paths_t
{
    template <typename UsedPathSizeT, typename PathT, typename AdapterT, typename HandlerT>
    status operator () (UsedPathSizeT&& used_path_size, PathT&& path, AdapterT&& adapter, HandlerT&& handler) const
    {
        std::ignore=used_path_size;
        return handler(std::forward<AdapterT>(adapter),std::forward<PathT>(path),hana::size(path));
    }
};
/**
 * @brief Generate paths from original member's path.
 * @param used_path_size Length of already used member's path prefix.
 * @param path Previous/original path.
 * @param adapter Validation adapter.
 * @param handler Validation handler to apply to member with generated path.
 * @return Validation status.
 *
 * Paths can be generated in case of element aggregations and trees.
 */
template <typename KeyT>
constexpr generate_paths_t<KeyT> generate_paths{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_GENERATE_PATHS_HPP
