#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/operator.hpp>

using namespace dracosha::validator;

BOOST_AUTO_TEST_SUITE(TestOperators)

BOOST_AUTO_TEST_CASE(CheckGteOperator)
{
    BOOST_CHECK(gte(10,5));
    BOOST_CHECK(gte(10,10));
    BOOST_CHECK(!gte(5,10));

    BOOST_CHECK(validate(10,gte,5));
    BOOST_CHECK(validate(10,gte,10));
    BOOST_CHECK(!validate(5,gte,10));

    BOOST_CHECK(validate(invokable([](){return 10;}),gte,invokable([](){return 5;})));
    BOOST_CHECK(validate(invokable([](){return 10;}),gte,invokable([](){return 10;})));
    BOOST_CHECK(!validate(invokable([](){return 5;}),gte,invokable([](){return 10;})));
}

struct Aaa
{
    size_t size=10;
};

BOOST_AUTO_TEST_CASE(CheckProperty)
{
    std::vector<char> v(5);
    BOOST_CHECK_EQUAL(property(v,size),v.size());
    BOOST_CHECK(!property(v,empty));
    BOOST_CHECK(validate(size,v,gte,3));

    Aaa a;
    BOOST_CHECK_EQUAL(property(a,size),10);
    BOOST_CHECK(!validate(size,a,gte,100));

    auto check=prepare_validate(size,gte,9);
    BOOST_CHECK(check(a));

    auto sz=size(gte,7);
    BOOST_CHECK(!sz(v));
    BOOST_CHECK(sz(a));

    auto val=value(gte,3);
    BOOST_CHECK(val(10));
    BOOST_CHECK(val(3));
    BOOST_CHECK(!val(1));
}

BOOST_AUTO_TEST_CASE(CheckGet)
{
    std::string str("hello");
    BOOST_CHECK_EQUAL(get(str,size),str.size());
    BOOST_CHECK_EQUAL(get(str,4),str.at(4));
}

BOOST_AUTO_TEST_CASE(CheckSingleValidatorCtor)
{
    BOOST_CHECK_EQUAL(_["hello"].key(),std::string("hello"));
    BOOST_CHECK_EQUAL(_[100].key(),100);
    std::string h="hello";
    BOOST_CHECK_EQUAL(_[h].key(),std::string("hello"));
}

BOOST_AUTO_TEST_CASE(CheckSingleValidatorValueOp)
{
    std::vector<int> vec1={1,2,3,4,5};
    std::vector<int> vec2={10,20,30};

    auto v1=_[2](gte,30);
    BOOST_CHECK(!v1.apply(vec1));
    BOOST_CHECK(v1.apply(vec2));

    auto v2=_[size](gte,5);
    BOOST_CHECK(v2.apply(vec1));
    BOOST_CHECK(!v2.apply(vec2));
}

BOOST_AUTO_TEST_CASE(CheckAggregate)
{
    auto v1=value(gte,"abcd");
    auto v2=_[size](gte,5);

    std::string str1("abcdef");
    BOOST_CHECK(apply(str1,v2));
    BOOST_CHECK(apply(str1,v1));

    auto and_tuple=hana::make_tuple(v1,v2);
    BOOST_CHECK(aggregate_and(str1,and_tuple));
    BOOST_CHECK(aggregate_or(str1,and_tuple));

    std::string str2("00000000");
    BOOST_CHECK(!aggregate_and(str2,and_tuple));
    BOOST_CHECK(aggregate_or(str2,and_tuple));

    std::string str3("000");
    BOOST_CHECK(!aggregate_and(str3,and_tuple));
    BOOST_CHECK(!aggregate_or(str3,and_tuple));
}

BOOST_AUTO_TEST_CASE(CheckLogicalAndOr)
{
    auto v1=value(gte,"abcd");
    auto v2=_[size](gte,5);

    std::string str1("abcdef");
    std::string str2("00000000");
    std::string str3("000");

    auto and1=AND(v1,v2);
    BOOST_CHECK(and1.apply(str1));
    BOOST_CHECK(!and1.apply(str2));
    BOOST_CHECK(!and1.apply(str3));

    auto and2=value(gte,"abcd") ^AND^ _[size](gte,5);
    BOOST_CHECK(and2.apply(str1));
    BOOST_CHECK(!and2.apply(str2));
    BOOST_CHECK(!and2.apply(str3));

    auto or1=OR(v1,v2);
    BOOST_CHECK(or1.apply(str1));
    BOOST_CHECK(or1.apply(str2));
    BOOST_CHECK(!or1.apply(str3));

    auto or2=value(gte,"abcd") ^OR^ _[size](gte,5);
    BOOST_CHECK(or2.apply(str1));
    BOOST_CHECK(or2.apply(str2));
    BOOST_CHECK(!or2.apply(str3));
}

BOOST_AUTO_TEST_CASE(CheckMapValidator)
{
    std::map<std::string,std::string> m;
    m["one"]="one_value";
    m["two"]="two_value";
    m["three"]="three_value";
    m["four"]="four_value";
    m["five"]="five_value";

    auto v01=_["one"](gte,"on");
    auto v02=_["four"](gte,"fo");
    auto v0=AND(v01,v02);
    BOOST_CHECK(v0.apply(m));

    auto v1=validator(
            _[size](gte,5),
            _["one"](gte,"one_v"),
            _["two"](gte,"t"),
            _["four"](value(gte,"four_val")),
            _["five"](value(gte,"five") ^AND^ size(gte,9))
            );

    BOOST_CHECK(v1.apply(m));

    std::map<std::string,std::string> m1;
    m1["one"]="one_value";
    m1["two"]="two_value";
    BOOST_CHECK(!v1.apply(m1));
}

BOOST_AUTO_TEST_CASE(CheckNestedValidator)
{
    auto v0=_["first_map"]["one_2"];
    BOOST_CHECK_EQUAL(v0.format_key_chain(),std::string("first_map.one_2"));

    std::map<std::string,std::map<std::string,std::string>> m;
    m["first_map"]={std::make_pair("one","one_value"),std::make_pair("two","two_value"),std::make_pair("three","three_value")};
    m["second_map"]={std::make_pair("one_2","one_value_2"),std::make_pair("two_2","two_value_2"),std::make_pair("three_2","three_value_2")};

    auto v1=validator(
            _[size](gte,2),
            _["first_map"](size(gte,3)),
            _["first_map"]["one"](size(gte,9)),
            _["second_map"]["two_2"](gte,"two_value_2")
            );
    BOOST_CHECK(v1.apply(m));

    auto v2=validator(
            _[size](gte,2),
            _["first_map"](size(gte,3)),
            _["first_map"]["one"](size(gte,9)),
            _["second_map"]["one_2"](gte,"one_value_200")
            );
    BOOST_CHECK(!v2.apply(m));
}

BOOST_AUTO_TEST_SUITE_END()
