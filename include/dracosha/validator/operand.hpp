/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operand.hpp
*
*  Defines operand wrapper.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_OPERAND_HPP
#define DRACOSHA_VALIDATOR_OPERAND_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/object_wrapper.hpp>
#include <dracosha/validator/reporting/concrete_phrase.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct master_sample_tag;
struct operand_tag;

/**
 * @brief Operand wrapper.
 *
 * Operand wrapper can be used to customize formatting of operand value.
 */
template <typename T>
class operand : public object_wrapper<T>
{
    public:

        using hana_tag=operand_tag;
        constexpr static const bool is_master_sample=hana::is_a<master_sample_tag,T>;

        /**
         * @brief Constructor.
         * @param val Operand's value.
         * @param description String to use in report formatting to represent the operand.
         */
        operand(
                T&& val,
                std::string description=std::string()
            ) : object_wrapper<T>(std::forward<T>(val)),
                _description(std::move(description))
        {}

        /**
         * @brief Operator of conversion to std::string.
         */
        operator std::string() const
        {
            return _description;
        }

    private:

        concrete_phrase _description;
};

/**
  @brief Extract value from operand.
  @param v Input.
  @return Either v as is or v.get() if input is of operand_tag.
  */
template <typename Tv>
auto extract_operand(Tv&& v) ->decltype(auto)
{
  return hana::if_(hana::is_a<operand_tag,decltype(v)>,
    [](auto&& x) -> decltype(auto) { return x.get(); },
    [](auto&& x) -> decltype(auto) { return hana::id(std::forward<decltype(x)>(x)); }
  )(std::forward<decltype(v)>(v));
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_OPERAND_HPP
