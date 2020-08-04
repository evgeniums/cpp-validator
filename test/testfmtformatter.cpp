#include <string>
#include <vector>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/formatter.hpp>
#include <dracosha/validator/properties.hpp>
#include <dracosha/validator/reporting/mapped_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>
#include <dracosha/validator/utils/hana_to_std_tuple.hpp>

#ifdef DRACOSHA_VALIDATOR_FMT

#include <fmt/format.h>

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

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

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

    std::string str3;
    detail::fmt_backend_formatter<std::string> formatter{str3};
    formatter.append("one",",","two",",","three");
    BOOST_CHECK_EQUAL(str3,std::string("one,two,three"));

    auto mt= [] (auto&&... args)
    {
        return std::make_tuple(std::forward<decltype(args)>(args)...);
    };

    auto t1=hana::make_tuple(1,2,3);
    auto t2=hana::unpack(t1,mt);
    static_assert(std::is_same<decltype(t2),std::tuple<int,int,int>>::value,"");

    auto t3=hana_to_std_tuple(hana::make_tuple(NonCopyable(),10,std::string("hi")));
    static_assert(std::is_same<decltype(t3),std::tuple<NonCopyable,int,std::string>>::value,"");

    std::string str4;
    auto t4=hana::make_tuple("hello","world",2020);
    fmt::format_to(std::back_inserter(str4),"{}",fmt::join(hana_to_std_tuple(t4)," "));
    BOOST_CHECK_EQUAL(str4,std::string("hello world 2020"));

    std::string str5;
    auto t5=hana::make_tuple("field1","field2","field3");
    detail::fmt_append_join(str5," of ",t5);
    BOOST_CHECK_EQUAL(str5,std::string("field1 of field2 of field3"));
}

#define DRACOSHA_VALIDATOR_TEST_FORMATTER
#include "testformatter.cpp"

namespace
{
auto make_backend_formatter(std::string& dst)
{
    return detail::fmt_backend_formatter<std::string>{dst};
}
}

BOOST_AUTO_TEST_CASE(CheckFmtPropertyFlag)
{
    checkPropertyFlag(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckFmtOrderAndPresentation)
{
    checkOrderAndPresentation(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckFmtDefaultFormatter)
{
    checkDefaultFormatter(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckFmtFormatterFromStrings)
{
    checkFormatterFromStrings(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckFmtFormatterFromTranslator)
{
    checkFormatterFromTranslator(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckFmtFormatterFromTranslatorRepository)
{
    checkFormatterFromTranslatorRepository(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckFmtFormatterFromMemberNames)
{
    checkFormatterFromMemberNames(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckFmtFormatterFromMemberNamesAndOperands)
{
    checkFormatterFromMemberNamesAndOperands(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckFmtFormatterWithRefs)
{
    checkFormatterWithRefs(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckFmtFormatterWithRvals)
{
    checkFormatterWithRvals(make_backend_formatter);
}

BOOST_AUTO_TEST_SUITE_END()

#endif
