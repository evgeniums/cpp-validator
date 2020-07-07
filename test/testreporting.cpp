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
}

BOOST_AUTO_TEST_CASE(CheckReportOp)
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

BOOST_AUTO_TEST_SUITE_END()
