#include <boost/test/unit_test.hpp>

#include <dracosha/validator/utils/invoke_or.hpp>
#include <dracosha/validator/utils/invoke_and.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestInvokeAndOr)

namespace {

struct Result
{
    enum Code
    {
        OK=0,
        FAIL=1
    };

    Code code;

    operator bool() const noexcept
    {
        return code==OK;
    }
};

struct ResultInverse
{
    enum Code
    {
        OK=0,
        FAIL=1
    };

    Code code;

    operator bool() const noexcept
    {
        return code==FAIL;
    }
};

Result funcSuccess(size_t& count, bool& status)
{
    status=true;
    ++count;
    return Result{Result::Code::OK};
}

Result funcFail(size_t& count, bool& status)
{
    std::ignore=count;
    status=false;
    return Result{Result::Code::FAIL};
}

ResultInverse funcSuccessInverse(size_t& count, bool& status)
{
    status=true;
    ++count;
    return ResultInverse{ResultInverse::Code::OK};
}

ResultInverse funcFailInverse(size_t& count, bool& status)
{
    std::ignore=count;
    status=false;
    return ResultInverse{ResultInverse::Code::FAIL};
}

}

BOOST_AUTO_TEST_CASE(TestInvokeAnd)
{
    size_t count=0;
    bool status=true;

    auto success=[&count,&status]()
    {
        status=true;
        ++count;
        return true;
    };
    auto fail=[&status]()
    {
        status=false;
        return false;
    };

    status=false;
    auto andSuccess1=invoke_and(success,success,success);
    BOOST_CHECK(andSuccess1);
    BOOST_CHECK_EQUAL(count,3);
    BOOST_CHECK(status);

    count=0;
    auto andFail1=invoke_and(success,success,fail,success,success);
    BOOST_CHECK(!andFail1);
    BOOST_CHECK_EQUAL(count,2);
    BOOST_CHECK(!status);

    count=0;
    auto andSuccess2=invoke_and(
                    DCS_FN(funcSuccess(count,status)),
                    DCS_FN(funcSuccess(count,status)),
                    DCS_FN(funcSuccess(count,status))
                );
    static_assert(std::is_same<decltype(andSuccess2),Result>::value,"Invalid return type");
    BOOST_CHECK(andSuccess2);
    BOOST_CHECK_EQUAL(count,3);
    BOOST_CHECK(status);

    count=0;
    auto andFail2=invoke_and(
                    DCS_FN(funcSuccess(count,status)),
                    DCS_FN(funcSuccess(count,status)),
                    DCS_FN(funcFail(count,status)),
                    DCS_FN(funcSuccess(count,status)),
                    DCS_FN(funcSuccess(count,status))
                );
    BOOST_CHECK(!andFail2);
    BOOST_CHECK_EQUAL(count,2);
    BOOST_CHECK(!status);
}

BOOST_AUTO_TEST_CASE(TestInvokeAndInverse)
{
    size_t count=0;
    bool status=true;

    auto success=[&count,&status]()
    {
        status=true;
        ++count;
        return false;
    };
    auto fail=[&status]()
    {
        status=false;
        return true;
    };

    status=false;
    auto andSuccess1=invoke_and_inverse(success,success,success);
    BOOST_CHECK(!andSuccess1);
    BOOST_CHECK_EQUAL(count,3);
    BOOST_CHECK(status);

    count=0;
    auto andFail1=invoke_and_inverse(success,success,fail,success,success);
    BOOST_CHECK(andFail1);
    BOOST_CHECK_EQUAL(count,2);
    BOOST_CHECK(!status);

    count=0;
    auto andSuccess2=invoke_and_inverse(
                    DCS_FN(funcSuccessInverse(count,status)),
                    DCS_FN(funcSuccessInverse(count,status)),
                    DCS_FN(funcSuccessInverse(count,status))
                );
    static_assert(std::is_same<decltype(andSuccess2),ResultInverse>::value,"Invalid return type");
    BOOST_CHECK(!andSuccess2);
    BOOST_CHECK_EQUAL(count,3);
    BOOST_CHECK(status);

    count=0;
    auto andFail2=invoke_and_inverse(
                    DCS_FN(funcSuccessInverse(count,status)),
                    DCS_FN(funcSuccessInverse(count,status)),
                    DCS_FN(funcFailInverse(count,status)),
                    DCS_FN(funcSuccessInverse(count,status)),
                    DCS_FN(funcSuccessInverse(count,status))
                );
    BOOST_CHECK(andFail2);
    BOOST_CHECK_EQUAL(count,2);
    BOOST_CHECK(!status);
}

BOOST_AUTO_TEST_CASE(TestInvokeOr)
{
    size_t count=0;
    size_t failCount=0;
    bool status=true;

    auto success=[&count,&status]()
    {
        status=true;
        ++count;
        return true;
    };
    auto fail=[&status,&failCount]()
    {
        ++failCount;
        status=false;
        return false;
    };

    status=false;
    auto orSuccess1=invoke_or(fail,fail,success,success,success,fail);
    BOOST_CHECK(orSuccess1);
    BOOST_CHECK_EQUAL(count,1);
    BOOST_CHECK_EQUAL(failCount,2);
    BOOST_CHECK(status);

    count=0;
    failCount=0;
    auto orFail1=invoke_or(fail,fail,fail);
    BOOST_CHECK(!orFail1);
    BOOST_CHECK_EQUAL(count,0);
    BOOST_CHECK_EQUAL(failCount,3);
    BOOST_CHECK(!status);

    count=0;
    failCount=0;
    auto orSuccess2=invoke_or(
                    DCS_FN(funcFail(count,status)),
                    DCS_FN(funcFail(count,status)),
                    DCS_FN(funcSuccess(count,status)),
                    DCS_FN(funcSuccess(count,status)),
                    DCS_FN(funcSuccess(count,status)),
                    DCS_FN(funcFail(count,status))
                );
    static_assert(std::is_same<decltype(orSuccess2),Result>::value,"Invalid return type");
    BOOST_CHECK(orSuccess2);
    BOOST_CHECK_EQUAL(count,1);
    BOOST_CHECK(status);

    count=0;
    failCount=0;
    auto orFail2=invoke_or(
                    DCS_FN(funcFail(count,status)),
                    DCS_FN(funcFail(count,status)),
                    DCS_FN(funcFail(count,status))
                );
    BOOST_CHECK(!orFail2);
    BOOST_CHECK_EQUAL(count,0);
    BOOST_CHECK(!status);
}

BOOST_AUTO_TEST_CASE(TestInvokeOrInverse)
{
    size_t count=0;
    size_t failCount=0;
    bool status=true;

    auto success=[&count,&status]()
    {
        status=true;
        ++count;
        return false;
    };
    auto fail=[&status,&failCount]()
    {
        ++failCount;
        status=false;
        return true;
    };

    status=false;
    auto orSuccess1=invoke_or_inverse(fail,fail,success,success,success,fail);
    BOOST_CHECK(!orSuccess1);
    BOOST_CHECK_EQUAL(count,1);
    BOOST_CHECK_EQUAL(failCount,2);
    BOOST_CHECK(status);

    count=0;
    failCount=0;
    auto orFail1=invoke_or_inverse(fail,fail,fail);
    BOOST_CHECK(orFail1);
    BOOST_CHECK_EQUAL(count,0);
    BOOST_CHECK_EQUAL(failCount,3);
    BOOST_CHECK(!status);

    count=0;
    failCount=0;
    auto orSuccess2=invoke_or_inverse(
                    DCS_FN(funcFailInverse(count,status)),
                    DCS_FN(funcFailInverse(count,status)),
                    DCS_FN(funcSuccessInverse(count,status)),
                    DCS_FN(funcSuccessInverse(count,status)),
                    DCS_FN(funcSuccessInverse(count,status)),
                    DCS_FN(funcFailInverse(count,status))
                );
    static_assert(std::is_same<decltype(orSuccess2),ResultInverse>::value,"Invalid return type");
    BOOST_CHECK(!orSuccess2);
    BOOST_CHECK_EQUAL(count,1);
    BOOST_CHECK(status);

    count=0;
    failCount=0;
    auto orFail2=invoke_or_inverse(
                    DCS_FN(funcFailInverse(count,status)),
                    DCS_FN(funcFailInverse(count,status)),
                    DCS_FN(funcFailInverse(count,status))
                );
    BOOST_CHECK(orFail2);
    BOOST_CHECK_EQUAL(count,0);
    BOOST_CHECK(!status);
}

BOOST_AUTO_TEST_SUITE_END()
