/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/filer_path.hpp
*
*  Defines "filter_path".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FILTER_PATH_HPP
#define DRACOSHA_VALIDATOR_FILTER_PATH_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

struct filter_path_t
{
    template <typename AdapterT, typename PathT>
    bool operator () (AdapterT&& adapter, PathT&& path) const
    {
        //! @todo Implement.
        std::ignore=adapter;
        std::ignore=path;
        return false;
    }
};
constexpr filter_path_t filter_path{};

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FILTER_PATH_HPP
