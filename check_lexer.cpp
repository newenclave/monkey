#include "catch/catch.hpp"
#include "lexer.h"

TEST_CASE( "mico", "[lexer]" ) {
    SECTION( "Test mico program" ) {

        std::string test = "let five = 5;\n"
                "let ten = 10;\n"
                "let add = fn(x, y) {\n"
                "x + y;\n"
                "};\n"
                "let result = add(five, ten);\n";

        auto tt = mico::lexer::tokens::all( );

        std::vector<std::string> results = {
             "LET"              ,
            "IDENT(five)"       ,
            "="                 ,
            "INT(5)"            ,
            ";"                 ,
            "LET"               ,
            "IDENT(ten)"        ,
            "="                 ,
            "INT(10)"           ,
            ";"                 ,
            "LET"               ,
            "IDENT(add)"        ,
            "="                 ,
            "FUN"               ,
            "("                 ,
            "IDENT(x)"          ,
            ","                 ,
            "IDENT(y)"          ,
            ")"                 ,
            "{"                 ,
            "IDENT(x)"          ,
            "+"                 ,
            "IDENT(y)"          ,
            ";"                 ,
            "}"                 ,
            ";"                 ,
            "LET"               ,
            "IDENT(result)"     ,
            "="                 ,
            "IDENT(add)"        ,
            "("                 ,
            "IDENT(five)"       ,
            ","                 ,
            "IDENT(ten)"        ,
            ")"                 ,
            ";"                 ,
            "EOF"               ,
        };
        auto lst = mico::lexer::tokens::get_list( tt,
                                                  test.begin( ), test.end( ) );

        REQUIRE( lst.size( ) == results.size( ) );
        size_t id = 0;

        for( auto &token: lst ) {
            REQUIRE( token.to_string( ) == results[id] );
            id++;
        }

    }
}
