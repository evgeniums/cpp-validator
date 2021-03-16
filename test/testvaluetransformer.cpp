#include <boost/test/unit_test.hpp>

#include <dracosha/validator/value_transformer.hpp>
#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestValueTransformer)

BOOST_AUTO_TEST_CASE(CheckTransformer)
{
    auto h1=[](auto&& val,
                std::enable_if_t<!std::is_integral<std::decay_t<decltype(val)>>::value,void*> =nullptr
            )
    {
        return val.size();
    };
    auto t1=make_value_transformer(h1,"value size");

    static_assert(decltype(t1)::has<std::string>(),"");
    static_assert(!decltype(t1)::has<int>(),"");

    std::string s1("Hello world");
    BOOST_CHECK_EQUAL(t1.get(s1),s1.size());
    BOOST_CHECK_EQUAL(property(s1,t1),s1.size());

    BOOST_CHECK_EQUAL(t1.name(),std::string("value size"));
}

BOOST_AUTO_TEST_CASE(CheckValidateTransformer)
{
    auto h1=[](auto&& val,
                std::enable_if_t<!std::is_integral<std::decay_t<decltype(val)>>::value,void*> =nullptr
            )
    {
        return val.size();
    };
    auto t1=make_value_transformer(h1,"value size");

    auto check=[&h1](auto&& v1, auto property_notation)
    {
        std::string s1("Hello world");
        BOOST_CHECK(v1.apply(s1));

        std::string s2("Hi");
        BOOST_CHECK(!v1.apply(s2));

        hana::eval_if
        (
            property_notation,
            [&](auto&&)
            {},
            [&](auto&& _)
            {
                int i3=100;
                BOOST_CHECK(_(v1).apply(i3)); // ignored

                auto a4=make_default_adapter(i3);
                a4.set_check_member_exists_before_validation(true);
                a4.set_unknown_member_mode(if_member_not_found::abort);
                BOOST_CHECK(!_(v1).apply(a4));
            }
        );
    };

    BOOST_TEST_CONTEXT("member notation")
    {
        auto v1=validator(
                _[t1](gte,5)
            );
        check(v1,std::false_type{});
    }

    BOOST_TEST_CONTEXT("property notation")
    {
        auto v2=validator(
                t1(gte,5)
            );
        check(v2,std::true_type{});
    }
}

BOOST_AUTO_TEST_CASE(CheckValidateTransformerReport)
{
    auto h1=[](auto&& val,
                std::enable_if_t<!std::is_integral<std::decay_t<decltype(val)>>::value,void*> =nullptr
            )
    {
        return val.size();
    };
    auto t1=make_value_transformer(h1,"value size");

    auto check=[&h1](auto&& v1, auto property_notation)
    {
        std::string rep;
        std::string s2("Hi");
        auto a2=make_reporting_adapter(s2,rep);
        BOOST_CHECK(!v1.apply(a2));
        BOOST_CHECK_EQUAL(rep,std::string("value size must be greater than or equal to 5"));

        hana::eval_if
        (
            property_notation,
            [&](auto&&)
            {},
            [&](auto&& _)
            {
                std::string rep;
                int i3=100;
                auto a4=make_reporting_adapter(i3,rep);
                a4.set_check_member_exists_before_validation(true);
                a4.set_unknown_member_mode(if_member_not_found::abort);
                BOOST_CHECK(!_(v1).apply(a4));
                BOOST_CHECK_EQUAL(rep,std::string("value size must exist"));
            }
        );
    };

    BOOST_TEST_CONTEXT("member notation")
    {
        auto v1=validator(
                _[t1](gte,5)
            );
        check(v1,std::false_type{});
    }

    BOOST_TEST_CONTEXT("property notation")
    {
        auto v2=validator(
                t1(gte,5)
            );
        check(v2,std::true_type{});
    }
}

BOOST_AUTO_TEST_CASE(CheckValidateTransformerFlag)
{
    auto h1=[](auto&& val,
                std::enable_if_t<!std::is_integral<std::decay_t<decltype(val)>>::value,void*> =nullptr
            )
    {
        return val.empty();
    };
    auto t1=make_value_transformer(h1,"content","must be empty","must not be empty");

    auto check1=[&h1](auto&& v1)
    {
        std::string rep;
        std::string s2("Hi");
        auto a2=make_reporting_adapter(s2,rep);
        BOOST_CHECK(!v1.apply(a2));
        BOOST_CHECK_EQUAL(rep,std::string("must be empty"));
    };
    auto check2=[&h1](auto&& v1)
    {
        std::string rep;
        std::string s2;
        auto a2=make_reporting_adapter(s2,rep);
        BOOST_CHECK(!v1.apply(a2));
        BOOST_CHECK_EQUAL(rep,std::string("must not be empty"));
    };

    BOOST_TEST_CONTEXT("member notation")
    {
        auto v1=validator(
                _[t1](flag,true)
            );
        check1(v1);
        auto v2=validator(
                _[t1](flag,false)
            );
        check2(v2);
    }
    BOOST_TEST_CONTEXT("property notation")
    {
        auto v1=validator(
                t1(flag,true)
            );
        check1(v1);
        auto v2=validator(
                t1(flag,false)
            );
        check2(v2);
    }
}

BOOST_AUTO_TEST_SUITE_END()
