#include <boost/test/unit_test.hpp>

#include <dracosha/validator/config.hpp>
#ifdef DRACOSHA_VALIDATOR_FMT
#undef DRACOSHA_VALIDATOR_FMT
#endif

#include <dracosha/validator/reporting/formatter.hpp>
#include <dracosha/validator/reporting/mapped_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>

using namespace dracosha::validator;

BOOST_AUTO_TEST_SUITE(TestStdFormatter)

#define DRACOSHA_VALIDATOR_TEST_FORMATTER
#include "testformatter.cpp"

namespace
{
auto make_backend_formatter(std::string& dst)
{
    return detail::std_backend_formatter{dst};
}
}

BOOST_AUTO_TEST_CASE(CheckStdOrderAndPresentation)
{
    checkOrderAndPresentation(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckStdDefaultFormatter)
{
    checkDefaultFormatter(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterFromStrings)
{
    checkFormatterFromStrings(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterFromMemberNames)
{
    checkFormatterFromMemberNames(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterFromMemberNamesAndValues)
{
    checkFormatterFromMemberNamesAndValues(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterWithRefs)
{
    checkFormatterWithRefs(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterWithRvals)
{
    checkFormatterWithRvals(make_backend_formatter);
}

BOOST_AUTO_TEST_SUITE_END()
