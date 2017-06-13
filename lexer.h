#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
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
                ,literal(std::move(v))
            { }

            std::string to_string( )
            {
                std::string res(type2name(name));
                if( !literal.empty( ) ) {
                    res = res + "(" + literal + ")";
                }
                return res;
            }

            type        name = type::ILLEGAL;
            std::string literal;

        };

        using table = etool::trees::trie::base<char, type>;

        static
        bool is_space( char c )
        {
            return ( c == ' '  )
                || ( c == '\t' )
                || ( c == '\n' )
                || ( c == '\r' )
                 ;
        }

        static
        bool is_digit02( char c, bool use_under = false )
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
        bool is_digit08( char c, bool use_under = false )
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
        bool is_digit10( char c, bool use_under = false )
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
            return false;
        }

        static
        bool is_digit16( char c, bool use_under = false )
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
            return false;
        }

        template <typename ItrT>
        static
        std::string read_number( type num_type, ItrT &itr, ItrT end )
        {
            typedef bool (*num_check)(char, bool);
            num_check chker = nullptr;
            switch (num_type) {
            case type::INT:
                chker = &is_digit10;
                break;
            case type::INT_BIN:
                chker = &is_digit02;
                break;
            case type::INT_OCT:
                chker = &is_digit08;
                break;
            case type::INT_HEX:
                chker = &is_digit16;
                break;
            default:
                break;
            }
            std::string res;

            for(; (itr != end) && chker(*itr, true); ++itr) {
                if( *itr != '_' ) {
                    res.push_back( *itr );
                }
            }

            return res;
        }

        static
        bool is_ident( char c )
        {
            return (c >= 'a' &&  c <= 'z')
                || (c >= 'A' &&  c <= 'Z')
                || is_digit10( c, true );
                 ;
        }

        template <typename ItrT>
        static
        std::string read_ident( ItrT &itr, ItrT end )
        {
            std::string res;
            for(; (itr != end) && is_ident( *itr ); ++itr) {
                res.push_back( *itr );
            }
            return res;
        }

        static
        void add_token( table &tt, std::string name, type inf )
        {
            tt.set(name.begin( ), name.end( ), inf);
        }

        static
        table all( )
        {
            table res;

            add_token( res, "let",  type::LET       );
            add_token( res, "fn",   type::FUNCTION  );
            add_token( res, "=",    type::ASSIGN    );
            add_token( res, "+",    type::PLUS      );
            add_token( res, "-",    type::MINUS     );
            add_token( res, ",",    type::COMMA     );
            add_token( res, ";",    type::SEMICOLON );
            add_token( res, "(",    type::LPAREN    );
            add_token( res, ")",    type::RPAREN    );
            add_token( res, "{",    type::LBRACE    );
            add_token( res, "}",    type::RBRACE    );
            add_token( res, "0b",   type::INT_BIN   );
            add_token( res, "0",    type::INT_OCT   );
            add_token( res, "0x",   type::INT_HEX   );

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
                    std::string value;
                    auto bb = next.iterator( );
                    switch (*next) {
                    case type::INT_BIN:
                    case type::INT_HEX:
                    case type::INT_OCT:
                        value = read_number( *next, bb, end );
                        break;
                    default:
                        break;
                    }
                    return std::make_pair( info(*next, value), bb );
                } else if( is_ident( *begin ) ){
                    auto bb = begin;
                    std::string value = read_ident( bb, end);
                    return std::make_pair( info(type::IDENT, value), bb );
                } else if( is_digit10( *begin, false ) ) {
                    auto bb = begin;
                    std::string value = read_number( type::INT, bb, end);
                    return std::make_pair( info(type::INT, value), bb );
                } else {
                    //std::cout << *begin;
                }
            }

            return std::make_pair( info( ), begin);
        }

        template <typename IterT>
        static
        std::vector<info> get_list( table &t, IterT begin, IterT end )
        {
            std::vector<info> res;

            while( begin != end ) {
                auto next = next_token( t, begin, end );
                if( next.first.name == type::ILLEGAL ) {
                    res.emplace_back( std::move(next.first) );
                    begin = end;
                } else {
                    res.emplace_back( std::move(next.first) );
                    begin = skip_space(next.second, end);
                }
            }

            res.emplace_back( info(type::END) );

            return res;
        }
    };

    inline
    std::ostream &operator << (std::ostream &o, tokens::type tt)
    {
        o << tokens::type2name( tt );
        return o;
    }

} }

#endif // LEXER_H
