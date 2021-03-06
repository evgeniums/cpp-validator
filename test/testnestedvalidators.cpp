#include <map>
#include <set>
#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/default_adapter.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestNestedValidators)

BOOST_AUTO_TEST_CASE(TestValidatorWithoutMember)
{
    auto v1=validator(
        _["field1"](exists,true)
    );

    auto v2=validator(
        v1
    );

    std::map<std::string,std::set<std::string>> m1{
        {"field1",{"value1"}}
    };
    BOOST_CHECK(v2.apply(m1));
    std::map<std::string,std::set<std::string>> m2{
        {"field2",{"value2"}}
    };
    BOOST_CHECK(!v2.apply(m2));

    auto v3=validator(exists,true);
    auto v4=validator(
        _["field1"](v3)
    );
    BOOST_CHECK(v4.apply(m1));
    BOOST_CHECK(!v4.apply(m2));
}

BOOST_AUTO_TEST_CASE(TestValidatorWithMember)
{
    auto v1=validator(
        _["level2"](exists,true)
    );
    auto v2=validator(
        _["level1"](v1)
    );

    std::map<std::string,std::set<std::string>> m1{
        {"level1",{"level2"}}
    };
    BOOST_CHECK(v2.apply(m1));

    auto v3=validator(
        _["level2"](exists,false)
    );
    auto v4=validator(
        _["level1"](v3)
    );
    BOOST_CHECK(!v4.apply(m1));
}

BOOST_AUTO_TEST_CASE(TestValidatorWithMemberReport)
{
    std::string rep;

    auto v1=validator(
        _["level2"](exists,true)
    );
    auto v2=validator(
        _["level1"](v1)
    );

    std::map<std::string,std::set<std::string>> m1{
        {"level1",{"level2"}}
    };
    BOOST_CHECK(v2.apply(m1));

    auto a1=make_reporting_adapter(m1,rep);
    auto v3=validator(
        _["level2"](exists,false)
    );
    auto v4=validator(
        _["level1"](v3)
    );
    BOOST_CHECK(!v4.apply(a1));
    BOOST_CHECK_EQUAL(rep,std::string("level2 of level1 must not exist"));
    rep.clear();

    //! @todo Fix custom member names in reports with nested validators
#if 0
    auto v5=validator(
        _["level2"]("name2")(exists,false)
    );
    auto v6=validator(
        _["level1"]("name1")(v5)
    );
    BOOST_CHECK(!v6.apply(a1));
    BOOST_CHECK_EQUAL(rep,std::string("level2 of level1 must not exist"));
    rep.clear();
#endif
}

BOOST_AUTO_TEST_SUITE_END()
