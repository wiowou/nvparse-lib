#ifndef NVPARSE_SELECTORS_HPP_INCLUDED
#define NVPARSE_SELECTORS_HPP_INCLUDED

#include <algorithm>
#include <map>
#include <set>
#include <utility>

#include "node.hpp"
#include "string.hpp"

namespace nvparsehtml
{
    template<class Ch>
    class Selector
    {
      public:

        Selector(DocumentNode<Ch>* doc) : m_doc(doc) {
            this->traverse_nodes(m_doc);
        }

        typename std::map<String<Ch>, Node<Ch>*>::const_iterator ids_begin() {
            return m_id_to_node.begin();
        }
        typename std::map<String<Ch>, Node<Ch>*>::const_iterator ids_end() {
            return m_id_to_node.end();
        }
        Node<Ch>* get_by_id(String<Ch> id) {
            if (m_id_to_node.find(id) == m_id_to_node.end()) return nullptr;
            return m_id_to_node[id];
        }

        typename std::map<String<Ch>, std::set<Node<Ch>*> >::const_iterator classes_begin() {
            return m_class_to_nodes.begin();
        }
        typename std::map<String<Ch>, std::set<Node<Ch>*> >::const_iterator classes_end() {
            return m_class_to_nodes.end();
        }
        void get_by_class(String<Ch> class_name, std::set<Node<Ch>*>& results) {
            if (m_class_to_nodes.find(class_name) == m_class_to_nodes.end()) return;
            results.insert(m_class_to_nodes[class_name].begin(), m_class_to_nodes[class_name].end());
        }
        Node<Ch>* get_by_class(String<Ch> class_name) {
            if (m_class_to_nodes.find(class_name) == m_class_to_nodes.end()) return nullptr;
            return *(m_class_to_nodes[class_name].begin());
        }

        typename std::map<String<Ch>, std::set<std::pair<Node<Ch>*, String<Ch>> > >::const_iterator attributes_begin() {
            return m_att_to_nodes.begin();
        }
        typename std::map<String<Ch>, std::set<std::pair<Node<Ch>*, String<Ch>> > >::const_iterator attributes_end() {
            return m_att_to_nodes.end();
        }
        void get_by_attribute(String<Ch> att_name, String<Ch> att_value, std::set<Node<Ch>*>& results) {
            if (m_att_to_nodes.find(att_name) == m_att_to_nodes.end()) return;
            for (const auto pair : m_att_to_nodes[att_name]) {
                if (pair.second == att_value) {
                    results.insert(pair.first);
                }
            }
        }
        void get_by_attribute(String<Ch> att_name, std::set<Node<Ch>*>& results) {
            if (m_att_to_nodes.find(att_name) == m_att_to_nodes.end()) return;
            for (const auto pair : m_att_to_nodes[att_name]) {
                results.insert(pair.first);
            }
        }

        typename std::map<String<Ch>, std::set<Node<Ch>*> >::const_iterator types_begin() {
            return m_type_to_nodes.begin();
        }
        typename std::map<String<Ch>, std::set<Node<Ch>*> >::const_iterator types_end() {
            return m_type_to_nodes.end();
        }
        void get_by_type(String<Ch> type_name, std::set<Node<Ch>*>& results) {
            if (m_type_to_nodes.find(type_name) == m_type_to_nodes.end()) return;
            results.insert(m_type_to_nodes[type_name].begin(), m_type_to_nodes[type_name].end());
        }
        Node<Ch>* get_by_type(String<Ch> type_name) {
            if (m_type_to_nodes.find(type_name) == m_type_to_nodes.end()) return nullptr;
            return *(m_type_to_nodes[type_name].begin());
        }

      private:
        DocumentNode<Ch>* m_doc;
        std::map<String<Ch>, Node<Ch>*> m_id_to_node;
        std::map<String<Ch>, std::set<Node<Ch>*> > m_class_to_nodes;
        std::map<String<Ch>, std::set<std::pair<Node<Ch>*, String<Ch>> > > m_att_to_nodes;
        std::map<String<Ch>, std::set<Node<Ch>*> > m_type_to_nodes;
        
        void traverse_nodes(Node<Ch> *node) {
            if (node == nullptr) return;
            if (!node->id().empty()) {
                m_id_to_node[node->id()] = node;
            }
            m_type_to_nodes[node->name()].insert(node);
            for (auto class_it = node->class_begin(); class_it != node->class_end(); ++class_it) {
                m_class_to_nodes[*class_it].insert(node);
            }
            for (auto att_it = node->attribute_begin(); att_it != node->attribute_end(); ++att_it) {
                String<Ch> att_name = att_it->first;
                String<Ch> att_value = att_it->second;
                m_att_to_nodes[att_name].insert(std::make_pair(node, att_value));
            }
            for (auto child_it = node->child_begin(); child_it != node->child_end(); ++child_it) {
                this->traverse_nodes(*child_it);
            }
        }
    };
}

#endif
