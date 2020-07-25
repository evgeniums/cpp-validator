#include <string>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/values.hpp>
#include <dracosha/validator/properties/value.hpp>
#include <dracosha/validator/properties/size.hpp>
#include <dracosha/validator/reporting/mapped_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>
#include <dracosha/validator/reporting/quotes_decorator.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/validator.hpp>

using namespace dracosha::validator;

namespace {

struct Dummy
{
};

template <typename T>
void check_bypass(const T& proc)
{
    BOOST_CHECK_EQUAL(proc(value),value.name());
    BOOST_CHECK_EQUAL(proc(size),size.name());
    BOOST_CHECK_EQUAL(proc(gte),std::string(gte));
    BOOST_CHECK_EQUAL(proc("field1"),std::string("field1"));
    BOOST_CHECK_EQUAL(proc(10),"element #10");
    BOOST_CHECK_EQUAL(proc(Dummy{}),std::string("<\?\?\?\?\?>"));
}

template <typename T>
void check_strings_bypass(const T& proc)
{
    BOOST_CHECK_EQUAL(proc(value),value.name());
    BOOST_CHECK_EQUAL(proc(size),size.name());
    BOOST_CHECK_EQUAL(proc(gte),std::string(gte));
    BOOST_CHECK_EQUAL(proc("field1"),std::string("field1"));
    BOOST_CHECK_EQUAL(proc(10),"10");
    BOOST_CHECK_EQUAL(proc(Dummy{}),std::string("<\?\?\?\?\?>"));
}

struct translator_env
{
    translator_env():_m(
                        {
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

    auto strings()
    {
        return make_translated_strings(_rep,"en");
    }

    template <typename T>
    void check(const T& strings, bool no_el=false)
    {
        BOOST_CHECK_EQUAL(strings(value),_m[value.name()]);
        BOOST_CHECK_EQUAL(strings(gte),_m[std::string(gte)]);
        BOOST_CHECK_EQUAL(strings("field1"),_m[std::string("field1")]);
        if (no_el)
        {
            BOOST_CHECK_EQUAL(strings(10),std::string("10"));
        }
        else
        {
            BOOST_CHECK_EQUAL(strings(10),std::string("element #10"));
        }
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
    check_strings_bypass(default_strings);
}

BOOST_AUTO_TEST_CASE(CheckTranslatedStrings)
{
    translator_env env;
    auto strings=env.strings();
    env.check(strings,true);
}

BOOST_AUTO_TEST_CASE(CheckMemberNames)
{
    BOOST_TEST_CONTEXT("Dummy traits")
    {
        auto mn=make_member_names(Dummy());
        using mn_type=decltype(mn);
        static_assert(std::is_lvalue_reference<typename mn_type::strings_type>::value,"");
        static_assert(!std::is_lvalue_reference<typename mn_type::traits_type>::value,"");
        static_assert(std::is_same<typename mn_type::traits_type,Dummy>::value,"");

        check_bypass(mn);
    }

    BOOST_TEST_CONTEXT("Default member names")
    {
        const auto& mn=get_default_member_names();
        using mn_type=decltype(mn);
        static_assert(std::is_lvalue_reference<typename std::decay_t<mn_type>::strings_type>::value,"");
        static_assert(std::is_same<typename std::decay_t<mn_type>::traits_type,const bypass_member_names_t&>::value,"");

        check_bypass(mn);
    }

    BOOST_TEST_CONTEXT("Strings with bypass member names")
    {
        translator_env env;
        auto mn=make_member_names(env.strings());
        using mn_type=decltype(mn);
        static_assert(!std::is_lvalue_reference<typename mn_type::strings_type>::value,"");
        static_assert(std::is_same<typename mn_type::traits_type,const bypass_member_names_t&>::value,"");

        env.check(mn);
    }

    BOOST_TEST_CONTEXT("Strings with dummy traits of member names")
    {
        translator_env env;
        auto strings=env.strings();
        env.check(make_member_names(Dummy(),strings));
    }

    BOOST_TEST_CONTEXT("Strings with mapped translator as traits")
    {
        translator_env env;
        mapped_translator tr;
        tr.strings()["field2"]="filed2_translated";

        auto mn=make_member_names(std::move(tr),env.strings());
        using mn_type=decltype(mn);
        static_assert(!std::is_lvalue_reference<typename mn_type::strings_type>::value,"");
        static_assert(!std::is_lvalue_reference<typename mn_type::traits_type>::value,"");

        env.check(mn);
        BOOST_CHECK_EQUAL(mn("field2"),std::string("filed2_translated"));
    }

    {
        translator_env env;
        auto strings=env.strings();
        mapped_translator tr;
        tr.strings()["field2"]="filed2_translated";
        tr.strings()["field1"]="other translation of field1";

        auto mn=make_member_names(tr,strings);
        using mn_type=decltype(mn);
        static_assert(std::is_lvalue_reference<typename mn_type::strings_type>::value,"");
        static_assert(std::is_lvalue_reference<typename mn_type::traits_type>::value,"");

        BOOST_CHECK_EQUAL(mn(value),env._m[value.name()]);
        BOOST_CHECK_EQUAL(mn(gte),env._m[std::string(gte)]);
        BOOST_CHECK_EQUAL(mn("field1"),std::string("other translation of field1"));
        BOOST_CHECK_EQUAL(mn("field2"),std::string("filed2_translated"));
        BOOST_CHECK_EQUAL(mn(10),std::string("element #10"));
        BOOST_CHECK_EQUAL(mn(Dummy{}),std::string("<\?\?\?\?\?>"));
    }

    {
        translator_env env;
        mapped_translator tr;
        tr.strings()["field2"]="filed2_translated";
        tr.strings()["field1"]="other translation of field1";

        auto mn=decorate_member_names(make_member_names(std::move(tr),env.strings()),quotes_decorator);

        BOOST_CHECK_EQUAL(mn(value),"\"value_translated\"");
        BOOST_CHECK_EQUAL(mn(gte),"\"gte_translated\"");
        BOOST_CHECK_EQUAL(mn("field1"),std::string("\"other translation of field1\""));
        BOOST_CHECK_EQUAL(mn("field2"),std::string("\"filed2_translated\""));
        BOOST_CHECK_EQUAL(mn(10),std::string("\"element #10\""));
        BOOST_CHECK_EQUAL(mn(Dummy{}),std::string("\"<\?\?\?\?\?>\""));
    }
}

BOOST_AUTO_TEST_CASE(CheckIntegralMemberName)
{
    auto mn=get_default_member_names();
    static_assert(std::is_integral<decltype(10)>::value,"");
    static_assert(!can_single_member_name<decltype(10),decltype(bypass_member_names)>::value,"");
    auto str=mn(10);
    BOOST_CHECK_EQUAL(str,"element #10");
}

BOOST_AUTO_TEST_CASE(CheckValues)
{
    BOOST_CHECK_EQUAL(std::string(default_values("hello")),std::string("hello"));
    BOOST_CHECK_EQUAL(default_values(5),5);
    BOOST_CHECK_EQUAL(default_values(true),std::string("true"));
    BOOST_CHECK_EQUAL(default_values(false),std::string("false"));
    NonCopyable nc;
    const auto& ncr=default_values(nc);
    std::ignore=ncr;

    translator_env env;
    auto translate_values=make_translated_values(env._rep,"en");
    BOOST_CHECK_EQUAL(std::string(translate_values("hello")),std::string("hello"));
    BOOST_CHECK_EQUAL(translate_values(5),5);
    BOOST_CHECK_EQUAL(translate_values(true),std::string("true_translated"));
    BOOST_CHECK_EQUAL(translate_values(false),std::string("false_translated"));

    auto decorator=[](const std::string& val)
    {
        return std::string("\"")+val+std::string("\"");
    };
    auto vals2=make_values(decorator);
    BOOST_CHECK_EQUAL(vals2("hi"),"\"hi\"");

    mapped_translator tr3;
    auto vals3=make_translated_values(tr3,decorator);
    BOOST_CHECK_EQUAL(vals3("hello"),"\"hello\"");

    mapped_translator tr4;
    auto vals4=make_translated_values(env._rep,"en",std::move(decorator));
    BOOST_CHECK_EQUAL(vals4(true),"\"true_translated\"");

    mapped_translator tr5;
    auto vals5=make_translated_values(env._rep,"en",quotes_decorator);
    BOOST_CHECK_EQUAL(vals5(true),"\"true_translated\"");
    BOOST_CHECK_EQUAL(vals5(100),100);
    BOOST_CHECK_EQUAL(vals5("Hello"),"\"Hello\"");
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

BOOST_AUTO_TEST_CASE(CheckReferenceWrapper)
{
    NonCopyable ncp{"hello cref"};

    auto fn=[](const NonCopyable& nc)
    {
        return nc.val;
    };

    auto res1=apply_ref(fn,ncp);
    BOOST_CHECK_EQUAL(res1,ncp.val);

    auto cr=cref(ncp);
    const auto& ncp1=extract_ref(cr);
    BOOST_CHECK_EQUAL(ncp1.val,ncp.val);

    auto res2=apply_ref(fn,cr);
    BOOST_CHECK_EQUAL(res2,ncp.val);

    auto mn=get_default_member_names();
    auto fn1=[&mn](const auto& v)
    {
        auto vv=cref(v);
        auto str=apply_ref(mn,vv);
        BOOST_CHECK_EQUAL(std::string("field1"),str);
    };
    fn1("field1");

    std::string str1;
    auto r1=ref(str1);
    std::string& str2=r1;
    str2="hi";
    std::ignore=str2;

    std::string str3;
    auto r2=cref(str3);
    const std::string& str4=r2;
    std::ignore=str4;
}

BOOST_AUTO_TEST_CASE(CheckNestedMember)
{
    auto mn=get_default_member_names();

    auto member1=_["field1"];
    auto str1=mn(member1);
    BOOST_CHECK_EQUAL(str1,std::string("field1"));

    auto member2=_["field1"]["field2"];
    auto str2=mn(member2);
    BOOST_CHECK_EQUAL(str2,std::string("field2 of field1"));

    auto member3=_["field1"]["field2"][size];
    auto str3=mn(member3);
    BOOST_CHECK_EQUAL(str3,std::string("size of field2 of field1"));
}

BOOST_AUTO_TEST_CASE(CheckMemberName)
{
    auto mn=get_default_member_names();

    auto member1=_["field1"]("member1 name");
    auto str1=mn(member1);
    BOOST_CHECK_EQUAL(str1,"member1 name");

    auto member2=_["field1"]["field2"]("member2 name");
    auto str2=mn(member2);
    BOOST_CHECK_EQUAL(str2,"member2 name");

    auto member3=_["field1"]["field2"][size]("size of nested field");
    auto str3=mn(member3);
    BOOST_CHECK_EQUAL(str3,"size of nested field");
}

BOOST_AUTO_TEST_CASE(CheckOperator)
{
    auto op1=_(gte,"not less than");
    BOOST_CHECK_EQUAL(std::string(op1),"not less than");
    BOOST_CHECK(op1(10,2));
}

BOOST_AUTO_TEST_CASE(CheckInvertOp)
{
    auto op1=_n(gt);
    BOOST_CHECK_EQUAL(std::string(op1),"must be less than or equal to");
    BOOST_CHECK(!op1(10,2));

    auto op2=_n(gte,"must be less than");
    BOOST_CHECK_EQUAL(std::string(op2),"must be less than");
    BOOST_CHECK(!op2(10,2));
}

BOOST_AUTO_TEST_SUITE_END()
