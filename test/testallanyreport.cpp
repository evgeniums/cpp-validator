#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestReporting)

BOOST_AUTO_TEST_CASE(CheckAggregationAnyReport)
{
    std::string rep1;

    std::map<std::string,std::map<size_t,std::string>> m1={
            {"field1",
             {
                 {1,"value1"},
                 {2,"value2"},
                 {3,"value3"},
                 {4,"value4"}
             }
            }
        };
    auto ra1=make_reporting_adapter(m1,rep1);

    auto v1=validator(
                _["field1"](ANY(value(gte,"zzz")))
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"at least one element of field1 must be greater than or equal to zzz");
    rep1.clear();

    auto v2=validator(
                _["field1"](ANY(value(gte,"zzz") ^AND^ size(eq,100)))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"at least one element of field1 must be greater than or equal to zzz");
    rep1.clear();

    auto v3=validator(
                _["field1"](ANY(value(gte,"zzz") ^OR^ size(eq,100)))
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"at least one element of field1 must be greater than or equal to zzz OR size of at least one element of field1 must be equal to 100");
    rep1.clear();

    std::map<size_t,std::string> m2={
        {1,"value1"},
        {2,"value2"},
        {3,"value3"},
        {4,"value4"}
    };
    auto ra2=make_reporting_adapter(m2,rep1);

    auto v4=validator(
                ANY(value(gte,"zzz") ^AND^ size(eq,100))
            );
    BOOST_CHECK(!v4.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"at least one element must be greater than or equal to zzz");
    rep1.clear();

    auto v5=validator(
                ANY(value(gte,"zzz") ^OR^ size(eq,100))
            );
    BOOST_CHECK(!v5.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"at least one element must be greater than or equal to zzz OR size of at least one element must be equal to 100");
    rep1.clear();

    std::map<size_t,std::vector<size_t>> m3={
        {1,{1,2,3,4,5}},
        {10,{10,20,30,40,50}},
        {100,{100,200,300,400,500}}
    };
    auto ra3=make_reporting_adapter(m3,rep1);
    auto v6=validator(
                ANY(size(gte,5) ^AND^ ANY(value(gte,1)))
            );
    BOOST_CHECK(v6.apply(ra3));
    rep1.clear();

    auto v7=validator(
                ANY(size(gte,5) ^AND^ ANY(value(gte,1000)))
            );
    BOOST_CHECK(!v7.apply(ra3));
    BOOST_CHECK_EQUAL(rep1,"at least one element of at least one element must be greater than or equal to 1000");
    rep1.clear();

    std::map<size_t,std::map<size_t,std::vector<size_t>>> m4={
        {5,
         {{10,{10,20,30,40,50}}}
        }
    };
    auto ra4=make_reporting_adapter(m4,rep1);
    auto v8=validator(
                _[5](ANY(size(gte,1) ^AND^ ANY(value(gte,1000))))
            );
    BOOST_CHECK(!v8.apply(ra4));
    BOOST_CHECK_EQUAL(rep1,"at least one element of at least one element of element #5 must be greater than or equal to 1000");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckAggregationAllReport)
{
    std::map<std::string,std::map<size_t,std::string>> m1={
            {"field1",
             {
                 {1,"value1"},
                 {2,"value2"},
                 {3,"value3"},
                 {4,"value4"}
             }
            }
        };

    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);

    auto v1=validator(
                _["field1"](ALL(value(gte,"value3")))
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"each element of field1 must be greater than or equal to value3");
    rep1.clear();

    auto v2=validator(
                _["field1"](ALL(value(gte,"val") ^AND^ size(gte,100)))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"size of each element of field1 must be greater than or equal to 100");
    rep1.clear();

    auto v3=validator(
                _["field1"](ALL(value(gte,"zzz") ^OR^ size(eq,100)))
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"each element of field1 must be greater than or equal to zzz OR size of each element of field1 must be equal to 100");
    rep1.clear();

    std::map<size_t,std::string> m2={
        {1,"value1"},
        {2,"value2"},
        {3,"value3"},
        {4,"value444"}
    };
    auto ra2=make_reporting_adapter(m2,rep1);

    auto v4=validator(
                ALL(size(gte,1) ^AND^ value(lte,"value3"))
            );
    BOOST_CHECK(!v4.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"each element must be less than or equal to value3");
    rep1.clear();

    auto v5=validator(
                ALL(value(gte,"value5") ^OR^ size(lte,7))
            );
    BOOST_CHECK(!v5.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,"each element must be greater than or equal to value5 OR size of each element must be less than or equal to 7");
    rep1.clear();

    std::map<size_t,std::vector<size_t>> m3={
        {1,{1,2,3,4,5}},
        {10,{10,20,30,40,50}},
        {100,{100,200,300,400,500}}
    };
    auto ra3=make_reporting_adapter(m3,rep1);
    auto v6=validator(
                ALL(size(gte,5) ^AND^ ALL(value(gte,1)))
            );
    BOOST_CHECK(v6.apply(ra3));
    rep1.clear();

    auto v7=validator(
                ALL(size(gte,5) ^AND^ ALL(value(lt,300)))
            );
    BOOST_CHECK(!v7.apply(ra3));
    BOOST_CHECK_EQUAL(rep1,"each element of each element must be less than 300");
    rep1.clear();

    std::map<size_t,std::map<size_t,std::vector<size_t>>> m4={
        {5,
         {{10,{10,20,30,40,50}}}
        }
    };
    auto ra4=make_reporting_adapter(m4,rep1);
    auto v8=validator(
                _[5](ALL(size(gte,1) ^AND^ ALL(value(lt,50))))
            );
    BOOST_CHECK(!v8.apply(ra4));
    BOOST_CHECK_EQUAL(rep1,"each element of each element of element #5 must be less than 50");
    rep1.clear();
}

BOOST_AUTO_TEST_SUITE_END()
