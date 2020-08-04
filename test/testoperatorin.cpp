#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/interval.hpp>
#include <dracosha/validator/operators/in.hpp>
#include <dracosha/validator/operators/lex_in.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestOperatorIn)

BOOST_AUTO_TEST_CASE(CheckInInterval)
{
    size_t val=90;
    auto a1=make_default_adapter(val);

    auto v1=validator(in,interval(1,100));
    BOOST_CHECK(v1.apply(a1));

    auto v2=validator(in,interval(95,100));
    BOOST_CHECK(!v2.apply(a1));

    auto v3=validator(in,interval(90,100));
    BOOST_CHECK(v3.apply(a1));

    auto v4=validator(in,interval(90,100,interval.open()));
    BOOST_CHECK(!v4.apply(a1));

    auto v5=validator(in,interval(90,100,interval.open_from()));
    BOOST_CHECK(!v5.apply(a1));

    auto v6=validator(in,interval(90,100,interval.open_to()));
    BOOST_CHECK(v6.apply(a1));

    auto v7=validator(in,interval(80,90,interval.open_to()));
    BOOST_CHECK(!v7.apply(a1));

    auto v8=validator(nin,interval(1,100));
    BOOST_CHECK(!v8.apply(a1));

    auto v9=validator(nin,interval(95,100));
    BOOST_CHECK(v9.apply(a1));

    auto v10=validator(nin,interval(90,100));
    BOOST_CHECK(!v10.apply(a1));

    auto v11=validator(nin,interval(90,100,interval.open()));
    BOOST_CHECK(v11.apply(a1));

    auto v12=validator(nin,interval(90,100,interval.open_from()));
    BOOST_CHECK(v12.apply(a1));

    auto v13=validator(nin,interval(90,100,interval.open_to()));
    BOOST_CHECK(!v13.apply(a1));

    auto v14=validator(nin,interval(80,90,interval.open_to()));
    BOOST_CHECK(v14.apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckInIntervalReport)
{
    std::string rep;
    size_t val=90;
    auto a1=make_reporting_adapter(val,rep);

    auto v1=validator(in,interval(1,100));
    BOOST_CHECK(v1.apply(a1));

    auto v2=validator(in,interval(95,100));
    BOOST_CHECK(!v2.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval [95,100]");
    rep.clear();

    auto v3=validator(in,interval(90,100));
    BOOST_CHECK(v3.apply(a1));

    auto v4=validator(in,interval(90,100,interval.open()));
    BOOST_CHECK(!v4.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval (90,100)");
    rep.clear();

    auto v5=validator(in,interval(90,100,interval.open_from()));
    BOOST_CHECK(!v5.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval (90,100]");
    rep.clear();

    auto v6=validator(in,interval(90,100,interval.open_to()));
    BOOST_CHECK(v6.apply(a1));

    auto v7=validator(in,interval(80,90,interval.open_to()));
    BOOST_CHECK(!v7.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval [80,90)");
    rep.clear();

    auto v8=validator(nin,interval(1,100));
    BOOST_CHECK(!v8.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in interval [1,100]");
    rep.clear();

    auto v9=validator(nin,interval(95,100));
    BOOST_CHECK(v9.apply(a1));

    auto v10=validator(nin,interval(90,100));
    BOOST_CHECK(!v10.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in interval [90,100]");
    rep.clear();

    auto v11=validator(nin,interval(90,100,interval.open()));
    BOOST_CHECK(v11.apply(a1));

    auto v12=validator(nin,interval(90,100,interval.open_from()));
    BOOST_CHECK(v12.apply(a1));

    auto v13=validator(nin,interval(90,100,interval.open_to()));
    BOOST_CHECK(!v13.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in interval [90,100)");
    rep.clear();

    auto v14=validator(nin,interval(80,90,interval.open_to()));
    BOOST_CHECK(v14.apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckLexInIntervalReport)
{
    std::string rep;
    std::string val("hello");
    auto a1=make_reporting_adapter(val,rep);

    auto v1=validator(lex_in,interval("a","z"));
    BOOST_CHECK(v1.apply(a1));

    auto v2=validator(lex_in,interval("A","Z"));
    BOOST_CHECK(!v2.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval [A,Z]");
    rep.clear();

    auto v3=validator(ilex_in,interval("A","Z"));
    BOOST_CHECK(v3.apply(a1));

    auto v4=validator(lex_in,interval("hello","z",interval.open()));
    BOOST_CHECK(!v4.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval (hello,z)");
    rep.clear();

    auto v5=validator(lex_in,interval("HELLO","z",interval.open()));
    BOOST_CHECK(v5.apply(a1));

    auto v6=validator(ilex_in,interval("HELLO","z",interval.open()));
    BOOST_CHECK(!v6.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval (HELLO,z)");
    rep.clear();

    auto v7=validator(lex_in,interval("hello","z",interval.open_from()));
    BOOST_CHECK(!v7.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval (hello,z]");
    rep.clear();

    auto v8=validator(lex_in,interval("HELLO","z",interval.open_from()));
    BOOST_CHECK(v8.apply(a1));

    auto v9=validator(ilex_in,interval("HELLO","z",interval.open_from()));
    BOOST_CHECK(!v9.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval (HELLO,z]");
    rep.clear();

    auto v10=validator(lex_in,interval("a","hello",interval.open_to()));
    BOOST_CHECK(!v10.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval [a,hello)");
    rep.clear();

    auto v11=validator(lex_in,interval("A","HELLO",interval.open_to()));
    BOOST_CHECK(!v11.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval [A,HELLO)");
    rep.clear();

    auto v12=validator(ilex_in,interval("A","HELLO",interval.open_to()));
    BOOST_CHECK(!v12.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval [A,HELLO)");
    rep.clear();

    auto v13=validator(lex_nin,interval("a","z"));
    BOOST_CHECK(!v13.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in interval [a,z]");
    rep.clear();

    auto v14=validator(lex_nin,interval("A","Z"));
    BOOST_CHECK(v14.apply(a1));

    auto v15=validator(ilex_nin,interval("A","Z"));
    BOOST_CHECK(!v15.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in interval [A,Z]");
    rep.clear();

    auto v16=validator(lex_nin,interval("hello","z",interval.open()));
    BOOST_CHECK(v16.apply(a1));

    auto v17=validator(lex_nin,interval("HELLO","z",interval.open()));
    BOOST_CHECK(!v17.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in interval (HELLO,z)");
    rep.clear();

    auto v18=validator(ilex_nin,interval("HELLO","z",interval.open()));
    BOOST_CHECK(v18.apply(a1));

    auto v19=validator(lex_nin,interval("hello","z",interval.open_from()));
    BOOST_CHECK(v19.apply(a1));

    auto v20=validator(lex_nin,interval("HELLO","z",interval.open_from()));
    BOOST_CHECK(!v20.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in interval (HELLO,z]");
    rep.clear();

    auto v21=validator(ilex_nin,interval("HELLO","z",interval.open_from()));
    BOOST_CHECK(v21.apply(a1));

    auto v22=validator(lex_nin,interval("a","hello",interval.open_to()));
    BOOST_CHECK(v22.apply(a1));

    auto v23=validator(lex_nin,interval("A","HELLO",interval.open_to()));
    BOOST_CHECK(v23.apply(a1));

    auto v24=validator(ilex_nin,interval("A","HELLO",interval.open_to()));
    BOOST_CHECK(v24.apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckInRange)
{
    size_t val=90;
    auto a1=make_default_adapter(val);

    auto v1=validator(in,range({70,80,90,100}));
    BOOST_CHECK(v1.apply(a1));

    auto v2=validator(in,range({70,80}));
    BOOST_CHECK(!v2.apply(a1));

    std::vector<size_t> vec3{70,80,90,100};
    auto v3=validator(in,range(vec3));
    BOOST_CHECK(v3.apply(a1));

    std::vector<size_t> vec4{70,80};
    auto v4=validator(in,range(vec4));
    BOOST_CHECK(!v4.apply(a1));

    auto v5=validator(in,range({70,80,90,100},sorted));
    BOOST_CHECK(v5.apply(a1));

    auto v6=validator(in,range({70,80},sorted));
    BOOST_CHECK(!v6.apply(a1));

    auto v7=validator(in,range(vec3,sorted));
    BOOST_CHECK(v7.apply(a1));

    auto v8=validator(in,range(vec4,sorted));
    BOOST_CHECK(!v8.apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckLexInRange)
{
    std::string val("hello");
    auto a1=make_default_adapter(val);

    auto v1=validator(lex_in,range({"one","two","hello","three"}));
    BOOST_CHECK(v1.apply(a1));

    auto v2=validator(lex_in,range({"HELLO","one","two"}));
    BOOST_CHECK(!v2.apply(a1));

    std::vector<std::string> vec3{"one","two","hello","three"};
    auto v3=validator(lex_in,range(vec3));
    BOOST_CHECK(v3.apply(a1));

    std::vector<std::string> vec4{"HELLO","one","two"};
    auto v4=validator(lex_in,range(vec4));
    BOOST_CHECK(!v4.apply(a1));

    auto v5=validator(lex_in,range({"book","cake","game","hello","tea","zero"},sorted));
    BOOST_CHECK(v5.apply(a1));

    auto v6=validator(lex_in,range({"HELLO","one","two"},sorted));
    BOOST_CHECK(!v6.apply(a1));

    std::vector<std::string> vec7{"book","cake","game","hello","tea","zero"};
    auto v7=validator(lex_in,range(vec7,sorted));
    BOOST_CHECK(v7.apply(a1));

    auto v8=validator(lex_in,range(vec4,sorted));
    BOOST_CHECK(!v8.apply(a1));

    auto v9=validator(ilex_in,range({"one","two","hello","three"}));
    BOOST_CHECK(v1.apply(a1));

    auto v10=validator(ilex_in,range({"HELLO","one","two"}));
    BOOST_CHECK(v10.apply(a1));

    auto v11=validator(ilex_in,range(vec3));
    BOOST_CHECK(v11.apply(a1));

    std::vector<std::string> vec5{"HELLO","one","two"};
    auto v12=validator(ilex_in,range(vec5));
    BOOST_CHECK(v12.apply(a1));

    auto v13=validator(ilex_in,range({"book","cake","game","hello","tea","zero"},sorted));
    BOOST_CHECK(v13.apply(a1));

    auto v14=validator(ilex_in,range({"HELLO","one","two"},sorted));
    BOOST_CHECK(v14.apply(a1));

    auto v15=validator(ilex_in,range(vec7,sorted));
    BOOST_CHECK(v15.apply(a1));

    auto v16=validator(ilex_in,range(vec5,sorted));
    BOOST_CHECK(v16.apply(a1));

    auto v17=validator(ilex_in,range({"one","two"}));
    BOOST_CHECK(!v17.apply(a1));

    std::vector<std::string> vec8{"one","two"};
    auto v18=validator(ilex_in,range(vec8));
    BOOST_CHECK(!v18.apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckInRangeReport)
{
    std::string rep;
    size_t val=90;
    auto a1=make_reporting_adapter(val,rep);

    auto v1=validator(in,range({70,80,90,100}));
    BOOST_CHECK(v1.apply(a1));

    auto v2=validator(in,range({70,80,100,1000}));
    BOOST_CHECK(!v2.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in range [70, 80, 100, 1000]");
    rep.clear();

    std::vector<size_t> vec3{70,80,90,100};
    auto v3=validator(in,range(vec3));
    BOOST_CHECK(v3.apply(a1));

    std::vector<size_t> vec4{70,80,100,1000};
    auto v4=validator(in,range(vec4));
    BOOST_CHECK(!v4.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in range [70, 80, 100, 1000]");
    rep.clear();

    auto v5=validator(in,range({70,80,90,100},sorted));
    BOOST_CHECK(v5.apply(a1));

    auto v6=validator(in,range({70,80,100,1000},sorted));
    BOOST_CHECK(!v6.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in range [70, 80, 100, 1000]");
    rep.clear();

    auto v7=validator(in,range(vec3,sorted));
    BOOST_CHECK(v7.apply(a1));

    auto v8=validator(in,range(vec4,sorted));
    BOOST_CHECK(!v8.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in range [70, 80, 100, 1000]");
    rep.clear();

    auto v9=validator(nin,range({70,80,90,100}));
    BOOST_CHECK(!v9.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in range [70, 80, 90, 100]");
    rep.clear();

    auto v10=validator(nin,range({70,80,100,1000}));
    BOOST_CHECK(v10.apply(a1));

    auto v11=validator(nin,range(vec3));
    BOOST_CHECK(!v11.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in range [70, 80, 90, 100]");
    rep.clear();

    auto v12=validator(nin,range(vec4));
    BOOST_CHECK(v12.apply(a1));

    auto v13=validator(nin,range({70,80,90,100},sorted));
    BOOST_CHECK(!v13.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in range [70, 80, 90, 100]");
    rep.clear();

    auto v14=validator(nin,range({70,80,100,1000},sorted));
    BOOST_CHECK(v14.apply(a1));

    auto v15=validator(nin,range(vec3,sorted));
    BOOST_CHECK(!v15.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in range [70, 80, 90, 100]");
    rep.clear();

    auto v16=validator(nin,range(vec4,sorted));
    BOOST_CHECK(v16.apply(a1));

    auto v17=validator(in,range({70,80,100,1000,10000},3));
    BOOST_CHECK(!v17.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in range [70, 80, 100, ... ]");
    rep.clear();

    std::vector<size_t> vec18{70,80,100,1000,10000};
    auto v18=validator(in,range(vec18,3));
    BOOST_CHECK(!v18.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in range [70, 80, 100, ... ]");
    rep.clear();
}

BOOST_AUTO_TEST_SUITE_END()
