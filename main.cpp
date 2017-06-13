
#define CATCH_CONFIG_RUNNER

#include <iostream>

#include "etool/trees/trie/base.h"
#include "catch/catch.hpp"

#include "lexer.h"

using namespace mico;

int main(int argc, char *argv[])
{

    int result = Catch::Session( ).run( argc, argv );
    return ( result < 0xff ? result : 0xff );

    return 0;
}
