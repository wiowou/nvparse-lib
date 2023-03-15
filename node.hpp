#ifndef NVPARSE_NODE_HPP_INCLUDED
#define NVPARSE_NODE_HPP_INCLUDED

#include <cassert>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <stdexcept>
#include <string>

#include "string.hpp"
#include "text.hpp"

namespace nvparsehtml {
template <typename Ch>
class DocumentNode;
//! Responsible for storing and retreiving an XHTML node's attributes and contents
template <typename Ch>
class Node {
   public:
    // clang-format off
    //! The node types
    enum NODE_TYPE {
        NODE_ELEMENT_VOID,  //!< An element node that is considered a singleton tag ike input.
        NODE_ELEMENT_TEXT,  //!< An element tag that has only text like a script or style tag.
        NODE_ELEMENT,       //!< An element node. Name contains element name. Value
                            //!< contains text of first data node.
        NODE_DOCTYPE,       //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE
                            //!< text.
        NODE_DATA,          //!< A data node. Name is empty. Value contains data text.
        NODE_CDATA,         //!< A CDATA node. Name is empty. Value contains data text.
        NODE_COMMENT,       //!< A comment node. Name is empty. Value contains comment
                            //!< text.
        NODE_DECLARATION,   //!< A declaration node. Name and value are empty.
                            //!< Declaration parameters (version, encoding and
                            //!< tandalone) are in node attributes.
        NODE_PI,            //!< A PI node. Name contains target. Value contains instructions.
        NODE_DOCUMENT       //!< A DOCUMENT node.
    };
    // clang-format on

   private:
    typedef typename std::list<Node *>::iterator list_iterator;

    static size_t node_counter;
    size_t m_ref_id;
    NODE_TYPE m_type;  // Type of node; always valid
    String<Ch> m_id;
    std::set<String<Ch>> m_classes;
    String<Ch> m_name;
    String<Ch> m_value;
    Node *m_parent;
    std::list<Node *> m_child_nodes;
    std::map<String<Ch>, String<Ch>> m_attributes;

   protected:
    void type(NODE_TYPE node_type) {
        m_type = node_type;
    }

   public:
    friend class DocumentNode<Ch>;
    //! Creates a new \ref Node of NODE_ELEMENT type.
    Node() : Node(NODE_ELEMENT) {
    }
    //! Creates a new \ref Node where the type can be specified.
    Node(NODE_TYPE type) : m_parent(nullptr), m_type(type) {
        m_ref_id = Node<Ch>::node_counter++;
    }
    Node(const Node &node) {
        m_ref_id = Node<Ch>::node_counter++;
        m_name = node.m_name;
        m_value = node.m_value;
        m_parent = nullptr;
        m_child_nodes = node.m_child_nodes;
        m_attributes = node.m_attributes;
    }
    Node operator=(const Node &rhs) {  // assignment operator
        m_ref_id = Node<Ch>::node_counter++;
        m_name = rhs.m_name;
        m_value = rhs.m_value;
        m_parent = nullptr;
        m_child_nodes = rhs.m_child_nodes;
        m_attributes = rhs.m_attributes;
    }
    //! Gets id.
    //! \return \ref String of the id.
    const String<Ch> id() const {
        return m_id;
    }
    //! Sets id.
    //! \param element_id \ref String of the id.
    void id(const String<Ch> &element_id) {
        m_id = element_id;
        m_attributes[String<Ch>("id", 2)] = element_id;
    }
    //! Determines whether this \ref Node contains a class.
    //! \param class_name \ref String of the class.
    bool contains_class(const String<Ch> class_name) {
        return m_classes.find(class_name) != m_classes.end();
    }
    //! Adds a class to this \ref Node.
    //! \param class_name \ref String of the class.
    void add_class(const String<Ch> &class_name) {
        m_classes.insert(class_name);
    }
    //! Removes a class from this \ref Node.
    //! \param class_name \ref String of the class.
    void remove_class(const String<Ch> &class_name) {
        m_classes.remove(class_name);
    }
    //! Gets number of classes.
    //! \return number of classes.
    size_t classes_size() {
        return m_classes.size();
    }
    //! Are there any classes?
    //! \return whether this \ref Node has any classes.
    bool classes_empty() {
        return m_classes.empty();
    }
    //! The beginning of the classes
    //! \return iterator to the first class
    typename std::set<String<Ch>>::iterator class_begin() {
        return m_classes.begin();
    }
    //! The end of the classes
    //! \return iterator to one past the last class
    typename std::set<String<Ch>>::iterator class_end() {
        return m_classes.end();
    }
    //! Gets the type of the \ref Node
    //! \return the \ref NODE_TYPE
    const NODE_TYPE type() const {
        return m_type;
    }
    //! Gets the \ref Node name. The name, aka element type.
    //! \return \ref String of the name.
    const String<Ch> name() const {
        return m_name;
    }
    //! Sets name. The name, aka element type.
    //! \param name \ref String of the name.
    void name(String<Ch> name) {
        m_name = name;
    }
    //! Gets the \ref Node value. The value is the text associated with.
    //! \return \ref String of the value.
    const String<Ch> value() const {
        return m_value;
    }
    //! Sets the value. The text associated with the \ref Node.
    //! \param value \ref String of the name.
    void value(String<Ch> value) {
        m_value = value;
    }
    //! Gets the \ref Node parent.
    //! \return \ref Node pointer to the parent.
    Node *parent() {
        return m_parent;
    }
    // void parent(Node* node) {
    //     m_parent = node;
    // }
    //! Gets the generated id.
    //! \return an int of the generated id.
    size_t ref_id() {
        return m_ref_id;
    }
    //! The beginning of the child nodes
    //! \return iterator to the first child node
    list_iterator child_begin() {
        return m_child_nodes.begin();
    }
    //! The end of the child nodes
    //! \return iterator to the first child node
    list_iterator child_end() {
        return m_child_nodes.end();
    }
    //! Are there any children?
    //! \return whether this \ref Node has any children.
    bool children_empty() const {
        return !m_child_nodes.empty();
    }
    //! Gets number of children.
    //! \return number of children.
    size_t children_size() const {
        return m_child_nodes.size();
    }
    //! Gets iterator to the child in question.
    //! \return list iterator. If not found, returns \ref child_end.
    list_iterator find_child(Node *node) const {
        for (auto it = m_child_nodes.begin(); it != m_child_nodes.end(); ++it) {
            if (*it == node)
                return it;
        }
        return m_child_nodes.end();
    }
    //! Appends a Node to this \ref Node.
    //! \param node \ref pointer to Node.
    void append_child(Node *node) {
        m_child_nodes.push_back(node);
        node->m_parent = this;
    }
    //! Removes a Node from this \ref Node.
    //! \param node \ref pointer to Node.
    void remove_child(Node *node) {
        m_child_nodes.remove(node);
        node->parent = nullptr;
    }
    //! Removes all child \ref Node s
    void clear_children() {
        m_child_nodes.clear();
    }
    //! Inserts a Node before the specified child node.
    //! \param child \ref iterator to child node.
    //! \param node \ref pointer to \ref Node.
    void insert_before_child(list_iterator child, Node *node) {
        m_child_nodes.insert(child, node);
        --child;
    }
    //! Inserts a Node after the specified child node.
    //! \param child \ref iterator to child node.
    //! \param node \ref pointer to \ref Node.
    void insert_after_child(list_iterator child, Node *node) {
        ++child;
        this->insert_before_child(child, node);
    }
    //! The beginning of the attributes
    //! \return iterator to the first attribute
    typename std::map<String<Ch>, String<Ch>>::iterator attribute_begin() {
        return m_attributes.begin();
    }
    //! The end of the attributes
    //! \return iterator to the one past the last attribute
    typename std::map<String<Ch>, String<Ch>>::iterator attribute_end() {
        return m_attributes.end();
    }
    //! Find an attribute's value.
    //! \param name attribute key.
    //! \param value attribute value.
    String<Ch> find_attribute(String<Ch> name) const {
        return m_attributes.find(name)->second;
    }
    //! Adds an attribute-value pair.
    //! \param name attribute key.
    //! \param value attribute value.
    void add_attribute(String<Ch> name, String<Ch> value) {
        m_attributes[name] = value;
    }
    //! Removes an attribute-value pair.
    //! \param name attribute key.
    void remove_attribute(String<Ch> name) {
        m_attributes.erase(name);
    }
    //! Removes all attribute-value pairs.
    void clear_attributes() {
        m_attributes.clear();
    }
};

template <typename Ch>
size_t Node<Ch>::node_counter = 0;

template <typename Ch>
inline bool operator<(const Node<Ch> &lhs, const Node<Ch> &rhs) {
    return lhs.ref_id() < rhs.ref_id();
}

}  // namespace nvparsehtml

#endif
