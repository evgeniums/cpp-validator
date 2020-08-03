#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/operators/lexicographical.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestLexicographical)

BOOST_AUTO_TEST_CASE(CheckLexEquals)
{
    std::string rep;
    std::string str1="First string";
    std::string str2="Second string";
    std::string str1i="first strinG";

    auto ra1=make_reporting_adapter(str1,rep);
    auto ra2=make_reporting_adapter(str2,rep);
    auto ra1i=make_reporting_adapter(str1i,rep);

    auto v1=validator(lex_eq,"First string");
    BOOST_CHECK(v1.apply(ra1));
    BOOST_CHECK(!v1.apply(ra1i));
    BOOST_CHECK_EQUAL(rep,"must be equal to First string");
    rep.clear();
    BOOST_CHECK(!v1.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must be equal to First string");
    rep.clear();

    auto v2=validator(ilex_eq,"First string");
    BOOST_CHECK(v2.apply(ra1));
    BOOST_CHECK(v2.apply(ra1i));
    BOOST_CHECK(!v2.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must be equal to First string");
    rep.clear();

    auto v3=validator(lex_ne,"First string");
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must be not equal to First string");
    rep.clear();
    BOOST_CHECK(v3.apply(ra1i));
    BOOST_CHECK(v3.apply(ra2));

    auto v4=validator(ilex_ne,"First string");
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must be not equal to First string");
    rep.clear();
    BOOST_CHECK(!v4.apply(ra1i));
    BOOST_CHECK_EQUAL(rep,"must be not equal to First string");
    rep.clear();
    BOOST_CHECK(v4.apply(ra2));
}

BOOST_AUTO_TEST_CASE(CheckLexCompare)
{
    std::string rep;
    std::string str1="String10";
    std::string str2="String1";
    std::string str3="Hello";
    std::string str4="s";
    std::string str1i="string10";
    std::string str2i="string1";

    auto ra1=make_reporting_adapter(str1,rep);
    auto ra2=make_reporting_adapter(str2,rep);
    auto ra3=make_reporting_adapter(str3,rep);
    auto ra4=make_reporting_adapter(str4,rep);
    auto ra1i=make_reporting_adapter(str1i,rep);
    auto ra2i=make_reporting_adapter(str2i,rep);

    auto v1=validator(lex_gt,"String");
    BOOST_CHECK(v1.apply(ra1));
    BOOST_CHECK(v1.apply(ra1i));
    BOOST_CHECK(!v1.apply(ra3));
    BOOST_CHECK_EQUAL(rep,"must be greater than String");
    rep.clear();

    auto v2=validator(lex_lt,"String");
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must be less than String");
    rep.clear();
    BOOST_CHECK(!v2.apply(ra1i));
    BOOST_CHECK_EQUAL(rep,"must be less than String");
    rep.clear();
    BOOST_CHECK(v2.apply(ra3));

    auto v3=validator(lex_gte,"String1");
    BOOST_CHECK(v3.apply(ra1));
    BOOST_CHECK(v3.apply(ra2));
    BOOST_CHECK(v3.apply(ra1i));
    BOOST_CHECK(!v3.apply(ra3));
    BOOST_CHECK_EQUAL(rep,"must be greater than or equal to String1");
    rep.clear();

    auto v4=validator(lex_lte,"String1");
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must be less than or equal to String1");
    rep.clear();
    BOOST_CHECK(v4.apply(ra2));
    BOOST_CHECK(!v4.apply(ra1i));
    BOOST_CHECK_EQUAL(rep,"must be less than or equal to String1");
    rep.clear();
    BOOST_CHECK(v4.apply(ra3));

    auto v5=validator(ilex_gt,"string1");
    BOOST_CHECK(v5.apply(ra1));
    BOOST_CHECK(!v5.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must be greater than string1");
    rep.clear();
    BOOST_CHECK(!v5.apply(ra3));
    BOOST_CHECK_EQUAL(rep,"must be greater than string1");
    rep.clear();
    BOOST_CHECK(!v5.apply(ra4));
    BOOST_CHECK_EQUAL(rep,"must be greater than string1");
    rep.clear();
    BOOST_CHECK(v5.apply(ra1i));
    BOOST_CHECK(!v5.apply(ra2i));
    BOOST_CHECK_EQUAL(rep,"must be greater than string1");
    rep.clear();

    auto v6=validator(ilex_lt,"string1");
    BOOST_CHECK(!v6.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must be less than string1");
    rep.clear();
    BOOST_CHECK(!v6.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must be less than string1");
    rep.clear();
    BOOST_CHECK(v6.apply(ra3));
    BOOST_CHECK(v6.apply(ra4));
    BOOST_CHECK(!v6.apply(ra1i));
    BOOST_CHECK_EQUAL(rep,"must be less than string1");
    rep.clear();
    BOOST_CHECK(!v6.apply(ra2i));
    BOOST_CHECK_EQUAL(rep,"must be less than string1");
    rep.clear();

    auto v7=validator(ilex_gte,"string1");
    BOOST_CHECK(v7.apply(ra1));
    BOOST_CHECK(v7.apply(ra2));
    BOOST_CHECK(!v7.apply(ra3));
    BOOST_CHECK_EQUAL(rep,"must be greater than or equal to string1");
    rep.clear();
    BOOST_CHECK(!v7.apply(ra4));
    BOOST_CHECK_EQUAL(rep,"must be greater than or equal to string1");
    rep.clear();
    BOOST_CHECK(v7.apply(ra1i));
    BOOST_CHECK(v7.apply(ra2i));

    auto v8=validator(ilex_lte,"string1");
    BOOST_CHECK(!v8.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must be less than or equal to string1");
    rep.clear();
    BOOST_CHECK(v8.apply(ra2));
    BOOST_CHECK(v8.apply(ra3));
    BOOST_CHECK(v8.apply(ra4));
    BOOST_CHECK(!v8.apply(ra1i));
    BOOST_CHECK_EQUAL(rep,"must be less than or equal to string1");
    rep.clear();
    BOOST_CHECK(v8.apply(ra2i));
}

BOOST_AUTO_TEST_CASE(CheckLexParts)
{
    std::string rep;
    std::string str1="string one";

    auto ra1=make_reporting_adapter(str1,rep);

    auto v1=validator(lex_contains,"string");
    BOOST_CHECK(v1.apply(ra1));
    auto v2=validator(lex_contains,"ing");
    BOOST_CHECK(v2.apply(ra1));
    auto v3=validator(lex_contains,"ING");
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must contain ING");
    rep.clear();
    auto v4=validator(ilex_contains,"ING");
    BOOST_CHECK(v4.apply(ra1));
    auto v5=validator(lex_contains,"something");
    BOOST_CHECK(!v5.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must contain something");
    rep.clear();

    auto v6=validator(lex_starts_with,"string");
    BOOST_CHECK(v6.apply(ra1));
    auto v7=validator(lex_starts_with,"STRING");
    BOOST_CHECK(!v7.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must start with STRING");
    rep.clear();
    auto v8=validator(ilex_starts_with,"STRING");
    BOOST_CHECK(v8.apply(ra1));
    auto v9=validator(ilex_starts_with,"one");
    BOOST_CHECK(!v9.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must start with one");
    rep.clear();

    auto v10=validator(lex_ends_with,"one");
    BOOST_CHECK(v10.apply(ra1));
    auto v11=validator(lex_ends_with,"ONE");
    BOOST_CHECK(!v11.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must end with ONE");
    rep.clear();
    auto v12=validator(ilex_ends_with,"ONE");
    BOOST_CHECK(v12.apply(ra1));
    auto v13=validator(ilex_ends_with,"string");
    BOOST_CHECK(!v13.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"must end with string");
    rep.clear();
}

BOOST_AUTO_TEST_SUITE_END()
