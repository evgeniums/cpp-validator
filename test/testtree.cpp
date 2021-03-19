#include <memory>

#include <boost/test/unit_test.hpp>

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/aggregation/tree.hpp>
#include <dracosha/validator/variadic_property.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

BOOST_AUTO_TEST_SUITE(TestTree)

namespace {

struct TreeNode
{
    TreeNode(std::string name) : _name(std::move(name))
    {}

    void add_child(std::shared_ptr<TreeNode> child)
    {
        _children.push_back(std::move(child));
    }

    const TreeNode& child(size_t index) const
    {
        return *_children.at(index);
    }

    auto mutable_child(size_t index)
    {
        return _children.at(index);
    }

    size_t child_count() const
    {
        return _children.size();
    }

    std::string name() const
    {
        return _name;
    }

    std::vector<std::shared_ptr<TreeNode>> _children;
    std::string _name;
};

DRACOSHA_VALIDATOR_PROPERTY(name)
DRACOSHA_VALIDATOR_PROPERTY(child_count)
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(child)

}

BOOST_AUTO_TEST_CASE(CheckTreeAll)
{
    auto v1=validator(
            _[tree(ALL,child,child_count)][name](gte,"Node")
         );

    TreeNode tr1("Node 0");
    BOOST_CHECK(v1.apply(tr1));

    TreeNode tr2("0");
    BOOST_CHECK(!v1.apply(tr2));

    tr1.add_child(std::make_shared<TreeNode>("Node 0.0"));
    BOOST_CHECK(v1.apply(tr1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.1"));
    BOOST_CHECK(v1.apply(tr1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.2"));
    BOOST_CHECK(v1.apply(tr1));

    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.0"));
    BOOST_CHECK(v1.apply(tr1));
    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.1"));
    BOOST_CHECK(v1.apply(tr1));
    tr1.mutable_child(1)->add_child(std::make_shared<TreeNode>("Node 0.1.0"));
    BOOST_CHECK(v1.apply(tr1));
    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("Node 0.2.0"));
    BOOST_CHECK(v1.apply(tr1));

    std::string rep;
    auto ra1=make_reporting_adapter(tr1,rep);
    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("0.2.1"));
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep,std::string("name of each tree node must be greater than or equal to Node"));
}

BOOST_AUTO_TEST_CASE(CheckTreeAny)
{
    auto v1=validator(
            _[tree(ANY,child,child_count)][name](gte,"Node")
         );

    TreeNode tr1("Node 0");
    BOOST_CHECK(v1.apply(tr1));

    tr1.add_child(std::make_shared<TreeNode>("Node 0.0"));
    BOOST_CHECK(v1.apply(tr1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.1"));
    BOOST_CHECK(v1.apply(tr1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.2"));
    BOOST_CHECK(v1.apply(tr1));

    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.0"));
    BOOST_CHECK(v1.apply(tr1));
    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.1"));
    BOOST_CHECK(v1.apply(tr1));
    tr1.mutable_child(1)->add_child(std::make_shared<TreeNode>("Node 0.1.0"));
    BOOST_CHECK(v1.apply(tr1));
    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("Node 0.2.0"));
    BOOST_CHECK(v1.apply(tr1));

    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("0.2.1"));
    BOOST_CHECK(v1.apply(tr1));

    std::string rep;
    TreeNode tr2("0");
    auto ra2=make_reporting_adapter(tr2,rep);
    BOOST_CHECK(!v1.apply(ra2));
    BOOST_CHECK_EQUAL(rep,std::string("name of at least one tree node must be greater than or equal to Node"));
}

BOOST_AUTO_TEST_CASE(CheckNestedTreeAll)
{
    auto v1=validator(
            _["field1"][tree(ALL,child,child_count)][name](gte,"Node")
         );

    std::map<std::string,TreeNode> m1{
        {"field1", TreeNode{"Node 0"}}
    };
    std::map<std::string,TreeNode> m2{
        {"field1", TreeNode{"0"}}
    };

    TreeNode& tr1=m1.at("field1");
    BOOST_CHECK(v1.apply(m1));

    tr1.add_child(std::make_shared<TreeNode>("Node 0.0"));
    BOOST_CHECK(v1.apply(m1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.1"));
    BOOST_CHECK(v1.apply(m1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.2"));
    BOOST_CHECK(v1.apply(m1));

    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.0"));
    BOOST_CHECK(v1.apply(m1));
    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.1"));
    BOOST_CHECK(v1.apply(m1));
    tr1.mutable_child(1)->add_child(std::make_shared<TreeNode>("Node 0.1.0"));
    BOOST_CHECK(v1.apply(m1));
    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("Node 0.2.0"));
    BOOST_CHECK(v1.apply(m1));

    std::string rep;
    auto ra1=make_reporting_adapter(m1,rep);
    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("0.2.1"));
    BOOST_CHECK(!v1.apply(ra1));
    BOOST_CHECK_EQUAL(rep,std::string("name of each tree node of field1 must be greater than or equal to Node"));
    rep.clear();

    auto ra2=make_reporting_adapter(m2,rep);
    BOOST_CHECK(!v1.apply(ra2));
    BOOST_CHECK_EQUAL(rep,std::string("name of each tree node of field1 must be greater than or equal to Node"));
    rep.clear();
}

BOOST_AUTO_TEST_CASE(CheckNestedTreeAny)
{
    auto v1=validator(
            _["field1"][tree(ANY,child,child_count)][name](gte,"Node")
         );

    std::map<std::string,TreeNode> m1{
        {"field1", TreeNode{"Node 0"}}
    };
    std::map<std::string,TreeNode> m2{
        {"field1", TreeNode{"0"}}
    };

    TreeNode& tr1=m1.at("field1");
    BOOST_CHECK(v1.apply(m1));

    tr1.add_child(std::make_shared<TreeNode>("Node 0.0"));
    BOOST_CHECK(v1.apply(m1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.1"));
    BOOST_CHECK(v1.apply(m1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.2"));
    BOOST_CHECK(v1.apply(m1));

    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.0"));
    BOOST_CHECK(v1.apply(m1));
    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.1"));
    BOOST_CHECK(v1.apply(m1));
    tr1.mutable_child(1)->add_child(std::make_shared<TreeNode>("Node 0.1.0"));
    BOOST_CHECK(v1.apply(m1));
    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("Node 0.2.0"));
    BOOST_CHECK(v1.apply(m1));

    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("0.2.1"));
    BOOST_CHECK(v1.apply(m1));

    std::string rep;
    auto ra2=make_reporting_adapter(m2,rep);
    BOOST_CHECK(!v1.apply(ra2));
    BOOST_CHECK_EQUAL(rep,std::string("name of at least one tree node of field1 must be greater than or equal to Node"));
}

BOOST_AUTO_TEST_CASE(CheckInvalidPath)
{
    auto v0=validator(
            _["field1"][name](gte,"Node")
         );

    auto v1=validator(
            _["field1"][tree(ANY,child,child_count)][name](gte,"Node")
         );

    auto s1=std::string("hello");

    // ignore invalid paths
    BOOST_CHECK(v0.apply(s1));
    BOOST_CHECK(v1.apply(s1));
}

BOOST_AUTO_TEST_SUITE_END()
