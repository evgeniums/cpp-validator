#ifdef BUILD_HABR_EXAMPLES

#include <iostream>
#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/validate.hpp>
#include <dracosha/validator/prevalidation/set_validated.hpp>
#include <dracosha/validator/operators/lexicographical.hpp>
#include <dracosha/validator/reporting/extend_translator.hpp>
#include <dracosha/validator/reporting/locale/ru.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// структура с переменными и методом вида setter
struct Foo
{
    std::string bar_value;

    uint32_t other_value;
    size_t some_size;

    void set_bar_value(std::string val)
    {
        bar_value=std::move(val);
    }
};

// зарегистрировать кастомные свойства
DRACOSHA_VALIDATOR_PROPERTY(bar_value);
DRACOSHA_VALIDATOR_PROPERTY(other_value);

// специализация шаблона класса set_member_t для записи свойства bar_value структуры Foo
DRACOSHA_VALIDATOR_NAMESPACE_BEGIN

template <>
struct set_member_t<Foo,DRACOSHA_VALIDATOR_PROPERTY_TYPE(bar_value)>
{
    template <typename ObjectT, typename MemberT, typename ValueT>
    void operator() (
            ObjectT& obj,
            MemberT&&,
            ValueT&& val
        ) const
    {
        obj.set_bar_value(std::forward<ValueT>(val));
    }
};

DRACOSHA_VALIDATOR_NAMESPACE_END

BOOST_AUTO_TEST_SUITE(TestHabrExamples)

BOOST_AUTO_TEST_CASE(CheckHabrexample1)
{
    // определение валидатора
    auto container_validator=validator(
       _[size](eq,1), // размер контейнера должен быть равен 1
       _["field1"](exists,true), // поле "field1" должно существовать в контейнере
       _["field1"](ne,"undefined") // поле "field1" должно быть не равно "undefined"
    );

    // успешная валидация
    std::map<std::string,std::string> map1={{"field1","value1"}};
    validate(map1,container_validator);

    // неуспешная валидация, с объектом ошибки
    error_report err;
    std::map<std::string,std::string> map2={{"field2","value2"}};
    validate(map2,container_validator,err);
    if (err)
    {
        std::cerr<<err.message()<<std::endl;
        /* напечатает:
        field1 must exist
        */
    }

    // неуспешная валидация, с исключением
    try
    {
        std::map<std::string,std::string> map3={{"field1","undefined"}};
        validate(map3,container_validator);
    }
    catch(const validation_error& ex)
    {
        std::cerr<<ex.what()<<std::endl;
        /* напечатает:
        field1 must be not equal to undefined
        */
    }
}

BOOST_AUTO_TEST_CASE(CheckHabrexample2)
{
    // определение валидатора
    auto v=validator(gt,100); // больше чем 100

    // объект ошибки
    error err;

    // условия не выполнены
    validate(90,v,err);
    if (err)
    {
      // валидация неуспешна
    }

    // условия выполнены
    validate(200,v,err);
    if (!err)
    {
      // валидация успешна
    }

    try
    {
        validate(200,v); // успешно
        validate(90,v); // генерирует исключение
    }
    catch (const validation_error& err)
    {
        std::cerr << err.what() << std::endl;
        /* напечатает:
        must be greater than 100
        */
    }

    int value1=90;
    if (!v.apply(value1))
    {
      // валидация неуспешна
    }

    int value2=200;
    if (v.apply(value2))
    {
      // валидация успешна
    }
}

BOOST_AUTO_TEST_CASE(CheckHabrExample3)
{
    // валидатор: размер меньше 15 и значение бинарно больше или равно "sample string"
    auto v=validator(
      length(lt,15),
      value(gte,"sample string")
    );

    // явное применение валидатора к переменным

    std::string str1="sample";
    if (!v.apply(str1))
    {
      // валидация неупешна потому что sample бинарно меньше, чем sample string
    }

    std::string str2="sample string+";
    if (v.apply(str2))
    {
      // валидация успешна
    }

    std::string str3="too long sample string";
    if (!v.apply(str3))
    {
      // валидация неуспешна, потому что длина строки больше 15 символов
    }
}

BOOST_AUTO_TEST_CASE(CheckHabrexample4)
{
    // валидатор: входит в интервал [95,100]
    auto v=validator(in,interval(95,100));

    // объект ошибки
    error_report err;

    // проверить значение
    size_t val=90;
    validate(val,v,err);
    if (err)
    {
        std::cerr << err.message() << std::endl;
        /* напечатает:
        must be in interval [95,100]
        */
    }
}

BOOST_AUTO_TEST_CASE(CheckHabrexample5)
{
    // составной валидатор
    auto v=validator(
                    _["field1"](gte,"xxxxxx")
                     ^OR^
                    _["field1"](size(gte,100) ^OR^ value(gte,"zzzzzzzzzzzz"))
                );

    // валидация контейнера и печать ошибки

    error_report err;
    std::map<std::string,std::string> test_map={{"field1","value1"}};
    validate(test_map,v,err);
    if (err)
    {
        std::cerr << err.message() << std::endl;
        /* напечатает:
        field1 must be greater than or equal to xxxxxx OR size of field1 must be greater than or equal to 100 OR field1 must be greater than or equal to zzzzzzzzzzzz
        */
    }
}

BOOST_AUTO_TEST_CASE(CheckHabrexample6)
{
    // составной валидатор элементов вложенных контейнеров
    auto v=validator(
                    _["field1"][1](in,range({10,20,30,40,50})),
                    _["field1"][2](lt,100),
                    _["field2"](exists,false),
                    _["field3"](empty(flag,true))
                );

    // валидация вложенного контейнера и печать ошибки
    error_report err;
    std::map<std::string,std::map<size_t,size_t>> nested_map={
                {"field1",{{1,5},{2,50}}},
                {"field3",{}}
            };
    validate(nested_map,v,err);
    if (err)
    {
        std::cerr << err.message() << std::endl;
        /* напечатает:
        element #1 of field1 must be in range [10, 20, 30, 40, 50]
        */
    }
}

// зарегистрировать новое свойство red_color
DRACOSHA_VALIDATOR_PROPERTY_FLAG(red_color,"Must be red","Must be not red");

BOOST_AUTO_TEST_CASE(CheckHabrexample7)
{
    // структура с getter методом
    struct Foo
    {
        bool red_color() const
        {
            return true;
        }
    };

    // валидатор зарегистрированного свойства red_color
    auto v=validator(
        _[red_color](flag,false)
    );

    // провести валидацию кастомного свойства и напечатать ошибку

    error_report err;
    Foo foo_instance;
    validate(foo_instance,v,err);
    if (err)
    {
        std::cerr << err.message() << std::endl;
        /* напечатает:
        "Must be not red"
        */
    }
}

BOOST_AUTO_TEST_CASE(CheckHabrexample8)
{
    // валидатор с кастомными свойствами
    auto v=validator(
        _[bar_value](ilex_ne,"UNKNOWN"), // лексикографическое "не равно" без учета регистра
        _[other_value](gte,1000) // больше или равно 1000
    );

    Foo foo_instance;
    error_report err;

    // запись валидного значение в свойство bar_value объекта foo_instance
    set_validated(foo_instance,_[bar_value],"Hello world",v,err);
    if (!err)
    {
        // свойство bar_value объекта foo_instance успешно записано
    }

    // попытка записи невалидного значение в свойство bar_value объекта foo_instance
    set_validated(foo_instance,_[bar_value],"unknown",v,err);
    if (err)
    {
        // запись не удалась
        std::cerr << err.message() << std::endl;
        /* напечатает:
         bar_value must be not equal to UNKNOWN
         */
    }
}

BOOST_AUTO_TEST_CASE(CheckHabrexample9)
{
    // переводчик ключей контейнера на русский язык с учетом рода, падежа и числа
    phrase_translator tr;
    tr["password"]={
                        {"пароль"},
                        {"пароля",grammar_ru::roditelny_padezh}
                   };
    tr["hyperlink"]={
                        {{"гиперссылка",grammar_ru::zhensky_rod}},
                        {{"гиперссылки",grammar_ru::zhensky_rod},grammar_ru::roditelny_padezh}
                    };
    tr["words"]={
                    {{"слова",grammar_ru::mn_chislo}}
                };

    /*
    финальный переводчик включает в себя встроенный переводчик на русский
    validator_translator_ru() и переводчик tr для имен элементов
    */
    auto tr1=extend_translator(validator_translator_ru(),tr);

    // контейнер для валидации
    std::map<std::string,std::string> m1={
        {"password","123456"},
        {"hyperlink","zzzzzzzzz"}
    };

    // адаптер с генерацией отчета об ошибке на русском языке
    std::string rep;
    auto ra1=make_reporting_adapter(m1,make_reporter(rep,make_formatter(tr1)));

    // различные валидаторы и печать ошибок на русском языке

    auto v1=validator(
        _["words"](exists,true)
     );
    if (!v1.apply(ra1))
    {
        std::cerr<<rep<<std::endl;
        /*
        напечатает:
        слова должны существовать
        */
    }
    rep.clear();

    auto v2=validator(
        _["hyperlink"](eq,"https://www.boost.org")
     );
    if (!v2.apply(ra1))
    {
        std::cerr<<rep<<std::endl;
        /*
        напечатает:
        гиперссылка должна быть равна https://www.boost.org
        */
    }
    rep.clear();

    auto v3=validator(
        _["password"](length(gt,7))
     );
    if (!v3.apply(ra1))
    {
        std::cerr<<rep<<std::endl;
        /*
        напечатает:
        длина пароля должна быть больше 7
        */
    }
    rep.clear();

    auto v4=validator(
        _["hyperlink"](length(lte,7))
     );
    if (!v4.apply(ra1))
    {
        std::cerr<<rep<<std::endl;
        /*
        напечатает:
        длина гиперссылки должна быть меньше или равна 7
        */
    }
    rep.clear();
}

BOOST_AUTO_TEST_SUITE_END()

#endif
