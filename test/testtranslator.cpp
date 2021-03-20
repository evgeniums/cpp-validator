#include <string>
#include <boost/test/unit_test.hpp>

#include <dracosha/validator/reporting/translator.hpp>
#include <dracosha/validator/reporting/no_translator.hpp>
#include <dracosha/validator/reporting/mapped_translator.hpp>
#include <dracosha/validator/reporting/phrase_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>
#include <dracosha/validator/reporting/member_operand.hpp>

#include <dracosha/validator/reporting/locale/sample_locale.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestTranslator)

BOOST_AUTO_TEST_CASE(CheckNoTranslator)
{
    std::string str("Hello!");
    auto str1=no_translator(str);
    BOOST_CHECK_EQUAL(str,str1);
}

BOOST_AUTO_TEST_CASE(CheckBaseTranslator)
{
    std::string str("Hello!");
    auto str1=translator()(str);
    BOOST_CHECK_EQUAL(str,std::string(str1));
}

BOOST_AUTO_TEST_CASE(CheckMappedTranslator)
{
    std::string not_translated("not translated");
    std::map<std::string,std::string> m=
    {
        {"one","one_translated"},
        {"two","two_translated"},
        {"three","three_translated"}
    };

    mapped_translator tr1;
    tr1.strings()["one"]=m["one"];
    tr1.strings()["two"]=m["two"];
    tr1.strings()["three"]=m["three"];
    BOOST_CHECK_EQUAL(m["one"],std::string(tr1("one")));
    BOOST_CHECK_EQUAL(m["two"],std::string(tr1("two")));
    BOOST_CHECK_EQUAL(m["three"],std::string(tr1("three")));
    BOOST_CHECK_EQUAL(not_translated,std::string(tr1(not_translated)));

    auto m1=m;
    mapped_translator tr2{std::move(m1)};
    BOOST_CHECK_EQUAL(m["one"],std::string(tr2("one")));
    BOOST_CHECK_EQUAL(m["two"],std::string(tr2("two")));
    BOOST_CHECK_EQUAL(m["three"],std::string(tr2("three")));
    BOOST_CHECK_EQUAL(not_translated,std::string(tr2(not_translated)));
}

BOOST_AUTO_TEST_CASE(CheckTranslatorRepository)
{
    std::string not_translated("not translated");
    std::map<std::string,std::string> m=
    {
        {"one","one_translated"},
        {"two","two_translated"},
        {"three","three_translated"}
    };
    std::map<std::string,std::string> m_sec=
    {
        {"one","one_translated_DE"},
        {"two","two_translated_DE"},
        {"three","three_translated_DE"}
    };
    std::map<std::string,std::string> m_def=
    {
        {"one","one_translated_DEF"},
        {"two","two_translated_DEF"},
        {"three","three_translated_DEF"}
    };

    auto check=[&not_translated,&m,&m_sec,&m_def](const translator_repository& rep)
    {
        auto ok=[&not_translated,&m](const translator& tr)
        {
            BOOST_CHECK_EQUAL(m["one"],std::string(tr("one")));
            BOOST_CHECK_EQUAL(m["two"],std::string(tr("two")));
            BOOST_CHECK_EQUAL(m["three"],std::string(tr("three")));
            BOOST_CHECK_EQUAL(not_translated,std::string(tr(not_translated)));
        };

        auto second_tr=[&not_translated,&m_sec](const translator& tr)
        {
            BOOST_CHECK_EQUAL(m_sec["one"],std::string(tr("one")));
            BOOST_CHECK_EQUAL(m_sec["two"],std::string(tr("two")));
            BOOST_CHECK_EQUAL(m_sec["three"],std::string(tr("three")));
            BOOST_CHECK_EQUAL(not_translated,std::string(tr(not_translated)));
        };

        auto default_tr=[&not_translated,&m_def](const translator& tr)
        {
            BOOST_CHECK_EQUAL(m_def["one"],std::string(tr("one")));
            BOOST_CHECK_EQUAL(m_def["two"],std::string(tr("two")));
            BOOST_CHECK_EQUAL(m_def["three"],std::string(tr("three")));
            BOOST_CHECK_EQUAL(not_translated,std::string(tr(not_translated)));
        };

        BOOST_TEST_CONTEXT("Full locale name with encoding")
        {
            auto tr=rep.find_translator("en_US.UTF-8");
            BOOST_REQUIRE(tr);
            ok(*tr);
        }

        BOOST_TEST_CONTEXT("Locale name with terrotory")
        {
            auto tr=rep.find_translator("en_US");
            BOOST_REQUIRE(tr);
            ok(*tr);
        }

        BOOST_TEST_CONTEXT("Language only locale name")
        {
            auto tr=rep.find_translator("en");
            BOOST_REQUIRE(tr);
            ok(*tr);
        }

        BOOST_TEST_CONTEXT("Locale name with other encoding")
        {
            auto tr=rep.find_translator("en_US.ASCII");
            BOOST_REQUIRE(tr);
            ok(*tr);
        }

        BOOST_TEST_CONTEXT("Locale name with other territory")
        {
            auto tr=rep.find_translator("en_GB.UTF-8");
            BOOST_REQUIRE(tr);
            ok(*tr);
        }

        BOOST_TEST_CONTEXT("Second language")
        {
            auto tr=rep.find_translator("de_DE.UTF-8");
            BOOST_REQUIRE(tr);
            second_tr(*tr);
        }

        BOOST_TEST_CONTEXT("Unknown locale")
        {
            auto tr=rep.find_translator("nl_BE.ISO8859-1");
            BOOST_REQUIRE(tr);
            default_tr(*tr);
        }
    };

    auto translator1=std::make_shared<mapped_translator>(m);
    auto translator2=std::make_shared<mapped_translator>(m_sec);
    auto def_translator=std::make_shared<mapped_translator>(m_def);
    std::set<std::string> locales1={"en_US.UTF-8","en_US","en"};
    std::set<std::string> locales2={"de"};

    BOOST_TEST_CONTEXT("Default ctor")
    {
        translator_repository rep;
        rep.add_translator(translator1,locales1);
        rep.add_translator(translator2,locales2);
        rep.set_default_translator(def_translator);
        check(rep);
    }

    std::map<std::string,std::shared_ptr<translator>> translators;
    for (auto&& it:locales1)
    {
        translators[it]=translator1;
    }
    for (auto&& it:locales2)
    {
        translators[it]=translator2;
    }

    BOOST_TEST_CONTEXT("Ctor from prepared translators")
    {
        translator_repository rep(def_translator,translators);
        check(rep);
    }

    BOOST_TEST_CONTEXT("Ctor from default translator")
    {
        translator_repository rep(def_translator);
        rep.add_translator(translator1,locales1);
        rep.add_translator(translator2,locales2);
        check(rep);
    }

    BOOST_TEST_CONTEXT("Ctor from translators only")
    {
        translator_repository rep(translators);
        rep.set_default_translator(def_translator);
        check(rep);
    }

    translator_repository rep(def_translator,translators);
    auto tr1=rep.find_translator("en_US.UTF-8");
    BOOST_REQUIRE(tr1);
    BOOST_CHECK(tr1.get()==translator1.get());
    rep.clear();
    auto tr2=rep.find_translator("en_US.UTF-8");
    BOOST_REQUIRE(tr2);
    BOOST_CHECK(tr2.get()==def_translator.get());
}

BOOST_AUTO_TEST_CASE(CheckConcretePhrase)
{
    std::map<std::string,std::string> m=
    {
        {"one","one_translated"},
        {"two","two_translated"},
        {"three","three_translated"}
    };

    mapped_translator tr1(m);
    BOOST_CHECK_EQUAL(std::string(tr1("one")),"one_translated");
    BOOST_CHECK_EQUAL(std::string(tr1(concrete_phrase("one"))),"one");
}

BOOST_AUTO_TEST_CASE(CheckPhraseTranslator)
{
    phrase_and_grammar_cats pc1{"translated word1"};
    phrase_and_grammar_cats pc2{"translated word2",grammar::plural};
    phrase_and_grammar_cats pc3{{"translated word3",grammar::plural},grammar::feminine};
    phrase_and_grammar_cats pc4{{"translated word4",grammar::feminine,grammar::plural},grammar::feminine,grammar::plural};
    std::string str0="translated word5";
    phrase_and_grammar_cats pc5{str0};

    BOOST_CHECK_EQUAL(pc1.phrase.text(),"translated word1");
    BOOST_CHECK_EQUAL(pc2.phrase.text(),"translated word2");
    BOOST_CHECK_EQUAL(pc3.phrase.text(),"translated word3");
    BOOST_CHECK_EQUAL(pc4.phrase.text(),"translated word4");
    BOOST_CHECK_EQUAL(pc5.phrase.text(),str0);

    phrase_translator tr1;

    tr1["word1"]="translated word1";
    tr1["word2"]=concrete_phrase("translated word2");
    tr1["word3"]={{"translated word3"}};
    tr1["word4"]={{"translated word4",grammar::plural}};
    tr1["word5"]={{{"translated word5",grammar::plural,grammar::feminine},grammar::feminine,grammar::plural}};

    BOOST_CHECK_EQUAL(std::string(tr1("word1")),"translated word1");
    BOOST_CHECK_EQUAL(std::string(tr1("word2")),"translated word2");
    BOOST_CHECK_EQUAL(std::string(tr1("word3")),"translated word3");
    BOOST_CHECK_EQUAL(std::string(tr1("word4")),"translated word4");
    BOOST_CHECK_EQUAL(std::string(tr1("word5")),"translated word5");

    std::string str1="masculine the word";
    std::string str2="masculine and plural the word";

    tr1["the word"]={
                        {"normal the word"},
                        {"plural the word",grammar::plural},
                        {"feminine the word",grammar::feminine},
                        {{"feminine and plural the word",grammar::plural},grammar::plural,grammar::feminine},
                        {str1,grammar::masculine},
                        {str2,grammar::plural,grammar::masculine}
                    };
    BOOST_CHECK_EQUAL(tr1("the word").text(),"normal the word");
    BOOST_CHECK_EQUAL(tr1("the word",grammar_categories_bitmask(grammar::plural)).text(),"plural the word");
    BOOST_CHECK_EQUAL(tr1("the word",grammar_categories_bitmask(grammar::feminine)).text(),"feminine the word");
    BOOST_CHECK_EQUAL(tr1("the word",grammar_categories_bitmask(grammar::plural,grammar::feminine)).text(),"feminine and plural the word");
    BOOST_CHECK_EQUAL(tr1("the word",grammar_categories_bitmask(grammar::masculine)).text(),str1);
    BOOST_CHECK_EQUAL(tr1("the word",grammar_categories_bitmask(grammar::masculine,grammar::plural)).text(),str2);

    tr1[value.name()]={
                        {"value"},
                        {{"values",grammar::plural},grammar::plural}
                    };
    BOOST_CHECK_EQUAL(tr1("value").text(),"value");
    auto plural_value=tr1("value",grammar_categories_bitmask(grammar::plural));
    BOOST_CHECK_EQUAL(plural_value.text(),"values");
    BOOST_CHECK(is_grammar_category_set(plural_value.grammar_cats(),grammar::plural));
    BOOST_CHECK_EQUAL(tr1("value",grammar_categories_bitmask(grammar::feminine)).text(),"value");
}

BOOST_AUTO_TEST_CASE(CheckSampleLocale)
{
    const auto& m=validator_translator_sample();

    // specials
    BOOST_CHECK_EQUAL(m(string_true).text(),std::string(string_true));
    BOOST_CHECK_EQUAL(m(string_false).text(),std::string(string_false));
    BOOST_CHECK_EQUAL(m(string_master_sample).text(),std::string(string_master_sample));
    BOOST_CHECK_EQUAL(m(string_empty).text(),std::string(string_empty));
    BOOST_CHECK_EQUAL(m(string_not_empty).text(),std::string(string_not_empty));
    BOOST_CHECK_EQUAL(m(string_conjunction_of).text(),std::string(string_conjunction_of));
    BOOST_CHECK_EQUAL(m(string_member_name_conjunction).text(),std::string(string_member_name_conjunction));
    BOOST_CHECK_EQUAL(m(string_element).text(),std::string(string_element));
    BOOST_CHECK_EQUAL(m(string_all).text(),std::string(string_all));
    BOOST_CHECK_EQUAL(m(string_all.iterator_description_str()).text(),std::string(string_all.iterator_description_str()));
    BOOST_CHECK_EQUAL(m(string_all.base_phrase_str()).text(),std::string(string_all.base_phrase_str()));
    BOOST_CHECK_EQUAL(m(string_all.key_description_str()).text(),std::string(string_all.key_description_str()));
    BOOST_CHECK_EQUAL(m(string_all.tree_description_str()).text(),std::string(string_all.tree_description_str()));
    BOOST_CHECK_EQUAL(m(string_all.name_str()).text(),std::string(string_all.name_str()));
    BOOST_CHECK_EQUAL(m(string_any).text(),std::string(string_any));
    BOOST_CHECK_EQUAL(m(string_any.iterator_description_str()).text(),std::string(string_any.iterator_description_str()));
    BOOST_CHECK_EQUAL(m(string_any.base_phrase_str()).text(),std::string(string_any.base_phrase_str()));
    BOOST_CHECK_EQUAL(m(string_any.key_description_str()).text(),std::string(string_any.key_description_str()));
    BOOST_CHECK_EQUAL(m(string_any.tree_description_str()).text(),std::string(string_any.tree_description_str()));
    BOOST_CHECK_EQUAL(m(string_any.name_str()).text(),std::string(string_any.name_str()));

    // flag descriptions
    BOOST_CHECK_EQUAL(m(flag_true_false_t::description).text(),std::string(flag_true_false_t::description));
    BOOST_CHECK_EQUAL(m(flag_true_false_t::n_description).text(),std::string(flag_true_false_t::n_description));
    BOOST_CHECK_EQUAL(m(flag_on_off_t::description).text(),std::string(flag_on_off_t::description));
    BOOST_CHECK_EQUAL(m(flag_on_off_t::n_description).text(),std::string(flag_on_off_t::n_description));
    BOOST_CHECK_EQUAL(m(flag_checked_unchecked_t::description).text(),std::string(flag_checked_unchecked_t::description));
    BOOST_CHECK_EQUAL(m(flag_checked_unchecked_t::n_description).text(),std::string(flag_checked_unchecked_t::n_description));
    BOOST_CHECK_EQUAL(m(flag_set_unset_t::description).text(),std::string(flag_set_unset_t::description));
    BOOST_CHECK_EQUAL(m(flag_set_unset_t::n_description).text(),std::string(flag_set_unset_t::n_description));
    BOOST_CHECK_EQUAL(m(flag_enable_disable_t::description).text(),std::string(flag_enable_disable_t::description));
    BOOST_CHECK_EQUAL(m(flag_enable_disable_t::n_description).text(),std::string(flag_enable_disable_t::n_description));

    // properties
    BOOST_CHECK_EQUAL(m(value.name()).text(),value.name());
    BOOST_CHECK_EQUAL(m(empty.name()).text(),empty.name());
    BOOST_CHECK_EQUAL(m(size.name()).text(),size.name());
    BOOST_CHECK_EQUAL(m(length.name()).text(),length.name());
    BOOST_CHECK_EQUAL(m(h_size.name()).text(),std::string(h_size.name()));

    // existance
    BOOST_CHECK_EQUAL(m(string_exists).text(),std::string(string_exists));
    BOOST_CHECK_EQUAL(m(string_not_exists).text(),std::string(string_not_exists));
    BOOST_CHECK_EQUAL(m(contains).text(),contains.str());
    BOOST_CHECK_EQUAL(m(_n(contains)).text(),contains.n_str());

    // logical
    BOOST_CHECK_EQUAL(m(string_and_t::conjunction_token).text(),std::string(string_and_t::conjunction_token));
    BOOST_CHECK_EQUAL(m(string_or_t::conjunction_token).text(),std::string(string_or_t::conjunction_token));
    BOOST_CHECK_EQUAL(m(string_not_t::open_token).text(),std::string(string_not_t::open_token));
    BOOST_CHECK_EQUAL(m(string_invert_op).text(),std::string(string_invert_op));

    // comparison
    BOOST_CHECK_EQUAL(m(eq).text(),std::string(eq));
    BOOST_CHECK_EQUAL(m(ne).text(),std::string(ne));
    BOOST_CHECK_EQUAL(m(lt).text(),std::string(lt));
    BOOST_CHECK_EQUAL(m(lte).text(),std::string(lte));
    BOOST_CHECK_EQUAL(m(gt).text(),std::string(gt));
    BOOST_CHECK_EQUAL(m(gte).text(),std::string(gte));

    // lexicographical
    BOOST_CHECK_EQUAL(m(lex_starts_with.str()).text(),lex_starts_with.str());
    BOOST_CHECK_EQUAL(m(lex_starts_with.n_str()).text(),lex_starts_with.n_str());
    BOOST_CHECK_EQUAL(m(lex_ends_with.str()).text(),lex_ends_with.str());
    BOOST_CHECK_EQUAL(m(lex_ends_with.n_str()).text(),lex_ends_with.n_str());

    // ranges and intervals
    BOOST_CHECK_EQUAL(m(range_str).text(),std::string(range_str));
    BOOST_CHECK_EQUAL(m(interval_str).text(),std::string(interval_str));
    BOOST_CHECK_EQUAL(m(in).text(),std::string(in));
    BOOST_CHECK_EQUAL(m(nin).text(),std::string(nin));

    // regex and strings
    BOOST_CHECK_EQUAL(m(regex_match.str()).text(),regex_match.str());
    BOOST_CHECK_EQUAL(m(regex_match.n_str()).text(),regex_match.n_str());
    BOOST_CHECK_EQUAL(m(regex_contains.str()).text(),regex_contains.str());
    BOOST_CHECK_EQUAL(m(regex_contains.n_str()).text(),regex_contains.n_str());
    BOOST_CHECK_EQUAL(m(str_alpha.str()).text(),str_alpha.str());
    BOOST_CHECK_EQUAL(m(str_alpha.n_str()).text(),str_alpha.n_str());
    BOOST_CHECK_EQUAL(m(str_hex.str()).text(),str_hex.str());
    BOOST_CHECK_EQUAL(m(str_hex.n_str()).text(),str_hex.n_str());
    BOOST_CHECK_EQUAL(m(str_int.str()).text(),str_int.str());
    BOOST_CHECK_EQUAL(m(str_int.n_str()).text(),str_int.n_str());
    BOOST_CHECK_EQUAL(m(str_float.str()).text(),str_float.str());
    BOOST_CHECK_EQUAL(m(str_float.n_str()).text(),str_float.n_str());
}

BOOST_AUTO_TEST_SUITE_END()
