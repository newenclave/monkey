#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <cstdint>

#include "etool/trees/trie/base.h"

namespace mico { namespace lexer {

    struct tokens {

        enum class type: std::uint16_t {
             ILLEGAL = 0
            ,END_OF_FILE
            // Identifiers + literals
            ,IDENT
            ,INT
            ,INT_BIN
            ,INT_OCT
            ,INT_HEX
            ,STRING

            ,FIRST_VALUE_TOKEN = 100

            // Operators
            ,ASSIGN
            ,PLUS
            ,MINUS
            ,BANG
            ,ASTERISK
            ,SLASH

            ,LT
            ,GT
            ,EQ
            ,NOT_EQ

            // Delimiters
            ,COMMA
            ,SEMICOLON
            ,COLON

            ,LPAREN
            ,RPAREN
            ,LBRACE
            ,RBRACE
            ,LBRACKET
            ,RBRACKET

            // Keywords
            ,LET
            ,FUNCTION
            ,TRUE
            ,FALSE
            ,IF
            ,ELSE
            ,RETURN

            ,LAST_VALUE_TOKEN
        };

        static
        const char *type2name( type t )
        {
            switch (t) {

            case type::ILLEGAL:
                return "ILLEGAL";
            case type::END_OF_FILE:
                return "EOF";

            case type::IDENT:
                return "IDENT";
            case type::STRING:
                return "STRING";

            case type::INT:
            case type::INT_BIN:
            case type::INT_OCT:
            case type::INT_HEX:
                return "INT";

            case type::LT:
                return "<";
            case type::GT:
                return ">";
            case type::EQ:
                return "==";
            case type::NOT_EQ:
                return "!=";

            case type::ASSIGN:
                return "=";
            case type::PLUS:
                return "+";
            case type::MINUS:
                return "-";

            case type::BANG:
                return "!";
            case type::ASTERISK:
                return "*";
            case type::SLASH:
                return "/";

            case type::COMMA:
                return ",";
            case type::SEMICOLON:
                return ";";
            case type::COLON:
                return ":";

            case type::LPAREN:
                return "(";
            case type::RPAREN:
                return ")";
            case type::LBRACE:
                return "{";
            case type::RBRACE:
                return "}";
            case type::LBRACKET:
                return "[";
            case type::RBRACKET:
                return "]";

            case type::LET:
                return "let";
            case type::FUNCTION:
                return "fn";
            case type::TRUE:
                return "true";
            case type::FALSE:
                return "false";
            case type::IF:
                return "if";
            case type::ELSE:
                return "else";
            case type::RETURN:
                return "return";

            ///  avoid warnings
            case type::FIRST_VALUE_TOKEN:
            case type::LAST_VALUE_TOKEN:
                break;
            }
            return "none";
        }

        struct info {

            info( ) = default;
            info( const info & ) = default;
            info( info && ) = default;
            info& operator = ( const info & ) = default;
            info& operator = ( info && ) = default;

            info( type n )
                :name(n)
                ,literal(type2name(n))
            { }

            info( type n, std::string v )
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
            return ( 'a' <= c &&  c <= 'z')
                || ( 'A' <= c &&  c <= 'Z')
                || ( c == '_' );
                 ;
        }

        static
        bool is_ident_( char c )
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
            for(; (itr != end) && is_ident_( *itr ); ++itr) {
                res.push_back( *itr );
            }
            return res;
        }

        template <typename ItrT>
        static
        std::string read_string( ItrT &itr, ItrT end )
        {
            std::string res;

            for( ; (itr != end) && (*itr != '"'); itr++ ) {
                auto next = std::next(itr);
                if( *itr == '\\' && next != end ) {
                    itr = next;
                }
                res.push_back( *itr );
            }

            if( itr != end ) {
                ++itr;
            }

            return res;
        }

        static
        void add_token( table &tt, type inf )
        {
            tt.set( std::string(type2name(inf) ), inf);
        }

        static
        void add_token( table &tt, type inf, std::string val )
        {
            tt.set( val, inf );
        }

        static
        table all( )
        {
            table res;

            static const
            auto fvt = static_cast<std::uint16_t>(type::FIRST_VALUE_TOKEN);

            static const
            auto lvt = static_cast<std::uint16_t>(type::LAST_VALUE_TOKEN);

            for( auto i = fvt + 1; i < lvt; ++i ) {
                add_token( res, static_cast<type>(i) );
            }

            add_token( res, type::STRING,  "\""   );

            add_token( res, type::INT_BIN, "0b"   );
            add_token( res, type::INT_OCT, "0"    );
            add_token( res, type::INT_HEX, "0x"   );

            return std::move(res);
        }

        template <typename IterT>
        static
        IterT skip_whitespaces( IterT begin, IterT end )
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
                return std::make_pair(info(type::END_OF_FILE, "EOF"), end);
            } else {
                auto bb = begin;
                auto next = t.get( begin, end, true );
                if( next ) {
                    std::string value = type2name(*next);
                    bb = next.iterator( );
                    switch (*next) {
                    case type::INT_BIN:
                    case type::INT_HEX:
                    case type::INT_OCT:
                        value.assign( begin, bb );
                        value += read_number( *next, bb, end );
                        break;
                    case type::STRING:
                        value = read_string( bb, end );
                        break;
                    default:
                        break;
                    }
                    return std::make_pair( info(*next, value), bb );
                } else if( is_ident( *begin ) ){
                    std::string value = read_ident( bb, end);
                    return std::make_pair( info(type::IDENT, value), bb );
                } else if( is_digit10( *begin, false ) ) {
                    std::string value = read_number( type::INT, bb, end);
                    return std::make_pair( info(type::INT, value), bb );
                } else {

                }
            }

            return std::make_pair( info( ), begin);
        }

        template <typename IterT>
        static
        std::vector<info> get_list( table &t, IterT begin, IterT end )
        {
            std::vector<info> res;

            begin = skip_whitespaces( begin, end );

            while( begin != end ) {
                auto next = next_token( t, begin, end );
                if( next.first.name == type::ILLEGAL ) {
                    res.emplace_back( std::move(next.first) );
                    begin = end;
                } else {
                    res.emplace_back( std::move(next.first) );
                    begin = skip_whitespaces(next.second, end);
                }
            }

            res.emplace_back( info(type::END_OF_FILE) );

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
