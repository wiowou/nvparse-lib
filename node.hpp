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

namespace nvparsehtml
{
    enum NODE_TYPE
    {
        NODE_ELEMENT_VOID,  //!< An element node that is considered a singleton tag, like input
        NODE_ELEMENT_TEXT,  //! An element tag that has only text, no children. A script or style tag
        NODE_ELEMENT,       //!< An element node. Name contains element name. Value contains text of first data node.
        NODE_DOCTYPE,       //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
        NODE_DATA,          //!< A data node. Name is empty. Value contains data text.
        NODE_CDATA,         //!< A CDATA node. Name is empty. Value contains data text.
        NODE_COMMENT,       //!< A comment node. Name is empty. Value contains comment text.
        NODE_DECLARATION,   //!< A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
        NODE_PI,             //!< A PI node. Name contains target. Value contains instructions.
        NODE_DOCUMENT
    };

    template<typename Ch>
    class DocumentNode;

    template<typename Ch>
    class Node
    {
        typedef typename std::list<Node*>::iterator list_iterator;

        static size_t node_counter;
        size_t m_ref_id;
        NODE_TYPE m_type;                       // Type of node; always valid
        String<Ch> m_id;
        std::set<String<Ch>> m_classes;
        String<Ch> m_name;
        String<Ch> m_value;
        Node* m_parent;
        std::list<Node*> m_child_nodes;
        std::map<String<Ch>, String<Ch>> m_attributes;
      protected:
        void type(NODE_TYPE node_type) {
            m_type = node_type;
        }
      public:
        friend class DocumentNode<Ch>;
        Node() : Node(NODE_ELEMENT) {}
        Node(NODE_TYPE type) : m_parent(nullptr), m_type(type) {
            m_ref_id = Node<Ch>::node_counter++;
        }
        Node(const Node& node) { //copy constructor
            m_ref_id = Node<Ch>::node_counter++;
            m_name = node.m_name;
            m_value = node.m_value;
            m_parent = nullptr;
            m_child_nodes = node.m_child_nodes;
            m_attributes = node.m_attributes;
        }
        Node operator=(const Node &rhs) { //assignment operator
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
        Node* parent() {
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
        list_iterator find_child(Node* node) const {
            for (auto it = m_child_nodes.begin(); it != m_child_nodes.end(); ++it) {
                if (*it == node) return it;
            }
            return m_child_nodes.end();
        }
        void append_child(Node* node) {
            m_child_nodes.push_back(node);
            node->m_parent = this;
        }
        void remove_child(Node* node) {
            m_child_nodes.remove(node);
            node->parent = nullptr;
        }
        void clear_children() {
            m_child_nodes.clear();
        }
        void insert_before_child(list_iterator child, Node* node) {
            m_child_nodes.insert(child, node);
            --child;
        }
        void insert_after_child(list_iterator child, Node* node) {
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

    template<typename Ch>
    size_t Node<Ch>::node_counter = 0;

    template<typename Ch>
    inline bool operator< (const Node<Ch>& lhs, const Node<Ch>& rhs) {
        return lhs.ref_id() < rhs.ref_id();
    }

    template<typename Ch>
    class DocumentNode : public Node<Ch>
    {
      private:
        bool m_parse_no_entity_translation;
        bool m_parse_normalize_whitespace;
        bool m_parse_trim_whitespace;
        bool m_parse_no_utf8;
        std::forward_list<Node<Ch>*> m_nodes;
      public:
        DocumentNode() : 
            m_parse_no_entity_translation(true), 
            m_parse_normalize_whitespace(false), 
            m_parse_trim_whitespace(false),
            m_parse_no_utf8(false) {
            this->type(NODE_DOCUMENT);
        }

        void clear() {
            for (auto &p : m_nodes) {
                if (p != nullptr)
                    delete p;
                p = nullptr;
            }
            m_nodes.clear();
        }

        void parse(Ch *text) {
            m_parse_no_entity_translation = false;
            m_parse_normalize_whitespace = false;
            m_parse_trim_whitespace = false;
            m_parse_no_utf8 = false;
            this->parse_document(text);
        }

        ~DocumentNode() {
            this->clear();
        }
      private:
        Node<Ch>* append_node(Node<Ch> *node) {
            if (node != nullptr)
                m_nodes.push_front(node);
            return node;
        }
        void parse_document(Ch *text)
        {
            assert(text);
            Text<Ch>::parse_no_entity_translation = m_parse_no_entity_translation;
            Text<Ch>::parse_normalize_whitespace = m_parse_normalize_whitespace;
            Text<Ch>::parse_trim_whitespace = m_parse_trim_whitespace;
            Text<Ch>::parse_no_utf8 = m_parse_no_utf8;
            
            // Remove current contents
            this->clear();
            
            // Parse BOM, if any
            parse_bom(text);
            
            // Parse children
            while (true)
            {
                // Skip whitespace before node
                Text<Ch>::template 
                skip<whitespace_pred<Ch>>(text);
                if (*text == 0)
                    break;

                // Parse and append new child
                if (*text == Ch('<'))
                {
                    ++text;     // Skip '<'
                    Node<Ch> *node = this->parse_node(text);
                    this->append_child(node);
                }
                else
                    throw std::runtime_error(std::string("expected '<' but got '") + *text + "'  text: " + std::string(text, 10));
            }
        }

        // is this a known void, script or style html element?
        // name must be lower case
        static NODE_TYPE classify_node(String<Ch> name)
        {
            if (name.length() < 2) return NODE_ELEMENT;

            String<Ch> area("rea", 3);
            String<Ch> base("ase", 3);
            String<Ch> br("r", 1);
            String<Ch> col("ol", 2);
            String<Ch> command("ommand", 6);
            String<Ch> embed("mbed", 4);
            String<Ch> hr("r", 1);
            String<Ch> img("mg", 2);
            String<Ch> input("nput", 4);
            String<Ch> keygen("eygen", 5);
            String<Ch> link("ink", 4);
            String<Ch> meta("eta", 3);
            String<Ch> param("aram", 4);
            String<Ch> script("cript", 5);
            String<Ch> source("ource", 5);
            String<Ch> style("tyle", 4);
            String<Ch> track("rack", 4);
            String<Ch> wbr("br", 2);

            const String<Ch> ame(name.data() + 1, name.length() - 1);
            bool isVoidElement = false;
            switch(name[0])
            {
                case Ch('a'):
                    isVoidElement = ame == area;
                    break;
                case Ch('b'):
                    isVoidElement = ame == br || ame == base;
                    break;
                case Ch('c'):
                    isVoidElement = ame == col || ame == command;
                    break;
                case Ch('e'):
                    isVoidElement = ame == embed;
                    break;
                case Ch('h'):
                    isVoidElement = ame == hr;
                    break;
                case Ch('i'):
                    isVoidElement = ame == img || ame == input;
                    break;
                case Ch('k'):
                    isVoidElement = ame == keygen;
                    break;
                case Ch('l'):
                    isVoidElement = ame == link;
                    break;
                case Ch('m'):
                    isVoidElement = ame == meta;
                    break;
                case Ch('p'):
                    isVoidElement = ame == param;
                    break;
                case Ch('s'):
                    isVoidElement = ame == source;
                    if (ame == script) {
                        return NODE_ELEMENT_TEXT;
                    }
                    if (ame == style) {
                        return NODE_ELEMENT_TEXT;
                    }
                    break;
                case Ch('t'):
                    isVoidElement = ame == track;
                    break;
                case Ch('w'):
                    isVoidElement = ame == wbr;
                    break;
            }
            if (isVoidElement) {
                return NODE_ELEMENT_VOID;
            }
            return NODE_ELEMENT;
        }

        static void parse_bom(Ch *&text)
        {
            // UTF-8?
            if (static_cast<unsigned char>(text[0]) == 0xEF && 
                static_cast<unsigned char>(text[1]) == 0xBB && 
                static_cast<unsigned char>(text[2]) == 0xBF)
            {
                text += 3;      // Skup utf-8 bom
            }
        }

        // Parse XML declaration (<?xml...)
        Node<Ch>* parse_xml_declaration(Ch *&text)
        {
            // Create declaration
            Node<Ch>* declaration = new Node<Ch>(NODE_DECLARATION);

            // Skip whitespace before attributes or ?>
            Text<Ch>::template 
            skip<whitespace_pred<Ch>>(text);

            // Parse declaration attributes
            this->parse_node_attributes(text, declaration);
            
            // Skip ?>
            if (text[0] != Ch('?') || text[1] != Ch('>'))
                throw std::runtime_error(std::string("expected '?>' but got '") + *text + "'  text: " + std::string(text, 10));
            text += 2;
            
            return declaration;
        }

        // Parse XML comment (<!--...)
        Node<Ch>* parse_comment(Ch *&text)
        {
            // Remember value start
            Ch *value = text;

            // Skip until end of comment
            while (text[0] != Ch('-') || text[1] != Ch('-') || text[2] != Ch('>'))
            {
                if (!text[0])
                    throw std::runtime_error("unexpected end of data");
                ++text;
            }

            // Create comment node
            Node<Ch>* comment = new Node<Ch>(NODE_COMMENT);
            comment->value(String(value, text - value));
            
            text += 3;     // Skip '-->'
            return comment;
        }

        // Parse DOCTYPE
        Node<Ch>* parse_doctype(Ch *&text)
        {
            // Remember value start
            Ch *value = text;

            // Skip to >
            while (*text != Ch('>'))
            {
                // Determine character type
                switch (*text)
                {
                
                // If '[' encountered, scan for matching ending ']' using naive algorithm with depth
                // This works for all W3C test files except for 2 most wicked
                case Ch('['):
                {
                    ++text;     // Skip '['
                    int depth = 1;
                    while (depth > 0)
                    {
                        switch (*text)
                        {
                            case Ch('['): ++depth; break;
                            case Ch(']'): --depth; break;
                            case 0: throw std::runtime_error("unexpected end of data");
                        }
                        ++text;
                    }
                    break;
                }
                
                // Error on end of text
                case Ch('\0'):
                    throw std::runtime_error("unexpected end of data");
                
                // Other character, skip it
                default:
                    ++text;

                }
            }
            
            // Create a new doctype node
            Node<Ch>* doctype = new Node<Ch>(NODE_DOCTYPE);
            doctype->value(String(value, text - value));

            text += 1;      // skip '>'
            return doctype;
        }

        // Parse PI
        Node<Ch>* parse_pi(Ch *&text)
        {
            // Create pi node
            Node<Ch>* pi = new Node<Ch>(NODE_PI);

            // Extract PI target name
            Ch *name = text;
            Text<Ch>::template 
            skip<node_name_pred<Ch>>(text);
 
            if (text == name)
                throw std::runtime_error(std::string("expected 'PI target' but got '") + *text + "'  text: " + std::string(text, 10));
            pi->name(String<Ch>(name, text - name));
            
            // Skip whitespace between pi target and pi
            Text<Ch>::template 
            skip<whitespace_pred<Ch>>(text);

            // Remember start of pi
            Ch *value = text;
            
            // Skip to '?>'
            while (text[0] != Ch('?') || text[1] != Ch('>'))
            {
                if (*text == Ch('\0'))
                    throw std::runtime_error("unexpected end of data");
                ++text;
            }

            // Set pi value (verbatim, no entity expansion or whitespace normalization)
            pi->value(String<Ch>(value, text - value));     
            
            text += 2;                          // Skip '?>'
            return pi;
        }

        // Parse and append data
        // Return character that ends data.
        // This is necessary because this character might have been overwritten by a terminating 0
        Ch parse_and_append_data(Node<Ch> *node, Ch *&text, Ch *contents_start)
        {
            // Backup to contents start if whitespace trimming is disabled
            if (!(m_parse_trim_whitespace))
                text = contents_start;     
            
            // Skip until end of data
            Ch *value = text, *end;
            if (m_parse_normalize_whitespace)
                end = 
                Text<Ch>::template 
                skip_and_expand_character_refs<text_pred<Ch>, text_pure_with_ws_pred<Ch>>(text);  
            else
            end = 
                Text<Ch>::template
                skip_and_expand_character_refs<text_pred<Ch>, text_pure_no_ws_pred<Ch>>(text);

            // Trim trailing whitespace if flag is set; leading was already trimmed by whitespace skip after >
            if (m_parse_trim_whitespace)
            {
                if (m_parse_normalize_whitespace)
                {
                    // Whitespace is already condensed to single space characters by skipping function, so just trim 1 char off the end
                    if (*(end - 1) == Ch(' '))
                        --end;
                }
                else
                {
                    // Backup until non-whitespace character is found
                    while (whitespace_pred<Ch>::test(*(end - 1)))
                        --end;
                }
            }
            // If characters are still left between end and value (this test is only necessary if normalization is enabled)
            // Add data to parent node if no data exists yet
            if (node->value().empty())
                node->value(String(value, end - value));

            // Return character that ends data
            return *text;
        }

        // Parse and append data
        // Return character that ends data.
        // This is necessary because this character might have been overwritten by a terminating 0
        Ch parse_as_text_only(Node<Ch> *node, Ch *&text, Ch *contents_start)
        {
            // Backup to contents start because text should never be changed
            text = contents_start;
            
            // Skip until end of data
            Ch *value = text, *end;
            while (true)
            {
                Text<Ch>::template 
                skip<text_pred<Ch>>(text);
                if (!text[0])
                    throw std::runtime_error("unexpected end of data");
                end = text;
                ++text; //skip '<'
                if (*text != Ch('/')) continue;
                ++text; //skip '/'
                Ch *closing_name = text;
                Text<Ch>::template 
                skip<node_name_pred<Ch>>(text);
                if (!text[0])
                    throw std::runtime_error("unexpected end of data");
                if (compare_ci(node->name(), String<Ch>(closing_name, text - closing_name)))
                {
                    text = end;
                    break;
                }
            }
            
            // If characters are still left between end and value (this test is only necessary if normalization is enabled)
            // Add data to parent node if no data exists yet
            if (node->value().empty())
                node->value(String<Ch>(value, end - value));

            // Return character that ends data
            return *text;
        }

        // Parse CDATA
        Node<Ch> *parse_cdata(Ch *&text)
        {
            // Skip until end of cdata
            Ch *value = text;
            while (text[0] != Ch(']') || text[1] != Ch(']') || text[2] != Ch('>'))
            {
                if (!text[0])
                    throw std::runtime_error("unexpected end of data");
                ++text;
            }

            // Create new cdata node
            Node<Ch> *cdata = new Node<Ch>(NODE_CDATA);
            cdata->value(String<Ch>(value, text - value));

            text += 3;      // Skip ]]>
            return cdata;
        }

        // Parse element node
        Node<Ch> *parse_element(Ch *&text)
        {
            // Extract element name
            Ch *name = text;
            // skip<node_name_pred, Flags>(text);
            Text<Ch>::template 
            skip<node_name_pred<Ch>>(text);
            if (text == name)
                throw std::runtime_error(std::string("expected 'element name' but got '") + *text + "'  text: " + std::string(text, 10));
            String<Ch> elementName(name, text - name);
            elementName.to_lowercase();
            // Get handle to element node
            NODE_TYPE type = classify_node(elementName);
            Node<Ch> *element = new Node<Ch>(type); 
            element->name(elementName);
            
            // Skip whitespace between element name and attributes or >
            // skip<whitespace_pred, Flags>(text);
            Text<Ch>::template 
            skip<whitespace_pred<Ch>>(text);

            // Parse attributes, if any
            this->parse_node_attributes(text, element);

            // Determine ending type
            if (*text == Ch('>'))
            {
                ++text;
                //if (element->type() != node_element_void)
                if (element->type() != NODE_ELEMENT_VOID)
                {
                    this->parse_node_contents(text, element);
                }
            }
            else if (*text == Ch('/'))
            {
                ++text;
                if (*text != Ch('>'))
                    throw std::runtime_error(std::string("expected '>' but got '") + *text + "'  text: " + std::string(text, 10));
                ++text;
            }
            else
                throw std::runtime_error(std::string("expected '>' but got '") + *text + "'  text: " + std::string(text, 10));

            // Return parsed element
            return element;
        }

        // Determine node type, and parse it
        Node<Ch> *parse_node(Ch *&text)
        {
            // Parse proper node type
            switch (text[0])
            {

            // <...
            default: 
                // Parse and append element node
                return this->append_node(parse_element(text));

            // <?...
            case Ch('?'): 
                ++text;     // Skip ?
                if ((text[0] == Ch('x') || text[0] == Ch('X')) &&
                    (text[1] == Ch('m') || text[1] == Ch('M')) && 
                    (text[2] == Ch('l') || text[2] == Ch('L')) &&
                    whitespace_pred<Ch>::test(text[3]))
                {
                    // '<?xml ' - xml declaration
                    text += 4;      // Skip 'xml '
                    return this->append_node(parse_xml_declaration(text));
                }
                else
                {
                    // Parse PI
                    return this->append_node(parse_pi(text));
                }
            
            // <!...
            case Ch('!'): 

                // Parse proper subset of <! node
                switch (text[1])    
                {
                
                // <!-
                case Ch('-'):
                    if (text[2] == Ch('-'))
                    {
                        // '<!--' - xml comment
                        text += 3;     // Skip '!--'
                        return this->append_node(parse_comment(text));
                    }
                    break;

                // <![
                case Ch('['):
                    if (text[2] == Ch('C') && text[3] == Ch('D') && text[4] == Ch('A') && 
                        text[5] == Ch('T') && text[6] == Ch('A') && text[7] == Ch('['))
                    {
                        // '<![CDATA[' - cdata
                        text += 8;     // Skip '![CDATA['
                        return this->append_node(parse_cdata(text));
                    }
                    break;

                // <!D
                case Ch('D'):
                    if (text[2] == Ch('O') && text[3] == Ch('C') && text[4] == Ch('T') && 
                        text[5] == Ch('Y') && text[6] == Ch('P') && text[7] == Ch('E') && 
                        whitespace_pred<Ch>::test(text[8]))
                    {
                        // '<!DOCTYPE ' - doctype
                        text += 9;      // skip '!DOCTYPE '
                        return this->append_node(parse_doctype(text));
                    }

                }   // switch

                // Attempt to skip other, unrecognized node types starting with <!
                ++text;     // Skip !
                while (*text != Ch('>'))
                {
                    if (*text == 0)
                        throw std::runtime_error("unexpected end of data");
                    ++text;
                }
                ++text;     // Skip '>'
                return nullptr;   // No node recognized
            }
        }

        // Parse contents of the node - children, data etc.
        void parse_node_contents(Ch *&text, Node<Ch> *node)
        {
            // For all children and text
            while (true)
            {
                // Skip whitespace between > and node contents
                Ch *contents_start = text;      // Store start of node contents before whitespace is skipped
                // skip<whitespace_pred, Flags>(text);
                Text<Ch>::template 
                skip<whitespace_pred<Ch>>(text);
                Ch next_char = *text;

            // After data nodes, instead of continuing the loop, control jumps here.
            // This is because zero termination inside parse_and_append_data() function
            // would wreak havoc with the above code.
            // Also, skipping whitespace after data nodes is unnecessary.
            after_data_node:    
                
                // Determine what comes next: node closing, child node, data node, or 0?
                switch (next_char)
                {
                
                // Node closing or child node
                case Ch('<'):
                    if (text[1] == Ch('/'))
                    {
                        // Node closing
                        text += 2;      // Skip '</'
                        // skip node name without validating
                        Text<Ch>::template 
                        skip<node_name_pred<Ch>>(text);
                        // Skip remaining whitespace after node name
                        Text<Ch>::template
                        skip<whitespace_pred<Ch>>(text);
                        if (*text != Ch('>'))
                            throw std::runtime_error(std::string("expected '>' but got '") + *text + "'  text: " + std::string(text, 10));
                        ++text;     // Skip '>'
                        return;     // Node closed, finished parsing contents
                    }
                    else
                    {
                        // Child node
                        ++text;     // Skip '<'
                        Node<Ch> *child = this->parse_node(text);
                        if (child != nullptr) {
                            node->append_child(child);
                        }
                    }
                    break;

                // End of data - error
                case Ch('\0'):
                    throw std::runtime_error("unexpected end of data");

                // Data node
                default:
                    //if (node->element_type() == node_element_script || node->element_type() == node_element_style)
                    if (node->type() == NODE_ELEMENT_TEXT)
                    {
                        next_char = parse_as_text_only(node, text, contents_start);
                    }
                    else
                    {
                        next_char = parse_and_append_data(node, text, contents_start);
                    }
                    goto after_data_node;   // Bypass regular processing after data nodes

                }
            }
        }

        // Parse XML attributes of the node
        void parse_node_attributes(Ch *&text, Node<Ch> *node)
        {
            // For all attributes 
            while (attribute_name_pred<Ch>::test(*text))
            {
                // Extract attribute name
                Ch *name = text;
                ++text;     // Skip first character of attribute name
                // skip<attribute_name_pred, Flags>(text);
                Text<Ch>::template 
                skip<attribute_name_pred<Ch>>(text);
                if (text == name)
                    throw std::runtime_error(std::string("expected 'attribute name' but got '") + *text + "'  text: " + std::string(text, 10));

                // Create new attribute
                String<Ch> att_name(name, text - name);
                att_name.to_lowercase();
                bool is_id = att_name == String<Ch>("id", 2);
                bool is_class = att_name == String<Ch>("class", 5);

                // Skip whitespace after attribute name
                Text<Ch>::template 
                skip<whitespace_pred<Ch>>(text);

                if (*text != Ch('=')) { //boolean attribute
                    if (is_id || is_class)
                        throw std::runtime_error(std::string("expected '=' but got '") + *text + "'  text: " + std::string(text, 10));
                    String<Ch> att_value;
                    node->add_attribute(att_name, att_value);
                    continue; 
                }
                    // RAPIDXML_PARSE_ERROR("expected =", text);
                // Skip =
                ++text;

                // Skip whitespace after =
                Text<Ch>::template 
                skip<whitespace_pred<Ch>>(text);

                // Skip quote and remember if it was ' or "
                Ch quote = *text;
                if (quote != Ch('\'') && quote != Ch('"'))
                    throw std::runtime_error(std::string("expected ' or \" but got ") + *text + "  text: " + std::string(text, 10));
                ++text;

                // Extract attribute value and expand char refs in it
                Ch *value = text, *end;
                if (quote == Ch('\''))
                    end = 
                    Text<Ch>::template
                    skip_and_expand_character_refs<attribute_value_pred<Ch, Ch('\'')>, attribute_value_pure_pred<Ch, Ch('\'')>>(text);
                else
                    end = 
                    Text<Ch>::template
                    skip_and_expand_character_refs<attribute_value_pred<Ch, Ch('"')>, attribute_value_pure_pred<Ch, Ch('"')>>(text);
                
                // Set attribute value
                String<Ch> att_value(value, end - value);
                if (is_id) {
                    node->id(att_value);
                    node->add_attribute(att_name, att_value);
                } else if (is_class) {
                    this->parse_classes(att_value.data(), att_value.length(), node);
                } else {
                    node->add_attribute(att_name, att_value);
                }
                
                // Make sure that end quote is present
                if (*text != quote)
                    throw std::runtime_error(std::string("expected ' or \" but got ") + *text + "  text: " + std::string(text, 10));
                ++text;     // Skip quote

                // Skip whitespace after attribute value
                Text<Ch>::template 
                skip<whitespace_pred<Ch>>(text);
            }
        }

        void parse_classes(Ch *text, size_t length, Node<Ch> *node) 
        {
            Ch *start = text;
            // Skip whitespace at the front
            Text<Ch>::template 
            skip<whitespace_pred<Ch>>(text);
            while (text - start < length)
            {
                Ch *class_name = text;
                Text<Ch>::template 
                skip<node_name_pred<Ch>>(text);
                if (text - start > length) text = start + length; 
                String<Ch> class_value(class_name, text - class_name);
                node->add_class(class_value);
                Text<Ch>::template 
                skip<whitespace_pred<Ch>>(text);
            }
        }
    };

}

#endif
