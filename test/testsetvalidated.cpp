#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/set_validated.hpp>

DRACOSHA_VALIDATOR_PROPERTY(field1)

namespace {
struct TestSetValidatorStruct
{
    size_t field1=0;
};
}

DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

template <>
struct set_member_t<TestSetValidatorStruct,DRACOSHA_VALIDATOR_PROPERTY_TYPE(field1)>
{
    template <typename ObjectT, typename MemberT, typename ValueT>
    void operator() (
            ObjectT& obj,
            MemberT&&,
            ValueT&& val
        ) const
    {
        obj.field1=val;
    }
};

DRACOSHA_VALIDATOR_NAMESPACE_END

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestSetValidated)

BOOST_AUTO_TEST_CASE(CheckSetValidatedContainer)
{
    auto v=validator(
        _["field1"](gte,100)
    );

    error_report err;

    std::map<std::string,size_t> m;

    set_validated(m,"field1",1000,v,err);
    BOOST_CHECK(!err);
    BOOST_REQUIRE(m.find("field1")!=m.end());
    BOOST_CHECK_EQUAL(m["field1"],1000);

    set_validated(m,"field1",50,v,err);
    BOOST_CHECK(err);
    BOOST_REQUIRE(m.find("field1")!=m.end());
    BOOST_CHECK_EQUAL(m["field1"],1000);

    BOOST_CHECK_NO_THROW(set_validated(m,"field1",300,v));
    BOOST_REQUIRE(m.find("field1")!=m.end());
    BOOST_CHECK_EQUAL(m["field1"],300);

    BOOST_CHECK_THROW(set_validated(m,"field1",50,v),validation_error);
    try
    {
        set_validated(m,"field1",50,v);
    }
    catch (const validation_error& e)
    {
        BOOST_CHECK_EQUAL(e.what(),"field1 must be greater than or equal to 100");
    }

    BOOST_REQUIRE(m.find("field1")!=m.end());
    BOOST_CHECK_EQUAL(m["field1"],300);
}

BOOST_AUTO_TEST_CASE(CheckSetValidatedProperty)
{
    auto v=validator(
        _[field1](gte,100)
    );

    error_report err;
    TestSetValidatorStruct val;

    set_validated(val,field1,1000,v,err);
    BOOST_CHECK(!err);
    BOOST_CHECK_EQUAL(val.field1,1000);

    set_validated(val,field1,50,v,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(val.field1,1000);

    BOOST_CHECK_NO_THROW(set_validated(val,field1,300,v));
    BOOST_CHECK_EQUAL(val.field1,300);

    BOOST_CHECK_THROW(set_validated(val,field1,50,v),validation_error);
    try
    {
        set_validated(val,field1,50,v);
    }
    catch (const validation_error& e)
    {
        BOOST_CHECK_EQUAL(e.what(),"field1 must be greater than or equal to 100");
    }

    BOOST_CHECK_EQUAL(val.field1,300);
}

BOOST_AUTO_TEST_SUITE_END()
