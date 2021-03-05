#include <set>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/prevalidation/resize_validated.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestPrevalidation)

BOOST_AUTO_TEST_CASE(CheckResizeValidated)
{
    error_report err;
    auto v1=validator(
                _["field1"](empty(flag,false)),
                _["field2"](size(gte,5) ^AND^ value(ne,"Invalid value")),
                _["field5"](length(gte,4)),
                _["field6"](NOT(length(lt,4)))
            );

    std::map<std::string,std::string> m1{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","value3"},
        {"field4","value4"},
        {"field5","value5"},
        {"field6","value6"}
    };

    resize_validated(m1,_["field1"],0,v1,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field1 must be not empty"));
    BOOST_CHECK_EQUAL(m1.find("field1")->second.size(),6);
    BOOST_CHECK_THROW(resize_validated(m1,_["field1"],0,v1),validation_error);
    try
    {
        resize_validated(m1,_["field1"],0,v1);
    }
    catch (const validation_error& e)
    {
        BOOST_CHECK_EQUAL(e.what(),"field1 must be not empty");
    }
    BOOST_CHECK_EQUAL(m1.find("field1")->second.size(),6);

    resize_validated(m1,_["field2"],3,v1,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("size of field2 must be greater than or equal to 5"));
    BOOST_CHECK_EQUAL(m1.find("field2")->second.size(),6);
    BOOST_CHECK_THROW(resize_validated(m1,_["field2"],3,v1),validation_error);
    try
    {
        resize_validated(m1,_["field2"],3,v1);
    }
    catch (const validation_error& e)
    {
        BOOST_CHECK_EQUAL(e.what(),"size of field2 must be greater than or equal to 5");
    }
    BOOST_CHECK_EQUAL(m1.find("field2")->second.size(),6);

    resize_validated(m1,_["field3"],3,v1,err);
    BOOST_CHECK(!err);
    BOOST_CHECK_EQUAL(m1.find("field3")->second.size(),3);
    BOOST_CHECK_NO_THROW(resize_validated(m1,_["field4"],3,v1));
    BOOST_CHECK_EQUAL(m1.find("field4")->second.size(),3);

    resize_validated(m1,_["field5"],3,v1,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("length of field5 must be greater than or equal to 4"));
    BOOST_CHECK_EQUAL(m1.find("field5")->second.size(),6);
    BOOST_CHECK_THROW(resize_validated(m1,_["field5"],3,v1),validation_error);
    try
    {
        resize_validated(m1,_["field5"],3,v1);
    }
    catch (const validation_error& e)
    {
        BOOST_CHECK_EQUAL(e.what(),"length of field5 must be greater than or equal to 4");
    }
    BOOST_CHECK_EQUAL(m1.find("field5")->second.size(),6);

    resize_validated(m1,_["field5"],5,v1,err);
    BOOST_CHECK(!err);
    BOOST_CHECK_EQUAL(m1.find("field5")->second.size(),5);
    BOOST_CHECK_NO_THROW(resize_validated(m1,_["field5"],5,v1));
    BOOST_CHECK_EQUAL(m1.find("field5")->second.size(),5);

    resize_validated(m1,_["field6"],3,v1,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(m1.find("field6")->second.size(),6);
    BOOST_CHECK_EQUAL(err.message(),std::string("NOT length of field6 must be less than 4"));

    std::map<std::string,std::vector<std::string>> m2{
        {"field1",{"value1"}},
        {"field2",{"value2"}},
        {"field3",{"value3"}}
    };
    auto v2=validator(
                _["field1"](ANY(size(gte,4))),
                _["field2"](ANY(empty(flag,false)))
            );

    resize_validated(m2,_["field1"][0],3,strict_any(v2),err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("size of at least one element of field1 must be greater than or equal to 4"));
    resize_validated(m2,_["field2"][0],0,strict_any(v2),err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("at least one element of field2 must be not empty"));

    auto v3=validator(
                _["field1"](ALL(size(gte,4))),
                _["field2"](ALL(empty(flag,false))),
                _["field3"](ALL(size(gte,_(m2))))
            );    
    resize_validated(m2,_["field1"][0],3,v3,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("size of each element of field1 must be greater than or equal to 4"));
    resize_validated(m2,_["field2"][0],0,v3,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("each element of field2 must be not empty"));
    resize_validated(m2,_["field3"][0],2,v3,err);
    BOOST_CHECK(!err); // not existing member is ignored

    std::map<std::string,std::string> m4{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","value3"},
        {"field4","value4"},
        {"field5","value5"},
        {"field6","value6"}
    };

    auto check_value_size=[&err,&m4](auto&& v)
    {
        resize_validated(m4,_["field1"],3,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field1"],6,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field1"],10,v,err);
        BOOST_CHECK(!err);

        resize_validated(m4,_["field2"],3,v,err);
        BOOST_CHECK(err);
        BOOST_CHECK_EQUAL(err.message(),std::string("field2 must be equal to value2"));
        resize_validated(m4,_["field2"],6,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field2"],10,v,err);
        BOOST_CHECK(err);
        BOOST_CHECK_EQUAL(err.message(),std::string("field2 must be equal to value2"));

        resize_validated(m4,_["field3"],3,v,err);
        BOOST_CHECK(err);
        BOOST_CHECK_EQUAL(err.message(),std::string("field3 must be greater than value3"));
        resize_validated(m4,_["field3"],6,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field3"],10,v,err);
        BOOST_CHECK(!err);

        resize_validated(m4,_["field4"],3,v,err);
        BOOST_CHECK(err);
        BOOST_CHECK_EQUAL(err.message(),std::string("field4 must be greater than or equal to value4"));
        resize_validated(m4,_["field4"],6,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field4"],10,v,err);
        BOOST_CHECK(!err);

        resize_validated(m4,_["field5"],3,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field5"],6,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field5"],10,v,err);
        BOOST_CHECK(err);
        BOOST_CHECK_EQUAL(err.message(),std::string("field5 must be less than value5"));

        resize_validated(m4,_["field6"],3,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field6"],6,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field6"],10,v,err);
        BOOST_CHECK(err);
        BOOST_CHECK_EQUAL(err.message(),std::string("field6 must be less than or equal to value6"));
    };

    auto v4=validator(
        _["field1"](ne,"value1"),
        _["field2"](eq,"value2"),
        _["field3"](gt,"value3"),
        _["field4"](gte,"value4"),
        _["field5"](lt,"value5"),
        _["field6"](lte,"value6")
    );
    check_value_size(v4);

    auto v5=validator(
        _["field1"](lex_ne,"value1"),
        _["field2"](lex_eq,"value2"),
        _["field3"](lex_gt,"value3"),
        _["field4"](lex_gte,"value4"),
        _["field5"](lex_lt,"value5"),
        _["field6"](lex_lte,"value6")
    );
    check_value_size(v5);

    auto v6=validator(
        _["field1"](ilex_ne,"value1"),
        _["field2"](ilex_eq,"value2"),
        _["field3"](ilex_gt,"value3"),
        _["field4"](ilex_gte,"value4"),
        _["field5"](ilex_lt,"value5"),
        _["field6"](ilex_lte,"value6")
    );
    check_value_size(v5);

    auto check_contains_size=[&err,&m4](auto&& v)
    {
        resize_validated(m4,_["field1"],3,v,err);
        BOOST_CHECK(err);
        BOOST_CHECK_EQUAL(err.message(),std::string("field1 must contain value1"));
        resize_validated(m4,_["field1"],6,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field1"],10,v,err);
        BOOST_CHECK(!err);

        resize_validated(m4,_["field2"],3,v,err);
        BOOST_CHECK(err);
        BOOST_CHECK_EQUAL(err.message(),std::string("field2 must start with value2"));
        resize_validated(m4,_["field2"],6,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field2"],10,v,err);
        BOOST_CHECK(!err);

        resize_validated(m4,_["field3"],3,v,err);
        BOOST_CHECK(err);
        BOOST_CHECK_EQUAL(err.message(),std::string("field3 must end with value3"));
        resize_validated(m4,_["field3"],6,v,err);
        BOOST_CHECK(!err);
        resize_validated(m4,_["field3"],10,v,err);
        BOOST_CHECK(!err);
    };

    auto v7=validator(
        _["field1"](lex_contains,"value1"),
        _["field2"](lex_starts_with,"value2"),
        _["field3"](lex_ends_with,"value3")
    );
    check_contains_size(v7);

    auto v8=validator(
        _["field1"](ilex_contains,"value1"),
        _["field2"](ilex_starts_with,"value2"),
        _["field3"](ilex_ends_with,"value3")
    );
    check_contains_size(v8);
}

BOOST_AUTO_TEST_SUITE_END()
