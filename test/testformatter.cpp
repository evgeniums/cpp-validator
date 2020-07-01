#include <string>
#include <vector>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/config.hpp>

#ifdef DRACOSHA_VALIDATOR_FMT
#include <fmt/format.h>

using namespace dracosha::validator;

BOOST_AUTO_TEST_SUITE(TestFmtFormatter)

//! \todo move fmt to special file under def
BOOST_AUTO_TEST_CASE(CheckFmtString)
{
    std::string str1, str2;
    std::vector<char> vec1, vec2;

    fmt::format_to(std::back_inserter(str1),"Hello {}",1);
    fmt::format_to(std::back_inserter(vec1),"Hello vector {}",1);

    fmt::format_to(std::back_inserter(str2),"prepend {} ",2);
    str1.insert(str1.begin(),str2.begin(),str2.end());
    BOOST_TEST_MESSAGE(str1);

    fmt::format_to(std::back_inserter(vec2),"prepend vector {} ",2);
    vec1.insert(vec1.begin(),vec2.begin(),vec2.end());
    BOOST_TEST_MESSAGE(std::string(vec1.data(),vec1.size()));

    auto gte_str=std::string(gte);
    BOOST_TEST_MESSAGE(gte_str);
    BOOST_CHECK_EQUAL(gte_str,std::string(gte.description));
}

#include "testformatter.ipp"

BOOST_AUTO_TEST_SUITE_END()

#endif

#ifdef DRACOSHA_VALIDATOR_FMT
#undef DRACOSHA_VALIDATOR_FMT
#endif

BOOST_AUTO_TEST_SUITE(TestStdFormatter)

#include "testformatter.ipp"

BOOST_AUTO_TEST_SUITE_END()
