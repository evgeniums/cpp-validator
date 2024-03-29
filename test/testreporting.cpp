#include <boost/test/unit_test.hpp>

#include <hatn/validator/validator.hpp>
#include <hatn/validator/adapters/reporting_adapter.hpp>
#include <hatn/validator/adapters/failed_members_adapter.hpp>
#include <hatn/validator/detail/hint_helper.hpp>

using namespace HATN_VALIDATOR_NAMESPACE;

namespace
{
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

BOOST_AUTO_TEST_SUITE(TestReporting)

BOOST_AUTO_TEST_CASE(CheckReportingAdapter)
{
    std::string report;
    NonCopyable nc;
    auto ra1=make_reporting_adapter(nc,report);
    using adapter_type1=decltype(ra1);
    static_assert(std::is_same<typename adapter_type1::reporter_type,decltype(make_reporter(report))>::value,"");
    static_assert(!std::is_lvalue_reference<typename adapter_type1::reporter_type>::value,"");
    static_assert(!std::is_rvalue_reference<typename adapter_type1::reporter_type>::value,"");

    auto rp2=make_reporter(report);
    auto& rp_ref2=rp2;
    auto ra2=make_reporting_adapter(nc,rp_ref2);
    using adapter_type2=decltype(ra2);
    static_assert(std::is_same<typename adapter_type2::reporter_type,decltype(rp_ref2)>::value,"");
    static_assert(std::is_lvalue_reference<typename adapter_type2::reporter_type>::value,"");

    auto ra3=make_reporting_adapter(nc,rp2);
    using adapter_type3=decltype(ra3);
    static_assert(std::is_lvalue_reference<typename adapter_type3::reporter_type>::value,"");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(CheckValidationReport)
{
    std::map<std::string,size_t> m1={{"field1",1}};

    auto v1=validator(
                _["field1"](gte,10)
            );

    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    const auto& reporter=ra1.traits().reporter();
    const auto& members=reporter.failed_members();
    BOOST_CHECK(members.empty());
    BOOST_CHECK_EQUAL(members.size(),0);

    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("field1 must be greater than or equal to 10"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();

    auto v2=validator(
                _["field1"](length(gte,10))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("length of field1 must be greater than or equal to 10"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();

    std::vector<size_t> vec1={10,20,30,40,50};
    auto ra2=make_reporting_adapter(vec1,rep1);
    const auto& members2=ra2.traits().reporter().failed_members();
    BOOST_CHECK(members2.empty());
    BOOST_CHECK_EQUAL(members2.size(),0);
    auto v3=validator(
                _[1](gte,100)
            );
    BOOST_CHECK(!v3.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,std::string("element #1 must be greater than or equal to 100"));
    BOOST_REQUIRE(!members2.empty());
    BOOST_CHECK_EQUAL(members2.size(),1);
    BOOST_CHECK_EQUAL(members2[0],"1");
    ra1.reset();
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckValidationExistsReport)
{
    std::map<std::string,size_t> m1={{"field1",1}};
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    const auto& members=ra1.traits().reporter().failed_members();
    BOOST_CHECK(members.empty());
    BOOST_CHECK_EQUAL(members.size(),0);

    auto v1=validator(
                _["field2"](exists,true)
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field2");
    ra1.reset();
    rep1.clear();

    auto v2=validator(
                _["field1"](exists,false)
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field1 must not exist");
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();

    auto v3=validator(
                _["field2"](exists,false)
            );
    BOOST_CHECK(v3.apply(ra1));
    rep1.clear();

    auto v4=validator(
                _["field2"](_(exists,"should exist"),true)
            );
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 should exist");
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field2");
    ra1.reset();
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckValidationReportAggregation)
{
    std::map<std::string,std::string> m1={{"field1","value1"}};
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    const auto& members=ra1.traits().reporter().failed_members();
    BOOST_CHECK(members.empty());
    BOOST_CHECK_EQUAL(members.size(),0);

    auto v1=validator(
                _["field1"](value(gte,"z1000") ^OR^ length(lt,3))
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("field1 must be greater than or equal to z1000 OR length of field1 must be less than 3"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();

    auto v2=validator(
                _["field1"](gte,"v"),
                _["field1"](size(gte,100))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("size of field1 must be greater than or equal to 100"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();

    auto v3=validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,100) ^OR^ value(gte,"zzzzzzzzzzzz"))
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("field1 must be greater than or equal to xxxxxx OR size of field1 must be greater than or equal to 100 OR field1 must be greater than or equal to zzzzzzzzzzzz"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();

    auto v4=validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,100) ^AND^ value(gte,"zzzzzzzzzzzz"))
            );
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("field1 must be greater than or equal to xxxxxx OR size of field1 must be greater than or equal to 100"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();

    auto v5=validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,1) ^AND^ value(gte,"zzzzzzzzzzzz"))
            );
    BOOST_CHECK(!v5.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("field1 must be greater than or equal to xxxxxx OR field1 must be greater than or equal to zzzzzzzzzzzz"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckValidationReportNot)
{
    std::map<std::string,size_t> m1={{"field1",10}};
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    const auto& reporter=ra1.traits().reporter();
    const auto& members=reporter.failed_members();
    BOOST_CHECK(members.empty());
    BOOST_CHECK_EQUAL(members.size(),0);

    auto v1=validator(
                NOT(_["field1"](eq,10))
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("NOT field1 must be equal to 10"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();

    auto v2=validator(
                _["field1"](NOT(value(gte,1)))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("NOT field1 must be greater than or equal to 1"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();

    auto v3=validator(
                NOT(
                    _["field1"](eq,10),
                    _["field1"](gte,8)
                )
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("NOT (field1 must be equal to 10 AND field1 must be greater than or equal to 8)"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();

    // validator reports only the first matched condition for nested OR operator
    auto v4=validator(
                NOT(
                    _["field1"](eq,10)
                    ^OR^
                    _["field1"](gte,100)
                )
            );
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("NOT field1 must be equal to 10"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckNestedValidationReport)
{
    std::map<std::string,std::map<size_t,size_t>> m1={
            {"field1",{{1,10}}}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    const auto& members=ra1.traits().reporter().failed_members();
    BOOST_CHECK(members.empty());
    BOOST_CHECK_EQUAL(members.size(),0);

    auto v1=validator(
                _[size](gte,0),
                _["field1"][1](gte,10)
            );
    BOOST_CHECK(v1.apply(ra1));
    rep1.clear();

    auto v2=validator(
                _["field1"][1](lt,5)
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"element #1 of field1 must be less than 5");
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1.1");
    ra1.reset();
    rep1.clear();

    auto v3=validator(
                _[100](lt,5)
            );
    BOOST_CHECK(v3.apply(ra1));
    BOOST_CHECK(members.empty());
    BOOST_CHECK_EQUAL(members.size(),0);
    ra1.reset();
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckOtherFieldReport)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    const auto& members=ra1.traits().reporter().failed_members();
    BOOST_CHECK(members.empty());
    BOOST_CHECK_EQUAL(members.size(),0);

    auto v1=validator(
                _["field1"](gt,_["field2"]),
                _["field2"](eq,_["field3"])
            );
    BOOST_CHECK(v1.apply(ra1));
    BOOST_CHECK(members.empty());
    BOOST_CHECK_EQUAL(members.size(),0);
    ra1.reset();
    rep1.clear();

    auto v2=validator(
                _["field2"](gt,_["field3"]) ^OR^ _["field3"](eq,_["field1"])
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must be greater than field3 OR field3 must be equal to field1");
    BOOST_CHECK(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),2);
    BOOST_CHECK_EQUAL(members[0],"field2");
    BOOST_CHECK_EQUAL(members[1],"field3");
    ra1.reset();
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckSampleObjectReport)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    std::map<std::string,int> m2={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    const auto& members=ra1.traits().reporter().failed_members();
    BOOST_CHECK(members.empty());
    BOOST_CHECK_EQUAL(members.size(),0);

    auto v1=validator(
                _["field1"](gte,_(m2)),
                _["field2"](eq,_(m2))
            );
    BOOST_CHECK(v1.apply(ra1));
    BOOST_CHECK(members.empty());
    BOOST_CHECK_EQUAL(members.size(),0);
    ra1.reset();
    rep1.clear();

    auto v2=validator(
                _["field2"](gt,_(m2)) ^OR^ _["field3"](lt,_(m2))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must be greater than field2 of sample OR field3 must be less than field3 of sample");
    BOOST_CHECK(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),2);
    BOOST_CHECK_EQUAL(members[0],"field2");
    BOOST_CHECK_EQUAL(members[1],"field3");
    ra1.reset();
    rep1.clear();

    auto v3=validator(
                _["field2"](gt,_(_(m2),"m2")) ^OR^ _["field3"](lt,_(_(m2),"m2"))
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must be greater than field2 of m2 OR field3 must be less than field3 of m2");
    BOOST_CHECK(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),2);
    BOOST_CHECK_EQUAL(members[0],"field2");
    BOOST_CHECK_EQUAL(members[1],"field3");
    ra1.reset();
    rep1.clear();

    std::map<size_t,size_t> m3={
        {1,10},
        {2,20},
        {3,30}
    };
    std::map<size_t,size_t> m4={
        {1,5},
        {2,20}
    };
    auto ra2=make_reporting_adapter(m4,rep1);
    const auto& members2=ra2.traits().reporter().failed_members();
    auto v4=validator(
                _[1](gt,_(_(m3),"m3"))
            );
    BOOST_CHECK(!v4.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"element #1 must be greater than element #1 of m3");
    BOOST_REQUIRE(!members2.empty());
    BOOST_CHECK_EQUAL(members2.size(),1);
    BOOST_CHECK_EQUAL(members2[0],"1");
    ra2.reset();
    rep1.clear();

    std::map<std::string,size_t> m5={
        {"one",5},
        {"two",20}
    };
    auto v5=validator(
                _[1](gt,_(m5))
            );
    BOOST_CHECK(v5.apply(ra2));

    auto ra5=make_reporting_adapter(m5,rep1);
    BOOST_CHECK(v5.apply(ra5));

    auto v6=validator(
                _["three"](gt,_(m5))
            );
    BOOST_CHECK(v6.apply(ra5));
}

BOOST_AUTO_TEST_CASE(CheckFlagValidationReport)
{
    std::string rep1;

    std::map<std::string,size_t> m1={{"field1",1}};
    auto ra1=make_reporting_adapter(m1,rep1);

    auto v1=validator(
                empty(flag,true)
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"must be empty");
    rep1.clear();

    auto v2=validator(
                empty(flag,false)
            );
    BOOST_CHECK(v2.apply(ra1));
    rep1.clear();

    auto v3=validator(
                empty(flag(flag_on_off),true)
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"empty must be on");
    rep1.clear();

    std::map<std::string,std::map<size_t,size_t>> m2={{"field1",{{1,10}}}};
    auto ra2=make_reporting_adapter(m2,rep1);

    auto v4=validator(
                _["field1"](empty(flag,true))
            );
    BOOST_CHECK(!v4.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"field1 must be empty");
    rep1.clear();

    auto v5=validator(
                _["field1"](empty(flag(flag_checked_unchecked),true))
            );
    BOOST_CHECK(!v5.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"empty of field1 must be checked");
    rep1.clear();

    auto v5_1=validator(
                _["field1"](empty(_(flag,flag_checked_unchecked),true))
            );
    BOOST_CHECK(!v5_1.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"empty of field1 must be checked");
    rep1.clear();

    auto v6=validator(
                _["field1"](empty(flag("expected to be true"),true))
            );
    BOOST_CHECK(!v6.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"empty of field1 expected to be true");
    rep1.clear();

    auto v6_1=validator(
                _["field1"](empty(_(flag,"expected to be true"),true))
            );
    BOOST_CHECK(!v6_1.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"empty of field1 expected to be true");
    rep1.clear();

    auto v7=validator(
                _[empty](flag,true)
            );
    BOOST_CHECK(!v7.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"must be empty");
    rep1.clear();

    auto v8=validator(
                _["field1"][empty](flag,true)
            );
    BOOST_CHECK(!v8.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"field1 must be empty");
    rep1.clear();

    auto v9=validator(
                _["field1"][empty](flag(flag_set_unset),true)
            );
    BOOST_CHECK(!v9.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"empty of field1 must be set");
    rep1.clear();

    auto v9_1=validator(
                _["field1"][empty](_(flag,flag_set_unset),true)
            );
    BOOST_CHECK(!v9_1.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"empty of field1 must be set");
    rep1.clear();

    auto v10=validator(
                _["field1"][empty](flag("expected to be true"),true)
            );
    BOOST_CHECK(!v10.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"empty of field1 expected to be true");
    rep1.clear();

    auto v11=validator(
                _["field1"][empty](_(flag,"expected to be true"),true)
            );
    BOOST_CHECK(!v11.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"empty of field1 expected to be true");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckContainsValidationReport)
{
    std::string rep1;

    std::map<std::string,size_t> m1={{"field1",1}};

    auto v1=validator(
                contains,"field2"
            );
    auto ra1=make_reporting_adapter(m1,rep1);
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"must contain field2");
    rep1.clear();

    std::map<std::string,std::map<size_t,std::string>> m2={
            {"field1",
             {
                 {1,"value10"},{2,"value100"},{3,"hi"},{4,"hello"}
             }
            }
        };
    auto ra2=make_reporting_adapter(m2,rep1);
    const auto& members=ra2.traits().reporter().failed_members();
    auto v2=validator(
                _["field1"](contains,30)
            );
    auto ret=v2.apply(ra2);
    BOOST_CHECK(!ret);
    BOOST_CHECK(ret.fail());
    BOOST_CHECK_EQUAL(rep1,std::string("field1 must contain element #30"));
    BOOST_REQUIRE(!members.empty());
    BOOST_CHECK_EQUAL(members.size(),1);
    BOOST_CHECK_EQUAL(members[0],"field1");
    ra1.reset();
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckCollectAllFailedMembers)
{
    std::string rep1;

    std::map<std::string,size_t> m1={
        {"field1",1},
        {"field2",2},
        {"field3",3},
        {"field4",4}
    };

    auto ra1=make_failed_members_adapter(m1);
    const auto& members1=ra1.traits().reporter().failed_members();

    auto v0=validator(
        _["field1"](eq,1),
        _["field4"](lt,5)
        );
    auto ret=v0.apply(ra1);
    BOOST_CHECK_EQUAL(int(ret.value()),int(status::code::success));
    BOOST_CHECK(ret.success()); // do not count on this because success() maybe returned when validation failed
    BOOST_REQUIRE(members1.empty()); // members are empty for success validation
    ra1.reset();

    auto v1=validator(
            _["field1"](eq,10),
            _["field4"](gte,5)
        );
    ret=v1.apply(ra1);
    BOOST_CHECK_EQUAL(int(ret.value()),int(status::code::ignore));
    BOOST_CHECK(ret.ignore());
    BOOST_REQUIRE(!members1.empty());
    BOOST_CHECK_EQUAL(members1.size(),2);
    BOOST_CHECK_EQUAL(members1[0],"field1");
    BOOST_CHECK_EQUAL(members1[1],"field4");
    ra1.reset();

    auto v2=validator(
            _["field1"](eq,10)
            ||
            _["field4"](gte,5)
        );
    ret=v2.apply(ra1);
    BOOST_CHECK(ret.ignore());
    BOOST_REQUIRE(!members1.empty());
    BOOST_CHECK_EQUAL(members1.size(),2);
    BOOST_CHECK_EQUAL(members1[0],"field1");
    BOOST_CHECK_EQUAL(members1[1],"field4");
    ra1.reset();

    auto v3=validator(
        _["field1"](eq,10)
        ||
        (_["field4"](gte,5) && _["field5"](exists,true) && _["field3"](eq,3))
        );
    ret=v3.apply(ra1);
    BOOST_CHECK(ret.success()); // can return success for actual failure
    BOOST_REQUIRE(!members1.empty()); // members are not empty for failed validation
    BOOST_CHECK_EQUAL(members1.size(),3);
    BOOST_CHECK_EQUAL(members1[0],"field1");
    BOOST_CHECK_EQUAL(members1[1],"field4");
    BOOST_CHECK_EQUAL(members1[2],"field5");
    ra1.reset();

    // validation succeeds
    auto v4=validator(
        _["field1"](value(eq,10) || value(eq,1))
        ||
        (_["field4"](gte,5) && _["field5"](exists,true) && _["field3"](eq,3))
        );
    ret=v4.apply(ra1);
    BOOST_REQUIRE(members1.empty()); // members are empty because validation succeeded
    ra1.reset();

    // validation fails
    auto v5=validator(
        _["field2"](value(lte,5) && value(gt,2))
        ||
        (_["field4"](gte,5) && _["field5"](exists,true) && _["field3"](eq,3))
        );
    ret=v5.apply(ra1);
    BOOST_CHECK(ret.success());
    BOOST_REQUIRE(!members1.empty()); // members are not empty for failed validation
    BOOST_CHECK_EQUAL(members1.size(),3);
    BOOST_CHECK_EQUAL(members1[0],"field2");
    BOOST_CHECK_EQUAL(members1[1],"field4");
    BOOST_CHECK_EQUAL(members1[2],"field5");
    ra1.reset();
}

BOOST_AUTO_TEST_SUITE_END()
