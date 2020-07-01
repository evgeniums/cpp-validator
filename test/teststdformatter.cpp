#if 0
#include <string>
#include <vector>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/config.hpp>

#ifdef DRACOSHA_VALIDATOR_FMT
#undef DRACOSHA_VALIDATOR_FMT
#endif

#include <dracosha/validator/reporting/formatter.hpp>

using namespace dracosha::validator;

BOOST_AUTO_TEST_SUITE(TestStdFormatter)

#include "testformatter.ipp"

BOOST_AUTO_TEST_SUITE_END()
#endif
