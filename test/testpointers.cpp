#include <memory>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestPointers)

namespace {

struct PointerName
{
    PointerName(std::string name) : _name(std::move(name))
    {}

    const std::string* name() const noexcept
    {
        return &_name;
    }

    const std::string* name_null() const noexcept
    {
        return nullptr;
    }

    std::string _name;
};

struct SharedPointerName
{
    SharedPointerName(std::string name) : _name(std::make_shared<std::string>(std::move(name)))
    {}

    std::shared_ptr<std::string> name() const noexcept
    {
        return _name;
    }

    std::shared_ptr<std::string> name_null() const noexcept
    {
        return std::shared_ptr<std::string>();
    }

    std::shared_ptr<std::string> _name;
};

DRACOSHA_VALIDATOR_PROPERTY(name)
DRACOSHA_VALIDATOR_PROPERTY(name_null)

}
#if 1
BOOST_AUTO_TEST_CASE(CheckPointers)
{
    auto check=[](auto&& o1, auto&& o2)
    {
        auto v1=validator(
                _[name](gte,"Hello")
             );
        BOOST_CHECK(v1.apply(o1));
        BOOST_CHECK(!v1.apply(o2));

        static_assert(can_check_contains<decltype(o2.name()),decltype(size)>(),"");
        BOOST_CHECK(check_exists(o2,hana::make_tuple(name,size)));

        auto v2=validator(
                _[name][size](gte,5)
             );
        BOOST_CHECK(v2.apply(o1));
        BOOST_CHECK(!v2.apply(o2));
    };

    BOOST_TEST_CONTEXT("raw pointer")
    {
        PointerName o1("Hello world");
        PointerName o2("He");
        check(o1,o2);
    }

    BOOST_TEST_CONTEXT("shared pointer")
    {
        SharedPointerName o1("Hello world");
        SharedPointerName o2("He");
        check(o1,o2);
    }
}
#endif
BOOST_AUTO_TEST_CASE(CheckNullPointers)
{
    auto check=[](auto&& o1)
    {
        BOOST_CHECK(!check_exists(o1,hana::make_tuple(name_null,size)));
        BOOST_CHECK(!check_exists(o1,hana::make_tuple(name_null)));

        auto v0=validator(
                _[name_null](exists,true)
             );
        BOOST_CHECK(!v0.apply(o1));

        auto v1=validator(
                _[name_null][size](exists,true)
             );
        BOOST_CHECK(!v1.apply(o1));

        auto v2=validator(
                _[name_null](gte,"Hello")
             );
        auto v3=validator(
                _[name_null][size](gte,5)
             );

        auto a1=make_default_adapter(o1);
        a1.set_check_member_exists_before_validation(true);
        a1.set_unknown_member_mode(if_member_not_found::abort);
        BOOST_CHECK(!v2.apply(a1));
        BOOST_CHECK(!v3.apply(a1));
    };

    BOOST_TEST_CONTEXT("raw pointer")
    {
        PointerName o1("Hello world");
        check(o1);
    }

    BOOST_TEST_CONTEXT("shared pointer")
    {
        SharedPointerName o1("Hello world");
        check(o1);
    }
}

BOOST_AUTO_TEST_SUITE_END()
