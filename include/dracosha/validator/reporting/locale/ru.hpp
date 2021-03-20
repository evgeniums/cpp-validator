/**
@copyright Evgeny Sidorov 2020

Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

*/

/****************************************************************************/

/** @file validator/reporting/locale/ru.hpp
*
*   Defines strings for localization for Russian language.
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
#include <dracosha/validator/operators/lexicographical.hpp>
#include <dracosha/validator/operators/in.hpp>
#include <dracosha/validator/operators/regex.hpp>
#include <dracosha/validator/operators/string_patterns.hpp>
#include <dracosha/validator/operators/number_patterns.hpp>
#include <dracosha/validator/range.hpp>
#include <dracosha/validator/interval.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/operand_formatter.hpp>
#include <dracosha/validator/reporting/aggregation_strings.hpp>
#include <dracosha/validator/reporting/flag_presets.hpp>
#include <dracosha/validator/reporting/phrase_translator.hpp>

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

//-------------------------------------------------------------

/**
 * @brief Grammatical categories used in Russian language.
 */
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
 * @return Translator of system phrases for Russian locale.
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

        m[string_all.base_phrase_str()]={
                            {"каждый"},
                            {"каждого",grammar_ru::roditelny_padezh},
                            {"каждая",grammar_ru::zhensky_rod},
                            {"каждой",grammar_ru::zhensky_rod,grammar_ru::roditelny_padezh},
                            {"каждое",grammar_ru::sredny_rod},
                            {"каждого",grammar_ru::sredny_rod,grammar_ru::roditelny_padezh},
                            {"каждые",grammar_ru::mn_chislo},
                            {"каждых",grammar_ru::mn_chislo,grammar_ru::roditelny_padezh}
                      }; // "each"
        m[string_all.iterator_description_str()]={
                            {"каждый итератор"},
                            {"каждого итератора",grammar_ru::roditelny_padezh}
                      }; // "each iterator"
        m[string_all.key_description_str()]={
                            {"каждый ключ"},
                            {"каждого ключа",grammar_ru::roditelny_padezh}
                      }; // "each iterator"
        m[string_all.tree_description_str()]={
                            {"каждый узел дерева"},
                            {"каждого узла дерева",grammar_ru::roditelny_padezh}
                      }; // "each tree node"
        m[string_all.name_str()]={
                            {"ВСЕ"}
                      }; // "ALL"

        m[string_any.base_phrase_str()]={
                            {"хотя бы один"},
                            {"хотя бы одного",grammar_ru::roditelny_padezh},
                            {"хотя бы одна",grammar_ru::zhensky_rod},
                            {"хотя бы одной",grammar_ru::zhensky_rod,grammar_ru::roditelny_padezh},
                            {"хотя бы одно",grammar_ru::sredny_rod},
                            {"хотя бы одного",grammar_ru::sredny_rod,grammar_ru::roditelny_padezh},
                            {"хотя бы одни",grammar_ru::mn_chislo},
                            {"хотя бы одних",grammar_ru::mn_chislo,grammar_ru::roditelny_padezh}
                      }; // "at least one"
        m[string_any.iterator_description_str()]={
                            {"хотя бы один итератор"},
                            {"хотя бы одного итератора",grammar_ru::roditelny_padezh}
                      }; // "at least one iterator"
        m[string_any.key_description_str()]={
                            {"хотя бы один ключ"},
                            {"хотя бы одного ключа",grammar_ru::roditelny_padezh}
                      }; // "at least one key"
        m[string_any.tree_description_str()]={
                            {"хотя бы один узел дерева"},
                            {"хотя бы одного узла дерева",grammar_ru::roditelny_padezh}
                      }; // "at least one tree node"
        m[string_any.name_str()]={
                            {"ЛЮБОЙ"}
                      }; // "ANY"

        // flag descriptions
        m[flag_true_false.str()]={
                                    {"должен быть истинным"},
                                    {"должна быть истинна",grammar_ru::zhensky_rod},
                                    {"должно быть истинно",grammar_ru::sredny_rod},
                                    {"должны быть истинны",grammar_ru::mn_chislo}
                                }; // "must be true"
        m[flag_true_false.n_str()]={
                                    {"должен быть ложным"},
                                    {"должна быть ложна",grammar_ru::zhensky_rod},
                                    {"должно быть ложно",grammar_ru::sredny_rod},
                                    {"должны быть ложны",grammar_ru::mn_chislo}
                                }; // "must be false"
        m[flag_on_off.str()]={
                                    {"должен быть включен"},
                                    {"должна быть включена",grammar_ru::zhensky_rod},
                                    {"должно быть включено",grammar_ru::sredny_rod},
                                    {"должны быть включены",grammar_ru::mn_chislo}
                             }; // "must be on"
        m[flag_on_off.n_str()]={
                                    {"должен быть выключен"},
                                    {"должна быть выключена",grammar_ru::zhensky_rod},
                                    {"должно быть выключено",grammar_ru::sredny_rod},
                                    {"должны быть выключены",grammar_ru::mn_chislo}
                               }; // "must be off"
        m[flag_checked_unchecked.str()]={
                                    {"должен быть отмечен"},
                                    {"должна быть отмечена",grammar_ru::zhensky_rod},
                                    {"должно быть отмечено",grammar_ru::sredny_rod},
                                    {"должны быть отмечены",grammar_ru::mn_chislo}
                               }; // "must be checked"
        m[flag_checked_unchecked.n_str()]={
                                    {"должен быть снят"},
                                    {"должна быть снята",grammar_ru::zhensky_rod},
                                    {"должно быть снято",grammar_ru::sredny_rod},
                                    {"должны быть сняты",grammar_ru::mn_chislo}
                               }; // "must be unchecked"
        m[flag_set_unset.str()]={
                                    {"должен быть установлен"},
                                    {"должна быть установлена",grammar_ru::zhensky_rod},
                                    {"должно быть установлено",grammar_ru::sredny_rod},
                                    {"должны быть установлены",grammar_ru::mn_chislo}
                               }; // "must be set"
        m[flag_set_unset.n_str()]={
                                    {"должен быть не установлен"},
                                    {"должна быть не установлена",grammar_ru::zhensky_rod},
                                    {"должно быть не установлено",grammar_ru::sredny_rod},
                                    {"должны быть не установлены",grammar_ru::mn_chislo}
                               }; // "must be unset"
        m[flag_enable_disable.str()]={
                                        {"должен быть активен"},
                                        {"должна быть активна",grammar_ru::zhensky_rod},
                                        {"должно быть активно",grammar_ru::sredny_rod},
                                        {"должны быть активны",grammar_ru::mn_chislo}
                                    }; // "must be enabled"
        m[flag_enable_disable.n_str()]={
                                        {"должен быть неактивен"},
                                        {"должна быть неактивна",grammar_ru::zhensky_rod},
                                        {"должно быть неактивно",grammar_ru::sredny_rod},
                                        {"должны быть неактивны",grammar_ru::mn_chislo}
                                    }; // "must be disabled"

        // properties
        m[value.name()]={{{"значение",grammar_ru::sredny_rod},grammar_ru::sredny_rod}}; // "value"
        m[empty.name()]={
                            {"пустой"},
                            {"пустая",grammar_ru::zhensky_rod},
                            {"пустое",grammar_ru::sredny_rod},
                            {"пустые",grammar_ru::mn_chislo}
                        }; // "empty"
        m[size.name()]={
                            {"размер"},
                            {"размера",grammar_ru::roditelny_padezh}
                        }; // "size"
        m[length.name()]={
                        {{"длина",grammar_ru::zhensky_rod},grammar_ru::sredny_rod},
                        {{"длины",grammar_ru::zhensky_rod},grammar_ru::sredny_rod,grammar_ru::roditelny_padezh}
                    }; // "length"
        m[h_size.name()]={
                        {"гетерогенный размер"},
                        {"гетерогенного размера",grammar_ru::roditelny_padezh}
                    };// "heterogeneous size"

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
        m[contains.str()]={
                            {{"должен содержать",grammar_ru::datelny_padezh}},
                            {{"должна содержать",grammar_ru::datelny_padezh},grammar_ru::zhensky_rod},
                            {{"должно содержать",grammar_ru::datelny_padezh},grammar_ru::sredny_rod},
                            {{"должны сожержать",grammar_ru::datelny_padezh},grammar_ru::mn_chislo}
                        }; // "must contain"
        m[contains.n_str()]={
                            {{"не должен содержать",grammar_ru::datelny_padezh}},
                            {{"не должна содержать",grammar_ru::datelny_padezh},grammar_ru::zhensky_rod},
                            {{"не должно содержать",grammar_ru::datelny_padezh},grammar_ru::sredny_rod},
                            {{"не должны сожержать",grammar_ru::datelny_padezh},grammar_ru::mn_chislo}
                        }; // "must not contain"

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

        // lexicographical
        m[lex_starts_with.str()]={
                {{"должен начинаться с",grammar_ru::roditelny_padezh}},
                {{"должна начинаться с",grammar_ru::roditelny_padezh},grammar_ru::zhensky_rod},
                {{"должно начинаться с",grammar_ru::roditelny_padezh},grammar_ru::sredny_rod},
                {{"должны начинаться с",grammar_ru::roditelny_padezh},grammar_ru::mn_chislo}
              }; // "must start with"
        m[lex_starts_with.n_str()]={
                {{"не должен начинаться с",grammar_ru::roditelny_padezh}},
                {{"не должна начинаться с",grammar_ru::roditelny_padezh},grammar_ru::zhensky_rod},
                {{"не должно начинаться с",grammar_ru::roditelny_padezh},grammar_ru::sredny_rod},
                {{"не должны начинаться с",grammar_ru::roditelny_padezh},grammar_ru::mn_chislo}
              }; // "must not start with"
        m[lex_ends_with.str()]={
                {{"должен оканчиваться на",grammar_ru::roditelny_padezh}},
                {{"должна оканчиваться на",grammar_ru::roditelny_padezh},grammar_ru::zhensky_rod},
                {{"должно оканчиваться на",grammar_ru::roditelny_padezh},grammar_ru::sredny_rod},
                {{"должны оканчиваться на",grammar_ru::roditelny_padezh},grammar_ru::mn_chislo}
              }; // "must end with"
        m[lex_ends_with.n_str()]={
                {{"не должен оканчиваться на",grammar_ru::roditelny_padezh}},
                {{"не должна оканчиваться на",grammar_ru::roditelny_padezh},grammar_ru::zhensky_rod},
                {{"не должно оканчиваться на",grammar_ru::roditelny_padezh},grammar_ru::sredny_rod},
                {{"не должны оканчиваться на",grammar_ru::roditelny_padezh},grammar_ru::mn_chislo}
              }; // "must not end with"

        // ranges and intervals
        m[in]={
                {{"должен быть в",grammar_ru::predlozhny_padezh}},
                {{"должна быть в",grammar_ru::predlozhny_padezh},grammar_ru::zhensky_rod},
                {{"должно быть в",grammar_ru::predlozhny_padezh},grammar_ru::sredny_rod},
                {{"должны быть в",grammar_ru::predlozhny_padezh},grammar_ru::mn_chislo}
              }; // "must be in"
        m[nin]={
                {{"должен быть вне",grammar_ru::roditelny_padezh}},
                {{"должна быть вне",grammar_ru::roditelny_padezh},grammar_ru::zhensky_rod},
                {{"должно быть вне",grammar_ru::roditelny_padezh},grammar_ru::sredny_rod},
                {{"должны быть вне",grammar_ru::roditelny_padezh},grammar_ru::mn_chislo}
              }; // "must be not in"
        m[range_str]={
                {"список"},
                {"списке",grammar_ru::predlozhny_padezh},
                {"списка",grammar_ru::roditelny_padezh}
              }; // range
        m[interval_str]={
                {"интервал"},
                {"интервале",grammar_ru::predlozhny_padezh},
                {"интервала",grammar_ru::roditelny_padezh}
              }; // interval

        // regex and strings
        m[regex_match.str()]={
                {"должен соответствовать выражению"},
                {"должна соответствовать выражению",grammar_ru::zhensky_rod},
                {"должно соответствовать выражению",grammar_ru::sredny_rod},
                {"должны соответствовать выражению",grammar_ru::mn_chislo}
            }; // "must match expression"
        m[regex_match.n_str()]={
                {"не должен соответствовать выражению"},
                {"не должна соответствовать выражению",grammar_ru::zhensky_rod},
                {"не должно соответствовать выражению",grammar_ru::sredny_rod},
                {"не должны соответствовать выражению",grammar_ru::mn_chislo}
            }; // "must not match expression"
        m[regex_contains.str()]={
                {"должен содержать выражению"},
                {"должна содержать выражению",grammar_ru::zhensky_rod},
                {"должно содержать выражению",grammar_ru::sredny_rod},
                {"должны содержать выражению",grammar_ru::mn_chislo}
            }; // "must contain expression"
        m[regex_contains.n_str()]={
                {"не должен содержать выражению"},
                {"не должна содержать выражению",grammar_ru::zhensky_rod},
                {"не должно содержать выражению",grammar_ru::sredny_rod},
                {"не должны содержать выражению",grammar_ru::mn_chislo}
            }; // "must not contain expression"
        m[str_alpha.str()]={
                {"должен содержать только буквы и цифры"},
                {"должна содержать только буквы и цифры",grammar_ru::zhensky_rod},
                {"должно содержать только буквы и цифры",grammar_ru::sredny_rod},
                {"должны содержать только буквы и цифры",grammar_ru::mn_chislo}
            }; // "must contain only letters and digits"
        m[str_alpha.n_str()]={
                {"должен содержать не только буквы и цифры"},
                {"должна содержать не только буквы и цифры",grammar_ru::zhensky_rod},
                {"должно содержать не только буквы и цифры",grammar_ru::sredny_rod},
                {"должны содержать не только буквы и цифры",grammar_ru::mn_chislo}
            }; // "must contain not only letters and digits"
        m[str_hex.str()]={
                {"должен быть шестнадцатеричным числом"},
                {"должна быть шестнадцатеричным числом",grammar_ru::zhensky_rod},
                {"должно быть шестнадцатеричным числом",grammar_ru::sredny_rod},
                {"должны быть шестнадцатеричным числом",grammar_ru::mn_chislo}
            }; // "must be a hexadecimal number"
        m[str_hex.n_str()]={
                {"не должен быть шестнадцатеричным числом"},
                {"не должна быть шестнадцатеричным числом",grammar_ru::zhensky_rod},
                {"не должно быть шестнадцатеричным числом",grammar_ru::sredny_rod},
                {"не должны быть шестнадцатеричным числом",grammar_ru::mn_chislo}
            }; // "must be not a hexadecimal number"
        m[str_int.str()]={
                {"должен быть целочисленным"},
                {"должна быть целочисленной",grammar_ru::zhensky_rod},
                {"должно быть целочисленным",grammar_ru::sredny_rod},
                {"должны быть целочисленными",grammar_ru::mn_chislo}
            }; // "must be integer"
        m[str_int.n_str()]={
                {"не должен целочисленным"},
                {"не должна целочисленной",grammar_ru::zhensky_rod},
                {"не должно целочисленным",grammar_ru::sredny_rod},
                {"не должны целочисленными",grammar_ru::mn_chislo}
            }; // "must not be integer"
        m[str_float.str()]={
                {"должен быть вещественным числом"},
                {"должна быть вещественным числом",grammar_ru::zhensky_rod},
                {"должно быть вещественным числом",grammar_ru::sredny_rod},
                {"должны быть вещественным числом",grammar_ru::mn_chislo}
            }; // "must be a floating point number"
        m[str_float.n_str()]={
                {"не должен вещественным числом"},
                {"не должна вещественным числом",grammar_ru::zhensky_rod},
                {"не должно вещественным числом",grammar_ru::sredny_rod},
                {"не должны вещественным числом",grammar_ru::mn_chislo}
            }; // "must be not a floating point number"
    }

    return m;
}

//-------------------------------------------------------------

DRACOSHA_VALIDATOR_NAMESPACE_END

#endif // DRACOSHA_VALIDATOR_RU_LOCALE_HPP
