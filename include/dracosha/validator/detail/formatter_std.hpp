/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/detail/formatter_std.hpp
*
*  Defines formatter that uses std::stringstream for strings formatting.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FORMATTER_STD_HPP
#define DRACOSHA_VALIDATOR_FORMATTER_STD_HPP

#include <string>
#include <sstream>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Append arguments to destination string
 * @param dst Destination string
 * @param sep Separator for joining arguments
 * @param parts Vector to join and append to string
 */
template <typename PartsT, typename SepT>
void std_append_join(std::string& dst, SepT&& sep, PartsT&& parts,
                     std::enable_if_t<!hana::is_a<hana::tuple_tag,PartsT>,void*> =nullptr)
{
    size_t i=0;
    std::stringstream ss;
    ss<<dst;
    for (auto&& it:parts)
    {
        if (i++!=0)
        {
            ss<<sep;
        }
        ss<<it;
    }
    dst=ss.str();
}

/**
 * @brief Append arguments to destination string
 * @param dst Destination string
 * @param sep Separator for joining arguments
 * @param parts Hana tuple to join and append to string
 */
template <typename PartsT, typename SepT>
void std_append_join(std::string& dst, SepT&& sep, PartsT&& parts,
                     std::enable_if_t<hana::is_a<hana::tuple_tag,PartsT>,void*> =nullptr)
{
    std::stringstream ss;
    ss<<dst;
    hana::fold(
        std::forward<PartsT>(parts),
        0u,
        [&ss,&sep](size_t i,auto&& v)
        {
            if (i!=0u)
            {
                ss<<sep;
            }
            ss<<extract_ref(std::forward<decltype(v)>(v));
            return i+1;
        }
    );
    dst=ss.str();
}

/**
 * @brief Append arguments to destination string
 * @param dst Destination string
 * @param sep Separator for joining arguments
 * @param args Arguments to join and append to string
 */
template <typename SepT, typename ...Args>
void std_append(std::string& dst, SepT&& sep, Args&&... args)
{
    std_append_join(dst,std::forward<SepT>(sep),make_cref_tuple(std::forward<Args>(args)...));
}

/**
 * @brief Format string from variadic arguments using std::stringstream backend
 */
struct std_formatter_t
{
    template <typename ...Args>
    static void append(std::string& dst, Args&&... args)
    {
        return std_append(dst,"",std::forward<Args>(args)...);
    }

    template <typename SepT, typename ...Args>
    static void append_join_args(std::string& dst, SepT&& sep, Args&&... args)
    {
        return std_append(dst,std::forward<SepT>(sep),std::forward<Args>(args)...);
    }

    template <typename SepT, typename PartsT>
    static void append_join(std::string& dst, SepT&& sep, PartsT&& parts)
    {
        return std_append_join(dst,std::forward<SepT>(sep),std::forward<PartsT>(parts));
    }
};

/**
 * @brief Append arguments to string using std::stringstream backend
 */
struct std_append_t
{
    template <typename DstT, typename ...Args>
    void operator () (DstT&& dst, Args&&... args) const
    {
        std_formatter_t::append(extract_ref(std::forward<DstT>(dst)),std::forward<Args>(args)...);
    }
};

/**
 * @brief Append arguments to string using std::stringstream backend
 */
struct std_append_join_args_t
{
    template <typename DstT, typename SepT, typename ...Args>
    void operator () (DstT&& dst, SepT&& sep, Args&&... args) const
    {
        std_formatter_t::append_join_args(extract_ref(std::forward<DstT>(dst)),std::forward<SepT>(sep),std::forward<Args>(args)...);
    }
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_STD_HPP
