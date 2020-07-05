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

#include <fmt/format.h>

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

template <char Sep>
struct fmt_single_element
{
    constexpr static auto value=hana::sum<hana::string_tag>(hana::make_tuple(hana::string_c<Sep>,hana::string_c<'{', '}'>));
};
template <>
struct fmt_single_element<0>
{
    constexpr static auto value=hana::string_c<'{', '}'>;
};

/**
 * @brief Append arguments to destination object
 */
template <char Sep, typename DstT, typename ...Args>
void fmt_append_with_separator_args(DstT& dst, Args&&... args)
{
    constexpr auto fmt_str_single =fmt_single_element<Sep>::value;

    constexpr auto fmt_str1 = hana::string_c<'{', '}'>;
    constexpr auto fmt_str_rest= hana::replicate<hana::tuple_tag>(
                    fmt_str_single,
                    hana::size_c<sizeof...(Args)-1>
                );
    constexpr auto fmt_str_all=hana::prepend(fmt_str_rest,fmt_str1);
    constexpr auto fmt_str=hana::sum<hana::string_tag>(fmt_str_all);

    fmt::format_to(std::back_inserter(dst),hana::to<const char*>(fmt_str),std::forward<Args>(args)...);
}

template <typename DstT, typename PartsT, typename SepT>
void fmt_append_join(DstT& dst, PartsT&& parts, SepT&& sep)
{
    fmt::format_to(std::back_inserter(dst),"{}",fmt::join(std::forward<PartsT>(parts),std::forward<SepT>(sep)));
}

/**
 * @brief Append arguments to destination object
 */
template <typename DstT, typename ...Args>
void fmt_append_args(DstT& dst, Args&&... args)
{
    fmt_append_with_separator_args<static_cast<char>(0)>(dst,std::forward<Args>(args)...);
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

    template <typename DstT, typename ...Args>
    static void append_with_separator(DstT& dst, Args&&... args)
    {
        return fmt_append_with_separator_args<' '>(dst,std::forward<Args>(args)...);
    }

    template <typename DstT, typename PartsT, typename SepT>
    static void append_join(DstT& dst, PartsT&& parts, SepT&& sep)
    {
        fmt_append_join(dst,std::forward<PartsT>(parts),std::forward<SepT>(sep));
    }
};

/**
 * @brief Append arguments to string using fmt backend
 */
struct fmt_append_t
{
    template <typename DstT,typename ...Args>
    void operator () (DstT&& dst,Args&&... args) const
    {
        fmt_formatter_t::append(extract_ref(std::forward<DstT>(dst)),std::forward<Args>(args)...);
    }
};

/**
 * @brief Append arguments to string using fmt backend
 */
struct fmt_append_with_separator_t
{
    template <typename DstT,typename ...Args>
    void operator () (DstT&& dst,Args&&... args) const
    {
        fmt_formatter_t::append_with_separator(extract_ref(std::forward<DstT>(dst)),std::forward<Args>(args)...);
    }
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_FMT_HPP
