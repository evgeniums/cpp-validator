#include <boost/test/unit_test.hpp>

#include <dracosha/validator/config.hpp>
#ifdef DRACOSHA_VALIDATOR_FMT
#undef DRACOSHA_VALIDATOR_FMT
#endif

#include <dracosha/validator/reporting/formatter.hpp>
#include <dracosha/validator/reporting/mapped_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

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

BOOST_AUTO_TEST_CASE(CheckStdPropertyFlag)
{
    checkPropertyFlag(make_backend_formatter);
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

BOOST_AUTO_TEST_CASE(CheckFmtFormatterFromTranslator)
{
    checkFormatterFromTranslator(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckFmtFormatterFromTranslatorRepository)
{
    checkFormatterFromTranslatorRepository(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterFromMemberNames)
{
    checkFormatterFromMemberNames(make_backend_formatter);
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterFromMemberNamesAndOperands)
{
    checkFormatterFromMemberNamesAndOperands(make_backend_formatter);
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
