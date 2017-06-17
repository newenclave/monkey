#ifndef AST_H
#define AST_H

#include <memory>

#include "lexer.h"

namespace mico { namespace ast {

    enum class node_type {
        NONE = 0,
        STATE,
        EXPR,
        STATE_IDENT,
        STATE_LET,
        STATE_RETURN,
        STATE_EXPR,

        EXPRESSION_IDENT,
        EXPRESSION_INT,
        EXPRESSION_PREFIX,
        EXPRESSION_INFIX,
    };

    struct node {

        using uptr = std::unique_ptr<node>;

        virtual ~node( ) { }
        virtual node_type type( ) const
        {
            return node_type::NONE;
        }

        virtual bool is_statement( ) const
        {
            return false;
        }

        virtual bool is_expression( ) const
        {
            return false;
        }

        virtual std::string literal( ) const = 0;
        virtual std::string to_string( ) const = 0;
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
        std::string to_string( ) const
        {
            return "<statement>";
        }

        virtual bool is_statement( ) const
        {
            return true;
        }

    };

    struct expression: public node {

        using uptr = std::unique_ptr<expression>;

        virtual node_type type( ) const
        {
            return node_type::EXPR;
        }

        std::string to_string( ) const
        {
            return "<expression>";
        }

        virtual bool is_expression( ) const
        {
            return true;
        }

    };

    struct ident_statement: public statement {
        node_type type( ) const
        {
            return node_type::STATE_IDENT;
        }
        lexer::tokens::type token( ) const
        {
            return lexer::tokens::type::IDENT;
        }

        std::string to_string( ) const
        {
            return value;
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

        std::string to_string( ) const
        {
            std::ostringstream oss;
            oss << literal( ) << " " << ident->to_string( )
                           << " = "
                           << (expr ? expr->to_string( ) : "<nill>") << ";";
            return oss.str( );
        }

        std::unique_ptr<ident_statement> ident;
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

        std::string to_string( ) const
        {
            std::ostringstream oss;
            oss << literal( ) << " "
                << (expr ? expr->to_string( ) : "<nill>") << ";";
            return oss.str( );
        }

        expression::uptr expr;
    };

    struct expr_statement: public statement {

        node_type type( ) const
        {
            return node_type::STATE_EXPR;
        }

        lexer::tokens::type token( ) const
        {
            return lexer::tokens::type::SEMICOLON;
        }

        std::string to_string( ) const
        {
            return expr->to_string( );
        }

        expression::uptr expr;
    };

    struct ident_expression: public expression {

        node_type type( ) const
        {
            return node_type::EXPRESSION_IDENT;
        }

        std::string literal( ) const
        {
            return value;
        }

        std::string to_string( ) const
        {
            return value;
        }

        std::string value;
    };

    struct int_expression: public expression {

        node_type type( ) const
        {
            return node_type::EXPRESSION_INT;
        }

        std::string literal( ) const
        {
            std::ostringstream oss;
            oss << value;
            return oss.str( );
        }

        std::string to_string( ) const
        {
            return literal( );
        }

        std::int64_t value;
    };

    struct prefix_expression: public expression {
        node_type type( ) const
        {
            return node_type::EXPRESSION_PREFIX;
        }

        std::string literal( ) const
        {
            std::ostringstream oss;
            oss << "(" << token << expr->to_string( ) << ")";
            return oss.str( );
        }

        std::string to_string( ) const
        {
            return literal( );
        }

        lexer::tokens::type token;
        expression::uptr    expr;
    };

    struct infix_expression: public expression {
        node_type type( ) const
        {
            return node_type::EXPRESSION_INFIX;
        }

        std::string literal( ) const
        {
            std::ostringstream oss;
            oss << "("
                << left->to_string( )
                << token
                << right->to_string( )
                << ")";
            return oss.str( );
        }

        std::string to_string( ) const
        {
            return literal( );
        }

        expression::uptr    left;
        lexer::tokens::type token;
        expression::uptr    right;
    };
}}


#endif // AST_H
