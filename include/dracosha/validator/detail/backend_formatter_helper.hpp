/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/backend_formatter_helper.hpp
*
*  Defines helper for backend formatter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_BACKEND_FORMATTER_HELPER_HPP
#define DRACOSHA_VALIDATOR_BACKEND_FORMATTER_HELPER_HPP

#include <dracosha/validator/config.hpp>

#ifdef DRACOSHA_VALIDATOR_FMT
#include <dracosha/validator/detail/formatter_fmt.hpp>
#else
#include <dracosha/validator/detail/formatter_std.hpp>
#endif

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

#ifdef DRACOSHA_VALIDATOR_FMT

template <typename DstT>
auto default_backend_formatter(DstT& dst)
{
    return detail::fmt_backend_formatter<DstT>{dst};
}

#else

auto default_backend_formatter(std::string& dst)
{
    return detail::std_backend_formatter{dst};
}

#endif

namespace detail
{

template <typename DstT, typename =hana::when<true>>
struct backend_formatter_helper
{
    template <typename ...Args>
    static void append(DstT& dst, Args&&... args)
    {
        default_backend_formatter(dst).append(std::forward<Args>(args)...);
    }

    template <typename SepT, typename ...Args>
    static void append_join_args(DstT& dst, SepT&& sep, Args&&... args)
    {
        default_backend_formatter(dst).append_join_args(std::forward<SepT>(sep),std::forward<Args>(args)...);
    }

    template <typename SepT, typename PartsT>
    static void append_join(DstT& dst, SepT&& sep, PartsT&& parts)
    {
        default_backend_formatter(dst).append_join(std::forward<SepT>(sep),std::forward<PartsT>(parts));
    }

    static auto wrap(DstT& dst)
    {
        return default_backend_formatter(dst);
    }
};

template <typename DstT>
struct backend_formatter_helper<DstT,
            hana::when<hana::is_a<backend_formatter_tag,DstT>>
        >
{
    template <typename ...Args>
    static void append(DstT& dst, Args&&... args)
    {
        dst.append(std::forward<Args>(args)...);
    }

    template <typename SepT, typename ...Args>
    static void append_join_args(DstT& dst, SepT&& sep, Args&&... args)
    {
        dst.append_join_args(std::forward<SepT>(sep),std::forward<Args>(args)...);
    }

    template <typename SepT, typename PartsT>
    static void append_join(DstT& dst, SepT&& sep, PartsT&& parts)
    {
        dst.append_join(std::forward<SepT>(sep),std::forward<PartsT>(parts));
    }

    static auto wrap(DstT& dst) -> decltype(auto)
    {
        return hana::id(dst);
    }
};

}

//-------------------------------------------------------------
DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_BACKEND_FORMATTER_HELPER_HPP
