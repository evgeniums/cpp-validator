/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/extract.hpp
*
*  Defines method to extract value from operand.
*
*/

/****************************************************************************/

#ifndef HATN_VALIDATOR_EXTRACT_HPP
#define HATN_VALIDATOR_EXTRACT_HPP

#include <hatn/validator/config.hpp>
#include <hatn/validator/lazy.hpp>
#include <hatn/validator/operand.hpp>

HATN_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
  @brief Extract value from argument.
  @param v Argument can be plain value or lazy handler.
  @return Either input argument or result of handler evaluation if input is a lazy handler.
  */
template <typename Tv>
auto extract(Tv&& v) -> decltype(auto)
{
  return hana::if_(hana::is_a<lazy_tag,decltype(v)>,
    [](auto&& x) -> decltype(auto) { return extract_operand(x()); },
    [](auto&& x) -> decltype(auto) { return extract_operand(std::forward<decltype(x)>(x)); }
  )(std::forward<decltype(v)>(v));
}

//-------------------------------------------------------------

HATN_VALIDATOR_NAMESPACE_END

#endif // HATN_VALIDATOR_EXTRACT_HPP
