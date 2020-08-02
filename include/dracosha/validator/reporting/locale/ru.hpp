/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** \file validator/reporting/locale/ru.hpp
*
*   Defines strings for localization for Russian language
*
*/

/****************************************************************************/

#ifndef DRACOSHA_VALIDATOR_RU_LOCALE_HPP
#define DRACOSHA_VALIDATOR_RU_LOCALE_HPP

#include <mutex>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/master_sample.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/operators.hpp>
#include <dracosha/validator/operators/invert_op.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/operand_formatter.hpp>
#include <dracosha/validator/reporting/aggregation_strings.hpp>
#include <dracosha/validator/reporting/flag_presets.hpp>
#include <dracosha/validator/reporting/phrase_translator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

enum grammar_ru : uint8_t
{
    muzhskoy_rod,
    zhensky_rod,
    sredny_rod,

    mn_chislo,

    imenitelny_padezh,
    roditelny_padezh,
    datelny_padezh,
    vinitelny_padezh,
    tvoritelny_padezh,
    predlozhny_padezh,

    END_=32
};

/**
 * @brief Construct translator of validator strings for Russian locale.
 * @return Translator of system phrases for Russian locale
 *
 */
inline const phrase_translator& validator_translator_ru()
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    static phrase_translator m;
    if (m.empty())
    {
        // logical
        m[string_and.conjunction_str()]=" И "; // " AND "
        m[string_or.conjunction_str()]=" ИЛИ "; // " OR "
        m[string_not.open_str()]="НЕТ "; // "NOT "
        m[string_invert_op]="НЕТ"; // "NOT"

        // specials

        m[string_conjunction_of]={{{"",grammar_ru::roditelny_padezh}}}; // "of"
        m[string_member_name_conjunction]={{{" ",grammar_ru::roditelny_padezh}}}; // " of "

        m[string_true]="истина"; // "true"
        m[string_false]="ложь"; // "false"

        m[string_master_sample]={
                                    {"образец"},
                                    {"образца",grammar_ru::roditelny_padezh}
                                }; // "sample"
        m[string_empty]={
                            {"должен быть пустым"},
                            {"должна быть пустой",grammar_ru::zhensky_rod},
                            {"должно быть пустым",grammar_ru::sredny_rod},
                            {"должны быть пустыми",grammar_ru::mn_chislo}
                        }; // "must be empty"
        m[string_not_empty]={
                                {"должен быть не пустым"},
                                {"должна быть не пустой",grammar_ru::zhensky_rod},
                                {"должно быть не пустым",grammar_ru::sredny_rod},
                                {"должны быть не пустыми",grammar_ru::mn_chislo}
                            }; // "must be not empty"

        m[string_element]={
                            {"элемент #"},
                            {"элемента #",grammar_ru::roditelny_padezh}
                          }; // "element #"
        m[string_any]={
                            {"хотя бы один элемент"},
                            {"хотя бы одного элемента",grammar_ru::roditelny_padezh}
                      }; // "at least one element"
        m[string_all]={
                            {"каждый элемент"},
                            {"каждого элемента",grammar_ru::roditelny_padezh}
                      }; // "each element"

        // flag descriptions
        m[flag_true_false.str()]="должен быть истинным"; // "must be true"
        m[flag_true_false.n_str()]="должен быть ложным"; // "must be false"
        m[flag_on_off.str()]="должен быть включен"; // "must be on"
        m[flag_on_off.n_str()]="должен быть выключен"; // "must be off"
        m[flag_checked_unchecked.str()]="должен быть отмечен"; // "must be checked"
        m[flag_checked_unchecked.n_str()]="должен быть снят"; // "must be unchecked"
        m[flag_set_unset.str()]="должен быть установлен"; // "must be set"
        m[flag_set_unset.n_str()]="должен быть не установлен"; // "must be unset"
        m[flag_enable_disable.str()]="должен быть активен"; // "must be enabled"
        m[flag_enable_disable.n_str()]="должен быть не активен"; // "must be disabled"

        // properties
        m[value.name()]={{{"значение",grammar_ru::sredny_rod},grammar_ru::sredny_rod}}; // "value"
        m[empty.name()]={
                            {"пустой"},
                            {"пустая",grammar_ru::zhensky_rod},
                            {"пустое",grammar_ru::sredny_rod},
                            {"пустые",grammar_ru::mn_chislo}
                        }; // "empty"
        m[size.name()]="размер"; // "size"
        m[length.name()]={{{"длина",grammar_ru::zhensky_rod},grammar_ru::sredny_rod}}; // "length"

        // existance
        m[string_exists]={
                            {"должен существовать"},
                            {"должна существовать",grammar_ru::zhensky_rod},
                            {"должно существовать",grammar_ru::sredny_rod},
                            {"должны существовать",grammar_ru::mn_chislo}
                        }; // "must exist"
        m[string_not_exists]={
                            {"не должен существовать"},
                            {"не должна существовать",grammar_ru::zhensky_rod},
                            {"не должно существовать",grammar_ru::sredny_rod},
                            {"не должны существовать",grammar_ru::mn_chislo}
                        }; // "must not exist"

        // comparison
        m[eq]={
                {{"должен быть равен",grammar_ru::datelny_padezh}},
                {{"должна быть равна",grammar_ru::datelny_padezh},grammar_ru::zhensky_rod},
                {{"должно быть равно",grammar_ru::datelny_padezh},grammar_ru::sredny_rod},
                {{"должны быть равны",grammar_ru::datelny_padezh},grammar_ru::mn_chislo}
            }; // "must be equal to"
        m[ne]={
                {{"должен быть не равен",grammar_ru::datelny_padezh}},
                {{"должна быть не равна",grammar_ru::datelny_padezh},grammar_ru::zhensky_rod},
                {{"должно быть не равно",grammar_ru::datelny_padezh},grammar_ru::sredny_rod},
                {{"должны быть не равны",grammar_ru::datelny_padezh},grammar_ru::mn_chislo}
              }; // "must be not equal to"
        m[lt]={
                {{"должен быть меньше",grammar_ru::roditelny_padezh}},
                {{"должна быть меньше",grammar_ru::roditelny_padezh},grammar_ru::zhensky_rod},
                {{"должно быть меньше",grammar_ru::roditelny_padezh},grammar_ru::sredny_rod},
                {{"должны быть меньше",grammar_ru::roditelny_padezh},grammar_ru::mn_chislo}
              }; // "must be less than"
        m[lte]={
                {{"должен быть меньше или равен",grammar_ru::datelny_padezh}},
                {{"должна быть меньше или равна",grammar_ru::datelny_padezh},grammar_ru::zhensky_rod},
                {{"должно быть меньше или равно",grammar_ru::datelny_padezh},grammar_ru::sredny_rod},
                {{"должны быть меньше или равны",grammar_ru::datelny_padezh},grammar_ru::mn_chislo}
              }; // "must be less than or equal to"
        m[gt]={
                {{"должен быть больше",grammar_ru::roditelny_padezh}},
                {{"должна быть больше",grammar_ru::roditelny_padezh},grammar_ru::zhensky_rod},
                {{"должно быть больше",grammar_ru::roditelny_padezh},grammar_ru::sredny_rod},
                {{"должны быть больше",grammar_ru::roditelny_padezh},grammar_ru::mn_chislo}
              }; // "must be greater than"
        m[gte]={
                {{"должен быть больше или равен",grammar_ru::datelny_padezh}},
                {{"должна быть больше или равна",grammar_ru::datelny_padezh},grammar_ru::zhensky_rod},
                {{"должно быть больше или равно",grammar_ru::datelny_padezh},grammar_ru::sredny_rod},
                {{"должны быть больше или равны",grammar_ru::datelny_padezh},grammar_ru::mn_chislo}
              }; // "must be greater than or equal to"
    }

    return m;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_RU_LOCALE_HPP
