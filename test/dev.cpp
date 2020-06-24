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

BOOST_AUTO_TEST_SUITE_END()
