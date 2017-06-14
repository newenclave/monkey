#ifndef PARSER_H
#define PARSER_H

#include <vector>

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

        static
        token_itr next_itr( token_itr src, token_itr end )
        {
            if( src != end && src->name != type::END_OF_FILE ) {
                ++src;
            }
            return src;
        }

        token_reader( tokens_list tok )
            :tokens_(std::move(tok))
            ,current_(tokens_.begin( ))
            ,peek_(next_itr(current_, tokens_.end( )))
        { }

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

        std::unique_ptr<ast::identifier> parse_ident( )
        {
            std::unique_ptr<ast::identifier> res(new ast::identifier);
            res->value = current( ).literal;
            return std::move(res);
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

            if(current_is( type::SEMICOLON )) {
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

            if(current_is( type::SEMICOLON )) {
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
                    advance( );
                    break;
                }
                if( stmt ) {
                    res.states.emplace_back( std::move(stmt) );
                }
            }

            return res;
        }

        tokens_list tokens_;
        token_itr   current_;
        token_itr   peek_;
        mutable std::vector<std::string> errors_;
    };

}}

#endif // PARSER_H
