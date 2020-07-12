#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

using namespace dracosha::validator;

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
    auto ra1=make_reporting_adapter(report,nc);
    using adapter_type1=decltype(ra1);
    static_assert(std::is_same<typename adapter_type1::reporter_type,decltype(make_reporter(report))>::value,"");
    static_assert(!std::is_lvalue_reference<typename adapter_type1::reporter_type>::value,"");
    static_assert(!std::is_rvalue_reference<typename adapter_type1::reporter_type>::value,"");
    static_assert(!std::is_lvalue_reference<typename adapter_type1::next_adapter_type>::value,"");
    static_assert(!std::is_rvalue_reference<typename adapter_type1::next_adapter_type>::value,"");

    auto rp2=make_reporter(report);
    auto& rp_ref2=rp2;
    auto ra2=make_reporting_adapter(rp_ref2,nc);
    using adapter_type2=decltype(ra2);
    static_assert(std::is_same<typename adapter_type2::reporter_type,decltype(rp_ref2)>::value,"");
    static_assert(std::is_lvalue_reference<typename adapter_type2::reporter_type>::value,"");

    auto ra3=make_reporting_adapter(rp2,nc);
    using adapter_type3=decltype(ra3);
    static_assert(std::is_lvalue_reference<typename adapter_type3::reporter_type>::value,"");

    auto a4=make_adapter(nc);
    auto ra4=make_reporting_adapter(report,a4);
    using adapter_type4=decltype(ra4);
    static_assert(!std::is_lvalue_reference<typename adapter_type4::reporter_type>::value,"");
    static_assert(std::is_lvalue_reference<typename adapter_type4::next_adapter_type>::value,"");

    auto ra5=make_reporting_adapter(rp2,a4);
    using adapter_type5=decltype(ra5);
    static_assert(std::is_lvalue_reference<typename adapter_type5::reporter_type>::value,"");
    static_assert(std::is_lvalue_reference<typename adapter_type5::next_adapter_type>::value,"");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(CheckReporter)
{
    std::string rep1;
    auto r1=make_reporter(rep1);
    r1.validate("field1",value,gte,10);
    BOOST_CHECK_EQUAL(rep1,std::string("field1 is greater than or equal to 10"));

    std::string rep2;
    auto w2=wrap_backend_formatter(rep2);
    auto r2=make_reporter(w2);
    r2.validate("field1",value,gte,10);
    BOOST_CHECK_EQUAL(rep2,std::string("field1 is greater than or equal to 10"));

    auto& dst2_f=r2.destination<decltype(w2)>();
    std::string& s2_1=dst2_f;
    std::string& s2_2=w2;
    BOOST_CHECK(&s2_1==&s2_2);
    BOOST_CHECK_EQUAL(std::string(dst2_f),std::string(w2));

    auto& dst2_s=r2.destination<std::string>();
    BOOST_CHECK_EQUAL(dst2_s,rep2);
}

BOOST_AUTO_TEST_CASE(CheckReporterAggregate)
{
    std::string rep1;
    auto r1=make_reporter(rep1);

    r1.aggregate_open(string_and);
    r1.validate("field1",value,gte,10);
    r1.aggregate_close(false);
    BOOST_CHECK_EQUAL(rep1,std::string("field1 is greater than or equal to 10"));
    rep1.clear();

    r1.aggregate_open(string_or);
    r1.validate("field1",value,gte,10);
    r1.validate("field1",size,lt,100);
    r1.aggregate_close(false);
    BOOST_CHECK_EQUAL(rep1,std::string("field1 is greater than or equal to 10 OR size of field1 is less than 100"));
    rep1.clear();

    r1.aggregate_open(string_not);
    r1.validate("field1",value,gte,10);
    r1.aggregate_close(false);
    BOOST_CHECK_EQUAL(rep1,std::string("NOT field1 is greater than or equal to 10"));
    rep1.clear();

    r1.aggregate_open(string_or);
    r1.validate("field1",value,gte,10);
    r1.validate("field1",size,lt,100);
    r1.aggregate_open(string_and);
        r1.validate("field10",value,gte,5);
        r1.validate("field10",size,lt,50);
    r1.aggregate_close(false);
    r1.aggregate_close(false);
    BOOST_CHECK_EQUAL(rep1,std::string("field1 is greater than or equal to 10 OR size of field1 is less than 100 OR (field10 is greater than or equal to 5 AND size of field10 is less than 50)"));
    rep1.clear();

    r1.aggregate_open(string_not);
    r1.aggregate_open(string_or);
    r1.validate("field1",value,gte,10);
    r1.validate("field1",size,lt,100);
    r1.aggregate_open(string_and);
        r1.validate("field10",value,gte,5);
        r1.validate("field10",size,lt,50);
    r1.aggregate_close(false);
    r1.aggregate_close(false);
    r1.aggregate_close(false);
    BOOST_CHECK_EQUAL(rep1,std::string("NOT (field1 is greater than or equal to 10 OR size of field1 is less than 100 OR (field10 is greater than or equal to 5 AND size of field10 is less than 50))"));
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckValidationReportOp)
{
    std::map<std::string,size_t> m1={{"field1",1}};

    auto v1=validator(
                _["field1"](gte,10)
            );

    std::string rep1;
    auto ra1=make_reporting_adapter(rep1,m1);
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("field1 is greater than or equal to 10"));

    auto v2=validator(
                _["field1"](length(gte,10))
            );
    rep1.clear();
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("length of field1 is greater than or equal to 10"));
}

BOOST_AUTO_TEST_CASE(CheckValidationReportAggregation)
{
    std::map<std::string,std::string> m1={{"field1","value1"}};
    std::string rep1;
    auto ra1=make_reporting_adapter(rep1,m1);

    auto v1=validator(
                _["field1"](value(gte,"z1000") ^OR^ length(lt,3))
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("field1 is greater than or equal to z1000 OR length of field1 is less than 3"));
    rep1.clear();

    auto v2=validator(
                _["field1"](gte,"v"),
                _["field1"](size(gte,100))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("size of field1 is greater than or equal to 100"));
    rep1.clear();

    auto v3=validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,100) ^OR^ value(gte,"zzzzzzzzzzzz"))
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("field1 is greater than or equal to xxxxxx OR size of field1 is greater than or equal to 100 OR field1 is greater than or equal to zzzzzzzzzzzz"));
    rep1.clear();

    auto v4=validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,100) ^AND^ value(gte,"zzzzzzzzzzzz"))
            );
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("field1 is greater than or equal to xxxxxx OR size of field1 is greater than or equal to 100"));
    rep1.clear();

    auto v5=validator(
                _["field1"](gte,"xxxxxx")
                 ^OR^
                _["field1"](size(gte,1) ^AND^ value(gte,"zzzzzzzzzzzz"))
            );
    BOOST_CHECK(!v5.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("field1 is greater than or equal to xxxxxx OR field1 is greater than or equal to zzzzzzzzzzzz"));
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckValidationReportNot)
{
    std::map<std::string,size_t> m1={{"field1",10}};
    std::string rep1;
    auto ra1=make_reporting_adapter(rep1,m1);

    auto v1=validator(
                NOT(_["field1"](eq,10))
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("NOT field1 is equal to 10"));
    rep1.clear();

    auto v2=validator(
                _["field1"](NOT(value(gte,1)))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("NOT field1 is greater than or equal to 1"));
    rep1.clear();

    auto v3=validator(
                NOT(
                    _["field1"](eq,10),
                    _["field1"](gte,8)
                )
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("NOT (field1 is equal to 10 AND field1 is greater than or equal to 8)"));
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
    BOOST_CHECK_EQUAL(rep1,std::string("NOT field1 is equal to 10"));
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckNestedValidationReportOp)
{
    std::map<std::string,std::map<size_t,size_t>> m1={
            {"field1",{{1,10}}}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(rep1,m1);

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
    BOOST_CHECK_EQUAL(rep1,"1 of field1 is less than 5");
    rep1.clear();

    auto v3=validator(
                _[100](lt,5)
            );
    BOOST_CHECK(v3.apply(ra1));
    BOOST_TEST_MESSAGE(rep1);
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckOtherFieldReportOp)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(rep1,m1);

    auto v1=validator(
                _["field1"](gt,_["field2"]),
                _["field2"](eq,_["field3"])
            );
    BOOST_CHECK(v1.apply(ra1));
    rep1.clear();

    auto v2=validator(
                _["field2"](gt,_["field3"]) ^OR^ _["field3"](eq,_["field1"])
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 is greater than field3 OR field3 is equal to field1");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckSampleObjectReportOp)
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
    auto ra1=make_reporting_adapter(rep1,m1);

    auto v1=validator(
                _["field1"](gte,_(m2)),
                _["field2"](eq,_(m2))
            );
    BOOST_CHECK(v1.apply(ra1));
    rep1.clear();

    auto v2=validator(
                _["field2"](gt,_(m2)) ^OR^ _["field3"](lt,_(m2))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 is greater than field2 of sample OR field3 is less than field3 of sample");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckNotExistingMemberIgnoreReportOp)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(rep1,m1);
    ra1.set_check_member_exists_before_validation(true);

    auto v1=validator(
                _["field1"](gte,9),
                _["field2"](eq,100)
            );
    BOOST_CHECK(v1.apply(ra1));
    rep1.clear();

    auto v2=validator(
                _["field2"](gt,100)
            );
    BOOST_CHECK(v2.apply(ra1));
    rep1.clear();

    auto v3=validator(
                _["field2"](gt,100) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field1 is equal to 20");
    rep1.clear();

    auto v4=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20))
            );
    BOOST_CHECK(v4.apply(ra1));
    rep1.clear();

    auto v5=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,9)
            );
    BOOST_CHECK(v5.apply(ra1));
    rep1.clear();

    auto v6=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field1 is greater than or equal to 100");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckNotExistingMemberAbortReportOp)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(rep1,m1);
    ra1.set_check_member_exists_before_validation(true);
    ra1.set_unknown_member_mode(if_member_not_found::abort);

    auto v1=validator(
                _["field1"](gte,9),
                _["field2"](eq,100)
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 is equal to 100");
    rep1.clear();

    auto v2=validator(
                _["field2"](gt,100)
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 is greater than 100");
    rep1.clear();

    auto v3=validator(
                _["field2"](gt,100) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 is greater than 100 OR field1 is equal to 20");
    rep1.clear();

    auto v4=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20))
            );
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 is greater than 100 OR field2 is equal to 20");
    rep1.clear();

    auto v5=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,9)
            );
    BOOST_CHECK(!v5.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 is greater than 100 OR field2 is equal to 20");
    rep1.clear();

    auto v6=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 is greater than 100 OR field2 is equal to 20");
    rep1.clear();
}

//BOOST_AUTO_TEST_CASE(CheckNotExistingOtherMemberIgnoreReportOp)
//{
//    std::map<std::string,int> m1={
//            {"field1",10},
//            {"field3",5}
//        };
//    std::string rep1;
//    auto ra1=make_reporting_adapter(rep1,m1);
//    ra1.set_check_member_exists_before_validation(true);
//    ra1.set_unknown_member_mode(if_member_not_found::abort);

//    auto v1=validator(
//                _["field1"](gte,9),
//                _["field2"](eq,_["field1"])
//            );
//    BOOST_CHECK(!v1.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field2 is equal to field1");
//    rep1.clear();
//    auto v1_1=validator(
//                _["field1"](gte,9),
//                _["field3"](eq,_["field4"])
//            );
//    BOOST_CHECK(!v1_1.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field3 is equal to field4");
//    rep1.clear();

//    auto v2=validator(
//                _["field2"](gt,_["field1"])
//            );
//    BOOST_CHECK(!v2.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field2 is equal to field1");
//    rep1.clear();
//    auto v2_1=validator(
//                _["field3"](gt,_["field4"])
//            );
//    BOOST_CHECK(!v2_1.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field3 is equal to field4");
//    rep1.clear();

//    auto v3=validator(
//                _["field2"](gt,_["field3"]) ^OR^ _["field1"](eq,20)
//            );
//    BOOST_CHECK(!v3.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field1 is equal to 20");
//    rep1.clear();
//    auto v3_1=validator(
//                _["field1"](gt,_["field4"]) ^OR^ _["field1"](eq,20)
//            );
//    BOOST_CHECK(!v3_1.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field1 is equal to 20");
//    rep1.clear();

//    auto v4=validator(
//                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"]))
//            );
//    BOOST_CHECK(v4.apply(ra1));
//    rep1.clear();
//    auto v4_1=validator(
//                _["field1"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"]))
//            );
//    BOOST_CHECK(v4_1.apply(ra1));
//    rep1.clear();

//    auto v5=validator(
//                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
//                _["field1"](gte,9)
//            );
//    BOOST_CHECK(v5.apply(ra1));
//    rep1.clear();
//    auto v5_1=validator(
//                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
//                _["field1"](gte,9)
//            );
//    BOOST_CHECK(v5_1.apply(ra1));
//    rep1.clear();

//    auto v6=validator(
//                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
//                _["field1"](gte,100)
//            );
//    BOOST_CHECK(!v6.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field1 is greater than or equal to 100");
//    rep1.clear();
//    auto v6_1=validator(
//                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
//                _["field1"](gte,100)
//            );
//    BOOST_CHECK(!v6_1.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field1 is greater than or equal to 100");
//    rep1.clear();
//}

//BOOST_AUTO_TEST_CASE(CheckNotExistingOtherMemberAbortReportOp)
//{
//    std::map<std::string,int> m1={
//            {"field1",10},
//            {"field3",5}
//        };
//    std::string rep1;
//    auto ra1=make_reporting_adapter(rep1,m1);
//    ra1.set_check_member_exists_before_validation(true);

//    auto v1=validator(
//                _["field1"](gte,9),
//                _["field2"](eq,_["field1"])
//            );
//    BOOST_CHECK(v1.apply(ra1));
//    rep1.clear();
//    auto v1_1=validator(
//                _["field1"](gte,9),
//                _["field3"](eq,_["field4"])
//            );
//    BOOST_CHECK(v1_1.apply(ra1));
//    rep1.clear();

//    auto v2=validator(
//                _["field2"](gt,100)
//            );
//    BOOST_CHECK(v2.apply(ra1));
//    rep1.clear();
//    auto v2_1=validator(
//                _["field1"](gt,_["field4"])
//            );
//    BOOST_CHECK(v2_1.apply(ra1));
//    rep1.clear();

//    auto v3=validator(
//                _["field2"](gt,_["field3"]) ^OR^ _["field1"](eq,20)
//            );
//    BOOST_CHECK(!v3.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field1 is equal to 20");
//    rep1.clear();
//    auto v3_1=validator(
//                _["field1"](gt,_["field4"]) ^OR^ _["field1"](eq,20)
//            );
//    BOOST_CHECK(!v3_1.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field1 is equal to 20");
//    rep1.clear();

//    auto v4=validator(
//                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"]))
//            );
//    BOOST_CHECK(v4.apply(ra1));
//    rep1.clear();
//    auto v4_1=validator(
//                _["field1"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"]))
//            );
//    BOOST_CHECK(v4_1.apply(ra1));
//    rep1.clear();

//    auto v5=validator(
//                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
//                _["field1"](gte,9)
//            );
//    BOOST_CHECK(v5.apply(ra1));
//    rep1.clear();
//    auto v5_1=validator(
//                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
//                _["field1"](gte,9)
//            );
//    BOOST_CHECK(v5_1.apply(ra1));
//    rep1.clear();

//    auto v6=validator(
//                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
//                _["field1"](gte,100)
//            );
//    BOOST_CHECK(!v6.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field1 is greater than or equal to 100");
//    rep1.clear();
//    auto v6_1=validator(
//                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
//                _["field1"](gte,100)
//            );
//    BOOST_CHECK(!v6_1.apply(ra1));
//    BOOST_CHECK_EQUAL(rep1,"field1 is greater than or equal to 100");
//    rep1.clear();
//}

BOOST_AUTO_TEST_SUITE_END()
