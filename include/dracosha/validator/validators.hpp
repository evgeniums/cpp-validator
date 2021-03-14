/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/detail/validator_impl.hpp
*
*  Defines implementation of validator.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP
#define DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP

#include <string>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct validator_tag;

template <typename ValidatorT, typename HintT>
class validator_with_hint_t : public ValidatorT
{
    public:

        using hint_type=typename adjust_storable_type<HintT>::type;

        template <typename HintT1, typename ... Args>
        validator_with_hint_t(HintT1&& hint, Args&&... args)
            : ValidatorT(std::forward<Args>(args)...),
              _hint(std::forward<HintT1>(hint))
        {
        }

        /**
         * @brief Apply validation to adapter.
         * @param a Adapter or object to validate.
         * @param args Arguments to pass for validation.
         * @return Validation status.
         */
        template <typename AdapterT, typename ... Args>
        auto apply(AdapterT&& a, Args&&... args) const
        {
            auto&& adpt=ensure_adapter(std::forward<AdapterT>(a));

            auto ret=adpt.hint_before(adpt,_hint);
            if (!ret)
            {
                return adpt.hint_after(adpt,ret,_hint);
            }
            ret=ValidatorT::apply(adpt,std::forward<Args>(args)...);
            return adpt.hint_after(adpt,ret,_hint);
        }

    private:

        hint_type _hint;
};

/**
 * @brief Validator.
 *
 * Validator holds the validating executer and invokes apply() method to perform validation
 * using embedded executor.
 */
template <typename HandlerT>
class validator_t
{
    public:

        using hana_tag=validator_tag;
        using with_check_exists=typename HandlerT::with_check_exists;

        bool check_exists_operand;

        /**
         * @brief Construtor
         * @param fn Validation HandlerT that will be called to perform validating.
         */
        validator_t(HandlerT fn, bool must_exist=false)
                : check_exists_operand(must_exist),
                  _fn(std::move(fn))
        {}

        /**
         * @brief Invoke validating with supplied args.
         * @param adpt Adapter or object to validate.
         * @param args Arguments to forward to embedded executor.
         * @return Validation status.
         */
        template <typename AdapterT, typename ... Args>
        auto apply(AdapterT&& adpt, Args&&... args) const
        {
            return _fn(ensure_adapter(std::forward<AdapterT>(adpt)),std::forward<Args>(args)...);
        }

        /**
         * @brief Create validator with hint.
         * @param h Hint.
         * @return Validator with hint.
         *
         * This object becomes invalid as it is moved to new validator.
         */
        template <typename T>
        auto hint(T&& h)
        {
            return validator_with_hint_t<validator_t<HandlerT>,T>(std::forward<T>(h),std::move(_fn),check_exists_operand);
        }

        /**
         * @brief Create validator with hint.
         * @param h Hint.
         * @return Validator with hint.
         *
         * This object becomes invalid as it is moved to new validator.
         */
        template <typename T>
        auto operator () (T&& h)
        {
            return hint(std::forward<T>(h));
        }

    private:

        HandlerT _fn;
};

template <typename MemberT, typename ValidatorT>
class validator_with_member_t
{
    public:

        using hana_tag=validator_tag;
        using with_check_exists=hana::false_;

        bool check_exists_operand;

        validator_with_member_t(MemberT member, ValidatorT v, bool =false)
            : check_exists_operand(false),
              _member(std::move(member)),
              _prepared_validator(std::move(v))
        {
        }

        /**
         * @brief Apply validation to adapter.
         * @param adpt Adapter or object to validate.
         * @return Validation status.
         */
        template <typename AdapterT>
        auto apply(AdapterT&& adpt) const
        {
            return apply_member(ensure_adapter(std::forward<AdapterT>(adpt)),_prepared_validator,_member);
        }

        template <typename AdapterT, typename SuperMemberT>
        auto apply(AdapterT&& adpt, SuperMemberT&& super) const
        {
            return apply_member(
                        ensure_adapter(std::forward<AdapterT>(adpt)),
                        _prepared_validator,
                        prepend_super_member(std::forward<SuperMemberT>(super),_member)
                    );
        }

        /**
         * @brief Create validator with hint.
         * @param h Hint.
         * @return Validator with hint.
         *
         * This object becomes invalid as it is moved to new validator.
         */
        template <typename T>
        auto hint(T&& h)
        {
            return validator_with_hint_t<validator_with_member_t<MemberT,ValidatorT>,T>(std::forward<T>(h),
                                                                            std::move(_member),
                                                                            std::move(_prepared_validator),
                                                                            check_exists_operand
                                                                            );
        }

        /**
         * @brief Create validator with hint.
         * @param h Hint.
         * @return Validator with hint.
         *
         * This object becomes invalid as it is moved to new validator.
         */
        template <typename T>
        auto operator () (T&& h)
        {
            return hint(std::forward<T>(h));
        }

    private:

        MemberT _member;
        ValidatorT _prepared_validator;
};

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP
