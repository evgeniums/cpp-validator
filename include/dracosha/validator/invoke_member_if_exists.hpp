/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/invoke_member_if_exists.hpp
*
*  Defines "invoke_member_if_exists".
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_INVOKE_MEMBER_IF_EXISTS_HPP
#define DRACOSHA_VALIDATOR_INVOKE_MEMBER_IF_EXISTS_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/embedded_object.hpp>
#include <dracosha/validator/status.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Implementer of invoke_member_if_exists().
 */
struct invoke_member_if_exists_impl
{
    template <typename FnT, typename AdapterT, typename MemberT>
    status operator () (FnT&& fn, AdapterT&& adapter, MemberT&& member) const
    {
        auto invoke=[](auto&& fn, auto&& adapter, auto&& member)
        {
            return fn(std::forward<decltype(adapter)>(adapter),std::forward<decltype(member)>(member));
        };

        using type=typename std::decay_t<AdapterT>::type;
        return hana::eval_if(
            hana::and_(
                typename type::filter_if_not_exists{},
                hana::not_(intermediate_ignore_check_exist(adapter,member))
            ),
            [&](auto&& _)
            {
                if (std::decay_t<FnT>::with_check_exists::value)
                {
                    auto ret=traits_of(adapter).validate_exists(
                                        _(adapter),
                                        _(member),
                                        _(fn).exists_operator,
                                        _(fn).check_exists_operand
                                    );
                    if (!ret)
                    {
                        return ret;
                    }
                }
                else
                {
                    if (!embedded_object_has_member(_(adapter),_(member)))
                    {
                        auto not_found_status=traits_of(_(adapter)).not_found_status();
                        if (not_found_status.value()==status::code::fail)
                        {
                            // some adapters need to know that member is not found
                            // for example, reporting adapter need it to construct corresponding report
                            traits_of(adapter).validate_exists(
                                                _(adapter),
                                                _(member),
                                                _(fn).exists_operator,
                                                true,
                                                false,
                                                true
                                            );
                        }
                        return not_found_status;
                    }
                }

                return hana::eval_if(
                    is_embedded_object_path_valid(_(adapter),_(member).path()),
                    [&](auto&& _)
                    {
                        return status(_(invoke(_(fn),_(adapter),_(member))));
                    },
                    [&](auto&&)
                    {
                        return status(status::code::ignore);
                    }
                );
            },
            [&](auto&& _)
            {
                return status(_(invoke(_(fn),_(adapter),_(member))));
            }
        );
    }
};
/**
 * @brief Helper for checking member existence before invoking validation handler.
 * @param fn Validation handler.
 * @param adapter Validation adapter.
 * @param member Member under validation.
 * @return Validation result.
 *
 * Actual checking of member existence is performed only if adapter allows such check.
 * But anyway, checking if member's path is valid for the object is performed always at compilation time.
 */
constexpr invoke_member_if_exists_impl invoke_member_if_exists{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_INVOKE_MEMBER_IF_EXISTS_HPP
