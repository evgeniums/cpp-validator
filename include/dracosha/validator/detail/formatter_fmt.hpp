/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/formatter_fmt.hpp
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
#include <dracosha/validator/reporting/concrete_phrase.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/utils/unwrap_object.hpp>

namespace fmt
{

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

/**
 * @brief Define fmt formatter of concrete_phrase.
 */
template <>
struct formatter<concrete_phrase>
{
    template <typename FormatContext>
    auto format(const concrete_phrase& ph, FormatContext& ctx) {
        return format_to(ctx.out(),ph.text());
    }
};

template <typename T>
struct formatter<object_wrapper<T>>
{
    template <typename T1, typename FormatContext>
    auto format(const T1& v, FormatContext& ctx) {
        return format_to(ctx.out(),"{}",unwrap_object(v));
    }
};

}

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Join vector of parts and append to destination object.
 * @param dst Destination object.
 * @param sep Separator for joining.
 * @param parts Vector of parts to join and append.
 */
template <typename DstT, typename SepT, typename PartsT>
void fmt_append_join(DstT& dst, SepT&& sep, PartsT&& parts,
                     std::enable_if_t<!hana::is_a<hana::tuple_tag,PartsT>,void*> =nullptr)
{
    fmt::format_to(std::back_inserter(dst),"{}",fmt::join(std::forward<PartsT>(parts),std::forward<SepT>(sep)));
}

/**
 * @brief Join hana::tuple of parts and append to destination object.
 * @param dst Destination object.
 * @param sep Separator for joining.
 * @param parts hana::tuple of parts to join and append.
 */
template <typename DstT, typename SepT, typename PartsT>
void fmt_append_join(DstT& dst, SepT&& sep, PartsT&& parts,
                     std::enable_if_t<hana::is_a<hana::tuple_tag,PartsT>,void*> =nullptr)
{
    fmt::format_to(std::back_inserter(dst),"{}",fmt::join(hana_to_std_tuple(std::forward<PartsT>(parts)),std::forward<SepT>(sep)));
}

/**
 * @brief Append arguments to destination object.
 */
template <typename DstT, typename SepT, typename ...Args>
void fmt_append_join_args(DstT& dst, SepT&& sep, Args&&... args)
{
    fmt::format_to(std::back_inserter(dst),"{}",fmt::join(std::make_tuple(std::forward<Args>(args)...),std::forward<SepT>(sep)));
}

/**
 * @brief Append arguments to destination object.
 */
template <typename DstT, typename ...Args>
void fmt_append_args(DstT& dst, Args&&... args)
{
    fmt_append_join_args(dst,"",std::forward<Args>(args)...);
}

struct backend_formatter_tag;

/**
 * @brief Backend formatter that uses libfmt fot formatting.
 *
 * Destination object is normally a std::string.
 * Also it can be a container of chars that can deal with inserter iterators,
 * i.e. it must be suitable for using std::back_inserter(dst), dst.insert(), dst.begin(), dst.end().
 */
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

    DstT& get()
    {
        return _dst;
    }

    static fmt_backend_formatter<DstT> clone(DstT& dst)
    {
        return fmt_backend_formatter<DstT>{dst};
    }
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP
