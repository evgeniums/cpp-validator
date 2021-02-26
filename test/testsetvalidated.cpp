#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/set_validated.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;
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

namespace validator_ns {

DRACOSHA_VALIDATOR_PROPERTY(GetX);

auto MyClassValidator=validator(
   _[GetX](in,interval(0,500))
);

}
using namespace validator_ns;

namespace {
class MyClass {
  double x;  // Must be in [0;500].

public:

  MyClass(double _x) : x(_x) {
      validate(*this,MyClassValidator);
  }

  double GetX() const
  {
     return x;
  }

  void SetX(double _x) {
    validate(_[validator_ns::GetX],_x,MyClassValidator);
    x = _x;
  }
};
}

BOOST_AUTO_TEST_SUITE(TestSetValidated)

BOOST_AUTO_TEST_CASE(CheckSetValidatedContainer)
{
    auto v=validator(
        _["field1"](gte,100)
    );

    error_report err;

    std::map<std::string,size_t> m;

    set_validated(m,_["field1"],1000,v,err);
    BOOST_CHECK(!err);
    BOOST_REQUIRE(m.find("field1")!=m.end());
    BOOST_CHECK_EQUAL(m["field1"],1000);

    set_validated(m,_["field1"],50,v,err);
    BOOST_CHECK(err);
    BOOST_REQUIRE(m.find("field1")!=m.end());
    BOOST_CHECK_EQUAL(m["field1"],1000);

    BOOST_CHECK_NO_THROW(set_validated(m,_["field1"],300,v));
    BOOST_REQUIRE(m.find("field1")!=m.end());
    BOOST_CHECK_EQUAL(m["field1"],300);

    BOOST_CHECK_THROW(set_validated(m,_["field1"],50,v),validation_error);
    try
    {
        set_validated(m,_["field1"],50,v);
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

    set_validated(val,_[field1],1000,v,err);
    BOOST_CHECK(!err);
    BOOST_CHECK_EQUAL(val.field1,1000);

    set_validated(val,_[field1],50,v,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(val.field1,1000);

    BOOST_CHECK_NO_THROW(set_validated(val,_[field1],300,v));
    BOOST_CHECK_EQUAL(val.field1,300);

    BOOST_CHECK_THROW(set_validated(val,_[field1],50,v),validation_error);
    try
    {
        set_validated(val,_[field1],50,v);
    }
    catch (const validation_error& e)
    {
        BOOST_CHECK_EQUAL(e.what(),"field1 must be greater than or equal to 100");
    }

    BOOST_CHECK_EQUAL(val.field1,300);
}

BOOST_AUTO_TEST_CASE(CheckSetValidatedObject)
{
    BOOST_CHECK_NO_THROW(MyClass obj1{100.0};);
    BOOST_CHECK_THROW(MyClass obj2{1000.0};,validation_error);

    MyClass obj3{100.0};
    BOOST_CHECK_NO_THROW(obj3.SetX(200.0););

    BOOST_CHECK_THROW(obj3.SetX(1000.0);,validation_error);
}

BOOST_AUTO_TEST_SUITE_END()
