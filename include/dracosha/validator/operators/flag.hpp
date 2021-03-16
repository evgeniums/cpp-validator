/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/operators/flag.hpp
*
* Defines flag operator for validation of booleans.
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_FLAG_HPP
#define DRACOSHA_VALIDATOR_FLAG_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/operators/operator.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/reporting/flag_presets.hpp>
#include <dracosha/validator/properties/value.hpp>
#include <dracosha/validator/utils/safe_compare.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Base class for "flag" operator.
 *
 * Base flag operator tries to use flag description defined in the property.
 * If that is not applicable then the default preset flag description is used.
 */
struct flag_t : public adjust_storable_ignore
{
    using hana_tag=operator_tag;

    /**
     * @brief Comparison operator.
     * @param a Left operand.
     * @param b Right operand.
     * @return Result of "is equal" operator.
     */
    template <typename T1, typename T2>
    constexpr bool operator() (const T1& a, const T2& b) const
    {
        return safe_compare_equal(a,b);
    }

    /**
     * @brief Get flag description if flag is used with a property and property is not a value.
     * @param prop Property.
     * @param b Operand.
     * @param of_member Property must use "property of member" order.
     * @return Flag description taken from the property if it has flag strings or default preset flag description otherwise.
     */
    template <typename PropT, typename FormatterT>
    std::string str(PropT&& prop,
                    const bool& b,
                    const FormatterT& formatter,
                    bool of_member=false
            ) const
    {
        return hana::eval_if(
            hana::and_(
                hana::is_a<property_tag,PropT>,
                hana::not_(std::is_same<std::decay_t<PropT>,type_p_value>{})
            ),
            [&](auto&& _)
            {
                return std::decay_t<PropT>::has_flag_str() ?
                        _(prop).flag_str(_(b),_(formatter),_(of_member))
                        :
                        default_flag_preset(_(b));
            },
            [&](auto&& _)
            {
                return default_flag_preset(_(b));
            }
        );
    }

    /**
     * @brief Check if property name should be prepended to the flag description.
     *
     * Property name should be prepended if the property is not of "value" type and it does not have flag strings.
     */
    template <typename PropT>
    constexpr static bool prepend_property(PropT&&)
    {
        return !std::is_same<std::decay_t<PropT>,type_p_value>::value && !std::decay_t<PropT>::has_flag_str();
    }
};

/**
 * @brief Flag operator with one of preset descriptions.
 */
template <typename T>
struct flag_op_with_preset : public flag_t
{
    public:

        /**
         * @brief Constructor.
         * @param v Preset descriptions helper.
         */
        template <typename T1>
        flag_op_with_preset(T1&& v) : _preset(std::forward<T1>(v))
        {}

        /**
         * @brief Get flag description.
         * @param b Operand.
         * @return Preset flag description.
         */
        template <typename PropT, typename FormatterT>
        std::string str(PropT&&,const bool& b, const FormatterT&, bool =false) const
        {
            return _preset(b);
        }

        /**
         * @brief Check if property name should be prepended to the flag description.
         *
         * Property name is prepended to all properties except for "value" property.
         */
        template <typename PropT>
        constexpr static bool prepend_property(PropT&&)
        {
            return !std::is_same<std::decay_t<PropT>,type_p_value>::value;
        }

    private:

        T _preset;
};

/**
 * @brief Flag operator with explicit string description.
 */
struct flag_op_with_string : public flag_t
{
    public:

        /**
         * @brief Constructor.
         * @param v Explicit flag description.
         */
        template <typename T1>
        flag_op_with_string(T1&& v) : _string(std::forward<T1>(v))
        {}

        /**
         * @brief Get flag description.
         * @return Explicit flag description.
         */
        template <typename PropT, typename FormatterT>
        std::string str(PropT&&,const bool&, const FormatterT&, bool =false) const
        {
            return _string;
        }

        /**
         * @brief Check if property name should be prepended to the flag description.
         *
         * Property name should be prepended if the property is not of "value" type and it does not have flag strings.
         */
        template <typename PropT>
        constexpr static bool prepend_property(PropT&&)
        {
            return !std::is_same<std::decay_t<PropT>,type_p_value>::value;
        }

    private:

        std::string _string;
};

/**
 * @brief Flag operator to be used in validators.
 */
struct flag_op : public flag_t
{
    using flag_t::operator ();

    /**
     * @brief Create flag operator with preset descriptions.
     * @param v Preset descriptions helper.
     * @return flag operator.
     */
    template <typename T>
    auto operator() (T&& v,
                     std::enable_if_t<
                        hana::is_a<flag_preset_tag,T>,
                        void*
                     > =nullptr) const
    {
        return flag_op_with_preset<T>{std::forward<T>(v)};
    }

    /**
     * @brief Create flag operator with explicit string descriptions.
     * @param v Explicit string description.
     * @return flag operator.
     */
    template <typename T>
    auto operator() (T&& v,
                     std::enable_if_t<
                        std::is_constructible<std::string,T>::value,
                        void*
                     > =nullptr) const
    {
        return flag_op_with_string{std::forward<T>(v)};
    }
};

/**
* @brief Flag operator for validation of booleans.
*
* Examples of operator usage:
*
* <pre>
  empty(flag,true) -> must be empty
  empty(flag,false) -> must be not empty
  empty(flag(flag_on_off),true) -> must be on
  empty(flag(flag_on_off),false) -> must be off
  empty(flag("custom description"),true) -> custom description
  </pre>
*
*/
constexpr flag_op flag{};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_FLAG_HPP
