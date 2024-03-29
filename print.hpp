#ifndef NVPARSE_PRINT_HPP_INCLUDED
#define NVPARSE_PRINT_HPP_INCLUDED

#include <iterator>
#include <ostream>

#include "document.hpp"
#include "node.hpp"
#include "text.hpp"

namespace nvparsehtml {
///////////////////////////////////////////////////////////////////////
// Printing flags

const bool print_no_indenting = false;  //!< Printer flag instructing the printer to suppress
                                        //!< indenting of XML. See print() function.

///////////////////////////////////////////////////////////////////////
// Internal

namespace internal {
template <class OutIt, class Ch>
inline OutIt print_children(OutIt out, Node<Ch> *node, int indent);
///////////////////////////////////////////////////////////////////////////
// Internal character operations

// Fill given output iterator with repetitions of the same character
template <class OutIt, class Ch>
inline OutIt fill_chars(OutIt out, int n, Ch ch) {
    for (int i = 0; i < n; ++i)
        *out++ = ch;
    return out;
}

// Copy characters from given range to given output iterator
template <class OutIt, class Ch>
inline OutIt copy_chars(const String<Ch> &s, OutIt out) {
    const Ch *begin = s.data();
    const Ch *end = begin + s.length();
    while (begin != end)
        *out++ = *begin++;
    return out;
}

// Copy characters from given range to given output iterator
// and combine multiple whitespace chars (space, \n, \t, \r) into space
template <class OutIt, class Ch>
inline OutIt copy_chars_combine_ws(const String<Ch> &s, OutIt out) {
    bool is_prev_char_ws = false;
    Ch *begin = s.data();
    const Ch *end = begin + s.length();
    while (whitespace_pred<Ch>::test(*begin) && begin != end) {
        ++begin;
    }
    while (begin != end) {
        if (is_prev_char_ws && whitespace_pred<Ch>::test(*begin)) {
            ++begin;
        } else if (is_prev_char_ws && !whitespace_pred<Ch>::test(*begin)) {
            *out++ = *begin++;
            is_prev_char_ws = false;
        } else {
            is_prev_char_ws = whitespace_pred<Ch>::test(*begin);
            if (is_prev_char_ws) {
                *out++ = Ch(' ');
            } else {
                *out++ = *begin++;
            }
        }
    }
    return out;
}

// Copy characters from given range to given output iterator
// and combine multiple whitespace chars (space, \n, \t, \r) into space
template <class OutIt, class Ch>
inline OutIt copy_chars_rewrite_cr(const String<Ch> &s, int indent, OutIt out) {
    bool is_prev_char_cr = false;
    Ch *begin = s.data();
    const Ch *end = begin + s.length();
    while (begin != end) {
        if (is_prev_char_cr && whitespace_pred<Ch>::test(*begin)) {
            ++begin;
        } else if (is_prev_char_cr && !whitespace_pred<Ch>::test(*begin)) {
            *out++ = *begin++;
            is_prev_char_cr = false;
        } else {
            is_prev_char_cr = *begin == Ch('\n');
            if (is_prev_char_cr) {
                *out++ = Ch('\n');
                if (!print_no_indenting)
                    out = fill_chars(out, indent, Ch('\t'));
            } else {
                *out++ = *begin++;
            }
        }
    }
    return out;
}

// Copy characters from given range to given output iterator and expand
// characters into references (&lt; &gt; &apos; &quot; &amp;)
template <class OutIt, class Ch>
inline OutIt copy_and_expand_chars(const String<Ch> &s, Ch noexpand, OutIt out) {
    const Ch *begin = s.data();
    const Ch *end = begin + s.length();
    while (begin != end) {
        if (*begin == noexpand) {
            *out++ = *begin;  // No expansion, copy character
        } else {
            switch (*begin) {
                case Ch('<'):
                    *out++ = Ch('&');
                    *out++ = Ch('l');
                    *out++ = Ch('t');
                    *out++ = Ch(';');
                    break;
                case Ch('>'):
                    *out++ = Ch('&');
                    *out++ = Ch('g');
                    *out++ = Ch('t');
                    *out++ = Ch(';');
                    break;
                case Ch('\''):
                    *out++ = Ch('&');
                    *out++ = Ch('a');
                    *out++ = Ch('p');
                    *out++ = Ch('o');
                    *out++ = Ch('s');
                    *out++ = Ch(';');
                    break;
                case Ch('"'):
                    *out++ = Ch('&');
                    *out++ = Ch('q');
                    *out++ = Ch('u');
                    *out++ = Ch('o');
                    *out++ = Ch('t');
                    *out++ = Ch(';');
                    break;
                case Ch('&'):
                    *out++ = Ch('&');
                    *out++ = Ch('a');
                    *out++ = Ch('m');
                    *out++ = Ch('p');
                    *out++ = Ch(';');
                    break;
                default:
                    *out++ = *begin;  // No expansion, copy character
            }
        }
        ++begin;  // Step to next character
    }
    return out;
}

///////////////////////////////////////////////////////////////////////////
// Internal printing operations

// Print attributes of the node
template <class OutIt, class Ch>
inline OutIt print_attributes(OutIt out, Node<Ch> *node) {
    String<Ch> id("id", 2);
    if (!node->id().empty()) {
        *out = Ch(' '), ++out;
        out = copy_chars(id, out);
        *out = Ch('='), ++out;
        *out = Ch('"'), ++out;
        out = copy_and_expand_chars(node->id(), Ch('\''), out);
        *out = Ch('"'), ++out;
    }
    if (!node->classes_empty()) {
        String<Ch> class_att("class", 5);
        *out = Ch(' '), ++out;
        out = copy_chars(class_att, out);
        *out = Ch('='), ++out;
        *out = Ch('"'), ++out;
        size_t count = 1;
        for (auto it = node->class_begin(); it != node->class_end(); ++it) {
            out = copy_chars(*it, out);
            if (count < node->classes_size()) {
                *out = Ch(' '), ++out;
            }
            ++count;
        }
        *out = Ch('"'), ++out;
    }
    for (auto it = node->attribute_begin(); it != node->attribute_end(); ++it) {
        if (it->first.empty() || it->first == id)
            continue;
        // Print attribute name
        *out = Ch(' '), ++out;
        out = copy_chars(it->first, out);
        if (it->second.empty())
            continue;
        *out = Ch('='), ++out;
        if (it->second.contains(Ch('"'))) {
            *out = Ch('\''), ++out;
            out = copy_and_expand_chars(it->second, Ch('"'), out);
            *out = Ch('\''), ++out;
        } else {
            *out = Ch('"'), ++out;
            out = copy_and_expand_chars(it->second, Ch('\''), out);
            *out = Ch('"'), ++out;
        }
    }
    return out;
}

// Print data node
template <class OutIt, class Ch>
inline OutIt print_data_node(OutIt out, Node<Ch> *node, int indent) {
    assert(node->type() == Node<Ch>::NODE_DATA);
    if (!print_no_indenting)
        out = fill_chars(out, indent, Ch('\t'));
    out = copy_and_expand_chars(node->value(), Ch(0), out);
    return out;
}

// Print data node
template <class OutIt, class Ch>
inline OutIt print_cdata_node(OutIt out, Node<Ch> *node, int indent) {
    assert(node->type() == Node<Ch>::NODE_CDATA);
    if (!print_no_indenting)
        out = fill_chars(out, indent, Ch('\t'));
    *out = Ch('<'), ++out;
    *out = Ch('!'), ++out;
    *out = Ch('['), ++out;
    *out = Ch('C'), ++out;
    *out = Ch('D'), ++out;
    *out = Ch('A'), ++out;
    *out = Ch('T'), ++out;
    *out = Ch('A'), ++out;
    *out = Ch('['), ++out;
    out = copy_chars(node->value(), out);
    *out = Ch(']'), ++out;
    *out = Ch(']'), ++out;
    *out = Ch('>'), ++out;
    return out;
}

// Print element node
template <class OutIt, class Ch>
inline OutIt print_element_node(OutIt out, Node<Ch> *node, int indent) {
    assert(node->type() <= Node<Ch>::NODE_ELEMENT);

    // Print element name and attributes, if any
    if (!print_no_indenting)
        out = fill_chars(out, indent, Ch('\t'));
    *out = Ch('<'), ++out;
    out = copy_chars(node->name(), out);
    out = print_attributes(out, node);

    // If node is childless
    if (node->children_empty() && node->value().empty()) {
        // Print childless node tag ending
        *out = Ch('/'), ++out;
        *out = Ch('>'), ++out;
        return out;
    } else if (node->children_empty()) {
        // Print normal node tag ending
        *out = Ch('>'), ++out;
        out = copy_chars_combine_ws(node->value(), out);
        // Print node end
        *out = Ch('<'), ++out;
        *out = Ch('/'), ++out;
        out = copy_chars(node->name(), out);
        *out = Ch('>'), ++out;
        return out;
    }

    // Print normal node tag ending
    *out = Ch('>'), ++out;
    // Print all children with full indenting
    if (!print_no_indenting)
        *out = Ch('\n'), ++out;
    out = print_children(out, node, indent + 1);
    if (!print_no_indenting)
        out = fill_chars(out, indent, Ch('\t'));
    // Print node end
    *out = Ch('<'), ++out;
    *out = Ch('/'), ++out;
    out = copy_chars(node->name(), out);
    *out = Ch('>'), ++out;

    return out;
}

// Print element void node
template <class OutIt, class Ch>
inline OutIt print_element_void_node(OutIt out, Node<Ch> *node, int indent) {
    assert(node->type() <= Node<Ch>::NODE_ELEMENT);

    // Print element name and attributes, if any
    if (!print_no_indenting)
        out = fill_chars(out, indent, Ch('\t'));
    *out = Ch('<'), ++out;
    out = copy_chars(node->name(), out);
    out = print_attributes(out, node);
    *out = Ch('>'), ++out;
    return out;
}

// Print element text node
template <class OutIt, class Ch>
inline OutIt print_element_text_node(OutIt out, Node<Ch> *node, int indent) {
    assert(node->type() <= Node<Ch>::NODE_ELEMENT_TEXT);

    // Print element name and attributes, if any
    if (!print_no_indenting)
        out = fill_chars(out, indent, Ch('\t'));
    *out = Ch('<'), ++out;
    out = copy_chars(node->name(), out);
    out = print_attributes(out, node);
    // Print normal node tag ending
    *out = Ch('>'), ++out;
    out = copy_chars(node->value(), out);
    if (!print_no_indenting)
        out = fill_chars(out, indent, Ch('\t'));
    // Print node end
    *out = Ch('<'), ++out;
    *out = Ch('/'), ++out;
    out = copy_chars(node->name(), out);
    *out = Ch('>'), ++out;

    return out;
}

// Print declaration node
template <class OutIt, class Ch>
inline OutIt print_declaration_node(OutIt out, Node<Ch> *node, int indent) {
    // Print declaration start
    if (!print_no_indenting)
        out = fill_chars(out, indent, Ch('\t'));
    *out = Ch('<'), ++out;
    *out = Ch('?'), ++out;
    *out = Ch('x'), ++out;
    *out = Ch('m'), ++out;
    *out = Ch('l'), ++out;

    // Print attributes
    out = print_attributes(out, node);

    // Print declaration end
    *out = Ch('?'), ++out;
    *out = Ch('>'), ++out;

    return out;
}

// Print comment node
template <class OutIt, class Ch>
inline OutIt print_comment_node(OutIt out, Node<Ch> *node, int indent) {
    assert(node->type() == Node<Ch>::NODE_COMMENT);
    if (!print_no_indenting)
        out = fill_chars(out, indent, Ch('\t'));
    *out = Ch('<'), ++out;
    *out = Ch('!'), ++out;
    *out = Ch('-'), ++out;
    *out = Ch('-'), ++out;
    out = copy_chars(node->value(), out);
    *out = Ch('-'), ++out;
    *out = Ch('-'), ++out;
    *out = Ch('>'), ++out;
    return out;
}

// Print doctype node
template <class OutIt, class Ch>
inline OutIt print_doctype_node(OutIt out, Node<Ch> *node, int indent) {
    assert(node->type() == Node<Ch>::NODE_DOCTYPE);
    if (!print_no_indenting)
        out = fill_chars(out, indent, Ch('\t'));
    *out = Ch('<'), ++out;
    *out = Ch('!'), ++out;
    *out = Ch('D'), ++out;
    *out = Ch('O'), ++out;
    *out = Ch('C'), ++out;
    *out = Ch('T'), ++out;
    *out = Ch('Y'), ++out;
    *out = Ch('P'), ++out;
    *out = Ch('E'), ++out;
    *out = Ch(' '), ++out;
    out = copy_chars(node->value(), out);
    *out = Ch('>'), ++out;
    return out;
}

// Print pi node
template <class OutIt, class Ch>
inline OutIt print_pi_node(OutIt out, Node<Ch> *node, int indent) {
    assert(node->type() == Node<Ch>::NODE_PI);
    if (!print_no_indenting)
        out = fill_chars(out, indent, Ch('\t'));
    *out = Ch('<'), ++out;
    *out = Ch('?'), ++out;
    out = copy_chars(node->name(), out);
    *out = Ch(' '), ++out;
    out = copy_chars(node->value(), out);
    *out = Ch('?'), ++out;
    *out = Ch('>'), ++out;
    return out;
}

// Print node
template <class OutIt, class Ch>
inline OutIt print_node(OutIt out, Node<Ch> *node, int indent) {
    // Print proper node type
    switch (node->type()) {
        // Element
        case Node<Ch>::NODE_ELEMENT_TEXT:
            out = print_element_text_node(out, node, indent);
            break;
        case Node<Ch>::NODE_ELEMENT_VOID:
            out = print_element_void_node(out, node, indent);
            break;
        case Node<Ch>::NODE_ELEMENT:
            out = print_element_node(out, node, indent);
            break;

        // Data
        case Node<Ch>::NODE_DATA:
            out = print_data_node(out, node, indent);
            break;

        // CDATA
        case Node<Ch>::NODE_CDATA:
            out = print_cdata_node(out, node, indent);
            break;

        // Declaration
        case Node<Ch>::NODE_DECLARATION:
            out = print_declaration_node(out, node, indent);
            break;

        // Comment
        case Node<Ch>::NODE_COMMENT:
            out = print_comment_node(out, node, indent);
            break;

        // Doctype
        case Node<Ch>::NODE_DOCTYPE:
            out = print_doctype_node(out, node, indent);
            break;

        // Pi
        case Node<Ch>::NODE_PI:
            out = print_pi_node(out, node, indent);
            break;

        // Document
        case Node<Ch>::NODE_DOCUMENT:
            out = print_children(out, node, indent);
            break;

            // Unknown
        default:
            assert(0);
            break;
    }

    // If indenting not disabled, add line break after node
    if (!print_no_indenting && node->type() != Node<Ch>::NODE_DOCUMENT)
        *out = Ch('\n'), ++out;

    // Return modified iterator
    return out;
}

// Print children of the node
template <class OutIt, class Ch>
inline OutIt print_children(OutIt out, Node<Ch> *node, int indent) {
    if (node->name() == String<Ch>("html", 4))
        indent = 0;
    for (auto it = node->child_begin(); it != node->child_end(); ++it) {
        out = print_node(out, *it, indent);
    }
    return out;
}

}  // namespace internal

///////////////////////////////////////////////////////////////////////////
// Printing

//! Prints XML to given output iterator.
//! \param out Output iterator to print to.
//! \param node Node to be printed. Pass xml_document to print entire document.
//! \param flags Flags controlling how XML is printed.
//! \return Output iterator pointing to position immediately after last
//! character of printed text.
template <class OutIt, class Ch>
inline OutIt print(OutIt out, Node<Ch> &node) {
    return internal::print_node(out, &node, 0);
}

//! Prints XML to given output stream.
//! \param out Output stream to print to.
//! \param node Node to be printed. Pass xml_document to print entire document.
//! \param flags Flags controlling how XML is printed.
//! \return Output stream.
template <class Ch>
inline std::basic_ostream<Ch> &print(std::basic_ostream<Ch> &out, Node<Ch> &node) {
    print(std::ostream_iterator<Ch>(out), node);
    return out;
}

//! Prints formatted XML to given output stream. Uses default printing flags.
//! Use print() function to customize printing process. \param out Output stream
//! to print to. \param node Node to be printed. \return Output stream.
template <class Ch>
inline std::basic_ostream<Ch> &operator<<(std::basic_ostream<Ch> &out, Node<Ch> &node) {
    return print(out, node);
}

}  // namespace nvparsehtml

#endif
