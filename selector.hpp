#ifndef NVPARSE_SELECTOR_HPP_INCLUDED
#define NVPARSE_SELECTOR_HPP_INCLUDED

#include <cassert>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>

#include "string.hpp"
#include "text.hpp"

namespace nvparsehtml {
//! Responsible for parsing a CSS selector and storing it as an AST
class Selector {
    //! The selector types
    enum ESELECTOR {
        SELECTOR_TYPE,               //!< element types like div, input, etc.
        SELECTOR_CLASS,              //!< . class names.
        SELECTOR_ID,                 //!< # id names.
        SELECTOR_ATTRIBUTE,          //!< [x='y'] attribute name value
        SELECTOR_OR,                 //!< , will match any of the lhs or rhs results
        SELECTOR_DESCENDANT,         //!< whitespace will match any descendant
        SELECTOR_CHILDREN,           //!< > will match children of lhs to rhs.
        SELECTOR_SIBLING,            //!< ~ matches downstream siblings.
        SELECTOR_SIBLING_ADJACENT,   //!< + matches adjacent downstream sibling only.
        SELECTOR_COL,                //!< || matches nodes belonging to a column.
        SELECTOR_SUEDO_CLASS,        //!< : suedo class info not contained in DOM tree.
        SELECTOR_SUEDO_CLASS_IS,     //!< :is suedo class info not contained in DOM tree.
        SELECTOR_SUEDO_CLASS_NOT,    //!< :not suedo class info not contained in DOM tree.
        SELECTOR_SUEDO_CLASS_WHERE,  //!< :where suedo class info not contained in DOM tree.
        SELECTOR_SUEDO_CLASS_HAS,    //!< :has suedo class info not contained in DOM tree.
        SELECTOR_SUEDO_ELEMENT,      //!< :: suedo elements not in html.
        SELECTOR_ROOT,               //!< denotes a root selector
    };

    enum ATT_OP {
        ATT_OP_HAS,         //!< no operator, no value
        ATT_OP_EQUALS,      //!< = must be exactly value
        ATT_OP_ONE_EQUALS,  //!< ~= one of whitespaced separated values is value
        ATT_OP_HYPHEN,      //!< |= can be exactly value or value-*
        ATT_OP_PREFIX,      //!< ^= prefixed by value
        ATT_OP_SUFFIX,      //!< $= suffixed by value
        ATT_OP_CONTAINS,    //!< *= contains value
    };

    struct Token {
        Token(ESELECTOR t_type)
            : type(t_type),
              left(nullptr),
              right(nullptr),
              sub(nullptr),
              op(ATT_OP_HAS),
              case_insensitive(false) {
        }

        ESELECTOR type;
        std::shared_ptr<Token> left;
        std::shared_ptr<Token> right;
        std::shared_ptr<Token> sub;
        std::string name;
        std::string value;
        ATT_OP op;
        bool case_insensitive;
    };

    std::shared_ptr<Token> m_ptoken;

   public:
    //! Initiatizes the local expression string
    //! \param expression CSS expression to parse as a string.
    Selector(const std::string &expression) : Selector(expression.c_str()) {
    }
    //! Initiatizes the local expression string
    //! \param expression CSS expression to parse as a c-string.
    Selector(const char *expression) {
        char *expr = const_cast<char *>(expression);
        m_ptoken = this->parse_to_list(expr);
    }

   private:
    std::shared_ptr<Token> parse_to_list(char *&expression) {
        assert(expression);
        auto root = std::make_shared<Token>(SELECTOR_ROOT);
        std::shared_ptr<Token> cur_token = root;
        char *text = expression;
        // Skip whitespace at the start
        Text<char>::template skip<whitespace_pred<char>>(text);
        while (true) {
            if (*text == 0) {
                break;
            }
            switch (*text) {
                case ' ':
                    cur_token = this->parse_space(text, cur_token);
                    break;
                case '.':
                    cur_token = this->parse_name(text, cur_token, SELECTOR_CLASS);
                    break;
                case '#':
                    cur_token = this->parse_name(text, cur_token, SELECTOR_ID);
                    break;
                case '[':
                    cur_token = this->parse_attribute(text, cur_token);
                    break;
                case ',':
                    cur_token = this->parse_symbol(text, cur_token, SELECTOR_OR);
                    break;
                case '>':
                    cur_token = this->parse_symbol(text, cur_token, SELECTOR_CHILDREN);
                    break;
                case '~':
                    cur_token = this->parse_symbol(text, cur_token, SELECTOR_SIBLING);
                    break;
                case '+':
                    cur_token = this->parse_symbol(text, cur_token, SELECTOR_SIBLING_ADJACENT);
                    break;
                case '|':
                    cur_token = this->parse_symbol(text, cur_token, SELECTOR_COL);
                    break;
                case ':':
                    cur_token = this->ignore_suedo(text, cur_token);
                    break;
                default:
                    cur_token = this->parse_type(text, cur_token);
            }
        }
        if (cur_token->type == SELECTOR_DESCENDANT) {  // remove whitespace at end
            cur_token = nullptr;
        }
        return root;
    }

    std::shared_ptr<Token> ignore_suedo(char *&text, std::shared_ptr<Token> prev_token) {
        ++text;
        char *start = text;
        if (*text == ':') {
            ++text;
            start = text;
            Text<char>::template skip<selector_text_pred<char>>(text);
            std::string name(start, text - start);
            auto token = std::make_shared<Token>(SELECTOR_SUEDO_ELEMENT);
            token->name = name;
            prev_token->right = token;
            token->left = prev_token;
            return token;
        }
        Text<char>::template skip<selector_text_pred<char>>(text);
        if (*text != '(') {
            std::string name(start, text - start);
            auto token = std::make_shared<Token>(SELECTOR_SUEDO_CLASS);
            token->name = name;
            prev_token->right = token;
            token->left = prev_token;
            return token;
        }
        String<char> name(start, text - start);
        name.to_lowercase();
        ESELECTOR type = SELECTOR_SUEDO_CLASS;
        if (name == String<char>("is", 2)) {
            type = SELECTOR_SUEDO_CLASS_IS;
        } else if (name == String<char>("not", 3)) {
            type = SELECTOR_SUEDO_CLASS_NOT;
        } else if (name == String<char>("has", 3)) {
            type = SELECTOR_SUEDO_CLASS_HAS;
        } else if (name == String<char>("where", 5)) {
            type = SELECTOR_SUEDO_CLASS_WHERE;
        } else {
            throw std::runtime_error(std::string("expected :is, :not, :where, :has but got ") +
                                     std::string(text, 10));
        }
        auto token = std::make_shared<Token>(type);
        ++text;
        start = text;
        size_t paren_count = 1;
        while (paren_count > 0) {
            ++text;
            if (*text == 0) {
                throw std::runtime_error("unexpected end of data");
            }
            if (*text == ')') {
                --paren_count;
            } else if (*text == '(') {
                ++paren_count;
            }
        }
        std::string expression(start, text - start);
        Selector selector(expression);
        token->sub = selector.m_ptoken;
        prev_token->right = token;
        token->left = prev_token;
        ++text;
        return token;
    }

    std::shared_ptr<Token> parse_symbol(char *&text,
                                        std::shared_ptr<Token> prev_token,
                                        ESELECTOR type) {
        ++text;
        if (*text == '|') {
            ++text;
        }
        if (prev_token->type == SELECTOR_DESCENDANT) {
            prev_token->type = type;
        }
        return prev_token;
    }

    std::shared_ptr<Token> parse_space(char *&text, std::shared_ptr<Token> prev_token) {
        Text<char>::template skip<whitespace_pred<char>>(text);
        switch (prev_token->type) {
            case SELECTOR_OR:
            case SELECTOR_DESCENDANT:
            case SELECTOR_CHILDREN:
            case SELECTOR_SIBLING:
            case SELECTOR_SIBLING_ADJACENT:
            case SELECTOR_COL:
                return prev_token;
            default:
                break;
        }
        auto token = std::make_shared<Token>(SELECTOR_DESCENDANT);
        prev_token->right = token;
        token->left = prev_token;
        return token;
    }

    std::shared_ptr<Token> parse_type(char *&text, std::shared_ptr<Token> prev_token) {
        char *start = text;
        Text<char>::template skip<selector_text_pred<char>>(text);
        if (text == start) {
            throw std::runtime_error(std::string("expected parseable text here: ") +
                                     std::string(text, 10));
        }
        std::string name(start, text - start);
        auto token = std::make_shared<Token>(SELECTOR_TYPE);
        token->name = name;
        prev_token->right = token;
        token->left = prev_token;
        return token;
    }

    std::shared_ptr<Token> parse_name(char *&text,
                                      std::shared_ptr<Token> prev_token,
                                      ESELECTOR type) {
        ++text;
        if (*text == 0) {
            throw std::runtime_error("unexpected end of data");
        }
        char *start = text;
        Text<char>::template skip<selector_text_pred<char>>(text);
        if (text == start) {
            throw std::runtime_error(std::string("expected parseable text here: ") +
                                     std::string(text, 10));
        }
        std::string name(start, text - start);
        auto token = std::make_shared<Token>(type);
        token->name = name;
        prev_token->right = token;
        token->left = prev_token;
        return token;
    }

    std::shared_ptr<Token> parse_attribute(char *&text, std::shared_ptr<Token> prev_token) {
        ++text;
        if (*text == 0) {
            throw std::runtime_error("unexpected end of data");
        }
        Text<char>::template skip<whitespace_pred<char>>(text);

        char *start = text;
        Text<char>::template skip<target_or_end_pred<char, char(']')>>(text);
        if (text == start) {
            throw std::runtime_error(std::string("expected parseable text here: ") +
                                     std::string(text, 10));
        }
        auto token = std::make_shared<Token>(SELECTOR_ATTRIBUTE);
        String<char> s(start, text - start);
        ++text;  // advance past ']'
        if (s[0] == '=') {
            throw std::runtime_error(std::string("expected parseable text here but got '=': ") +
                                     std::string(text, 10));
        }
        size_t eq_loc = s.length();
        for (size_t i = 1; i < s.length(); ++i) {
            if (s[i] == '=') {
                eq_loc = i;
                break;
            }
        }
        if (eq_loc >= s.length() - 1) {
            char *end = start;
            Text<char>::template skip<selector_text_pred<char>>(end);
            std::string name(start, end - start);
            token->name = name;
            prev_token->right = token;
            token->left = prev_token;
            return token;
        }

        char *val_start = start + eq_loc + 1;
        char *end = val_start;
        Text<char>::template skip<whitespace_pred<char>>(val_start);
        if (*val_start == '"') {
            ++val_start;
            end = val_start;
            Text<char>::template skip<attribute_value_pred<char, char('"')>>(end);
        } else if (*val_start == '\'') {
            ++val_start;
            end = val_start;
            Text<char>::template skip<attribute_value_pred<char, char('\'')>>(end);
        } else {
            end = val_start;
            Text<char>::template skip<selector_text_pred<char>>(end);
        }
        std::string value(val_start, end - val_start);
        token->value = value;
        switch (s[eq_loc - 1]) {
            case '~':
                token->op = ATT_OP_ONE_EQUALS;
                break;
            case '|':
                token->op = ATT_OP_HYPHEN;
                break;
            case '^':
                token->op = ATT_OP_PREFIX;
                break;
            case '$':
                token->op = ATT_OP_SUFFIX;
                break;
            case '*':
                token->op = ATT_OP_CONTAINS;
                break;
            default:
                token->op = ATT_OP_EQUALS;
        }
        end = start;
        Text<char>::template skip<selector_text_pred<char>>(end);
        std::string name(start, end - start);
        token->name = name;
        prev_token->right = token;
        token->left = prev_token;
        return token;
    }
};
}  // namespace nvparsehtml
#endif
