#include <set>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/prevalidation/clear_validated.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestPrevalidation)

BOOST_AUTO_TEST_CASE(CheckClearValidated)
{
    error_report err;

    auto v1=validator(
                _["field1"](empty(flag,false)),
                _["field2"](size(gte,2))
            );

    std::map<std::string,std::string> m1{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","value3"},
        {"field4","value4"}
    };

    clear_validated(m1,_["field1"],v1,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field1 must be not empty"));
    BOOST_CHECK(!m1.find("field1")->second.empty());

    BOOST_CHECK_THROW(clear_validated(m1,_["field1"],v1),validation_error);
    try
    {
        clear_validated(m1,_["field1"],v1);
    }
    catch (const validation_error& e)
    {
        BOOST_CHECK_EQUAL(e.what(),"field1 must be not empty");
    }
    BOOST_CHECK(!m1.find("field1")->second.empty());

    clear_validated(m1,_["field2"],v1,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("size of field2 must be greater than or equal to 2"));
    BOOST_CHECK(!m1.find("field2")->second.empty());

    BOOST_CHECK_THROW(clear_validated(m1,_["field2"],v1),validation_error);
    try
    {
        clear_validated(m1,_["field2"],v1);
    }
    catch (const validation_error& e)
    {
        BOOST_CHECK_EQUAL(e.what(),"size of field2 must be greater than or equal to 2");
    }
    BOOST_CHECK(!m1.find("field2")->second.empty());

    clear_validated(m1,_["field3"],v1,err);
    BOOST_CHECK(!err);
    BOOST_CHECK(m1.find("field3")->second.empty());

    BOOST_CHECK_NO_THROW(clear_validated(m1,_["field4"],v1));
    BOOST_CHECK(m1.find("field4")->second.empty());

    auto v2=validator(
        _["field1"](ANY(empty(flag,false)))
    );
    auto v3=validator(
        _["field1"](ANY(empty(flag,true)))
    );
    std::map<std::string,std::set<std::string>> s2{
        {"field1",{"value1","value2","value3"}},
        {"field2",{"value1","value2","value3"}}
    };
    BOOST_CHECK(v2.apply(s2));
    BOOST_CHECK(!v3.apply(s2));

    std::map<std::string,std::map<std::string,std::string>> m3{
        {"field1",{{"value1","content1"},{"value2","content2"},{"value3","content3"}}},
        {"field2",{{"value1","content1"},{"value2","content2"},{"value3","content3"}}}
    };

    clear_validated(m3,_["field1"]["value1"],strict_any(v2),err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(m3["field1"]["value1"].size(),8);
    clear_validated(m3,_["field2"]["value1"],strict_any(v2),err);
    BOOST_CHECK(!err);
    BOOST_CHECK_EQUAL(m3["field2"]["value1"].size(),0);
    clear_validated(m3,_["field1"]["value1"],v2,err);
    BOOST_CHECK(!err);
    BOOST_CHECK_EQUAL(m3["field1"]["value1"].size(),0);

    auto v4=validator(
        _["field1"](ne,""),
        _["field2"](eq,"value2"),
        _["field3"](gt,"value3"),
        _["field4"](gte,"value4"),
        _["field5"](lt,"value5"),
        _["field6"](lte,"value6")
    );
    std::map<std::string,std::string> m4{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","value3"},
        {"field4","value4"},
        {"field5","value5"},
        {"field6","value6"}
    };
    clear_validated(m4,_["field1"],v4,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field1 must be not equal to"));
    clear_validated(m4,_["field2"],v4,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field2 must be equal to value2"));
    clear_validated(m4,_["field3"],v4,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field3 must be greater than value3"));
    clear_validated(m4,_["field4"],v4,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field4 must be greater than or equal to value4"));
    clear_validated(m4,_["field5"],v4,err);
    BOOST_CHECK(!err);
    clear_validated(m4,_["field6"],v4,err);
    BOOST_CHECK(!err);

    auto v5=validator(
        _["field1"](lex_ne,""),
        _["field2"](lex_eq,"value2"),
        _["field3"](lex_gt,"value3"),
        _["field4"](lex_gte,"value4"),
        _["field5"](lex_lt,"value5"),
        _["field6"](lex_lte,"value6")
    );
    clear_validated(m4,_["field1"],v5,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field1 must be not equal to"));
    clear_validated(m4,_["field2"],v5,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field2 must be equal to value2"));
    clear_validated(m4,_["field3"],v5,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field3 must be greater than value3"));
    clear_validated(m4,_["field4"],v5,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field4 must be greater than or equal to value4"));
    clear_validated(m4,_["field5"],v5,err);
    BOOST_CHECK(!err);
    clear_validated(m4,_["field6"],v5,err);
    BOOST_CHECK(!err);

    auto v6=validator(
        _["field1"](ilex_ne,""),
        _["field2"](ilex_eq,"value2"),
        _["field3"](ilex_gt,"value3"),
        _["field4"](ilex_gte,"value4"),
        _["field5"](ilex_lt,"value5"),
        _["field6"](ilex_lte,"value6")
    );
    clear_validated(m4,_["field1"],v6,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field1 must be not equal to"));
    clear_validated(m4,_["field2"],v6,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field2 must be equal to value2"));
    clear_validated(m4,_["field3"],v6,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field3 must be greater than value3"));
    clear_validated(m4,_["field4"],v6,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field4 must be greater than or equal to value4"));
    clear_validated(m4,_["field5"],v6,err);
    BOOST_CHECK(!err);
    clear_validated(m4,_["field6"],v6,err);
    BOOST_CHECK(!err);

    auto v7=validator(
        _["field1"](lex_contains,"value1"),
        _["field2"](lex_starts_with,"value2"),
        _["field3"](lex_ends_with,"value3"),
        _["field4"](ilex_contains,"value4"),
        _["field5"](ilex_starts_with,"value5"),
        _["field6"](ilex_ends_with,"value6")
    );
    clear_validated(m4,_["field1"],v7,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field1 must contain value1"));
    clear_validated(m4,_["field2"],v7,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field2 must start with value2"));
    clear_validated(m4,_["field3"],v7,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field3 must end with value3"));
    clear_validated(m4,_["field4"],v7,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field4 must contain value4"));
    clear_validated(m4,_["field5"],v7,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field5 must start with value5"));
    clear_validated(m4,_["field6"],v7,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field6 must end with value6"));

    auto v8=validator(
                _["field1"](empty(flag,true)),
                _["field2"](size(gte,2))
            );
    clear_validated(m1,_["field1"],v8,err);
    BOOST_CHECK(!err);
}

BOOST_AUTO_TEST_SUITE_END()
