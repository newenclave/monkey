#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <cstdint>

#include "etool/trees/trie/base.h"

namespace mico { namespace lexer {

    struct tokens {

        enum class type {
             ILLEGAL = 0
            ,END
            // Identifiers + literals
            ,IDENT
            ,INT
            ,INT_BIN
            ,INT_OCT
            ,INT_HEX

            // Operators
            ,ASSIGN
            ,PLUS
            ,MINUS

            // Delimiters
            ,COMMA
            ,SEMICOLON
            ,LPAREN
            ,RPAREN
            ,LBRACE
            ,RBRACE

            // Keywords
            ,LET
            ,FUNCTION
        };

        static
        const char *type2name( type t )
        {
            switch (t) {
            case type::ILLEGAL:
                return "ILLEGAL";
            case type::END:
                return "EOF";
            case type::IDENT:
                return "IDENT";
            case type::INT:
            case type::INT_BIN:
            case type::INT_OCT:
            case type::INT_HEX:
                return "INT";
            case type::ASSIGN:
                return "=";
            case type::PLUS:
                return "+";
            case type::MINUS:
                return "-";
            case type::COMMA:
                return ",";
            case type::SEMICOLON:
                return ";";
            case type::LPAREN:
                return "(";
            case type::RPAREN:
                return ")";
            case type::LBRACE:
                return "{";
            case type::RBRACE:
                return "}";
            case type::LET:
                return "LET";
            case type::FUNCTION:
                return "FUN";
            }
            return "none";
        }

        struct info {

            info( ) = default;
            info( const info & ) = default;
            info( info && ) = default;
            info& operator = ( const info & ) = default;
            info& operator = ( info && ) = default;

            info( type n, std::string v = "" )
                :name(n)
                ,value(std::move(v))
            { }

            type        name = type::ILLEGAL;
            std::string value;

        };

        using table = etool::trees::trie::base<char, info>;

        static
        void add_token( table &tt, std::string name, info inf )
        {
            tt.set(name.begin( ), name.end( ), std::move(inf));
        }

        static
        bool is_space( char c )
        {
            return c == ' '
                || c == '\t'
                || c == '\n'
                || c == '\r'
                 ;
        }

        static
        bool is_digin02( char c, bool use_under = false )
        {
            switch (c) {
            case '0':
            case '1':
                return true;
            case '_':
                return use_under;
            }
            return false;
        }

        static
        bool is_digin08( char c, bool use_under = false )
        {
            switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                return true;
            case '_':
                return use_under;
            }
            return false;
        }

        static
        bool is_digin10( char c, bool use_under = false )
        {
            switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return true;
            case '_':
                return use_under;
            }
        }

        static
        bool is_digin16( char c, bool use_under = false )
        {
            switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':

            case 'a': case 'A':
            case 'b': case 'B':
            case 'c': case 'C':
            case 'd': case 'D':
            case 'e': case 'E':
            case 'f': case 'F':
                return true;
            case '_':
                return use_under;
            }
        }

        static
        bool is_ident( char c )
        {
            return (c >= 'a' &&  c <= 'z')
                || (c >= 'A' &&  c <= 'Z')
                || (c == '_')
                 ;
        }

        static
        table all( )
        {
            table res;

            add_token( res, "let",  info(type::LET) );
            add_token( res, "fun",  info(type::FUNCTION) );
            add_token( res, "=",    info(type::ASSIGN) );
            add_token( res, "+",    info(type::PLUS) );
            add_token( res, "-",    info(type::MINUS) );
            add_token( res, ",",    info(type::COMMA) );
            add_token( res, ";",    info(type::SEMICOLON) );
            add_token( res, "(",    info(type::LPAREN) );
            add_token( res, ")",    info(type::RPAREN) );
            add_token( res, "{",    info(type::LBRACE) );
            add_token( res, "}",    info(type::RBRACE) );

            add_token( res, "0b",   info(type::INT_BIN) );
            add_token( res, "0",    info(type::INT_OCT) );
            add_token( res, "0x",   info(type::INT_HEX) );

            return std::move(res);
        }


        template <typename IterT>
        static
        IterT skip_space( IterT begin, IterT end )
        {
            while( (begin != end) && is_space( *begin ) ) {
                ++begin;
            }
            return begin;
        }

        template <typename IterT>
        static
        std::pair<info, IterT> next_token( table &t, IterT begin, IterT end )
        {
            if( begin == end ) {
                return std::make_pair(info(type::END), end);
            } else {
                auto next = t.get( begin, end, true );
                if( next ) {
                    switch (next->name) {
                    case type::INT_BIN:
                    case type::INT_HEX:
                    case type::INT_OCT:

                        break;
                    default:
                        break;
                    }
                    return std::make_pair( *next, next.iterator( ));
                } else {
                    std::cout << *begin;
                }
            }

            return std::make_pair(info( ), begin);
        }

    };

} }

#endif // LEXER_H
