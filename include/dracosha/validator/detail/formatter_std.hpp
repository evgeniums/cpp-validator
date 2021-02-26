/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/formatter_std.hpp
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
 * @brief Append arguments to destination string.
 * @param dst Destination string.
 * @param sep Separator for joining arguments.
 * @param parts Vector to join and append to string.
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
 * @brief Append arguments to destination string.
 * @param dst Destination string.
 * @param sep Separator for joining arguments.
 * @param parts Hana tuple to join and append to string.
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
 * @brief Append arguments to destination string.
 * @param dst Destination string.
 * @param sep Separator for joining arguments.
 * @param args Arguments to join and append to string.
 */
template <typename SepT, typename ...Args>
void std_append(std::string& dst, SepT&& sep, Args&&... args)
{
    std_append_join(dst,std::forward<SepT>(sep),make_cref_tuple(std::forward<Args>(args)...));
}

struct backend_formatter_tag;

/**
 * @brief Backend formatter that uses std::stringstream fot formatting.
 */
struct std_backend_formatter
{
    using hana_tag=backend_formatter_tag;
    using type=std::string;

    std::string& _dst;

    template <typename ...Args>
    void append(Args&&... args)
    {
        return std_append(_dst,"",std::forward<Args>(args)...);
    }

    template <typename SepT, typename ...Args>
    void append_join_args(SepT&& sep, Args&&... args)
    {
        return std_append(_dst,std::forward<SepT>(sep),std::forward<Args>(args)...);
    }

    template <typename SepT, typename PartsT>
    void append_join(SepT&& sep, PartsT&& parts)
    {
        return std_append_join(_dst,std::forward<SepT>(sep),std::forward<PartsT>(parts));
    }

    operator std::string& ()
    {
        return _dst;
    }

    std::string& get()
    {
        return _dst;
    }

    static std_backend_formatter clone(std::string& dst)
    {
        return std_backend_formatter{dst};
    }
};

}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FORMATTER_STD_HPP
