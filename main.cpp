
#define CATCH_CONFIG_RUNNER

#include <iostream>

#include "etool/trees/trie/base.h"
#include "catch/catch.hpp"

#include "lexer.h"

#include "parser.h"

using namespace mico;


int main(int argc, char *argv[])
{

    std::string input =
            "let x = 5;             "
            "let y = 0x10 ;            "
            "let octo = 037373;   "
            "return add(100);"
            "foobar;"
            "1000;"
            "01000;"
            "0xFFFFFFFF;"
            "return 5+6;"
            ;
    auto tt  = lexer::tokens::all( );
    auto lst = lexer::tokens::get_list( tt, input.begin( ), input.end( ) );

    parser::token_reader token_reader(std::move(lst));
    auto prog = token_reader.parse( );


    for( auto &e: token_reader.errors_ ) {
        std::cout << e << "\n";
    }

    for( auto &l: prog.states ) {
        std::cout << l->token( )
                  << " " << l->to_string( ) << "\n";
    }

//    int result = Catch::Session( ).run( argc, argv );
//    return ( result < 0xff ? result : 0xff );

    return 0;
}
