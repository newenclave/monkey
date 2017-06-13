#ifndef AST_H
#define AST_H

#include "lexer.h"

namespace mico { namespace ast {

    enum class node_type {
        NONE = 0,
        STATE,
        EXPR,
    };

    struct node {
        virtual ~node( ) { }
        virtual node_type type( ) const
        {
            return node_type::NONE;
        }
        virtual std::string literal( ) const = 0;
    };

    struct statement: public node {
        virtual node_type type( ) const
        {
            return node_type::STATE;
        }
    };

    struct expression: public node {
        virtual node_type type( ) const
        {
            return node_type::EXPR;
        }
    };

}}


#endif // AST_H
