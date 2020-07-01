#include <string>
#include <boost/test/unit_test.hpp>

#include <dracosha/validator/reporting/translator.hpp>
#include <dracosha/validator/reporting/no_translator.hpp>
#include <dracosha/validator/reporting/mapped_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>

using namespace dracosha::validator;

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
    BOOST_CHECK_EQUAL(str,str1);
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
    BOOST_CHECK_EQUAL(m["one"],tr1("one"));
    BOOST_CHECK_EQUAL(m["two"],tr1("two"));
    BOOST_CHECK_EQUAL(m["three"],tr1("three"));
    BOOST_CHECK_EQUAL(not_translated,tr1(not_translated));

    auto m1=m;
    mapped_translator tr2{std::move(m1)};
    BOOST_CHECK_EQUAL(m["one"],tr2("one"));
    BOOST_CHECK_EQUAL(m["two"],tr2("two"));
    BOOST_CHECK_EQUAL(m["three"],tr2("three"));
    BOOST_CHECK_EQUAL(not_translated,tr2(not_translated));
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
            BOOST_CHECK_EQUAL(m["one"],tr("one"));
            BOOST_CHECK_EQUAL(m["two"],tr("two"));
            BOOST_CHECK_EQUAL(m["three"],tr("three"));
            BOOST_CHECK_EQUAL(not_translated,tr(not_translated));
        };

        auto second_tr=[&not_translated,&m_sec](const translator& tr)
        {
            BOOST_CHECK_EQUAL(m_sec["one"],tr("one"));
            BOOST_CHECK_EQUAL(m_sec["two"],tr("two"));
            BOOST_CHECK_EQUAL(m_sec["three"],tr("three"));
            BOOST_CHECK_EQUAL(not_translated,tr(not_translated));
        };

        auto default_tr=[&not_translated,&m_def](const translator& tr)
        {
            BOOST_CHECK_EQUAL(m_def["one"],tr("one"));
            BOOST_CHECK_EQUAL(m_def["two"],tr("two"));
            BOOST_CHECK_EQUAL(m_def["three"],tr("three"));
            BOOST_CHECK_EQUAL(not_translated,tr(not_translated));
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

BOOST_AUTO_TEST_SUITE_END()
