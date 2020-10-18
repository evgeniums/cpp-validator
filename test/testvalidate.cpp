#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestValidate)

BOOST_AUTO_TEST_CASE(CheckValidateError)
{
    auto v=validator(gte,100);

    error err;

    validate(1000,v,err);
    BOOST_CHECK(!err);

    validate(50,v,err);
    BOOST_CHECK(err);
}

BOOST_AUTO_TEST_CASE(CheckValidateReport)
{
    auto v=validator(gte,100);

    error_report err;

    validate(1000,v,err);
    BOOST_CHECK(!err);

    validate(50,v,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),"must be greater than or equal to 100");
}

BOOST_AUTO_TEST_CASE(CheckValidateThrow)
{
    auto v=validator(lte,100);

    BOOST_CHECK_THROW(
        validate(1000,v),
        validation_error
    );

    BOOST_CHECK_NO_THROW(
        validate(50,v)
    );

    try
    {
        validate(1000,v);
    }
    catch (const validation_error& err)
    {
        BOOST_CHECK_EQUAL(err.what(),"must be less than or equal to 100");
    }
}

BOOST_AUTO_TEST_CASE(CheckPreValidateError)
{
    auto v=validator(
        _["field1"](gte,100)
    );

    error_report err;

    validate(_["field1"],1000,v,err);
    BOOST_CHECK(!err);

    validate(_["field1"],50,v,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),"field1 must be greater than or equal to 100");
}

BOOST_AUTO_TEST_CASE(CheckPreValidateThrow)
{
    auto v=validator(
        _["field1"](gte,100)
    );

    BOOST_CHECK_NO_THROW(validate(_["field1"],1000,v));

    BOOST_CHECK_THROW(validate(_["field1"],50,v),validation_error);

    try
    {
        validate(_["field1"],50,v);
    }
    catch (const validation_error& err)
    {
        BOOST_CHECK_EQUAL(err.what(),"field1 must be greater than or equal to 100");
    }
}

BOOST_AUTO_TEST_SUITE_END()
