#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/operators/regex.hpp>
#include <dracosha/validator/operators/string_patterns.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestRegex)

BOOST_AUTO_TEST_CASE(CheckRegexMatch)
{
    std::string rep;

    auto v1=validator(
        regex_match,"[0-9a-zA-Z_]+"
    );
    std::string str1="abcAnz120";
    auto ra1=make_reporting_adapter(str1,rep);
    BOOST_CHECK(v1.apply(ra1));

    auto v2=validator(
        regex_match,boost::regex("[0-9a-zA-Z_]+")
    );
    BOOST_CHECK(v2.apply(ra1));

    auto v2_std=validator(
        regex_match,std::regex("[0-9a-zA-Z_]+")
    );
    BOOST_CHECK(v2_std.apply(ra1));

    std::string str2="Hello world";
    auto ra2=make_reporting_adapter(str2,rep);
    BOOST_CHECK(!v1.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must match expression [0-9a-zA-Z_]+");
    rep.clear();

    BOOST_CHECK(!v2.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must match expression [0-9a-zA-Z_]+");
    rep.clear();

    BOOST_CHECK(!v2_std.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must match expression");
    rep.clear();

    auto v3=validator(
        regex_nmatch,boost::regex("[0-9a-zA-Z_]+")
    );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must not match expression [0-9a-zA-Z_]+");
    rep.clear();
    BOOST_CHECK(v3.apply(ra2));
    rep.clear();

    auto v3_std=validator(
        regex_nmatch,std::regex("[0-9a-zA-Z_]+")
    );
    BOOST_CHECK(!v3_std.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must not match expression");
    rep.clear();
    BOOST_CHECK(v3_std.apply(ra2));
}

BOOST_AUTO_TEST_CASE(CheckRegexContains)
{
    std::string rep;

    auto v1=validator(
        regex_contains,"[0-9][0-9][0-9]"
    );
    std::string str1="abcAnz120";
    auto ra1=make_reporting_adapter(str1,rep);
    BOOST_CHECK(v1.apply(ra1));

    auto v2=validator(
        regex_contains,boost::regex("[0-9][0-9][0-9]")
    );
    BOOST_CHECK(v2.apply(ra1));

    auto v2_std=validator(
        regex_contains,std::regex("[0-9][0-9][0-9]")
    );
    BOOST_CHECK(v2.apply(ra1));

    std::string str2="Hello world 12 34";
    auto ra2=make_reporting_adapter(str2,rep);
    BOOST_CHECK(!v1.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must contain expression [0-9][0-9][0-9]");
    rep.clear();

    BOOST_CHECK(!v2.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must contain expression [0-9][0-9][0-9]");
    rep.clear();

    BOOST_CHECK(!v2_std.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must contain expression");
    rep.clear();

    auto v3=validator(
        regex_ncontains,"[0-9][0-9][0-9]"
    );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must not contain expression [0-9][0-9][0-9]");
    rep.clear();
    BOOST_CHECK(v3.apply(ra2));
}

BOOST_AUTO_TEST_CASE(CheckAlpha)
{
    std::string rep;

    std::string str1="abcAnz120";
    auto ra1=make_reporting_adapter(str1,rep);
    std::string str2="Hello world!";
    auto ra2=make_reporting_adapter(str2,rep);

    auto v1=validator(
        str_alpha,true
    );
    BOOST_CHECK(v1.apply(ra1));
    BOOST_CHECK(!v1.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must contain only letters and digits");
    rep.clear();

    auto v2=validator(
        str_alpha,false
    );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must contain not only letters and digits");
    rep.clear();
    BOOST_CHECK(v2.apply(ra2));

    std::map<std::string,std::string> m3={
        {"field1","AbZ90_O"},
        {"field2","How are you?"}
    };
    auto ra3=make_reporting_adapter(m3,rep);

    auto v3=validator(
        _["field1"](str_alpha,true),
        _["field2"](str_alpha,false)
    );
    BOOST_CHECK(v3.apply(ra3));

    auto v4=validator(
        _["field1"](str_alpha,false),
        _["field2"](str_alpha,false)
    );
    BOOST_CHECK(!v4.apply(ra3));
    BOOST_CHECK_EQUAL(rep,"field1 must contain not only letters and digits");
    rep.clear();

    auto v5=validator(
        _["field1"](str_alpha,true),
        _["field2"](str_alpha,true)
    );
    BOOST_CHECK(!v5.apply(ra3));
    BOOST_CHECK_EQUAL(rep,"field2 must contain only letters and digits");
    rep.clear();
}

BOOST_AUTO_TEST_CASE(CheckHex)
{
    std::string rep;

    std::string str1="abcdef01234567890ABCDEF";
    auto ra1=make_reporting_adapter(str1,rep);
    std::string str2="Hello world!";
    auto ra2=make_reporting_adapter(str2,rep);

    auto v1=validator(
        str_hex,true
    );
    BOOST_CHECK(v1.apply(ra1));
    BOOST_CHECK(!v1.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must be a hexadecimal number");
    rep.clear();

    auto v2=validator(
        str_hex,false
    );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must be not a hexadecimal number");
    rep.clear();
    BOOST_CHECK(v2.apply(ra2));

    std::map<std::string,std::string> m3={
        {"field1",str1},
        {"field2",str2}
    };
    auto ra3=make_reporting_adapter(m3,rep);

    auto v3=validator(
        _["field1"](str_hex,true),
        _["field2"](str_hex,false)
    );
    BOOST_CHECK(v3.apply(ra3));

    auto v4=validator(
        _["field1"](str_hex,false),
        _["field2"](str_hex,false)
    );
    BOOST_CHECK(!v4.apply(ra3));
    BOOST_CHECK_EQUAL(rep,"field1 must be not a hexadecimal number");
    rep.clear();

    auto v5=validator(
        _["field1"](str_hex,true),
        _["field2"](str_hex,true)
    );
    BOOST_CHECK(!v5.apply(ra3));
    BOOST_CHECK_EQUAL(rep,"field2 must be a hexadecimal number");
    rep.clear();
}

BOOST_AUTO_TEST_SUITE_END()
