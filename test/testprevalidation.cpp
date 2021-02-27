#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

#include <dracosha/validator/prevalidation/prevalidation_adapter.hpp>
#include <dracosha/validator/prevalidation/set_validated.hpp>

namespace hana=boost::hana;
#if 1
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

namespace vld=DRACOSHA_VALIDATOR_NAMESPACE;

DRACOSHA_VALIDATOR_PROPERTY(GetX);

auto MyClassValidator=vld::validator(
   vld::_[GetX](vld::in,vld::interval(0,500))
);

}
using namespace validator_ns;

namespace {
class MyClass {
  double x;  // Must be in [0;500].

public:

  MyClass(double _x) : x(_x) {
      vld::validate(*this,MyClassValidator);
  }

  double GetX() const
  {
     return x;
  }

  void SetX(double _x) {
    vld::validate(vld::_[validator_ns::GetX],_x,MyClassValidator);
    x = _x;
  }
};
}
#endif
namespace
{

DRACOSHA_VALIDATOR_PROPERTY(val1)
DRACOSHA_VALIDATOR_PROPERTY(val2)

struct NonCopyable
{
    NonCopyable()=default;
    ~NonCopyable()=default;
    NonCopyable(const NonCopyable&)=delete;
    NonCopyable(NonCopyable&&)=default;
    NonCopyable& operator= (const NonCopyable&)=delete;
    NonCopyable& operator= (NonCopyable&&)=default;

    std::string val1;
    int val2=0;
};
}

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestPrevalidation)
#if 1
BOOST_AUTO_TEST_CASE(CheckPrevalidationReport)
{
    std::string rep1;

    auto v1=validator(
                _[size](ne,100),
                _["field1"](gte,"1"),
                _["field10"](size(lt,3)),
                _[10](gte,1000)
            );
    auto sa1=make_prevalidation_adapter(_["field1"],"value10",rep1);
    auto&& val1=sa1.traits().get();
    BOOST_CHECK_EQUAL(val1,"value10");
    BOOST_CHECK(v1.apply(sa1));
    rep1.clear();

    auto sa2=make_prevalidation_adapter(_["field10"],std::string("value10"),rep1);
    BOOST_CHECK(!v1.apply(sa2));
    BOOST_CHECK_EQUAL(rep1,"size of field10 must be less than 3");
    rep1.clear();

    auto sa3=make_prevalidation_adapter(_["field1"],"0",rep1);
    BOOST_CHECK(!v1.apply(sa3));
    BOOST_CHECK_EQUAL(rep1,"field1 must be greater than or equal to 1");
    rep1.clear();

    std::string str4="0";
    auto sa4=make_prevalidation_adapter(_["field100"],str4,rep1);
    BOOST_CHECK(v1.apply(sa4));
    rep1.clear();

    auto sa5=make_prevalidation_adapter(_["field200"],100,rep1);
    BOOST_CHECK(v1.apply(sa5));
    rep1.clear();

    auto sa6=make_prevalidation_adapter(_[100],NonCopyable(),rep1);
    BOOST_CHECK(v1.apply(sa6));
    rep1.clear();

    NonCopyable nc;
    auto sa7=make_prevalidation_adapter(_["field300"],nc,rep1);
    BOOST_CHECK(v1.apply(sa7));
    rep1.clear();

    auto sa8=make_prevalidation_adapter(_[10],5000,rep1);
    BOOST_CHECK(v1.apply(sa8));
    rep1.clear();

    auto sa9=make_prevalidation_adapter(_[10],100,rep1);
    BOOST_CHECK(!v1.apply(sa9));
    BOOST_CHECK_EQUAL(rep1,"element #10 must be greater than or equal to 1000");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckAggregationPrevalidationReport)
{
    auto v1=validator(
                _[size](eq,1),
                _["field1"](gte,"value1"),
                _["field10"](value(gte,"10") ^AND^ size(lt,3)),
                _["field100"](value(gt,"how are you") ^OR^ size(lte,5))
            );
    std::string rep1;

    auto sa1=make_prevalidation_adapter(_["field10"],"hi",rep1);
    BOOST_CHECK(v1.apply(sa1));
    rep1.clear();

    auto sa2=make_prevalidation_adapter(_["field10"],"01",rep1);
    BOOST_CHECK(!v1.apply(sa2));
    BOOST_CHECK_EQUAL(rep1,"field10 must be greater than or equal to 10");
    rep1.clear();

    auto sa3=make_prevalidation_adapter(_["field10"],"1000",rep1);
    BOOST_CHECK(!v1.apply(sa3));
    BOOST_CHECK_EQUAL(rep1,"size of field10 must be less than 3");
    rep1.clear();

    auto sa4=make_prevalidation_adapter(_[10],5000,rep1);
    BOOST_CHECK(v1.apply(sa4));
    rep1.clear();

    auto sa5=make_prevalidation_adapter(_["field100"],"zzzzzzzzz",rep1);
    BOOST_CHECK(v1.apply(sa5));
    rep1.clear();

    auto sa6=make_prevalidation_adapter(_["field100"],"0123",rep1);
    BOOST_CHECK(v1.apply(sa6));
    rep1.clear();

    auto sa7=make_prevalidation_adapter(_["field100"],"01234567890",rep1);
    BOOST_CHECK(!v1.apply(sa7));
    BOOST_CHECK_EQUAL(rep1,"field100 must be greater than how are you OR size of field100 must be less than or equal to 5");
    rep1.clear();

    auto v2=validator(
                _["field10"](gte,"value1")
                ^OR^
                _["field10"](eq,"10")
            );

    auto sa8=make_prevalidation_adapter(_["field10"],"zzzzzzz",rep1);
    BOOST_CHECK(v2.apply(sa8));
    rep1.clear();

    auto sa9=make_prevalidation_adapter(_["field10"],"10",rep1);
    BOOST_CHECK(v2.apply(sa9));
    rep1.clear();

    auto sa10=make_prevalidation_adapter(_["field10"],"100",rep1);
    BOOST_CHECK(!v2.apply(sa10));
    BOOST_CHECK_EQUAL(rep1,"field10 must be greater than or equal to value1 OR field10 must be equal to 10");
    rep1.clear();

    auto v3=validator(
                _["field10"](NOT(value(gte,"value1"))),
                _["field100"](value(gt,"how are you") ^OR^ size(lte,5))
            );
    auto sa11=make_prevalidation_adapter(_["field10"],"10",rep1);
    BOOST_CHECK(v3.apply(sa11));
    rep1.clear();
    auto sa12=make_prevalidation_adapter(_["field10"],"value100",rep1);
    BOOST_CHECK(!v3.apply(sa12));
    BOOST_CHECK_EQUAL(rep1,"NOT field10 must be greater than or equal to value1");
    rep1.clear();

    auto v4=validator(
                NOT(_["field10"](gte,"value1"))
            );
    BOOST_CHECK(v4.apply(sa11));
    rep1.clear();
    BOOST_CHECK(!v4.apply(sa12));
    BOOST_CHECK_EQUAL(rep1,"NOT field10 must be greater than or equal to value1");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckNestedPrevalidationReport)
{
    std::vector<float> vec1;
    auto v1=validator(
                _[size](eq,1),
                _["field1"][vec1](gte,"value1"),
                _["field10"]["field20"](value(gte,"10") ^AND^ size(lt,3))
            );
    std::string rep1;

    auto sa1=make_prevalidation_adapter(_["field1000"][100],"hi",rep1);
    BOOST_CHECK(v1.apply(sa1));
    rep1.clear();

    auto sa2=make_prevalidation_adapter(_["field10"][100],"hello",rep1);
    BOOST_CHECK(v1.apply(sa2));
    rep1.clear();

    auto sa3=make_prevalidation_adapter(_["field10"]["field200"],"hello",rep1);
    BOOST_CHECK(v1.apply(sa3));
    rep1.clear();

    auto sa4=make_prevalidation_adapter(_["field10"]["field20"],"99",rep1);
    BOOST_CHECK(v1.apply(sa4));
    rep1.clear();

    auto sa5=make_prevalidation_adapter(_["field10"]["field20"],"0",rep1);
    BOOST_CHECK(!v1.apply(sa5));
    BOOST_CHECK_EQUAL(rep1,"field20 of field10 must be greater than or equal to 10");
    rep1.clear();

    auto sa6=make_prevalidation_adapter(_["field10"]["field20"],"999",rep1);
    BOOST_CHECK(!v1.apply(sa6));
    BOOST_CHECK_EQUAL(rep1,"size of field20 of field10 must be less than 3");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckSampleObjectPrevalidationReport)
{
    std::map<size_t,size_t> sample={
        {1,10},
        {2,20},
        {3,30}
    };

    auto v1=validator(
                _[size](eq,1),
                _[1](gte,_(sample)),
                _[10](eq,_(sample))
            );
    std::string rep1;

    auto sa1=make_prevalidation_adapter(_[1],100,rep1);
    BOOST_CHECK(v1.apply(sa1));
    rep1.clear();

    auto sa2=make_prevalidation_adapter(_[20],1000,rep1);
    BOOST_CHECK(v1.apply(sa1));
    rep1.clear();

    auto sa3=make_prevalidation_adapter(_[1],5,rep1);
    BOOST_CHECK(!v1.apply(sa3));
    BOOST_CHECK_EQUAL(rep1,"element #1 must be greater than or equal to element #1 of sample");
    rep1.clear();

    auto v2=validator(
                _[size](eq,1),
                _[1](gte,_(_(sample),"sample object")),
                _[10](eq,_(_(sample),"sample object"))
            );
    BOOST_CHECK(!v2.apply(sa3));
    BOOST_CHECK_EQUAL(rep1,"element #1 must be greater than or equal to element #1 of sample object");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckPropertyPrevalidation)
{
    auto v=validator(
        _[val1](eq,"value"),
        _[val2](lt,100)
    );

    NonCopyable obj;
    std::string rep;
    auto sa1=make_prevalidation_adapter(_[val1],"hello",rep);
    BOOST_CHECK(!v.apply(sa1));
    rep.clear();

    auto sa2=make_prevalidation_adapter(_[val1],"value",rep);
    BOOST_CHECK(v.apply(sa2));
    rep.clear();
}


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
#endif
BOOST_AUTO_TEST_CASE(CheckSingleMemberAnyAllReport)
{
#if 1
    std::string rep1;

    auto v0=validator(
                _['a'](ALL(value(gte,9)))
            );
    auto pa0=make_prevalidation_adapter(_['a'],10,rep1);
    BOOST_CHECK(v0.apply(pa0));
    rep1.clear();

    auto v1=validator(
                _["field1"](ALL(value(gte,9))),
                _["field2"](size(gte,100))
            );
    auto pa1=make_prevalidation_adapter(_["field1"],10,rep1);
    BOOST_CHECK(v1.apply(pa1));
    rep1.clear();

    auto v2=validator(
                _["field1"]["field1_1"](ALL(value(gte,9))),
                _["field2"](size(gte,100))
            );
    auto pa2=make_prevalidation_adapter(_["field1"]["field1_1"],10,rep1);
    BOOST_CHECK(v2.apply(pa2));
    rep1.clear();

    auto pa3=make_prevalidation_adapter(_["field1"]["field1_1"],5,rep1);
    BOOST_CHECK(!v2.apply(pa3));
    BOOST_CHECK_EQUAL(rep1,std::string("field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    static_assert(is_container_t<std::vector<int>>::value,"");

    auto pa4=make_prevalidation_adapter(_["field1"]["field1_1"],std::vector<int>{10,11,12},rep1);
    v2.apply(pa4);
    BOOST_CHECK(v2.apply(pa4));
    rep1.clear();

    auto pa5=make_prevalidation_adapter(_["field1"]["field1_1"],std::vector<int>{1},rep1);

    BOOST_TEST_CONTEXT("check adapter_with_aggregation_iterator"){
        static_assert(hana::is_a<prevalidation_adapter_tag,typename decltype(pa5)::type>,"");
        std::vector<int> vec1{1};
        auto a1=adapter_with_aggregation_iterator(
            pa5,
            vec1.begin()
        );
        BOOST_CHECK(a1.traits().next_adapter_impl().is_member_checked());
    }

    BOOST_CHECK(!v2.apply(pa5));
    BOOST_CHECK_EQUAL(rep1,std::string("each element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    auto pa6=make_prevalidation_adapter(_["field1"]["field1_1"],std::vector<int>{10,11,12,1},rep1);
    BOOST_CHECK(!v2.apply(pa6));
    BOOST_CHECK_EQUAL(rep1,std::string("each element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    auto v3=validator(
                _["field1"]["field1_1"](ANY(value(gte,9))),
                _["field2"](size(gte,100))
            );

    BOOST_CHECK(v3.apply(pa2));
    rep1.clear();
    BOOST_CHECK(v3.apply(pa3));
    rep1.clear();
    BOOST_CHECK(v3.apply(pa6));
    rep1.clear();
    BOOST_CHECK(v3.apply(pa5));
    rep1.clear();

    //"check that there is no extra copy in strict_any()"
    {
        std::ignore=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(NonCopyable{}),rep1);
        NonCopyable nc;
        std::ignore=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(nc),rep1);
    }

    // check strict ANY

    auto vec7=std::vector<int>{1,2,3,4};
    auto pa7=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(vec7),rep1);
    BOOST_CHECK(!v3.apply(pa7));
    BOOST_CHECK_EQUAL(rep1,std::string("at least one element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    int val8=8;
    auto pa8=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(val8),rep1);
    BOOST_CHECK(!v3.apply(pa8));
    BOOST_CHECK_EQUAL(rep1,std::string("field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    auto vec9=std::vector<int>{1,2,3,4,9};
    auto pa9=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(vec9),rep1);
    BOOST_CHECK(v3.apply(pa9));
    rep1.clear();

    int val10=10;
    auto pa10=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(val10),rep1);
    BOOST_CHECK(v3.apply(pa10));
    rep1.clear();

    auto pa11=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(11),rep1);
    BOOST_CHECK(v3.apply(pa11));
    rep1.clear();

    auto pa11_1=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(7),rep1);
    BOOST_CHECK(!v3.apply(pa11_1));
    BOOST_CHECK_EQUAL(rep1,std::string("field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    auto pa12=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(std::vector<int>{1,2,3,4}),rep1);
    BOOST_CHECK(!v3.apply(pa12));
    BOOST_CHECK_EQUAL(rep1,std::string("at least one element of field1_1 of field1 must be greater than or equal to 9"));
    rep1.clear();

    // check literals
    auto v4=validator(
                _["field1"]["field1_1"](ANY(value(gte,"Hello"))),
                _["field2"](size(gte,100))
            );

    auto pa13=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any("Hello"),rep1);
    auto pa14=make_prevalidation_adapter(_["field1"]["field1_1"],"Hello",rep1);
    auto pa14_1=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any("Aaa"),rep1);

    BOOST_CHECK(v4.apply(pa13));
    rep1.clear();
    BOOST_CHECK(v4.apply(pa14));
    rep1.clear();
    BOOST_CHECK(!v4.apply(pa14_1));
    BOOST_CHECK_EQUAL(std::string("field1_1 of field1 must be greater than or equal to Hello"),rep1);
    rep1.clear();

    auto v5=validator(
                _["field1"]["field1_1"](ALL(value(gte,"Hello"))),
                _["field2"](size(gte,100))
            );
    auto pa15=make_prevalidation_adapter(_["field1"]["field1_1"],"Zzz",rep1);
    BOOST_CHECK(v5.apply(pa15));
    rep1.clear();
    auto pa16=make_prevalidation_adapter(_["field1"]["field1_1"],"Aaa",rep1);
    BOOST_CHECK(!v5.apply(pa16));
    BOOST_CHECK_EQUAL(std::string("field1_1 of field1 must be greater than or equal to Hello"),rep1);
    rep1.clear();

#else
    std::string rep1;
    NonCopyable nc;
    std::ignore=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(nc),rep1);

    std::ignore=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(1),rep1);
    std::ignore=make_prevalidation_adapter(_["field1"]["field1_1"],strict_any(NonCopyable{}),rep1);

#endif
}

BOOST_AUTO_TEST_SUITE_END()
