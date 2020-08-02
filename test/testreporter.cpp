#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/reporting/reporter.hpp>
#include <dracosha/validator/reporting/reporter_with_object_name.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestReporter)

BOOST_AUTO_TEST_CASE(CheckReporter)
{
    std::string rep1;
    auto r1=make_reporter(rep1);
    r1.validate("field1",value,gte,10);
    BOOST_CHECK_EQUAL(rep1,std::string("field1 must be greater than or equal to 10"));

    std::string rep2;
    auto w2=wrap_backend_formatter(rep2);
    auto r2=make_reporter(w2);
    r2.validate("field1",value,gte,10);
    BOOST_CHECK_EQUAL(rep2,std::string("field1 must be greater than or equal to 10"));

    auto& dst2_f=r2.destination<decltype(w2)>();
    std::string& s2_1=dst2_f;
    std::string& s2_2=w2;
    BOOST_CHECK(&s2_1==&s2_2);
    BOOST_CHECK_EQUAL(std::string(dst2_f),std::string(w2));

    auto& dst2_s=r2.destination<std::string>();
    BOOST_CHECK_EQUAL(dst2_s,rep2);
}

BOOST_AUTO_TEST_CASE(CheckReporterAggregate)
{
    std::string rep1;
    auto r1=make_reporter(rep1);

    r1.aggregate_open(string_and);
    r1.validate("field1",value,gte,10);
    r1.aggregate_close(false);
    BOOST_CHECK_EQUAL(rep1,std::string("field1 must be greater than or equal to 10"));
    rep1.clear();

    r1.aggregate_open(string_or);
    r1.validate("field1",value,gte,10);
    r1.validate("field1",size,lt,100);
    r1.aggregate_close(false);
    BOOST_CHECK_EQUAL(rep1,std::string("field1 must be greater than or equal to 10 OR size of field1 must be less than 100"));
    rep1.clear();

    r1.aggregate_open(string_not);
    r1.validate("field1",value,gte,10);
    r1.aggregate_close(false);
    BOOST_CHECK_EQUAL(rep1,std::string("NOT field1 must be greater than or equal to 10"));
    rep1.clear();

    r1.aggregate_open(string_or);
    r1.validate("field1",value,gte,10);
    r1.validate("field1",size,lt,100);
    r1.aggregate_open(string_and);
        r1.validate("field10",value,gte,5);
        r1.validate("field10",size,lt,50);
    r1.aggregate_close(false);
    r1.aggregate_close(false);
    BOOST_CHECK_EQUAL(rep1,std::string("field1 must be greater than or equal to 10 OR size of field1 must be less than 100 OR (field10 must be greater than or equal to 5 AND size of field10 must be less than 50)"));
    rep1.clear();

    r1.aggregate_open(string_not);
    r1.aggregate_open(string_or);
    r1.validate("field1",value,gte,10);
    r1.validate("field1",size,lt,100);
    r1.aggregate_open(string_and);
        r1.validate("field10",value,gte,5);
        r1.validate("field10",size,lt,50);
    r1.aggregate_close(false);
    r1.aggregate_close(false);
    r1.aggregate_close(false);
    BOOST_CHECK_EQUAL(rep1,std::string("NOT (field1 must be greater than or equal to 10 OR size of field1 must be less than 100 OR (field10 must be greater than or equal to 5 AND size of field10 must be less than 50))"));
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckReporterWithObjectName)
{
    std::string rep1;
    auto r1=make_reporter_with_object_name(rep1,"object");

    r1.validate_operator(gte,10);
    BOOST_CHECK_EQUAL(rep1,std::string("object must be greater than or equal to 10"));
    rep1.clear();

    r1.validate_property(size,gte,10);
    BOOST_CHECK_EQUAL(rep1,std::string("size of object must be greater than or equal to 10"));
    rep1.clear();

    r1.validate(_["field1"],value,gte,10);
    BOOST_CHECK_EQUAL(rep1,std::string("field1 of object must be greater than or equal to 10"));
    rep1.clear();

    r1.validate_with_other_member(_["field1"],value,gte,_["field2"]);
    BOOST_CHECK_EQUAL(rep1,std::string("field1 of object must be greater than or equal to field2 of object"));
    rep1.clear();

    r1.validate_with_master_sample(_["field1"],value,gte,_["field1"],0);
    BOOST_CHECK_EQUAL(rep1,std::string("field1 of object must be greater than or equal to field1 of sample"));
    rep1.clear();
}

BOOST_AUTO_TEST_SUITE_END()
