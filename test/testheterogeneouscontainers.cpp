#include <boost/test/unit_test.hpp>

#include <dracosha/validator/utils/heterogeneous_size.hpp>
#include <dracosha/validator/heterogeneous_property.hpp>
#include <dracosha/validator/property.hpp>
#include <dracosha/validator/utils/adjust_storable_type.hpp>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/properties/h_size.hpp>
#include <dracosha/validator/utils/foreach_if.hpp>
#include <dracosha/validator/utils/conditional_fold.hpp>
#include <dracosha/validator/validate.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestHeterogeneousContainers)

BOOST_AUTO_TEST_CASE(CheckSize)
{
    auto c1=hana::make_tuple(1,"hello",10.10f);
    static_assert(hana::value(heterogeneous_size(c1))==3,"");

    auto c2=std::make_tuple(1,"hello",10.10f,std::string("hi"));
    static_assert(hana::value(heterogeneous_size(c2))==4,"");

    auto c3=std::string("hello world");
    static_assert(hana::value(heterogeneous_size(c3))==0,"");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(CheckHasProperty)
{
    heterogeneous_property_t<2> p1;

    auto c1=hana::make_tuple(1,"hello",10.10f);
    static_assert(decltype(p1)::has<decltype(c1)>(),"");

    auto c2=hana::make_tuple(1,"hello");
    static_assert(!decltype(p1)::has<decltype(c2)>(),"");

    auto c3=std::string("hello");
    static_assert(!decltype(p1)::has<decltype(c3)>(),"");

    BOOST_CHECK(true);
}

namespace {

DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY(zero,0)
DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY(one,1)
DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY(two,2)
DRACOSHA_VALIDATOR_HETEROGENEOUS_PROPERTY(three,3)
}

BOOST_AUTO_TEST_CASE(CheckGetProperty)
{
    auto c1=hana::make_tuple(1,std::string("hello"),10.10f);
    auto c2=hana::make_tuple(3,std::string("hi"));

    const auto& p1_0=property(c1,zero);
    BOOST_CHECK_EQUAL(p1_0,1);
    BOOST_CHECK_EQUAL(property(c2,zero),3);

    BOOST_CHECK_EQUAL(property(c1,one),std::string("hello"));
    BOOST_CHECK_EQUAL(property(c2,one),std::string("hi"));

    BOOST_CHECK_GT(property(c1,two),10.0f);

    static_assert(is_constant_size<decltype(hana::size_c<1>)>::value,"");
    auto cp1=adjust_storable(hana::size_c<1>);
    BOOST_CHECK_EQUAL(property(c1,cp1),std::string("hello"));
    BOOST_CHECK_EQUAL(property(c2,cp1),std::string("hi"));
}

BOOST_AUTO_TEST_CASE(CheckValidatePlainTuple)
{
    auto check=[](const auto& o1)
    {
        static_assert(hana::is_a<property_tag,decltype(one)>(),"");
        static_assert(has_property<decltype(o1),decltype(one)>(),"");
        static_assert(can_check_contains<decltype(o1),decltype(one)>(),"");
        static_assert(decltype(is_member_path_valid(o1,hana::make_tuple(one)))::value,"");
        BOOST_CHECK(check_exists(o1,hana::make_tuple(one)));

        auto v1=validator(
                _[one](eq,1)
             );
        BOOST_CHECK(v1.apply(o1));
        auto v2=validator(
                _[one](eq,10)
             );
        BOOST_CHECK(!v2.apply(o1));
        auto v3=validator(
                _[one](value(exists,true) ^AND^ value(eq,1))
             );
        BOOST_CHECK(v3.apply(o1));
        auto v4=validator(
                _[one](value(exists,false) ^AND^ value(eq,1))
             );
        BOOST_CHECK(!v4.apply(o1));
        auto v5=validator(
                _[one](value(exists,true) ^AND^ value(eq,100))
             );
        BOOST_CHECK(!v5.apply(o1));
        auto v6=validator(
                _[three](value(exists,true) ^AND^ value(eq,100))
             );
        BOOST_CHECK(!v6.apply(o1));
        auto v7=validator(
                _[hana::size_c<1>](value(exists,true) ^AND^ value(eq,1))
             );
        BOOST_CHECK(v7.apply(o1));
        auto v8=validator(
                _[hana::size_c<1>](value(exists,true) ^AND^ value(eq,10))
             );
        BOOST_CHECK(!v8.apply(o1));
    };

    BOOST_TEST_CONTEXT("hana tuple")
    {
        auto o1=hana::make_tuple("aaa",1,std::string("hello world"));
        check(o1);
    }
    BOOST_TEST_CONTEXT("std tuple")
    {
        auto o1=std::make_tuple("aaa",1,std::string("hello world"));
        check(o1);
    }
}

BOOST_AUTO_TEST_CASE(CheckValidateNestedTuple)
{
    auto check=[](const auto& o1)
    {
        auto v1=validator(
                _[two][one](eq,1)
             );
        BOOST_CHECK(v1.apply(o1));
        auto v2=validator(
                _[two][one](eq,10)
             );
        BOOST_CHECK(!v2.apply(o1));
        auto v3=validator(
                _[two][one](value(exists,true) ^AND^ value(eq,1))
             );
        BOOST_CHECK(v3.apply(o1));
        auto v4=validator(
                _[two][one](value(exists,false) ^AND^ value(eq,1))
             );
        BOOST_CHECK(!v4.apply(o1));
        auto v5=validator(
                _[two][one](value(exists,true) ^AND^ value(eq,100))
             );
        BOOST_CHECK(!v5.apply(o1));
        auto v6=validator(
                _[two][three](value(exists,true) ^AND^ value(eq,100))
             );
        BOOST_CHECK(!v6.apply(o1));
        auto v7=validator(
                _[hana::size_c<2>][hana::size_c<1>](value(exists,true) ^AND^ value(eq,1))
             );
        BOOST_CHECK(v7.apply(o1));
        auto v8=validator(
                _[std::integral_constant<size_t,2>{}][std::integral_constant<size_t,1>{}](value(exists,true) ^AND^ value(eq,10))
             );
        BOOST_CHECK(!v8.apply(o1));
    };

    BOOST_TEST_CONTEXT("hana tuple")
    {
        auto o1=hana::make_tuple("aaa",10,hana::make_tuple("hi",1),std::string("hello world"));
        check(o1);
    }
    BOOST_TEST_CONTEXT("std tuple")
    {
        auto o1=std::make_tuple("aaa",10,hana::make_tuple("hi",1),std::string("hello world"));
        check(o1);
    }
}

BOOST_AUTO_TEST_CASE(CheckValidateNestedTupleReport)
{
    auto check=[](const auto& obj)
    {
        std::string rep;
        auto o1=make_reporting_adapter(obj,rep);

        auto v2=validator(
                _[two][one](eq,10)
             );
        BOOST_CHECK(!v2.apply(o1));
        BOOST_CHECK_EQUAL(rep,std::string("one of two must be equal to 10"));
        rep.clear();

        auto v4=validator(
                _[two][one](value(exists,false) ^AND^ value(eq,1))
             );
        BOOST_CHECK(!v4.apply(o1));
        BOOST_CHECK_EQUAL(rep,std::string("one of two must not exist"));
        rep.clear();

        auto v5=validator(
                _[two][one](value(exists,true) ^AND^ value(eq,100))
             );
        BOOST_CHECK(!v5.apply(o1));
        BOOST_CHECK_EQUAL(rep,std::string("one of two must be equal to 100"));
        rep.clear();

        auto v6=validator(
                _[two][three](value(exists,true) ^AND^ value(eq,100))
             );
        BOOST_CHECK(!v6.apply(o1));
        BOOST_CHECK_EQUAL(rep,std::string("three of two must exist"));
        rep.clear();

        auto v8=validator(
                _[std::integral_constant<size_t,2>{}][std::integral_constant<size_t,1>{}](value(exists,true) ^AND^ value(eq,10))
             );
        BOOST_CHECK(!v8.apply(o1));
        BOOST_CHECK_EQUAL(rep,std::string("element #1 of element #2 must be equal to 10"));
        rep.clear();
    };

    BOOST_TEST_CONTEXT("hana tuple")
    {
        auto o1=hana::make_tuple("aaa",10,hana::make_tuple("hi",1),std::string("hello world"));
        check(o1);
    }
    BOOST_TEST_CONTEXT("std tuple")
    {
        auto o1=std::make_tuple("aaa",10,hana::make_tuple("hi",1),std::string("hello world"));
        check(o1);
    }
}

BOOST_AUTO_TEST_CASE(CheckPropertyNotation)
{
    auto check=[](const auto& o1)
    {
        auto v1=validator(
                _[two](one(eq,1))
             );
        BOOST_CHECK(v1.apply(o1));
        auto v2=validator(
                _[two](one(eq,10))
             );
        BOOST_CHECK(!v2.apply(o1));
    };

    BOOST_TEST_CONTEXT("hana tuple")
    {
        auto o1=hana::make_tuple("aaa",10,hana::make_tuple("hi",1),std::string("hello world"));
        check(o1);
    }
    BOOST_TEST_CONTEXT("std tuple")
    {
        auto o1=std::make_tuple("aaa",10,hana::make_tuple("hi",1),std::string("hello world"));
        check(o1);
    }
}

BOOST_AUTO_TEST_CASE(CheckPropertyNotationReport)
{
    auto check=[](const auto& obj)
    {
        std::string rep;
        auto o1=make_reporting_adapter(obj,rep);

        auto v1=validator(
                _[two](one(eq,1))
             );
        BOOST_CHECK(v1.apply(o1));
        auto v2=validator(
                _[two](one(eq,10))
             );
        BOOST_CHECK(!v2.apply(o1));
        BOOST_CHECK_EQUAL(rep,std::string("one of two must be equal to 10"));
        rep.clear();
    };

    BOOST_TEST_CONTEXT("hana tuple")
    {
        auto o1=hana::make_tuple("aaa",10,hana::make_tuple("hi",1),std::string("hello world"));
        check(o1);
    }
    BOOST_TEST_CONTEXT("std tuple")
    {
        auto o1=std::make_tuple("aaa",10,hana::make_tuple("hi",1),std::string("hello world"));
        check(o1);
    }
}

BOOST_AUTO_TEST_CASE(CheckHSize)
{
    auto check=[](const auto& obj)
    {
        std::string rep;
        auto o1=make_reporting_adapter(obj,rep);

        auto v1=validator(
                _[two][h_size](eq,2)
             );
        BOOST_CHECK(v1.apply(o1));
        auto v2=validator(
                _[two][h_size](eq,3)
             );
        BOOST_CHECK(!v2.apply(o1));
        BOOST_CHECK_EQUAL(rep,std::string("heterogeneous size of two must be equal to 3"));
        rep.clear();
        auto v3=validator(
                _[two](h_size(eq,2))
             );
        BOOST_CHECK(v3.apply(o1));
        auto v4=validator(
                _[two](h_size(eq,3))
             );
        BOOST_CHECK(!v4.apply(o1));
        BOOST_CHECK_EQUAL(rep,std::string("heterogeneous size of two must be equal to 3"));
        rep.clear();
    };

    BOOST_TEST_CONTEXT("hana tuple")
    {
        auto o1=hana::make_tuple("aaa",10,hana::make_tuple("hi",1),std::string("hello world"));
        check(o1);
    }
    BOOST_TEST_CONTEXT("std tuple")
    {
        auto o1=std::make_tuple("aaa",10,hana::make_tuple("hi",1),std::string("hello world"));
        check(o1);
    }

    std::string rep;
    std::string obj("hello");
    auto o1=make_reporting_adapter(obj,rep);

    auto v1=validator(
            _[h_size](eq,0)
         );
    BOOST_CHECK(v1.apply(o1));
    auto v2=validator(
            _[h_size](eq,3)
         );
    BOOST_CHECK(!v2.apply(o1));
    BOOST_CHECK_EQUAL(rep,std::string("heterogeneous size must be equal to 3"));
    rep.clear();
    auto v3=validator(
            h_size(eq,0)
         );
    BOOST_CHECK(v3.apply(o1));
    auto v4=validator(
            h_size(eq,3)
         );
    BOOST_CHECK(!v4.apply(o1));
    BOOST_CHECK_EQUAL(rep,std::string("heterogeneous size must be equal to 3"));
    rep.clear();
}

BOOST_AUTO_TEST_CASE(CheckForeach)
{
    auto fn1=[](auto&& val, auto&&)
    {
       return val.size()>=3;
    };

    auto c1=hana::make_tuple(std::string("one"),std::string("two"),string_view("three"));
    auto res1=foreach_if(c1,predicate_and,fn1);
    BOOST_CHECK(res1);

    auto c2=hana::make_tuple(std::string("hi"),std::string("hey"),string_view(""));
    auto res2=foreach_if(c2,predicate_and,fn1);
    BOOST_CHECK(!res2);
    auto res3=foreach_if(c2,predicate_or,fn1);
    BOOST_CHECK(res3);
}

BOOST_AUTO_TEST_CASE(CheckHeterogeneousAllAny)
{
    auto v1=validator(
                _[ALL](size(gte,3))
            );
    auto v2=validator(
                _[ANY](size(gte,3))
            );
    auto c1=hana::make_tuple(std::string("one"),std::string("two"),string_view("three"));
    BOOST_CHECK(v1.apply(c1));
    BOOST_CHECK(v2.apply(c1));
    auto c2=hana::make_tuple(std::string("hi"),std::string("hey"),string_view(""));
    BOOST_CHECK(!v1.apply(c2));
    BOOST_CHECK(v2.apply(c2));
    auto c3=hana::make_tuple(std::string("hi"),std::string("ho"),string_view(""));
    BOOST_CHECK(!v1.apply(c3));
    BOOST_CHECK(!v2.apply(c3));

    std::string rep;
    auto ra3=make_reporting_adapter(c3,rep);
    BOOST_CHECK(!v1.apply(ra3));
    BOOST_CHECK_EQUAL(rep,std::string("size of each element must be greater than or equal to 3"));
    rep.clear();
    BOOST_CHECK(!v2.apply(ra3));
    BOOST_CHECK_EQUAL(rep,std::string("size of at least one element must be greater than or equal to 3"));
    rep.clear();
}

BOOST_AUTO_TEST_CASE(CheckImplicitFromDoc)
{
    auto v1=validator(
        _[std::integral_constant<size_t,1>{}](gt,100)
    );
    auto v2=validator(
        _[hana::size_c<1>](gt,100)
    );

    auto t1=std::make_tuple(200,50,"hello");
    auto t2=hana::make_tuple(200,50,"hello");

    error_report err;

    validate(t1,v1,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("element #1 must be greater than 100"));

    validate(t2,v2,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("element #1 must be greater than 100"));
}

BOOST_AUTO_TEST_CASE(CheckExplicitFromDoc)
{
    auto v1=validator(
        _[one](gt,100)
    );
    auto v2=validator(
        _[zero](lt,100)
    );

    error_report err;

    auto t1=std::make_tuple(200,50,"hello");
    validate(t1,v1,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("one must be greater than 100"));

    auto t2=hana::make_tuple(200,50,"hello");
    validate(t2,v2,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("zero must be less than 100"));
}

BOOST_AUTO_TEST_SUITE_END()
