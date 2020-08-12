#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/operators/regex.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestRegex)

BOOST_AUTO_TEST_CASE(CheckRegex)
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

    std::string str2="Hello world";
    auto ra2=make_reporting_adapter(str2,rep);
    BOOST_CHECK(!v1.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must match expression [0-9a-zA-Z_]+");
    rep.clear();

    BOOST_CHECK(!v2.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"must match expression [0-9a-zA-Z_]+");
    rep.clear();
}

BOOST_AUTO_TEST_SUITE_END()
