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
enum NODE_TYPE {
    NODE_ELEMENT_VOID,  //!< An element node that is considered a singleton tag,
                        //!< like input
    NODE_ELEMENT_TEXT,  //! An element tag that has only text, no children. A
                        //! script or style tag
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
    NODE_DOCUMENT
};

template <typename Ch>
class DocumentNode;

template <typename Ch>
class Node {
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
    Node() : Node(NODE_ELEMENT) {
    }
    Node(NODE_TYPE type) : m_parent(nullptr), m_type(type) {
        m_ref_id = Node<Ch>::node_counter++;
    }
    Node(const Node &node) {  // copy constructor
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
    const String<Ch> id() const {
        return m_id;
    }
    void id(const String<Ch> &element_id) {
        m_id = element_id;
        m_attributes[String<Ch>("id", 2)] = element_id;
    }
    bool contains_class(const String<Ch> class_name) {
        return m_classes.find(class_name) != m_classes.end();
    }
    void add_class(const String<Ch> &class_name) {
        m_classes.insert(class_name);
    }
    void remove_class(const String<Ch> &class_name) {
        m_classes.remove(class_name);
    }
    size_t classes_size() {
        return m_classes.size();
    }
    bool classes_empty() {
        return m_classes.empty();
    }
    typename std::set<String<Ch>>::iterator class_begin() {
        return m_classes.begin();
    }
    typename std::set<String<Ch>>::iterator class_end() {
        return m_classes.end();
    }
    const NODE_TYPE type() const {
        return m_type;
    }
    const String<Ch> name() const {
        return m_name;
    }
    void name(String<Ch> name) {
        m_name = name;
    }
    const String<Ch> value() const {
        return m_value;
    }
    void value(String<Ch> value) {
        m_value = value;
    }
    Node *parent() {
        return m_parent;
    }
    // void parent(Node* node) {
    //     m_parent = node;
    // }
    size_t ref_id() {
        return m_ref_id;
    }
    list_iterator child_begin() {
        return m_child_nodes.begin();
    }
    list_iterator child_end() {
        return m_child_nodes.end();
    }
    bool children_empty() const {
        return !m_child_nodes.empty();
    }
    size_t children_size() const {
        return m_child_nodes.size();
    }
    list_iterator find_child(Node *node) const {
        for (auto it = m_child_nodes.begin(); it != m_child_nodes.end(); ++it) {
            if (*it == node)
                return it;
        }
        return m_child_nodes.end();
    }
    void append_child(Node *node) {
        m_child_nodes.push_back(node);
        node->m_parent = this;
    }
    void remove_child(Node *node) {
        m_child_nodes.remove(node);
        node->parent = nullptr;
    }
    void clear_children() {
        m_child_nodes.clear();
    }
    void insert_before_child(list_iterator child, Node *node) {
        m_child_nodes.insert(child, node);
        --child;
    }
    void insert_after_child(list_iterator child, Node *node) {
        ++child;
        this->insert_before_child(child, node);
    }
    typename std::map<String<Ch>, String<Ch>>::iterator attribute_begin() {
        return m_attributes.begin();
    }
    typename std::map<String<Ch>, String<Ch>>::iterator attribute_end() {
        return m_attributes.end();
    }
    typename std::map<String<Ch>, String<Ch>>::iterator find_attribute(String<Ch> name) const {
        return m_attributes.find(name);
    }
    void add_attribute(String<Ch> name, String<Ch> value) {
        m_attributes[name] = value;
    }
    void remove_attribute(String<Ch> name) {
        m_attributes.erase(name);
    }
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
