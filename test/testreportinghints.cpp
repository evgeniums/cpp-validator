#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/detail/hint_helper.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestReporting)

BOOST_AUTO_TEST_CASE(CheckCustomDescriptions)
{
    auto m0=_["field1"];
    static_assert(!decltype(m0)::has_name,"");

    auto m1=_["field1"]("hello");
    static_assert(decltype(m1)::has_name,"");
    BOOST_CHECK_EQUAL(std::string(m1.name()),"hello");

    auto m2=_["field1"]("spaces",grammar::plural);
    static_assert(decltype(m2)::has_name,"");
    BOOST_CHECK_EQUAL(std::string(m2.name()),"spaces");
    BOOST_CHECK(grammar_category<grammar>.is_set(m2.name().grammar_cats(),grammar::plural));
    BOOST_CHECK(!grammar_category<grammar>.is_set(m2.name().grammar_cats(),grammar::neuter));

    auto m3=_["field1"]("cats",grammar::plural,grammar::feminine);
    static_assert(decltype(m3)::has_name,"");
    BOOST_CHECK_EQUAL(std::string(m3.name()),"cats");
    BOOST_CHECK(grammar_category<grammar>.is_set(m3.name().grammar_cats(),grammar::plural));
    BOOST_CHECK(grammar_category<grammar>.is_set(m3.name().grammar_cats(),grammar::feminine));
    BOOST_CHECK(!grammar_category<grammar>.is_set(m3.name().grammar_cats(),grammar::neuter));

    std::string rep1;
    std::vector<size_t> vec1={10,20,30,40,50};
    auto ra1=make_reporting_adapter(vec1,rep1);

    auto v1=validator(
                _[1](gte,_(100,"one hundred"))
            );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("element #1 must be greater than or equal to one hundred"));
    rep1.clear();

    auto v2=validator(
                _[1]("first element")(gte,_(100,"one hundred"))
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("first element must be greater than or equal to one hundred"));
    rep1.clear();

    auto v3=validator(
                _[1]("first element")(_(gte,"must be not less than"),_(100,"one hundred"))
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("first element must be not less than one hundred"));
    rep1.clear();

    auto v4=validator(
                _[1]("first element")(
                    value(_(gte,"must be not less than"),_(100,"one hundred"))
                    ^OR^
                    value(_(eq,"must be the same as"),_(1,"one"))
                )
            );
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("first element must be not less than one hundred OR first element must be the same as one"));
    rep1.clear();

    auto v5=validator(
                _[1]("first element")(
                    value(_n(gte,"must be less than"),_(5,"five"))
                    ^OR^
                    value(_n(eq,"must not be the same as"),_(20,"twenty"))
                )
            );
    BOOST_CHECK(!v5.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("first element must be less than five OR first element must not be the same as twenty"));
    rep1.clear();

    auto v6=validator(
                _[1]("")(
                    value(_(gte,""),_(500,""))
                )
            );
    BOOST_CHECK(!v6.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string(""));
    rep1.clear();

    auto v7=validator(
                _[1]("")(
                    value(_n(eq,""),_(20,""))
                )
            );
    BOOST_CHECK(!v7.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string(""));
    rep1.clear();

    auto v8=validator(
                _[1]("first element")(
                    value(_n(gte),_(5,"five"))
                    ^OR^
                    value(_n(eq),_(20,"twenty"))
                )
            );
    BOOST_CHECK(!v8.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("first element must be less than five OR first element must be not equal to twenty"));
    rep1.clear();

    auto v9=validator(
                _[1]("first element")(
                    value(_n(_n(gte)),_(5000,"five thousands"))
                    ^OR^
                    value(_n(_n(eq)),_(200,"two hundreds"))
                )
            );
    BOOST_CHECK(!v9.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("first element must be greater than or equal to five thousands OR first element must be equal to two hundreds"));
    rep1.clear();
}

BOOST_AUTO_TEST_CASE(CheckExplicitValidationReport)
{
    std::map<std::string,size_t> m1={{"field1",1}};
    std::string rep1;
    auto ra1=make_reporting_adapter(m1,rep1);

    auto v1=validator(
                _["field1"](gte,10)
            )("Explicit description");
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("Explicit description"));
    rep1.clear();

    auto v1_1=validator(
                _["field1"](gte,10)
            ).hint("Explicit description");
    BOOST_CHECK(!v1_1.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("Explicit description"));
    rep1.clear();

    auto v2=validator(
                _["field1"](gte,10)("Explicit description")
            );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("Explicit description"));
    rep1.clear();

    auto v3=validator(
                _["field1"](gte,10)("Explicit description 1")
                ^OR^
                _["field1"](eq,100)("Explicit description 2")
            );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("Explicit description 1 OR Explicit description 2"));
    rep1.clear();

    auto v4=validator(
                _["field1"](gte,10)("Explicit description 1")
                ^OR^
                _["field1"](eq,100)("Explicit description 2")
            )("Explicit description");
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("Explicit description"));
    rep1.clear();

    auto v5=validator(
                _["field1"](value(gte,10) ^OR^ value(eq,15))("Explicit description 1")
                ^OR^
                _["field1"](eq,100)("Explicit description 2")
            );
    BOOST_CHECK(!v5.apply(ra1));
    BOOST_CHECK_EQUAL(rep1,std::string("Explicit description 1 OR Explicit description 2"));
    rep1.clear();

    std::map<std::string,std::vector<size_t>> m2={{"field1",{1,2,3,4,5}}};
    auto ra2=make_reporting_adapter(m2,rep1);
    auto v6=validator(
                _["field1"](ANY(value(gte,10) ^OR^ value(eq,15)))("Explicit description 1")
                ^OR^
                _["field1"](eq,100)("Explicit description 2")
            );
    BOOST_CHECK(!v6.apply(ra2));
    BOOST_CHECK_EQUAL(rep1,std::string("Explicit description 1 OR Explicit description 2"));
    rep1.clear();

    auto a3=make_default_adapter(m2);
    BOOST_CHECK(!v6.apply(a3));
}

BOOST_AUTO_TEST_SUITE_END()
