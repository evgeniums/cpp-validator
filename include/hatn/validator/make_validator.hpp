/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/make_validator.hpp
*
*  Defines creator of validator instances.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_MAKE_HPP
#define HATN_VALIDATOR_MAKE_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/validators.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Create validator instance.
  @param fn Validation handler that will be embedded into validator instance.
*/
template <typename T>
auto make_validator(T fn)
{
    auto exists_params=content_of_check_exists(fn);
    return validator_t<
            T,
            std::decay_t<decltype(exists_params.second)>
      >(
        std::move(fn),
        exists_params.first,
        exists_params.second
       );
}

/**
  @brief Create member validator instance.
  @param fn Validation handler that will be embedded into validator instance.
*/
template <typename MemberT, typename ValidatorT>
auto make_member_validator(MemberT member, ValidatorT&& v)
{
    auto exists_params=content_of_check_exists(v);
    return validator_with_member_t<
            std::decay_t<MemberT>,
            std::decay_t<ValidatorT>,
            std::decay_t<decltype(exists_params.second)>
        >(
            std::forward<MemberT>(member),
            std::forward<ValidatorT>(v),
            exists_params.first,
            exists_params.second
        );
}

/**
  @brief Create validator instance on heap.
  @param fn Validation handler that will be embedded into validator instance.
*/
template <typename T>
auto make_validator_on_heap(T fn)
{
    auto exists_params=content_of_check_exists(fn);
    return new validator_t<
            decltype(fn),
            std::decay_t<decltype(exists_params.second)>
        >(
            std::move(fn),
            exists_params.first,
            exists_params.second
        );
}

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_MAKE_HPP
