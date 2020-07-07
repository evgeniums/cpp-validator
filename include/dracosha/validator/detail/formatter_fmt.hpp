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

/**
 * @brief Format string from variadic arguments using fmt backend
 */
struct fmt_formatter_t
{
    template <typename DstT, typename ...Args>
    static void append(DstT& dst, Args&&... args)
    {
        return fmt_append_args(dst,std::forward<Args>(args)...);
    }

    template <typename DstT, typename SepT, typename ...Args>
    static void append_join_args(DstT& dst, SepT&& sep, Args&&... args)
    {
        return fmt_append_join_args(dst,std::forward<SepT>(sep),std::forward<Args>(args)...);
    }

    template <typename DstT,  typename SepT, typename PartsT>
    static void append_join(DstT& dst, SepT&& sep, PartsT&& parts)
    {
        fmt_append_join(dst,std::forward<SepT>(sep),std::forward<PartsT>(parts));
    }
};

/**
 * @brief Append arguments to string using fmt backend
 */
struct fmt_append_t
{
    template <typename DstT,typename ...Args>
    void operator () (DstT&& dst, Args&&... args) const
    {
        fmt_formatter_t::append(extract_ref(std::forward<DstT>(dst)),std::forward<Args>(args)...);
    }
};

/**
 * @brief Append arguments to string using fmt backend
 */
struct fmt_append_join_args_t
{
    template <typename DstT, typename SepT,typename ...Args>
    void operator () (DstT&& dst, SepT&& sep, Args&&... args) const
    {
        fmt_formatter_t::append_join_args(extract_ref(std::forward<DstT>(dst)),std::forward<SepT>(sep),std::forward<Args>(args)...);
    }
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP
