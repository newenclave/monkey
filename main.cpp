#include <iostream>

#include "etool/trees/trie/base.h"

#include "lexer.h"

int main(int argc, char *argv[])
{
    std::string test = "!let x = 1";

    auto tt = mico::lexer::tokens::all( );

    auto res = mico::lexer::tokens::next_token(tt, test.begin( ), test.end( ) );

    std::cout << mico::lexer::tokens::type2name(res.first.name) << "\n";

    return 0;
}
