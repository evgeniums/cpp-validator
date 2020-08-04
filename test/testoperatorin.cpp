#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/interval.hpp>
#include <dracosha/validator/operators/in.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestOperatorIn)

BOOST_AUTO_TEST_CASE(CheckInInterval)
{
    size_t val=90;
    auto a1=make_default_adapter(val);

    auto v1=validator(in,interval(1,100));
    BOOST_CHECK(v1.apply(a1));

    auto v2=validator(in,interval(95,100));
    BOOST_CHECK(!v2.apply(a1));

    auto v3=validator(in,interval(90,100));
    BOOST_CHECK(v3.apply(a1));

    auto v4=validator(in,interval(90,100,interval.open()));
    BOOST_CHECK(!v4.apply(a1));

    auto v5=validator(in,interval(90,100,interval.open_from()));
    BOOST_CHECK(!v5.apply(a1));

    auto v6=validator(in,interval(90,100,interval.open_to()));
    BOOST_CHECK(v6.apply(a1));

    auto v7=validator(in,interval(80,90,interval.open_to()));
    BOOST_CHECK(!v7.apply(a1));

    auto v8=validator(nin,interval(1,100));
    BOOST_CHECK(!v8.apply(a1));

    auto v9=validator(nin,interval(95,100));
    BOOST_CHECK(v9.apply(a1));

    auto v10=validator(nin,interval(90,100));
    BOOST_CHECK(!v10.apply(a1));

    auto v11=validator(nin,interval(90,100,interval.open()));
    BOOST_CHECK(v11.apply(a1));

    auto v12=validator(nin,interval(90,100,interval.open_from()));
    BOOST_CHECK(v12.apply(a1));

    auto v13=validator(nin,interval(90,100,interval.open_to()));
    BOOST_CHECK(!v13.apply(a1));

    auto v14=validator(nin,interval(80,90,interval.open_to()));
    BOOST_CHECK(v14.apply(a1));
}

BOOST_AUTO_TEST_CASE(CheckInIntervalReport)
{
    std::string rep;
    size_t val=90;
    auto a1=make_reporting_adapter(val,rep);

    auto v1=validator(in,interval(1,100));
    BOOST_CHECK(v1.apply(a1));

    auto v2=validator(in,interval(95,100));
    BOOST_CHECK(!v2.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval [95,100]");
    rep.clear();

    auto v3=validator(in,interval(90,100));
    BOOST_CHECK(v3.apply(a1));

    auto v4=validator(in,interval(90,100,interval.open()));
    BOOST_CHECK(!v4.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval (90,100)");
    rep.clear();

    auto v5=validator(in,interval(90,100,interval.open_from()));
    BOOST_CHECK(!v5.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval (90,100]");
    rep.clear();

    auto v6=validator(in,interval(90,100,interval.open_to()));
    BOOST_CHECK(v6.apply(a1));

    auto v7=validator(in,interval(80,90,interval.open_to()));
    BOOST_CHECK(!v7.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be in interval [80,90)");
    rep.clear();

    auto v8=validator(nin,interval(1,100));
    BOOST_CHECK(!v8.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in interval [1,100]");
    rep.clear();

    auto v9=validator(nin,interval(95,100));
    BOOST_CHECK(v9.apply(a1));

    auto v10=validator(nin,interval(90,100));
    BOOST_CHECK(!v10.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in interval [90,100]");
    rep.clear();

    auto v11=validator(nin,interval(90,100,interval.open()));
    BOOST_CHECK(v11.apply(a1));

    auto v12=validator(nin,interval(90,100,interval.open_from()));
    BOOST_CHECK(v12.apply(a1));

    auto v13=validator(nin,interval(90,100,interval.open_to()));
    BOOST_CHECK(!v13.apply(a1));
    BOOST_CHECK_EQUAL(rep,"must be not in interval [90,100)");
    rep.clear();

    auto v14=validator(nin,interval(80,90,interval.open_to()));
    BOOST_CHECK(v14.apply(a1));
}

BOOST_AUTO_TEST_SUITE_END()
