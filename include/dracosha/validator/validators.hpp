/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/validator.hpp
*
*  Defines validator templates.
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP
#define DRACOSHA_VALIDATOR_VALIDATOR_IMPL_HPP

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>
#include <dracosha/validator/adapters/make_intermediate_adapter.hpp>
#include <dracosha/validator/prepend_super_member.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

struct validator_tag;

//-------------------------------------------------------------

/**
 * @brief Base template for validators with explicit description hints.
 */
template <typename ValidatorT, typename HintT>
class validator_with_hint_t : public ValidatorT
{
    public:

        using hint_type=typename adjust_storable_type<HintT>::type;

        /**
         * @brief Constructor.
         * @param hint Description hint.
         * @param args Arguments to forward to constructor of original validator.
         */
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

//-------------------------------------------------------------

/**
 * @brief Validator.
 *
 * Validator holds the validating executer and invokes apply() method to perform validation
 * using embedded executor.
 */
template <typename HandlerT, typename ExistsOperatorT=exists_t>
class validator_t
{
    public:

        using hana_tag=validator_tag;
        using with_check_exists=typename HandlerT::with_check_exists;

        const bool check_exists_operand;
        const ExistsOperatorT& exists_operator;

        /**
         * @brief Construtor
         * @param fn Validation HandlerT that will be called to perform validating.
         */
        validator_t(HandlerT fn,
                    bool must_exist=false,
                    const ExistsOperatorT& exists_op=exists
                    )
                : check_exists_operand(must_exist),
                  exists_operator(exists_op),
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
            return validator_with_hint_t<validator_t<HandlerT,ExistsOperatorT>,T>(
                        std::forward<T>(h),
                        std::move(_fn),
                        check_exists_operand,
                        exists_operator
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

        HandlerT _fn;
};

//-------------------------------------------------------------

/**
 * @brief Base template for member validators.
 */
template <typename MemberT, typename ValidatorT, typename ExistsOperatorT=exists_t>
class validator_with_member_t
{
    public:

        using hana_tag=validator_tag;
        using with_check_exists=hana::false_;

        const bool check_exists_operand;
        const exists_t& exists_operator;

        /**
         * @brief Constructor.
         * @param member Member.
         * @param v Validation handler.
         */
        validator_with_member_t(MemberT member, ValidatorT v, bool =false, const ExistsOperatorT& =exists)
            : check_exists_operand(false),
              exists_operator(exists),
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
            auto&& adapter=ensure_adapter(std::forward<AdapterT>(adpt));
            auto tmp_adapter=make_intermediate_adapter(adapter,path_of(super));
            return apply_member(
                        tmp_adapter,
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
            return validator_with_hint_t<validator_with_member_t<MemberT,ValidatorT,ExistsOperatorT>,T>(std::forward<T>(h),
                                                                            std::move(_member),
                                                                            std::move(_prepared_validator),
                                                                            check_exists_operand,
                                                                            exists_operator
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
