/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/cref.hpp
*
*  Defines class for holding constant reference and helpers for that class
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_CREF_HPP
#define DRACOSHA_VALIDATOR_CREF_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/detail/extract_cref.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct cref_tag;

/**
 * @brief Copyable wrapper of constant references
 */
template <typename T>
struct cref_t
{
    using hana_tag=cref_tag;
    using type=T;

    /**
     * @brief Ctor
     * @param v Const reference to kepp
     */
    cref_t(const T& v)
        : _v(v)
    {}

    /**
     * @brief Get constant reference
     * @return Constance reference to value
     */
    const T& get() const
    {
        return _v;
    }

    /**
     * @brief Get constant reference
     * @return Constance reference to value
     */
    operator const T& () const
    {
        return _v;
    }

    /**
     * @brief Get type of the value
     * @return Hana type_c object
     */
    constexpr static auto type_c()
    {
        return hana::type_c<T>;
    }

    private:

        const T& _v;
};

/**
  @brief Create a cref object
  @param v Const reference to wrap into cref object
  @return cref object
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto cref =[](const auto& v)
{
    return cref_t<std::remove_reference_t<decltype(v)>>{v};
};

/**
  @brief Extract const reference from argument
  @param cr Value to extract const reference from.
  @return If cr is of cref type than kept reference is returned, othervise the value itself is returned
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto extract_cref =[](auto&& cr) -> decltype(auto)
{
    return detail::extract_cref<decltype(cr)>(std::forward<decltype(cr)>(cr));
};

/**
  @brief Invoke a function with an argument.
  Function or argument or both can be either of cref or some other type.

  @param fn Callable onject or cref of callable object
  @param cr Argument to fn or cref of argument to fn
  @return Result of fn invokation
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto apply_cref =[](auto&& fn,auto&& cr) -> decltype(auto)
{
    return extract_cref(fn)(extract_cref(std::forward<decltype(cr)>(cr)));
};

/**
  @brief Make a tuple of cref that wrap input arguments
  @param v Input argument cref to which to put into the tuple
  @return Tuple with cref objects
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto make_cref_tuple =[](auto&&... v) -> decltype(auto)
{
    return hana::make_tuple(cref(std::forward<decltype(v)>(v))...);
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_CREF_HPP
