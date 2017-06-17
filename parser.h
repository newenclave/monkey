#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <functional>

#include "lexer.h"
#include "ast.h"

namespace mico { namespace parser {


    struct program {
        std::vector<ast::statement::uptr> states;
    };

    struct token_reader {

        using tokens_list = std::vector<lexer::tokens::info>;
        using token_itr = tokens_list::iterator;

        using statement_ptr = ast::statement::uptr;

        using type = lexer::tokens::type;

        using prefix_parser_fun = std::function<ast::expression::uptr( )>;
        using postfix_parser_fun =
                  std::function<ast::expression::uptr(ast::expression::uptr)>;


        using prefix_call_map  = std::map<type, prefix_parser_fun>;
        using postfix_call_map = std::map<type, postfix_parser_fun>;

        enum class precedence {
             LOWEST = 0
            ,EQUALS
             // ==
            ,LESSGREATER // > or <
            ,SUM // +
            ,PRODUCT // *
            ,PREFIX // -X or !X
            ,CALL // myFunction(X)
        };
        using precedence_map = std::map<type, precedence>;

        static
        token_itr next_itr( token_itr src, token_itr end )
        {
            if( src != end && src->name != type::END_OF_FILE ) {
                ++src;
            }
            return src;
        }

        static
        std::int64_t char2value( char c )
        {
            switch (c) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                return std::int64_t(c - '0');
            case 'a': case 'b': case 'c':
            case 'd': case 'e': case 'f':
                return std::int64_t(c - 'a' + 10);
            case 'A': case 'B': case 'C':
            case 'D': case 'E': case 'F':
                return std::int64_t(c - 'A' + 10);
            default:
                break;
            }
            return -1;
        }

        int intbase2int( type t )
        {
            switch (t) {
            case type::INT_OCT:
                return 8;
            case type::INT_BIN:
                return 2;
            case type::INT_HEX:
                return 16;
            default:
                break;
            }
            return 10;
        }

        static
        std::int64_t parse_int( std::string data, int s = 10 )
        {
            std::int64_t res = 0;
            for( auto c: data ) {
                res *= s;
                auto next = char2value( c );
                if( next < 0 ) {
                    return -1;
                }
                res += next;
            }
            return res;
        }

        token_reader( tokens_list tok )
            :tokens_(std::move(tok))
            ,current_(tokens_.begin( ))
            ,peek_(next_itr(current_, tokens_.end( )))
        {
            prefix_calls_[type::IDENT] = [this]( ){
                return parse_ident_expression( );
            };

            prefix_calls_[type::INT] = [this]( ){
                return parse_int_expression( );
            };
            prefix_calls_[type::INT_BIN] = [this]( ){
                return parse_int_expression( );
            };
            prefix_calls_[type::INT_OCT] = [this]( ){
                return parse_int_expression( );
            };
            prefix_calls_[type::INT_HEX] = [this]( ){
                return parse_int_expression( );
            };

            prefix_calls_[type::MINUS] = [this]( ) {
                return parse_prefix( );
            };
            prefix_calls_[type::BANG] = [this]( ) {
                return parse_prefix( );
            };
            prefix_calls_[type::PLUS] = [this]( ) {
                return parse_prefix( );
            };

            precedences_[type::EQ]       = precedence::EQUALS;
            precedences_[type::NOT_EQ]   = precedence::EQUALS;

            precedences_[type::LT]       = precedence::LESSGREATER;
            precedences_[type::GT]       = precedence::LESSGREATER;

            precedences_[type::PLUS]     = precedence::SUM;
            precedences_[type::MINUS]    = precedence::SUM;

            precedences_[type::SLASH]    = precedence::PRODUCT;
            precedences_[type::ASTERISK] = precedence::PRODUCT;

            postfix_call_[type::PLUS] = [this](ast::expression::uptr expr) {
                return parse_postfix( std::move(expr) );
            };
            postfix_call_[type::MINUS] = [this](ast::expression::uptr expr) {
                return parse_postfix( std::move(expr) );
            };
            postfix_call_[type::SLASH] = [this](ast::expression::uptr expr) {
                return parse_postfix( std::move(expr) );
            };
            postfix_call_[type::ASTERISK] = [this](ast::expression::uptr expr) {
                return parse_postfix( std::move(expr) );
            };
            postfix_call_[type::EQ] = [this](ast::expression::uptr expr) {
                return parse_postfix( std::move(expr) );
            };
            postfix_call_[type::NOT_EQ] = [this](ast::expression::uptr expr) {
                return parse_postfix( std::move(expr) );
            };
            postfix_call_[type::LT] = [this](ast::expression::uptr expr) {
                return parse_postfix( std::move(expr) );
            };
            postfix_call_[type::GT] = [this](ast::expression::uptr expr) {
                return parse_postfix( std::move(expr) );
            };

        }

        precedence cur_precedence( )
        {
            auto f = precedences_.find( current( ).name );
            if( f != precedences_.end( ) ) {
                return f->second;
            }
            return precedence::LOWEST;
        }

        precedence peek_precedence( )
        {
            auto f = precedences_.find( peek( ).name );
            if( f != precedences_.end( ) ) {
                return f->second;
            }
            return precedence::LOWEST;
        }

        const lexer::tokens::info &current( ) const
        {
            return *current_;
        }

        const lexer::tokens::info &peek( ) const
        {
            return *peek_;
        }

        bool current_is( type t ) const
        {
            return ( current_ != tokens_.end( ))
                && ( current_->name == t )
                 ;
        }

        bool peek_is( type t ) const
        {
            return ( peek_ != tokens_.end( ))
                && ( peek_->name == t )
                 ;
        }

        bool expect_peek( type t )
        {
            if( peek_is(t) ) {
                advance( );
                return true;
            } else {
                std::ostringstream oss;
                oss << "Expected '" << t
                    << "' but got '" << peek( ).name << "' ("
                    << peek( ).to_string( ) << ")";
                errors_.push_back( oss.str( ) );
                return false;
            }
        }

        void advance( )
        {
            current_ = peek_;
            peek_    = next_itr(peek_, tokens_.end( ));
        }

        bool eof( ) const
        {
            return ( current_       == tokens_.end( ) )
                || ( current_->name == type::END_OF_FILE )
                 ;
        }

        ast::expression::uptr parse_int_expression( )
        {
            std::unique_ptr<ast::int_expression>
                                res(new ast::int_expression);

            res->value = parse_int( current( ).literal,
                                    intbase2int( current( ).name ) );

            return res;
        }

        ast::expression::uptr parse_ident_expression( )
        {
            std::unique_ptr<ast::ident_expression>
                                res(new ast::ident_expression);

            res->value = current( ).literal;

            return res;
        }

        ast::expression::uptr parse_expression( precedence p )
        {
            auto pref_call = prefix_calls_.find( current( ).name );
            if( pref_call == prefix_calls_.end( ) ) {
                return std::unique_ptr<ast::expression>( );
            }

            auto left = pref_call->second( );
            while( (peek( ).name != type::SEMICOLON) && (p < peek_precedence( )) ) {
                auto infix = postfix_call_.find( peek( ).name );
                if( infix != postfix_call_.end( ) ) {
                    advance( );
                    left = infix->second(std::move(left));
                } else {
                    return left;
                }
            }

            return left;
        }

        std::unique_ptr<ast::expr_statement>
        parse_state_expression( precedence p )
        {
            std::unique_ptr<ast::expr_statement> res(new ast::expr_statement);
            res->expr = parse_expression( p );
            advance( );
            return res;
        }

        std::unique_ptr<ast::ident_statement> parse_ident( )
        {
            std::unique_ptr<ast::ident_statement> res(new ast::ident_statement);
            res->value = current( ).literal;
            return std::move(res);
        }

        std::unique_ptr<ast::prefix_expression> parse_prefix( )
        {
            std::unique_ptr<ast::prefix_expression>
                    res(new ast::prefix_expression);

            res->token = current( ).name;
            advance( );
            res->expr  = parse_expression( precedence::PREFIX );

            return res;
        }

        std::unique_ptr<ast::infix_expression>
        parse_postfix( ast::expression::uptr left )
        {
            std::unique_ptr<ast::infix_expression>
                    res(new ast::infix_expression);

            res->left = std::move(left);
            res->token = current( ).name;

            auto preced = cur_precedence( );
            advance( );
            res->right  = parse_expression( preced );

            return res;
        }

        std::unique_ptr<ast::let_statement> parse_let( )
        {
            advance( );
            if( !current_is(type::IDENT) ) {
                std::ostringstream oss;
                oss << "IDENT not found in LET statement; "
                    << current( ).to_string( ) << " found";
                errors_.push_back( oss.str( ) );
                return std::unique_ptr<ast::let_statement>( );
            }

            std::unique_ptr<ast::let_statement> res(new ast::let_statement);
            res->ident = parse_ident( );

            if( !expect_peek( type::ASSIGN ) ) {
                return std::unique_ptr<ast::let_statement>( );
            }

            while( !eof( ) && !current_is( type::SEMICOLON ) ) {
                advance( );
            }

            return res;
        }

        std::unique_ptr<ast::return_statement> parse_return( )
        {
            advance( );
            std::unique_ptr<ast::return_statement>
                    res(new ast::return_statement);
            while( !eof( ) && !current_is( type::SEMICOLON ) ) {
                advance( );
            }
            return res;
        }

        program parse( )
        {
            program res;

            while( !eof( ) ) {
                statement_ptr stmt;
                switch( current( ).name ) {
                case type::LET:
                    stmt = parse_let( );
                    break;
                case type::RETURN:
                    stmt = parse_return( );
                    break;
                default:
                    stmt = parse_state_expression( precedence::LOWEST );
                    break;
                }
                if( stmt ) {
                    res.states.emplace_back( std::move(stmt) );
                }
                advance( );
            }

            return res;
        }

        tokens_list tokens_;
        token_itr   current_;
        token_itr   peek_;
        mutable std::vector<std::string> errors_;
        prefix_call_map  prefix_calls_;
        postfix_call_map postfix_call_;
        precedence_map   precedences_;
    };

}}

#endif // PARSER_H
