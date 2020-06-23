#include <boost/test/unit_test.hpp>

#include <boost/hana/functional.hpp>
#include <boost/hana/lazy.hpp>
#include <boost/hana/greater_equal.hpp>
#include <boost/hana/core/when.hpp>
#include <boost/hana.hpp>

namespace hana = boost::hana;

namespace cppvalidator
{

struct invokable_tag;
template <typename T>
struct invokable_t
{
    using hana_tag=invokable_tag;
    T fn;

    auto operator()() const -> decltype(fn())
    {
        return fn();
    }
};

template <typename T>
auto invokable(T&& fn) -> invokable_t<T>
{
    return invokable_t<T>{std::forward<T>(fn)};
}

template <typename T>
auto val(T&& v)
{
  return hana::if_(hana::is_a<invokable_tag,T>,
    [](auto&& x) { return x(); },
    [](auto&& x) { return hana::id(std::forward<decltype(x)>(x)); }
  )(std::forward<T>(v));
}

struct p_value_t
{
    template <typename T>
    constexpr static T get(T&& v)
    {
        return hana::id(std::forward<T>(v));
    }
};
constexpr p_value_t p_value{};

constexpr auto has_size = hana::is_valid([](auto&& v) -> decltype((void)v.size()){});
auto try_get_size =[](auto&& v)
{
  return hana::if_(has_size(v),
    [](auto&& x) { return x.size(); },
    [](auto&& x) { return hana::id(std::forward<decltype(x)>(x)); }
  )(std::forward<decltype(v)>(v));
};
struct p_size_t
{
    template <typename T>
    static auto get(T&& v) -> decltype(auto)
    {
        return try_get_size(std::forward<T>(v));
    }
};
constexpr p_size_t p_size{};


template <typename ValT, typename PropertyT=p_value_t>
auto get_property(ValT&& v) -> decltype(auto)
{
    return PropertyT::get(v);
}

auto gte = [](const auto& a, const auto& b)
{
    return a>=b;
};

auto value_impl = [](auto a, auto b, auto op)
{
    return op(val(a),val(b));
};

auto value = [](auto a, auto op, auto b)
{
    return op(val(a),val(b));
};

auto value_validator = [](auto op, auto b) {
    return hana::reverse_partial(value_impl,b,op);
};

auto property_impl = [](auto a, auto b, auto property, auto op)
{
    return op(get_property<decltype(val(a)),decltype(property)>(val(a)),
              get_property<decltype(val(b)),decltype(property)>(val(b)));
};

auto property = [](auto property, auto a, auto op, auto b)
{
    return property_impl(a,b,property,op);
};

auto property_validator = [](auto property,auto op, auto b)
{
    return hana::reverse_partial(property_impl,b,property,op);
};

struct container_accessors
{
    // T next();
    // T at();
};

struct all_elements_t
{
};

template <typename OpT, typename BT, typename PropertyT=p_value_t>
constexpr auto validator(PropertyT property,OpT&& op, BT&& b)
    -> decltype(auto)
{
    return hana::reverse_partial(property_impl,std::forward<BT>(b),std::forward<PropertyT>(property),std::forward<OpT>(op));
}

}

namespace vld=cppvalidator;

BOOST_AUTO_TEST_SUITE(TestCppValidator)

BOOST_AUTO_TEST_CASE(CheckScalarValue)
{
    BOOST_CHECK(!vld::property(vld::p_value,10,vld::gte,20));
    std::string str("hsdfha;");
    BOOST_CHECK(vld::property(vld::p_size,str,vld::gte,3));
    size_t sz=100;
    BOOST_CHECK(vld::value(sz,vld::gte,20));

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
                    vld::p_value,
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
                    vld::p_size,
                    vld::gte,
                    7
                );
    BOOST_CHECK(v3(hello));

    auto v4=vld::property_validator(
                    vld::p_value,
                    vld::gte,
                    vld::invokable(
                        []()
                        {
                            return std::string("Hi");
                        }
                    )
                );
    BOOST_CHECK(!v4(hello));
    auto samplestr=std::string("How are you?");
    BOOST_CHECK(v4(
                    vld::invokable(
                        [&samplestr]()
                        {
                            return samplestr;
                        }
                    )
                ));

    count=20;
    auto v5=vld::validator(
                    vld::p_size,
                    vld::gte,
                    vld::invokable(
                        [&count]()
                        {
                            return count;
                        }
                    )
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
                    vld::p_value,
                    vld::gte,
                    7
                );
    BOOST_CHECK(v6(50));
}

BOOST_AUTO_TEST_SUITE_END()
