/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/string_view.hpp
*
*   Defaines alias for string_view depending on the supported c++ standard.
*   For c++17 std::string_view is used, for c++14 boost::string_view is used.
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_STRING_VIEW_HPP
#define DRACOSHA_VALIDATOR_STRING_VIEW_HPP

#if __cplusplus < 201703L
#include <boost/utility/string_view.hpp>
#else
#include <string_view>
#endif

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{
#if __cplusplus < 201703L
    using string_view=boost::string_view;
#else
    using string_view=std::string_view;
#endif

/**
 * @brief Create string view around container
 * @param container Container to wrap
 */
template <typename T>
string_view to_string_view(const T& container) noexcept
{
    return string_view(container.data(),container.size());
}
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_STRING_VIEW_HPP
