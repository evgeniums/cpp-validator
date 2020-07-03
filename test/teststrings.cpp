#include <string>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/values.hpp>
#include <dracosha/validator/properties/value.hpp>
#include <dracosha/validator/properties/size.hpp>
#include <dracosha/validator/reporting/mapped_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>
#include <dracosha/validator/cref.hpp>

using namespace dracosha::validator;

namespace {

struct Dummy
{
};

template <typename T>
void check_bypass(const T& proc)
{
    BOOST_CHECK_EQUAL(proc(string_and),std::string(string_and));
    BOOST_CHECK_EQUAL(proc(value),value.name());
    BOOST_CHECK_EQUAL(proc(size),size.name());
    BOOST_CHECK_EQUAL(proc(gte),std::string(gte));
    BOOST_CHECK_EQUAL(proc("field1"),std::string("field1"));
    BOOST_CHECK_EQUAL(proc(10),std::string("10"));
    BOOST_CHECK_EQUAL(proc(Dummy{}),std::string("<\?\?\?\?\?>"));
}

struct translator_env
{
    translator_env():_m(
                        {
                            {std::string(string_and),"and_translated"},
                            {value.name(),"value_translated"},
                            {std::string(gte),"gte_translated"},
                            {std::string("field1"),"field1_translated"},
                            {std::string("false"),"false_translated"},
                            {std::string("true"),"true_translated"}
                        }
                      )
    {
        auto translator1=std::make_shared<mapped_translator>(_m);
        std::set<std::string> locales1={"en_US.UTF-8","en_US","en"};
        _rep.add_translator(translator1,locales1);
    }

    translated_strings strings()
    {
        return make_translated_strings(_rep,"en");
    }

    template <typename T>
    void check(const T& strings)
    {
        BOOST_CHECK_EQUAL(strings(string_and),_m[std::string(string_and)]);
        BOOST_CHECK_EQUAL(strings(value),_m[value.name()]);
        BOOST_CHECK_EQUAL(strings(gte),_m[std::string(gte)]);
        BOOST_CHECK_EQUAL(strings("field1"),_m[std::string("field1")]);
        BOOST_CHECK_EQUAL(strings(10),std::string("10"));
        BOOST_CHECK_EQUAL(strings(Dummy{}),std::string("<\?\?\?\?\?>"));
    }

    std::map<std::string,std::string> _m;
    translator_repository _rep;
};

struct NonCopyable
{
    NonCopyable()=default;
    ~NonCopyable()=default;
    NonCopyable(const NonCopyable&)=delete;
    NonCopyable(NonCopyable&&)=default;
    NonCopyable& operator= (const NonCopyable&)=delete;
    NonCopyable& operator= (NonCopyable&&)=default;

    std::string val;
};

}

BOOST_AUTO_TEST_SUITE(TestStrings)

BOOST_AUTO_TEST_CASE(CheckDefaultStrings)
{
    check_bypass(default_strings);
}

BOOST_AUTO_TEST_CASE(CheckTranslatedStrings)
{
    translator_env env;
    auto strings=env.strings();
    env.check(strings);
}

BOOST_AUTO_TEST_CASE(CheckMemberNames)
{
    BOOST_TEST_CONTEXT("Dummy traits")
    {
        check_bypass(member_names(Dummy()));
    }

    BOOST_TEST_CONTEXT("Default member names")
    {
        check_bypass(member_names());
    }

    BOOST_TEST_CONTEXT("Strings with bypass member names")
    {
        translator_env env;
        auto strings=env.strings();
        env.check(member_names(strings));
    }

    BOOST_TEST_CONTEXT("Strings with dummy traits of member names")
    {
        translator_env env;
        auto strings=env.strings();
        env.check(member_names(Dummy(),strings));
    }

    BOOST_TEST_CONTEXT("Strings with mapped translator as traits")
    {
        translator_env env;
        auto strings=env.strings();
        mapped_translator tr;
        tr.strings()["field2"]="filed2_translated";
        auto mn=member_names(tr,strings);
        env.check(mn);
        BOOST_CHECK_EQUAL(mn("field2"),std::string("filed2_translated"));
    }

    {
        translator_env env;
        auto strings=env.strings();
        mapped_translator tr;
        tr.strings()["field2"]="filed2_translated";
        tr.strings()["field1"]="other translation of field1";

        auto mn=member_names(tr,strings);
        BOOST_CHECK_EQUAL(mn(string_and),env._m[std::string(string_and)]);
        BOOST_CHECK_EQUAL(mn(value),env._m[value.name()]);
        BOOST_CHECK_EQUAL(mn(gte),env._m[std::string(gte)]);
        BOOST_CHECK_EQUAL(mn("field1"),std::string("other translation of field1"));
        BOOST_CHECK_EQUAL(mn("field2"),std::string("filed2_translated"));
        BOOST_CHECK_EQUAL(mn(10),std::string("10"));
        BOOST_CHECK_EQUAL(mn(Dummy{}),std::string("<\?\?\?\?\?>"));
    }
}

BOOST_AUTO_TEST_CASE(CheckValues)
{
    BOOST_CHECK_EQUAL(std::string(values("hello")),std::string("hello"));
    BOOST_CHECK_EQUAL(values(5),5);
    BOOST_CHECK_EQUAL(values(true),std::string("true"));
    BOOST_CHECK_EQUAL(values(false),std::string("false"));
    NonCopyable nc;
    const auto& ncr=values(nc);
    std::ignore=ncr;

    translator_env env;
    auto translate_values=make_translated_values(env._rep,"en");
    BOOST_CHECK_EQUAL(std::string(translate_values("hello")),std::string("hello"));
    BOOST_CHECK_EQUAL(translate_values(5),5);
    BOOST_CHECK_EQUAL(translate_values(true),std::string("true_translated"));
    BOOST_CHECK_EQUAL(translate_values(false),std::string("false_translated"));
}

BOOST_AUTO_TEST_CASE(CheckHanaZipTransform)
{
    auto fn1=[](const std::string& str)
    {
        return str+"+";
    };
    auto fn2=[](const std::string& str)
    {
        return str+"-";
    };
    auto fn3=[](const std::string& str1, const std::string& str2)
    {
        return str1+str2;
    };

    auto t1=hana::make_tuple(fn1,fn2);
    auto t2=hana::make_tuple(std::string("a"),std::string("b"));

    auto res=hana::unpack(hana::transform(hana::zip(t1,t2),hana::fuse(hana::apply)),fn3);
    BOOST_CHECK_EQUAL(res,std::string("a+b-"));
}

BOOST_AUTO_TEST_CASE(CheckCref)
{
    NonCopyable ncp{"hello cref"};

    auto fn=[](const NonCopyable& nc)
    {
        return nc.val;
    };

    auto res1=apply_cref(fn,ncp);
    BOOST_CHECK_EQUAL(res1,ncp.val);

    auto cr=cref(ncp);
    const auto& ncp1=extract_cref(cr);
    BOOST_CHECK_EQUAL(ncp1.val,ncp.val);

    auto res2=apply_cref(fn,cr);
    BOOST_CHECK_EQUAL(res2,ncp.val);

    auto mn=member_names();
    auto fn1=[&mn](const auto& v)
    {
        auto vv=cref(v);
        auto str=apply_cref(mn,vv);
        BOOST_CHECK_EQUAL(std::string("field1"),str);
    };
    fn1("field1");
}

BOOST_AUTO_TEST_SUITE_END()
