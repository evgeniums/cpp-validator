/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/utils/reference_wrapper.hpp
*
*  Defines reference wrapper whith hana tag and helpers for that wrapper
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_REFERENCE_WRAPPER_HPP
#define DRACOSHA_VALIDATOR_REFERENCE_WRAPPER_HPP

#include <dracosha/validator/config.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct ref_tag;

//-------------------------------------------------------------

namespace detail
{

/**
 * @brief Helper to extract value from object or reference.
 */
template <typename T, typename =hana::when<true>>
struct extract_ref_t
{
};

/**
 * @brief Return the value itself if it is not a reference wrapper
 */
template <typename T>
struct extract_ref_t<T,
                    hana::when<!hana::is_a<ref_tag,T>>>
{
    constexpr auto operator() (T&& v) const -> decltype(auto)
    {
        return hana::id(std::forward<T>(v));
    }
};

/**
 * @brief Extract reference if value is a reference wrapper
 */
template <typename T>
struct extract_ref_t<T,
                    hana::when<hana::is_a<ref_tag,T>>>
{
    constexpr auto operator() (T&& v) const -> decltype(auto)
    {
        return v.get();
    }
};
template <typename T>
constexpr extract_ref_t<T> extract_ref_impl{};
}

//-------------------------------------------------------------

/**
 * @brief Copyable wrapper of references
 */
template <typename T>
struct reference_wrapper
{
    using hana_tag=ref_tag;
    using type=T;

    /**
     * @brief Ctor
     * @param v Const reference to kepp
     */
    reference_wrapper(T& v)
        : _v(v)
    {}

    /**
     * @brief Get constant reference
     * @return Constance reference to value
     */
    T& get() const
    {
        return _v;
    }

    /**
     * @brief Get constant reference
     * @return Constance reference to value
     */
    operator T& () const
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

        T& _v;
};

/**
  @brief Create a const reference wrapper
  @param v Object to wrap
  @return Constant reference wrapper
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto cref =[](const auto& v)
{
    return reference_wrapper<std::remove_reference_t<decltype(v)>>{v};
};

/**
  @brief Create a reference wrapper
  @param v Object to wrap
  @return Reference wrapper
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto ref =[](auto& v)
{
    return reference_wrapper<std::remove_reference_t<decltype(v)>>{v};
};

/**
  @brief Extract reference from argument
  @param vr Value to extract a reference from.
  @return If vr is a reference wrapper than kept reference is returned, othervise the value itself is returned
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto extract_ref =[](auto&& vr) -> decltype(auto)
{
    return detail::extract_ref_impl<decltype(vr)>(std::forward<decltype(vr)>(vr));
};

/**
  @brief Invoke a function with an argument.
  Function or argument or both can be either of vref or some other type.

  @param fn Callable onject or vref of callable object
  @param vr Argument to fn or vref of argument to fn
  @return Result of fn invokation
*/
BOOST_HANA_CONSTEXPR_LAMBDA auto apply_ref =[](auto&& fn,auto&& vr, auto&&... args) -> decltype(auto)
{
    return extract_ref(fn)(extract_ref(std::forward<decltype(vr)>(vr)),std::forward<decltype(args)>(args)...);
};

/**
  @brief Make a tuple of const reference wrappers that wrap a pack of arguments
  @param v A pack of arguments to put into the tuple wrapped in const references
  @return Tuple with constant reference wrappers
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto make_cref_tuple =[](auto&&... v) -> decltype(auto)
{
    return hana::make_tuple(cref(std::forward<decltype(v)>(v))...);
};

/**
  @brief Make a tuple of reference wrappers that wrap a pack of arguments
  @param v A pack of arguments to put into the tuple wrapped in references
  @return Tuple with reference wrappers
  */
BOOST_HANA_CONSTEXPR_LAMBDA auto make_ref_tuple =[](auto&&... v) -> decltype(auto)
{
    return hana::make_tuple(ref(std::forward<decltype(v)>(v))...);
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_REFERENCE_WRAPPER_HPP
