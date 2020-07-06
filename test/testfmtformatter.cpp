#include <string>
#include <vector>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/formatter.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/reporting/mapped_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>

#ifdef DRACOSHA_VALIDATOR_FMT

#include <fmt/format.h>

using namespace dracosha::validator;

BOOST_AUTO_TEST_SUITE(TestFmtFormatter)

BOOST_AUTO_TEST_CASE(CheckFmtString)
{
    std::string str1, str2;
    std::vector<char> vec1, vec2;

    fmt::format_to(std::back_inserter(str1),"Hello {}",1);
    fmt::format_to(std::back_inserter(vec1),"Hello vector {}",1);

    fmt::format_to(std::back_inserter(str2),"prepend {} ",2);
    str1.insert(str1.begin(),str2.begin(),str2.end());
    BOOST_CHECK_EQUAL(str1,std::string("prepend 2 Hello 1"));

    fmt::format_to(std::back_inserter(vec2),"prepend vector {} ",2);
    vec1.insert(vec1.begin(),vec2.begin(),vec2.end());
    BOOST_CHECK_EQUAL(std::string(vec1.data(),vec1.size()),std::string("prepend vector 2 Hello vector 1"));

    auto gte_str=std::string(gte);
    BOOST_CHECK_EQUAL(gte_str,std::string(gte.description));

    detail::fmt_formatter_t formatter{};

    std::string str3;
    formatter.append(str3,"one",",","two",",","three");
    BOOST_CHECK_EQUAL(str3,std::string("one,two,three"));

    std::string str4;
//    detail::fmt_append_join(str4,std::string(" , "),hana::make_tuple("one","two","three"));
//    auto t=std::make_tuple("one","two","three");
//    fmt::join(t,std::string(","));
//    std::tuple<int, char> t = {1, 'a'};
//     Prints "1, a"
//    fmt::print("{}", fmt::join(t, ", "));
//    BOOST_TEST_MESSAGE(str4);
}

#define DRACOSHA_VALIDATOR_TEST_FORMATTER
#include "testformatter.cpp"

BOOST_AUTO_TEST_CASE(CheckStdOrderAndPresentation)
{
    checkOrderAndPresentation();
}

BOOST_AUTO_TEST_CASE(CheckStdDefaultFormatter)
{
    checkDefaultFormatter();
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterFromStrings)
{
    checkFormatterFromStrings();
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterFromMemberNames)
{
    checkFormatterFromMemberNames();
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterFromMemberNamesAndValues)
{
    checkFormatterFromMemberNamesAndValues();
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterWithRefs)
{
    checkFormatterWithRefs();
}

BOOST_AUTO_TEST_SUITE_END()

#endif
