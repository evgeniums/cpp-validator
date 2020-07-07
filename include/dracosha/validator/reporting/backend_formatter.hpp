/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/backend_formatter.hpp
*
*  Defines backend formatter.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_BACKEND_FORMATTER_HPP
#define DRACOSHA_VALIDATOR_BACKEND_FORMATTER_HPP

#include <dracosha/validator/config.hpp>

#ifdef DRACOSHA_VALIDATOR_FMT
#include <dracosha/validator/detail/formatter_fmt.hpp>
#else
#include <dracosha/validator/detail/formatter_std.hpp>
#endif

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

#ifdef DRACOSHA_VALIDATOR_FMT
constexpr detail::fmt_formatter_t backend_formatter{};
constexpr detail::fmt_append_join_args_t formatter_append_join_args{};
#else
constexpr detail::std_formatter_t backend_formatter{};
constexpr detail::std_append_join_args_t formatter_append_join_args{};
#endif

//-------------------------------------------------------------
DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_BACKEND_FORMATTER_HPP
