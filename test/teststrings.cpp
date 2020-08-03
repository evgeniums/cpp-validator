#include <string>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/reporting/strings.hpp>
#include <dracosha/validator/reporting/member_names.hpp>
#include <dracosha/validator/reporting/operand_formatter.hpp>
#include <dracosha/validator/properties/value.hpp>
#include <dracosha/validator/properties/size.hpp>
#include <dracosha/validator/reporting/mapped_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>
#include <dracosha/validator/reporting/quotes_decorator.hpp>
#include <dracosha/validator/utils/reference_wrapper.hpp>
#include <dracosha/validator/validator.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

namespace {

struct Dummy : public default_member_names_traits_t
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
    BOOST_CHECK_EQUAL(std::string(proc(value)),value.name());
    BOOST_CHECK_EQUAL(std::string(proc(size)),size.name());
    BOOST_CHECK_EQUAL(std::string(proc(gte)),std::string(gte));
    BOOST_CHECK_EQUAL(std::string(proc("field1")),std::string("field1"));
    BOOST_CHECK_EQUAL(std::string(proc(10)),"10");
    BOOST_CHECK_EQUAL(std::string(proc(Dummy{})),std::string("<\?\?\?\?\?>"));
}

struct translator_env
{
    translator_env():_m(
                        {
                            {value.name(),"value_translated"},
                            {std::string(gte),"gte_translated"},
                            {std::string("field1"),"field1_translated"},
                            {std::string("false"),"false_translated"},
                            {std::string("true"),"true_translated"},
                            {std::string("Hello"),"Hello translated"}
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

    auto mn()
    {
        return make_translated_member_names(_rep,"en");
    }

    const auto& translator()
    {
        return *_rep.find_translator("en");
    }

    template <typename T>
    void check(const T& strings, bool no_el=false)
    {
        BOOST_CHECK_EQUAL(std::string(strings(value)),_m[value.name()]);
        BOOST_CHECK_EQUAL(std::string(strings(gte)),_m[std::string(gte)]);
        BOOST_CHECK_EQUAL(std::string(strings("field1")),_m[std::string("field1")]);
        if (no_el)
        {
            BOOST_CHECK_EQUAL(std::string(strings(10)),std::string("10"));
        }
        else
        {
            BOOST_CHECK_EQUAL(std::string(strings(10)),std::string("element #10"));
        }
        BOOST_CHECK_EQUAL(std::string(strings(Dummy{})),std::string("<\?\?\?\?\?>"));
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
        static_assert(!std::is_lvalue_reference<typename mn_type::traits_type>::value,"");
        static_assert(std::is_same<typename mn_type::traits_type,Dummy>::value,"");

        check_bypass(mn);
    }

    BOOST_TEST_CONTEXT("Default member names")
    {
        const auto& mn=get_default_member_names();
        using mn_type=decltype(mn);
        static_assert(std::is_same<typename std::decay_t<mn_type>::traits_type,const default_member_names_traits_t&>::value,"");

        check_bypass(mn);
    }

    BOOST_TEST_CONTEXT("Strings with bypass member names")
    {
        translator_env env;
        auto mn=env.mn();
        using mn_type=decltype(mn);
        static_assert(std::is_same<typename mn_type::traits_type,translate_member_names_traits_t<default_member_names_traits_t>>::value,"");

        env.check(mn);
    }

    BOOST_TEST_CONTEXT("Translated member names")
    {
        translator_env env;
        env.check(make_translated_member_names(make_member_names(Dummy()),env.translator()));
    }

    {
        translator_env env;

        mapped_translator tr;
        tr.strings()["field2"]="filed2_translated";
        tr.strings()["field1"]="other translation of field1";

        auto mn=make_translated_member_names(tr);

        BOOST_CHECK_EQUAL(std::string(mn("field1")),std::string("other translation of field1"));
        BOOST_CHECK_EQUAL(std::string(mn("field2")),std::string("filed2_translated"));
        BOOST_CHECK_EQUAL(std::string(mn(10)),std::string("element #10"));
        BOOST_CHECK_EQUAL(std::string(mn(Dummy{})),std::string("<\?\?\?\?\?>"));
    }

    {
        translator_env env;
        auto mn=make_decorated_member_names(make_translated_member_names(env.translator()),quotes_decorator);

        BOOST_CHECK_EQUAL(std::string(mn(value)),"\"value_translated\"");
        BOOST_CHECK_EQUAL(std::string(mn(gte)),"\"gte_translated\"");
        BOOST_CHECK_EQUAL(std::string(mn("field1")),std::string("\"field1_translated\""));
        BOOST_CHECK_EQUAL(std::string(mn("field2")),std::string("\"field2\""));
        BOOST_CHECK_EQUAL(std::string(mn(10)),std::string("\"element #10\""));
        BOOST_CHECK_EQUAL(std::string(mn(Dummy{})),std::string("\"<\?\?\?\?\?>\""));
    }
}

BOOST_AUTO_TEST_CASE(CheckIntegralMemberName)
{
    auto mn=get_default_member_names();
    static_assert(std::is_integral<decltype(10)>::value,"");
    static_assert(!can_single_member_name<decltype(10),decltype(default_member_names_traits)>::value,"");
    auto str=mn(10);
    BOOST_CHECK_EQUAL(str,"element #10");
}

BOOST_AUTO_TEST_CASE(CheckOperands)
{
    BOOST_CHECK_EQUAL(std::string(default_operand_formatter("hello")),std::string("hello"));
    BOOST_CHECK_EQUAL(default_operand_formatter(5),5);
    BOOST_CHECK_EQUAL(default_operand_formatter(true),std::string("true"));
    BOOST_CHECK_EQUAL(default_operand_formatter(false),std::string("false"));
    NonCopyable nc;
    const auto& ncr=default_operand_formatter(nc);
    std::ignore=ncr;

    translator_env env;
    auto translate_operands=make_translated_operand_formatter(env._rep,"en");
    BOOST_CHECK_EQUAL(std::string(translate_operands("hello")),std::string("hello"));
    BOOST_CHECK_EQUAL(translate_operands(5),5);
    BOOST_CHECK_EQUAL(std::string(translate_operands(true)),std::string("true_translated"));
    BOOST_CHECK_EQUAL(std::string(translate_operands(false)),std::string("false_translated"));

    auto decorator=[](const std::string& val)
    {
        return std::string("\"")+val+std::string("\"");
    };
    auto operands2=make_operand_formatter(decorator);
    BOOST_CHECK_EQUAL(operands2("hi"),"\"hi\"");

    mapped_translator tr3;
    auto operands3=make_translated_operand_formatter(tr3,decorator);
    BOOST_CHECK_EQUAL(operands3("hello"),"\"hello\"");

    mapped_translator tr4;
    auto operands4=make_translated_operand_formatter(env._rep,"en",std::move(decorator));
    BOOST_CHECK_EQUAL(operands4(true),"\"true_translated\"");

    mapped_translator tr5;
    auto operands5=make_translated_operand_formatter(env._rep,"en",quotes_decorator,std::true_type());
    BOOST_CHECK_EQUAL(std::string(operands5(true)),"\"true_translated\"");
    BOOST_CHECK_EQUAL(operands5(100),100);
    BOOST_CHECK_EQUAL(operands5("Hello"),"\"Hello translated\"");
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
    std::string str1=mn(member1);
    BOOST_CHECK_EQUAL(str1,std::string("field1"));

    auto member2=_["field1"]["field2"];
    std::string str2=mn(member2);
    BOOST_CHECK_EQUAL(str2,std::string("field2 of field1"));

    auto member3=_["field1"]["field2"][size];
    std::string str3=mn(member3);
    BOOST_CHECK_EQUAL(str3,std::string("size of field2 of field1"));

    std::string str4=dotted_member_names(member1);
    BOOST_CHECK_EQUAL(str4,std::string("[field1]"));

    std::string str5=dotted_member_names(member2);
    BOOST_CHECK_EQUAL(str5,std::string("[field1].[field2]"));

    std::string str6=dotted_member_names(member3);
    BOOST_CHECK_EQUAL(str6,std::string("[field1].[field2].[size]"));
}

BOOST_AUTO_TEST_CASE(CheckExplicitMemberName)
{
    auto mn=get_default_member_names();

    auto member1=_["field1"]("member1 name");
    std::string str1=mn(member1);
    BOOST_CHECK_EQUAL(str1,"member1 name");

    auto member2=_["field1"]["field2"]("member2 name");
    std::string str2=mn(member2);
    BOOST_CHECK_EQUAL(str2,"member2 name");

    auto member3=_["field1"]["field2"][size]("size of nested field");
    std::string str3=mn(member3);
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

BOOST_AUTO_TEST_CASE(CheckMemberProperty)
{
    auto member=_["field1"];
    auto mp=make_member_property(member,size);
    auto&& mn=get_default_member_names();
    auto str=mn(mp);
    BOOST_CHECK_EQUAL(std::string(str),"size of field1");
}

BOOST_AUTO_TEST_SUITE_END()

