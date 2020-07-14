#if 0
#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/single_member_adapter.hpp>

using namespace dracosha::validator;

BOOST_AUTO_TEST_SUITE(TestSingleMember)

BOOST_AUTO_TEST_CASE(CheckSingleMemberReport)
{
    std::map<std::string,std::string> m1;
    auto p1=hana::make_tuple(size);
    auto p2=hana::make_tuple("field10");
    auto p3=hana::make_tuple("field1");
    BOOST_CHECK(check_member_path(m1,p1));
    BOOST_CHECK(check_member_path(m1,p2));
    BOOST_CHECK(check_member_path(m1,p3));

    auto v1=validator(
                _[size](ne,100),
                _["field10"](value(gte,"0") ^AND^ size(lt,3)),
                _["field1"](gte,"value1")
            );

    std::map<std::string,std::string> obj1={{"field1","value10"},{"field10","value100"}};
    std::string rep1;
    auto ra1=make_reporting_adapter(rep1,obj1);
    BOOST_CHECK(!v1.apply(ra1));
    rep1.clear();
    auto sa1=make_single_member_reporting_adapter(rep1,_["field1"],obj1);
    BOOST_CHECK(v1.apply(sa1));
    rep1.clear();

    std::map<std::string,std::string> obj2={{"field1","0"},{"field10","value100"}};
    auto ra2=make_reporting_adapter(rep1,obj2);
    BOOST_CHECK(!v1.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"size of field10 is less than 3");
    rep1.clear();
    auto sa2=make_single_member_reporting_adapter(rep1,_["field1"],obj2);
    BOOST_CHECK(!v1.apply(sa2));
    BOOST_CHECK_EQUAL(rep1,"field1 is greater than or equal to value1");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckSingleMemberAggregationReport)
{
    auto v1=validator(
                _[size](eq,1),
                _["field1"](gte,"value1"),
                _["field10"](value(gte,"0") ^AND^ size(lt,3))
            );
    std::string rep1;

    std::map<std::string,std::string> obj1={{"field1","value10"},{"field10","value100"}};
    auto sa1=make_single_member_reporting_adapter(rep1,_["field10"],obj1);
    BOOST_CHECK(!v1.apply(sa1));
    BOOST_CHECK_EQUAL(rep1,"size of field10 is less than 3");
    rep1.clear();

    std::map<std::string,std::string> obj2={{"field1","value10"},{"field10","1"}};
    auto sa2=make_single_member_reporting_adapter(rep1,_["field10"],obj2);
    BOOST_CHECK(v1.apply(sa2));
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckNestedSingleMember)
{
    std::vector<float> vec1;
    auto v1=validator(
                _[size](eq,1),
                _["field1"][vec1](gte,"value1"),
                _["field10"]["field20"](value(gte,"0") ^AND^ size(lt,3))
            );
    std::string rep1;

    std::map<std::string,std::map<std::string,std::string>> obj1=
    {
        {
            "field10",
            {
                {"field1","value10"},
                {"field20","value100"}
            }
        }
    };
    auto sa1=make_single_member_reporting_adapter(rep1,_["field10"]["field20"],obj1);
    BOOST_CHECK(!v1.apply(sa1));
    BOOST_CHECK_EQUAL(rep1,"size of field20 of field10 is less than 3");
    rep1.clear();

    std::map<std::string,std::map<std::string,std::string>> obj2={{"field10",{{"field1","value10"},{"field20","1"}}}};
    auto sa2=make_single_member_reporting_adapter(rep1,_["field10"]["field20"],obj2);
    BOOST_CHECK(v1.apply(sa2));
    rep1.clear();
}

BOOST_AUTO_TEST_SUITE_END()
#endif
