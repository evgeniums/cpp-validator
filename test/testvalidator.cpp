#include <boost/test/unit_test.hpp>

#include <boost/hana/functional.hpp>
#include <boost/hana/lazy.hpp>
#include <boost/hana/greater_equal.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana/traits.hpp>

namespace hana = boost::hana;

namespace cppvalidator
{

struct Invokable;
template <typename T>
struct invokable_t
{
    using hana_tag=Invokable;
    T handler;

    invokable_t(T handler):handler(std::move(handler))
    {}
    auto operator()() const
                -> decltype(handler())
    {
        return handler();
    }
};

template <typename T>
constexpr auto invokable(T fn) -> invokable_t<T>
{
    return invokable_t<T>(std::move(fn));
}

struct val_t
{
    template <typename T>
    constexpr auto operator()(const T& t,
                                typename std::enable_if<
                                    std::is_same<
                                        typename hana::tag_of<T>::type,
                                        Invokable
                                    >::value
                                ,void*>::type =nullptr) const
        -> decltype(t())
    {
        return t();
    }

    template <typename T>
    constexpr auto operator()(T&& t,
                              typename std::enable_if<
                                  !std::is_same<
                                      typename hana::tag_of<T>::type,
                                      Invokable
                                  >::value
                              ,void*>::type =nullptr) const
        -> T
    {
        return hana::id(t);
    }
};
constexpr val_t val{};

struct property_value_t
{
    template <typename T>
    constexpr static T get(T&& v)
    {
        return hana::id(std::forward<T>(v));
    }
};

auto has_size = hana::is_valid([](auto t) -> decltype(
  hana::traits::declval(t).size()
) { });
struct property_size_t
{
    template <typename T,
              typename std::enable_if<has_size(hana::type_c<T>),void*>::type = nullptr
              >
    constexpr static size_t get(const T& v)
    {
        return v.size();
    }

    template <typename T,
              typename std::enable_if<!has_size(hana::type_c<T>),void*>::type = nullptr
              >
    constexpr static T get(T&& v)
    {
        return hana::id(v);
    }
};

constexpr property_value_t property_value{};
constexpr property_size_t property_size{};


template <typename ValT, typename PropertyT=property_value_t>
BOOST_HANA_CONSTEXPR_LAMBDA auto get_property(ValT&& v) -> decltype(PropertyT::get(v))
{
    return PropertyT::get(v);
}

BOOST_HANA_CONSTEXPR_LAMBDA auto gte = [](auto a, auto b)
{
    return a>=b;
};

BOOST_HANA_CONSTEXPR_LAMBDA auto apply = [](auto a, auto b, auto op)
{
    return op(val(a),val(b));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto value_validator = [](auto op, auto b) {
    return hana::reverse_partial(apply,b,op);
};

BOOST_HANA_CONSTEXPR_LAMBDA auto apply_property = [](auto a, auto b, auto property, auto op)
{
    return op(get_property<decltype(val(a)),decltype(property)>(val(a)),
              get_property<decltype(val(b)),decltype(property)>(val(b)));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto property_validator = [](auto op, auto b, auto property)
{
    return hana::reverse_partial(apply_property,b,property,op);
};

//struct all_elements_t
//{
//};
//struct any_element_t
//{
//};
//template <typename T>
//struct element_range_t
//{
//    T begin;
//    T end;
//};
//template <typename T>
//struct element_at_t
//{
//    T index;
//};

//BOOST_HANA_CONSTEXPR_LAMBDA auto container_validator = [](auto op, auto b, auto elements, auto property)
//{
//    return hana::reverse_partial(apply_container,b,property,op);
//};

template <typename OpT, typename BT, typename PropertyT=property_value_t>
constexpr auto validator(OpT&& op, BT&& b, PropertyT property=PropertyT())
    -> decltype(hana::reverse_partial(apply_property,std::forward<BT>(b),std::forward<PropertyT>(property),std::forward<OpT>(op)))
{
    return hana::reverse_partial(apply_property,std::forward<BT>(b),std::forward<PropertyT>(property),std::forward<OpT>(op));
}

}

namespace vld=cppvalidator;

BOOST_AUTO_TEST_SUITE(TestCppValidator)

BOOST_AUTO_TEST_CASE(CheckScalarValue)
{
    auto v1=vld::value_validator(
                    vld::gte,
                    5
                );
    uint32_t val1=5;
    int64_t val2=-1;
    int16_t val3=100;

    BOOST_CHECK(v1(val1));
    BOOST_CHECK(!v1(val2));
    BOOST_CHECK(v1(val3));
    BOOST_CHECK(v1(
            vld::invokable(
                [](){return 30;}
            )
        ));

    int count=0;
    auto v2=vld::validator(
                    vld::gte,
                    vld::invokable(
                        [&count](){return count;}
                    )
                );
    auto fn2=vld::invokable(
                [&count](){return count+5;}
            );

    BOOST_CHECK(v2(count));
    ++count;
    BOOST_CHECK(v2(count));
    ++count;
    BOOST_CHECK(v2(count));
    ++count;
    BOOST_CHECK(v2(fn2));

    std::string hello("Hello world");
    auto v3=vld::validator(
                    vld::gte,
                    7,
                    vld::property_size
                );
    BOOST_CHECK(v3(hello));

    auto v4=vld::property_validator(
                    vld::gte,
                    vld::invokable(
                        []()
                        {
                            return std::string("Hi");
                        }
                    ),
                    vld::property_value
                );
    BOOST_CHECK(!v4(hello));
    BOOST_CHECK(v4(
                    vld::invokable(
                        []()
                        {
                            return std::string("How are you?");
                        }
                    )
                ));

    count=20;
    auto v5=vld::validator(
                    vld::gte,
                    vld::invokable(
                        [&count]()
                        {
                            return count;
                        }
                    ),
                    vld::property_size
                );
    BOOST_CHECK(!v5(hello));
    BOOST_CHECK(v5(
                    vld::invokable(
                        []()
                        {
                            return std::string("aaaaaaaaaaaaaaaaaaaa");
                        }
                    )
                ));
    count=30;
    BOOST_CHECK(!v5(
                    vld::invokable(
                        []()
                        {
                            return std::string("aaaaaaaaaaaaaaaaaaaa");
                        }
                    )
                ));

    auto v6=vld::validator(
                    vld::gte,
                    7
                );
    BOOST_CHECK(v6(50));
}

BOOST_AUTO_TEST_SUITE_END()
