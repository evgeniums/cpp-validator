/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/formatter_fmt.hpp
*
*  Defines formatter that uses fmt library for strings formatting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP
#define DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP

#include <fmt/ranges.h>
#include <fmt/format.h>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/hana_to_std_tuple.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <typename DstT, typename SepT, typename PartsT>
void fmt_append_join(DstT& dst, SepT&& sep, PartsT&& parts,
                     std::enable_if_t<!hana::is_a<hana::tuple_tag,PartsT>,void*> =nullptr)
{
    fmt::format_to(std::back_inserter(dst),"{}",fmt::join(std::forward<PartsT>(parts),std::forward<SepT>(sep)));
}

template <typename DstT, typename SepT, typename PartsT>
void fmt_append_join(DstT& dst, SepT&& sep, PartsT&& parts,
                     std::enable_if_t<hana::is_a<hana::tuple_tag,PartsT>,void*> =nullptr)
{
    fmt::format_to(std::back_inserter(dst),"{}",fmt::join(hana_to_std_tuple(std::forward<PartsT>(parts)),std::forward<SepT>(sep)));
}

/**
 * @brief Append arguments to destination object
 */
template <typename DstT, typename SepT, typename ...Args>
void fmt_append_join_args(DstT& dst, SepT&& sep, Args&&... args)
{
    fmt::format_to(std::back_inserter(dst),"{}",fmt::join(std::make_tuple(std::forward<Args>(args)...),std::forward<SepT>(sep)));
}

/**
 * @brief Append arguments to destination object
 */
template <typename DstT, typename ...Args>
void fmt_append_args(DstT& dst, Args&&... args)
{
    fmt_append_join_args(dst,"",std::forward<Args>(args)...);
}

struct backend_formatter_tag;

template <typename DstT>
struct fmt_backend_formatter
{
    using hana_tag=backend_formatter_tag;
    using type=DstT;

    DstT& _dst;

    template <typename ...Args>
    void append(Args&&... args)
    {
        fmt_append_args(_dst,std::forward<Args>(args)...);
    }

    template <typename SepT, typename ...Args>
    void append_join_args(SepT&& sep, Args&&... args)
    {
        fmt_append_join_args(_dst,std::forward<SepT>(sep),std::forward<Args>(args)...);
    }

    template <typename SepT, typename PartsT>
    void append_join(SepT&& sep, PartsT&& parts)
    {
        fmt_append_join(_dst,std::forward<SepT>(sep),std::forward<PartsT>(parts));
    }

    operator DstT& ()
    {
        return _dst;
    }
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP
