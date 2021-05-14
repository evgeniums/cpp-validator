#undef NDEBUG

#include <dracosha/validator/validator.hpp>
#include <dracosha/validator/variadic_property.hpp>
#include <dracosha/validator/aggregation/tree.hpp>
#include <dracosha/validator/adapters/reporting_adapter.hpp>

using namespace DRACOSHA_VALIDATOR_NAMESPACE;

// define tree node
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

    size_t child_count() const noexcept
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

// name proeprty is used to access name of a node
DRACOSHA_VALIDATOR_PROPERTY(name)
// child_count property is used to get number of the node's children
DRACOSHA_VALIDATOR_PROPERTY(child_count)
// child is a variadic property for accessing a node's child by index
DRACOSHA_VALIDATOR_VARIADIC_PROPERTY(child)

int main()
{
    // define tree validator
    auto v1=validator(
            _[tree(ALL,child,child_count)][name](gte,"Node")
         );

    // tree with one top level node satisfying validation conditions
    TreeNode tr1("Node 0");
    assert(v1.apply(tr1));

    // tree with one top level node not satisfying validation conditions
    TreeNode tr2("0");
    assert(!v1.apply(tr2));

    // populate top node with children
    tr1.add_child(std::make_shared<TreeNode>("Node 0.0"));
    assert(v1.apply(tr1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.1"));
    assert(v1.apply(tr1));
    tr1.add_child(std::make_shared<TreeNode>("Node 0.2"));
    assert(v1.apply(tr1));

    // populate nested nodes
    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.0"));
    assert(v1.apply(tr1));
    tr1.mutable_child(0)->add_child(std::make_shared<TreeNode>("Node 0.0.1"));
    assert(v1.apply(tr1));
    tr1.mutable_child(1)->add_child(std::make_shared<TreeNode>("Node 0.1.0"));
    assert(v1.apply(tr1));
    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("Node 0.2.0"));
    assert(v1.apply(tr1));

    // validate nested node that doesn't satisfy validation conditions and construct report
    std::string rep;
    auto ra1=make_reporting_adapter(tr1,rep);
    tr1.mutable_child(2)->add_child(std::make_shared<TreeNode>("0.2.1"));
    assert(!v1.apply(ra1));
    assert(rep==std::string("name of each tree node must be greater than or equal to Node"));

    std::cout << "Example 23 done" << std::endl;
    return 0;
}
