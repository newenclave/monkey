#ifndef AST_H
#define AST_H

#include <memory>

#include "lexer.h"

namespace mico { namespace ast {

    enum class node_type {
        NONE = 0,
        STATE,
        EXPR,
        STATE_LET,
        STATE_RETURN,
    };

    struct node {

        using uptr = std::unique_ptr<node>;

        virtual ~node( ) { }
        virtual node_type type( ) const
        {
            return node_type::NONE;
        }

        virtual std::string literal( ) const = 0;

    };

    struct statement: public node {

        using uptr = std::unique_ptr<statement>;

        node_type type( ) const
        {
            return node_type::STATE;
        }

        virtual lexer::tokens::type token( ) const = 0;

        std::string literal( ) const
        {
            return lexer::tokens::type2name( token( ) );
        }

    };

    struct expression: public node {

        using uptr = std::unique_ptr<expression>;

        virtual node_type type( ) const
        {
            return node_type::EXPR;
        }
    };

    struct identifier: public statement {
        lexer::tokens::type token( ) const
        {
            return lexer::tokens::type::IDENT;
        }
        std::string value;
    };

    struct let_statement: public statement {
        node_type type( ) const
        {
            return node_type::STATE_LET;
        }

        lexer::tokens::type token( ) const
        {
            return lexer::tokens::type::LET;
        }
        std::unique_ptr<identifier> ident;
        std::unique_ptr<expression> expr;
    };

    struct return_statement: public statement {

        node_type type( ) const
        {
            return node_type::STATE_RETURN;
        }

        lexer::tokens::type token( ) const
        {
            return lexer::tokens::type::RETURN;
        }
        std::unique_ptr<expression> expr;
    };

}}


#endif // AST_H
