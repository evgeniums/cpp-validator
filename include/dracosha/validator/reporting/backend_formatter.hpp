/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/backend_formatter.hpp
*
*  Defines backend formatter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_BACKEND_FORMATTER_HPP
#define DRACOSHA_VALIDATOR_BACKEND_FORMATTER_HPP

#include <boost/algorithm/string/trim.hpp>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/detail/backend_formatter_helper.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

/**
 * @brief Backend formatter.
 *
 * Backend formatter performs actual formatting and constructs final reporting string.
 */
struct backend_formatter_t
{
    /**
     * @brief Append arguments to destination object.
     * @param dst Destination object.
     * @param args Arguments to append.
     */
    template <typename DstT, typename ...Args>
    static void append(DstT& dst, Args&&... args)
    {
        detail::backend_formatter_helper<DstT>::append(dst,std::forward<Args>(args)...);
    }

    /**
     * @brief Join arguments and append to destination object.
     * @param dst Destination object.
     * @param sep Separator for joining.
     * @param args Arguments to join and append.
     */
    template <typename DstT, typename SepT, typename ...Args>
    static void append_join_args(DstT& dst, SepT&& sep, Args&&... args)
    {
        detail::backend_formatter_helper<DstT>::append_join_args(dst,std::forward<SepT>(sep),std::forward<Args>(args)...);
        boost::trim(detail::to_dst(dst));
    }

    /**
     * @brief Join parts and append to destination object.
     * @param dst Destination object.
     * @param sep Separator for joining.
     * @param parts Parts to join and append, can be either std::vector or hana::tuple.
     */
    template <typename DstT,  typename SepT, typename PartsT>
    static void append_join(DstT& dst, SepT&& sep, PartsT&& parts)
    {
        detail::backend_formatter_helper<DstT>::append_join(dst,std::forward<SepT>(sep),std::forward<PartsT>(parts));
        boost::trim(detail::to_dst(dst));
    }
};

/**
  Instance of backend formatter.
*/
constexpr backend_formatter_t backend_formatter{};

/**
 * @brief Wrap destination object into backend formatter using default backend formatter.
 * @param dst Destination object.
 * @return Backend formatter wrapping destination object.
 */
template <typename DstT>
auto wrap_backend_formatter(DstT& dst)
{
    return detail::backend_formatter_helper<DstT>::wrap(dst);
}

/**
 * @brief Wrap destination object into backend formatter using sample backend formatter.
 * @param dst Destination object.
 * @param fm Sample backend formatter to clone from.
 * @return Backend formatter wrapping destination object.
 */
template <typename DstT, typename BackendFormatterSampleT>
auto wrap_backend_formatter(DstT& dst, BackendFormatterSampleT& fm)
{
    return fm.clone(dst);
}

//-------------------------------------------------------------
DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_BACKEND_FORMATTER_HPP
