#if 0
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

template <typename PropertyT,typename OpT, typename T>
constexpr auto validator(PropertyT property,OpT&& op, T&& b)
    -> decltype(auto)
{
    return hana::reverse_partial(property_impl,std::forward<T>(b),std::forward<PropertyT>(property),std::forward<OpT>(op));
}

constexpr auto and_impl=[](auto&& a, auto&& b)
{
    return a && b;
};
constexpr auto or_impl=[](auto&& a, auto&& b)
{
    return a || b;
};

constexpr auto AND=hana::infix([](auto&& ...xs)
{
    return hana::fold(hana::make_tuple(std::forward<decltype(xs)>(xs)...),and_impl);
});
constexpr auto OR=hana::infix([](auto&& ...xs)
{
    return hana::fold(hana::make_tuple(std::forward<decltype(xs)>(xs)...),or_impl);
});

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

    BOOST_CHECK(
                    vld::AND(
                        vld::property(vld::p_size,str,vld::gte,4),
                        vld::value(sz,vld::gte,20)
                    )
                );

    BOOST_CHECK(
                    vld::property(vld::p_size,str,vld::gte,4)
                    ^vld::AND^
                    vld::value(sz,vld::gte,20)
                    ^vld::AND^
                    vld::value(sz,vld::gte,99)
                );

    BOOST_CHECK(
                    vld::property(vld::p_size,str,vld::gte,500)
                    ^vld::OR^
                    vld::value(sz,vld::gte,20)
                    ^vld::OR^
                    vld::value(sz,vld::gte,1000)
                );

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

#if 0

    validator(
                AND_(
                    value(gte,10),
                    property(size,gte,100)
                    )
             );

    -> prepared(a) {

        AND(value_(a,gte,10),property_(a,size,gte,100))
    }

    aggregate_and(auto&& a,auto&& xs)
    {
        bool ok=true;
        int count=hana::while(i<hana::size(xs),0,
        [&](int i)
        {
            if (ok)
            {
                ok=xs[i](a);
            }
            return ++i;
        }
        return ok;
    }

    aggregate_or(auto&& a,auto&& xs)
    {
        bool ok=false;
        hana::while(i<hana::size(xs),0,
        [&](int i)
        {
            if (!ok)
            {
                ok=xs[i](a);
            }
            return ++i;
        }
        return ok;
    }

    prepare_end(auto&& ..xs)
    {
        return hana::reverse_partial(aggregate_and,hana::make_tuple(std::forward<decltype(xs)>(xs)...));
    }
    prepare_or(auto&& ..xs)
    {
        return hana::reverse_partial(aggregate_or,hana::make_tuple(std::forward<decltype(xs)>(xs)...));
    }

#endif

#if 0

    auto v0=validator(
        _[size](gte,10),
        _["field1"](value(gte,10) ^AND^ size(gte,100)),
        _["field100"](gte,fn0_100),
        _["field200"][10](value(gte,10) ^AND^ value(lte,100))
        _["container1"](element(all,value(ne,0)))
        _["field300"](ne,_["field100"][12])
        _["field400"](ne,_(other))
        _["field500"](eq,_(other)["field1"])
    );

    v0(object)==true;
    v0(adapter(object))==true;
    v0["field100"](value)==true;
    v0(adapter);

    err.set_formatter();
    v0(object,err)==true;
    v0(adapter(object),err)==true;
    v0["field100"](value,err)==true;

    _[size](gte,10) -> [](auto&& v){return validate(size,v,gte,10);}
    _["field100"](gte,fn0_100) -> [](auto&& v){return validate(v["field100"],gte,10);}

    _["field1"](value(gte,10) ^AND^ size(gte,100)) -> [](auto&& vv){return AND(value(gte,10),size(gte,100)(vv))(v["field1")];}

    _[](auto&& k) -> pre_op(k)
    _(other) -> struct other_wrap

    validate_invoker
    {
        HandlerT handler;

    }

    pre_op
    {
       operator () (auto&& op, auto&& val)
       {
           return [this,op,val](auto&& v){validate(*this(v),op,val)};
       }
        operator () (auto&& op, auto&& val, auto& err)
        {
            auto ok=[this,op,val](auto&& v){validate(*this(v),op,val)};
            if (!ok)
            {
                err.format(v,op,chain);
            }
            return ok;
        }

        <with aggregator>
       operator () (auto&& v, auto&& val)
       {
            return pre_validate_aggregator(*this(v),val);
       }

        <with property>
       operator () (auto&& v, auto&& property)
       {
            return pre_validate_property(*this(v),property);
       }

        <if T is property>
        operator () (auto&& v)
        {
             return property(v,k);
        }
        <if T is not property>
        operator () (auto&& v)
        {
            return v[k];
        }

        <if other is adapter>
        operator () (auto&& v, OpT op, OtherWrap other)
        {
            return pre_validate(unfold(v),op,other[k]);
        }

        <if other is adapter>
        operator () (auto&& v, OpT op, pre_op self_field)
        {
            return pre_validate(unfold(v),op,unfold(v)[self_field]);
        }

        operator [] (auto&& kn)
        {
            return pre_op(chain,kn);
        }

        unfold(auto&& v)
        {
            return unfold(chain(*this(k)));
        }

       T k;
       list<pre_op> chain;
    }

    struct other_wrap
    {
        operator [] (auto&& k)
        {
            return property(v,k);
        }
        operator [] (auto&& k)
        {
            return other[k];
        }

        T other;
    }

    auto v=validator(
        AND_(
            _["field1"](value(gte,10) ^AND^ size(gte,100)),
            _["field100"](gte,15),
            _["field200"](value(gte,10) ^AND^ value(lte,100)),
            _[id](AND_(
                   value(gte,10),
                   property(size,gte,100)
                   )),
            _["field2"][id](AND_(
                   value(gte,10),
                   property(size,gte,100)
                   )),
            _["field2"][id](AND_(
                   value(gte,10),
                   property(size,gte,at("field10"))
                   )),
            at("field3")(AND_(
                   value(gte,10),
                   property(size,gte,at(other,"field10"))
                   )),
            at("field5").all_elements()(AND_(
                   value(gte,10),
                   property(size,gte,at(other,"field10"))
                   ))
            at("field6").element(all)(AND_(
                   value(gte,10),
                   property(size,gte,at(other,"field10"))
                   ))
            at("field6").element(any)(AND_(
                   value(gte,10),
                   property(size,gte,at(other,"field10"))
                   ))
            at("field6").element(index,10)(AND_(
                   value(gte,10),
                   property(size,gte,at(other,"field10"))
                   ))
            at("field6").element(range,range_gen)(AND_(
                   value(gte,10),
                   property(size,gte,at(other,"field10"))
                   ))
        )
    );

    struct adapter_a
    {
        operator () (op, auto b)
        {
            print("{} op {}",this->context,this->extract(b));
        }
    };

    auto op(auto a,auto b)
    {
        if (is_adapter(a))
        {
            return a(op,b);
        }
        else
        {
            return _op_(a,b);
        }
    }


    v(object);

    dispatch_at(object,field)
    {
        if (is_adapter(object))
        {
            context=concat(object.context,field);
            return adapter(object,context);
        }
        else
        {
            return object[field];
        }
    }

    apply_at(object,at(field1,field2,field3)) -> a=dispatch_at(dispatch_at(dispatch_at(object,field1),field2),field3))
    apply(a,and(value(op1,val1),property(prop2,op2,val2))) ->  apply_value(a,op1,val1) && apply_property(a,prop2,op2,val2)
            -> op1(a,val1) && op2(prop2(a),val2)
#endif
}

BOOST_AUTO_TEST_SUITE_END()
#endif
