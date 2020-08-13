#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/operators/number_patterns.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestNumberPatterns)

BOOST_AUTO_TEST_CASE(CheckStrInt)
{
    std::string rep;

    std::string str1="12345678";
    auto ra1=make_reporting_adapter(str1,rep);
    std::string str2="Hello world!";
    auto ra2=make_reporting_adapter(str2,rep);
    std::string str3="-12345";
    auto ra4=make_reporting_adapter(str3,rep);

    auto v1=validator(
        str_int,true
    );
    BOOST_CHECK(v1.apply(ra1));
    BOOST_CHECK(v1.apply(ra4));
    BOOST_CHECK(!v1.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must be integer");
    rep.clear();

    auto v2=validator(
        str_int,false
    );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must not be integer");
    rep.clear();
    BOOST_CHECK(v2.apply(ra2));

    std::map<std::string,std::string> m3={
        {"field1",str1},
        {"field2",str2}
    };
    auto ra3=make_reporting_adapter(m3,rep);

    auto v3=validator(
        _["field1"](str_int,true),
        _["field2"](str_int,false)
    );
    BOOST_CHECK(v3.apply(ra3));

    auto v4=validator(
        _["field1"](str_int,false),
        _["field2"](str_int,false)
    );
    BOOST_CHECK(!v4.apply(ra3));
    BOOST_CHECK_EQUAL(rep,"field1 must not be integer");
    rep.clear();

    auto v5=validator(
        _["field1"](str_int,true),
        _["field2"](str_int,true)
    );
    BOOST_CHECK(!v5.apply(ra3));
    BOOST_CHECK_EQUAL(rep,"field2 must be integer");
    rep.clear();
}

BOOST_AUTO_TEST_CASE(CheckStrFloat)
{
    std::string rep;

    std::string str1="12345678";
    auto ra1=make_reporting_adapter(str1,rep);
    std::string str2="1234.5678";
    auto ra2=make_reporting_adapter(str2,rep);
    std::string str3="-12345678";
    auto ra3=make_reporting_adapter(str3,rep);
    std::string str4="-1234.5678";
    auto ra4=make_reporting_adapter(str4,rep);
    std::string str5="1234e-10";
    auto ra5=make_reporting_adapter(str5,rep);
    std::string str6="1234e20";
    auto ra6=make_reporting_adapter(str6,rep);
    std::string str7="abcdef01234";
    auto ra7=make_reporting_adapter(str7,rep);

    auto v1=validator(
        str_float,true
    );
    BOOST_CHECK(v1.apply(ra1));
    BOOST_CHECK(v1.apply(ra2));
    BOOST_CHECK(v1.apply(ra3));
    BOOST_CHECK(v1.apply(ra4));
    BOOST_CHECK(v1.apply(ra5));
    BOOST_CHECK(v1.apply(ra6));
    BOOST_CHECK(!v1.apply(ra7));
    BOOST_CHECK_EQUAL(rep,"must be a floating point number");
    rep.clear();

    auto v2=validator(
        str_float,false
    );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must be not a floating point number");
    rep.clear();
    BOOST_CHECK(!v2.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must be not a floating point number");
    rep.clear();
    BOOST_CHECK(!v2.apply(ra3));
    BOOST_CHECK_EQUAL(rep,"must be not a floating point number");
    rep.clear();
    BOOST_CHECK(!v2.apply(ra4));
    BOOST_CHECK_EQUAL(rep,"must be not a floating point number");
    rep.clear();
    BOOST_CHECK(!v2.apply(ra5));
    BOOST_CHECK_EQUAL(rep,"must be not a floating point number");
    rep.clear();
    BOOST_CHECK(!v2.apply(ra6));
    BOOST_CHECK_EQUAL(rep,"must be not a floating point number");
    rep.clear();
}

BOOST_AUTO_TEST_SUITE_END()
