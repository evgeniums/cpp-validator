#include <iostream>
#include <boost/test/unit_test.hpp>

#include <dracosha/validator/config.hpp>
#include <dracosha/validator/reporting/formatter.hpp>
#include <dracosha/validator/reporting/mapped_translator.hpp>
#include <dracosha/validator/reporting/translator_repository.hpp>

#ifndef DRACOSHA_VALIDATOR_FMT

using namespace dracosha::validator;

BOOST_AUTO_TEST_SUITE(TestStdFormatter)

#define DRACOSHA_VALIDATOR_TEST_FORMATTER
#include "testformatter.cpp"

BOOST_AUTO_TEST_CASE(CheckFmtOrderAndPresentation)
{
    checkOrderAndPresentation();
}

BOOST_AUTO_TEST_CASE(CheckFmtDefaultFormatter)
{
    checkDefaultFormatter();
}

BOOST_AUTO_TEST_CASE(CheckFormatterFromStrings)
{
    checkFormatterFromStrings();
}

BOOST_AUTO_TEST_CASE(CheckFormatterFromMemberNames)
{
    checkFormatterFromMemberNames();
}

BOOST_AUTO_TEST_CASE(CheckFormatterFromMemberNamesAndValues)
{
    checkFormatterFromMemberNamesAndValues();
}

BOOST_AUTO_TEST_CASE(CheckFormatterWithRefs)
{
    checkFormatterWithRefs();
}

BOOST_AUTO_TEST_CASE(CheckStdFormatterWithRvals)
{
    checkFormatterWithRvals();
}

BOOST_AUTO_TEST_SUITE_END()

#endif
