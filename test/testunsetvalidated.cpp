#include <set>
#include <iterator>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/prevalidation/unset_validated.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestPrevalidation)

BOOST_AUTO_TEST_CASE(CheckUnsetValidated)
{
    error_report err;

    auto v1=validator(
                _["field1"](exists,true),
                _["field1"](eq,100)
            );
    std::map<std::string,std::string> m1{
        {"field1","value1"},
        {"field2","value2"},
        {"field3","value3"}
    };

    unset_validated(m1,_["field1"],v1,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field1 must exist"));
    BOOST_CHECK(m1.find("field1")!=m1.end());

    BOOST_CHECK_THROW(unset_validated(m1,_["field1"],v1),validation_error);
    try
    {
        unset_validated(m1,_["field1"],v1);
    }
    catch (const validation_error& e)
    {
        BOOST_CHECK_EQUAL(e.what(),"field1 must exist");
    }
    BOOST_CHECK(m1.find("field1")!=m1.end());

    unset_validated(m1,_["field2"],v1,err);
    BOOST_CHECK(!err);
    BOOST_CHECK(m1.find("field2")==m1.end());

    BOOST_CHECK_NO_THROW(unset_validated(m1,_["field3"],v1));
    BOOST_CHECK(m1.find("field3")==m1.end());

    auto v2=validator(
                _["field1"](contains,"value1"),
                _["field1"](size(gte,3))
            );
    std::map<std::string,std::map<std::string,std::string>> m2{
        {"field1",{{"value1","content1"},{"value2","content2"},{"value3","content3"}}}
    };
    unset_validated(m2,_["field1"]["value1"],v2,err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("field1 must contain value1"));
    unset_validated(m2,_["field1"]["value2"],v2,err);
    BOOST_CHECK(!err);
    BOOST_CHECK_EQUAL(err.message(),std::string(""));

    auto v3=validator(
                _["field1"](ANY(contains,"content1"))
            );
    std::map<std::string,std::map<std::string,std::set<std::string>>> m3{
        {"field1",{{"value1",{"content1"}},{"value2",{"content2"}},{"value3",{"content3"}}}}
    };
    auto& s3=m3["field1"]["value1"];
    unset_validated(m3,_["field1"]["value1"]["content1"],strict_any(v3),err);
    BOOST_CHECK(err);
    BOOST_CHECK_EQUAL(err.message(),std::string("at least one element of field1 must contain content1"));
    BOOST_CHECK(s3.find("content1")!=s3.end());

    unset_validated(m3,_["field1"]["value1"]["content1"],v3,err);
    BOOST_CHECK(!err);
    BOOST_CHECK(s3.find("content1")==s3.end());
}

BOOST_AUTO_TEST_SUITE_END()
