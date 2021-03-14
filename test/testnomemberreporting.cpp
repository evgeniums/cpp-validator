#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/detail/hint_helper.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestReporting)
#if 1
BOOST_AUTO_TEST_CASE(CheckNotExistingMemberIgnoreReport)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    ra1.set_check_member_exists_before_validation(true);

    auto v1=validator(
                _["field1"](gte,9),
                _["field2"](eq,100)
            );
    BOOST_CHECK(v1.apply(ra1));
    rep1.clear();

    auto v2=validator(
                _["field2"](gt,100)
            );
    BOOST_CHECK(v2.apply(ra1));
    rep1.clear();

    auto v3=validator(
                _["field2"](gt,100) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field1 must be equal to 20");
    rep1.clear();

    auto v4=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20))
            );
    BOOST_CHECK(v4.apply(ra1));
    rep1.clear();

    auto v5=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,9)
            );
    BOOST_CHECK(v5.apply(ra1));
    rep1.clear();

    auto v6=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field1 must be greater than or equal to 100");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckNotExistingMemberAbortReport)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    ra1.set_check_member_exists_before_validation(true);
    ra1.set_unknown_member_mode(if_member_not_found::abort);

    auto v1=validator(
                _["field1"](gte,9),
                _["field2"](eq,100)
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    rep1.clear();

    auto v2=validator(
                _["field2"](gt,100)
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    rep1.clear();

    auto v3=validator(
                _["field2"](gt,100) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist OR field1 must be equal to 20");
    rep1.clear();

    auto v4=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20))
            );
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    rep1.clear();

    auto v5=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,9)
            );
    BOOST_CHECK(!v5.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    rep1.clear();

    auto v6=validator(
                _["field2"](value(gt,100) ^OR^ value(eq,20)),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckNotExistingOtherMemberIgnoreReport)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    ra1.set_check_member_exists_before_validation(true);

    auto v1=validator(
                _["field1"](gte,9),
                _["field2"](eq,_["field1"])
            );
    BOOST_CHECK(v1.apply(ra1));
    rep1.clear();
    auto v1_1=validator(
                _["field1"](gte,9),
                _["field3"](eq,_["field4"])
            );
    BOOST_CHECK(v1_1.apply(ra1));
    rep1.clear();

    auto v2=validator(
                _["field2"](gt,100)
            );
    BOOST_CHECK(v2.apply(ra1));
    rep1.clear();
    auto v2_1=validator(
                _["field1"](gt,_["field4"])
            );
    BOOST_CHECK(v2_1.apply(ra1));
    rep1.clear();

    auto v3=validator(
                _["field2"](gt,_["field3"]) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field1 must be equal to 20");
    rep1.clear();
    auto v3_1=validator(
                _["field1"](gt,_["field4"]) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3_1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field1 must be equal to 20");
    rep1.clear();

    auto v4=validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"]))
            );
    BOOST_CHECK(v4.apply(ra1));
    rep1.clear();
    auto v4_1=validator(
                _["field1"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"]))
            );
    BOOST_CHECK(v4_1.apply(ra1));
    rep1.clear();

    auto v5=validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
                _["field1"](gte,9)
            );
    BOOST_CHECK(v5.apply(ra1));
    rep1.clear();
    auto v5_1=validator(
                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
                _["field1"](gte,9)
            );
    BOOST_CHECK(v5_1.apply(ra1));
    rep1.clear();

    auto v6=validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field1 must be greater than or equal to 100");
    rep1.clear();
    auto v6_1=validator(
                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6_1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field1 must be greater than or equal to 100");
    rep1.clear();
}
#endif
BOOST_AUTO_TEST_CASE(CheckNotExistingOtherMemberAbortReport)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    ra1.set_check_member_exists_before_validation(true);
    ra1.set_unknown_member_mode(if_member_not_found::abort);
#if 1
    auto v1=validator(
                _["field1"](gte,9),
                _["field2"](eq,_["field1"])
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    rep1.clear();
    auto v1_1=validator(
                _["field1"](gte,9),
                _["field3"](eq,_["field4"])
            );
    BOOST_CHECK(!v1_1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field3 must be equal to field4");
    rep1.clear();

    auto v2=validator(
                _["field2"](eq,_["field1"])
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    rep1.clear();
    auto v2_1=validator(
                _["field3"](eq,_["field4"])
            );
    BOOST_CHECK(!v2_1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field3 must be equal to field4");
    rep1.clear();

    auto v3=validator(
                _["field2"](gt,_["field3"]) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist OR field1 must be equal to 20");
    rep1.clear();
    auto v3_1=validator(
                _["field1"](gt,_["field4"]) ^OR^ _["field1"](eq,20)
            );
    BOOST_CHECK(!v3_1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field1 must be greater than field4 OR field1 must be equal to 20");
    rep1.clear();

    auto v4=validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"]))
            );
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    rep1.clear();
#endif
    auto v4_1=validator(
                _["field1"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"]))
            );
    BOOST_CHECK(!v4_1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field1 must be greater than field4 OR field1 must be equal to field4");
    rep1.clear();
#if 1
    auto v5=validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
                _["field1"](gte,9)
            );
    BOOST_CHECK(!v5.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    rep1.clear();
    auto v5_1=validator(
                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
                _["field1"](gte,9)
            );
    BOOST_CHECK(!v5_1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field3 must be greater than field4 OR field3 must be equal to field4");
    rep1.clear();

    auto v6=validator(
                _["field2"](value(gt,_["field1"]) ^OR^ value(eq,_["field1"])),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    rep1.clear();
    auto v6_1=validator(
                _["field3"](value(gt,_["field4"]) ^OR^ value(eq,_["field4"])),
                _["field1"](gte,100)
            );
    BOOST_CHECK(!v6_1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field3 must be greater than field4 OR field3 must be equal to field4");
    rep1.clear();
#endif
}
#if 1
BOOST_AUTO_TEST_CASE(CheckNotExistingMemberSampleIgnoreReport)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    std::map<std::string,int> m2={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    ra1.set_check_member_exists_before_validation(true);

    auto v1=validator(
                _["field1"](gte,_(m2)),
                _["field2"](eq,_(m2))
            );
    BOOST_CHECK(v1.apply(ra1));
    rep1.clear();

    auto v2=validator(
                _["field2"](gt,_(m2)) ^OR^ _["field3"](lt,_(m2))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field3 must be less than field3 of sample");
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckNotExistingMemberSampleAbortReport)
{
    std::map<std::string,int> m1={
            {"field1",10},
            {"field3",5}
        };
    std::map<std::string,int> m2={
            {"field1",10},
            {"field2",5},
            {"field3",5}
        };
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);
    ra1.set_check_member_exists_before_validation(true);
    ra1.set_unknown_member_mode(if_member_not_found::abort);

    auto v1=validator(
                _["field1"](gte,_(m2)),
                _["field2"](eq,_(m2))
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist");
    rep1.clear();

    auto v2=validator(
                _["field2"](gt,_(m2)) ^OR^ _["field3"](lt,_(m2))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,"field2 must exist OR field3 must be less than field3 of sample");
    rep1.clear();
}
#endif
BOOST_AUTO_TEST_SUITE_END()
