#include <string>
#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>
#include <dracosha/validator/reporting/reporter_with_object_name.hpp>
#include <dracosha/validator/reporting/extend_translator.hpp>

#include <dracosha/validator/reporting/locale/ru.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestTranslatorRu)

BOOST_AUTO_TEST_CASE(CheckTranslatorRu)
{
    auto sys_tr=validator_translator_ru();

    BOOST_CHECK_EQUAL(sys_tr(string_true).text(),"истина");
    BOOST_CHECK_EQUAL(sys_tr(string_false).text(),"ложь");
    BOOST_CHECK_EQUAL(sys_tr(string_master_sample).text(),"образец");
    BOOST_CHECK_EQUAL(sys_tr(string_conjunction_of).text(),"");
    BOOST_CHECK_EQUAL(sys_tr(string_member_name_conjunction).text()," ");
    BOOST_CHECK_EQUAL(sys_tr(string_element).text(),"элемент #");
    BOOST_CHECK_EQUAL(sys_tr(string_any).text(),"хотя бы один элемент");
    BOOST_CHECK_EQUAL(sys_tr(string_all).text(),"каждый элемент");

    BOOST_CHECK_EQUAL(sys_tr(string_empty).text(),"должен быть пустым");
    BOOST_CHECK_EQUAL(sys_tr(string_not_empty).text(),"должен быть не пустым");
    BOOST_CHECK_EQUAL(sys_tr(string_empty,grammar_categories_bitmask(grammar_ru::zhensky_rod)).text(),"должна быть пустой");
    BOOST_CHECK_EQUAL(sys_tr(string_empty,grammar_categories_bitmask(grammar_ru::sredny_rod)).text(),"должно быть пустым");
    BOOST_CHECK_EQUAL(sys_tr(string_empty,grammar_categories_bitmask(grammar_ru::mn_chislo)).text(),"должны быть пустыми");
    BOOST_CHECK_EQUAL(sys_tr(string_not_empty,grammar_categories_bitmask(grammar_ru::zhensky_rod)).text(),"должна быть не пустой");
    BOOST_CHECK_EQUAL(sys_tr(string_not_empty,grammar_categories_bitmask(grammar_ru::sredny_rod)).text(),"должно быть не пустым");
    BOOST_CHECK_EQUAL(sys_tr(string_not_empty,grammar_categories_bitmask(grammar_ru::mn_chislo)).text(),"должны быть не пустыми");
}

BOOST_AUTO_TEST_CASE(CheckOfRu)
{
    phrase_translator m;
    m[string_member_name_conjunction]={{{" ",grammar_ru::roditelny_padezh}}}; // " of "
    auto sep=m(string_member_name_conjunction);
    BOOST_CHECK(is_grammar_category_set(sep.grammar_cats(),grammar_ru::roditelny_padezh));
}

BOOST_AUTO_TEST_CASE(CheckValidatorRu)
{
    std::string rep;
    std::map<std::string,std::string> m1={
        {"password","123456"},
        {"hyperlink","zzzzzzzzz"}
    };

    std::map<std::string,
            std::map<std::string,std::map<std::string,std::string>>
    > m2={
        {
            "planet",
            {
                {
                    "lake",{
                                    {"bank","123456"},
                                    {"water","wet"},
                                    {"hyperlink","zzzzzzzzz"}
                                }
                }
            }
        }
    };

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
    tr["planet"]={
                        {{"планета",grammar_ru::zhensky_rod}},
                        {{"планеты",grammar_ru::zhensky_rod},grammar_ru::roditelny_padezh}
                    };
    tr["lake"]={
                        {{"озеро",grammar_ru::sredny_rod}},
                        {{"озера",grammar_ru::sredny_rod},grammar_ru::roditelny_padezh}
                    };
    tr["bank"]={
                        {"берег"},
                        {"берега",grammar_ru::roditelny_padezh}
                   };
    tr["water"]={
                        {{"вода",grammar_ru::zhensky_rod}},
                        {{"воды",grammar_ru::zhensky_rod},grammar_ru::roditelny_padezh}
                    };

    auto tr1=extend_translator(validator_translator_ru(),tr);
    auto ra1=make_reporting_adapter(m1,make_reporter(rep,make_formatter(tr1)));

    auto v1=validator(
        _["words"](exists,true)
     );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"слова должны существовать");
    rep.clear();

    auto v2=validator(
        _["hyperlink"](eq,"https://www.boost.org")
     );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"гиперссылка должна быть равна https://www.boost.org");
    rep.clear();

    auto v3=validator(
        _["password"](length(gt,7))
     );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"длина пароля должна быть больше 7");
    rep.clear();

    auto v4=validator(
        _["hyperlink"](length(lte,7))
     );
    BOOST_CHECK(!v4.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"длина гиперссылки должна быть меньше или равна 7");
    rep.clear();

    auto ra2=make_reporting_adapter(m2,make_reporter(rep,make_formatter(tr1)));

    auto v5=validator(
        _["planet"]["lake"]["bank"](eq,"пологий")
     );
    BOOST_CHECK(!v5.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"берег озера планеты должен быть равен пологий");
    rep.clear();

    auto v6=validator(
        _["planet"]["lake"]["bank"](length(gt,7))
     );
    BOOST_CHECK(!v6.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"длина берега озера планеты должна быть больше 7");
    rep.clear();

    auto v7=validator(
        _["planet"]["lake"]["bank"][size](gt,7)
     );
    BOOST_CHECK(!v7.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"размер берега озера планеты должен быть больше 7");
    rep.clear();

    auto v8=validator(
        _["planet"]["lake"]["bank"](eq,"желтый песок")
     );
    BOOST_CHECK(!v8.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"берег озера планеты должен быть равен желтый песок");
    rep.clear();

    auto v9=validator(
        _["planet"]["lake"]["water"](eq,"чистая")
     );
    BOOST_CHECK(!v9.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"вода озера планеты должна быть равна чистая");
    rep.clear();

    auto v10=validator(
        _["planet"]["lake"]["bank"][size](gt,_(m2))
     );
    BOOST_CHECK(!v10.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"размер берега озера планеты должен быть больше размера берега озера планеты  образца");
    rep.clear();

    auto v11=validator(
        _["planet"]["lake"]["bank"](size(lt,_["planet"]["lake"]["water"]))
     );
    BOOST_CHECK(!v11.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"размер берега озера планеты должен быть меньше размера воды озера планеты");
    rep.clear();

    auto v12=validator(
        _["planet"]["lake"]["bank"](gt,_["planet"]["lake"]["water"])
     );
    BOOST_CHECK(!v12.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"берег озера планеты должен быть больше воды озера планеты");
    rep.clear();

    auto v13=validator(
        _["planet"]["lake"]["bank"](length(lt,_["planet"]["lake"]["water"]))
     );
    BOOST_CHECK(!v13.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"длина берега озера планеты должна быть меньше длины воды озера планеты");
    rep.clear();

    auto v14=validator(
        _["planet"]["lake"]["bank"](size(lt,_["planet"]))
     );
    BOOST_CHECK(!v14.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"размер берега озера планеты должен быть меньше размера планеты");
    rep.clear();
}

BOOST_AUTO_TEST_CASE(CheckValidatorWithObjectNameRu)
{
    std::string rep;
    std::map<std::string,std::string> m1={
        {"password","123456"},
        {"hyperlink","zzzzzzzzz"}
    };

    phrase_translator tr;
    tr["password"]={
                        {"пароль"},
                        {"пароля",grammar_ru::roditelny_padezh}
                   };
    tr["hyperlink"]={
                        {{"гиперссылка",grammar_ru::zhensky_rod}},
                        {{"гиперссылки",grammar_ru::zhensky_rod},grammar_ru::roditelny_padezh}
                    };
    tr["service"]={
                        {{"служба",grammar_ru::zhensky_rod}},
                        {{"службы",grammar_ru::zhensky_rod},grammar_ru::roditelny_padezh}
                    };

    auto tr1=extend_translator(validator_translator_ru(),tr);
    auto ra1=make_reporting_adapter(m1,make_reporter_with_object_name(rep,make_formatter(tr1),"service"));

    auto v1=validator(
        value(eq,"apache")
     );
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"служба должна быть равна apache");
    rep.clear();

    auto v2=validator(
        size(gt,10)
     );
    BOOST_CHECK(!v2.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"размер службы должен быть больше 10");
    rep.clear();

    auto v3=validator(
        _["password"](size(gt,10))
     );
    BOOST_CHECK(!v3.apply(ra1));
    BOOST_CHECK_EQUAL(rep,"размер пароля службы должен быть больше 10");
    rep.clear();

    bool f=false;
    auto ra2=make_reporting_adapter(f,make_reporter_with_object_name(rep,make_formatter(tr1),"service"));
    auto v4=validator(
        value(flag(flag_enable_disable),true)
     );
    BOOST_CHECK(!v4.apply(ra2));
    BOOST_CHECK_EQUAL(rep,"служба должна быть активна");
    rep.clear();
}

BOOST_AUTO_TEST_SUITE_END()
