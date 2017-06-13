#include "catch/catch.hpp"
#include "lexer.h"

using namespace mico;

TEST_CASE( "mico", "[lexer]" ) {

    auto tt = lexer::tokens::all( );

    struct expect {
        lexer::tokens::type t;
        std::string         literal;
    };

    SECTION( "Test mico program", "[1]" ) {

        std::string input =
        "   let five = 5;                  "
        "   let ten = 10;                  "
        "                                  "
        "   let add = fn(x, y) {           "
        "     x + y;                       "
        "   };                             "
        "                                  "
        "   let result = add(five, ten);   "
        "   !-/*5;                         "
        "   5 < 10 > 5;                    "
        "                                  "
        "   if (5 < 10) {                  "
        "       return true;               "
        "   } else {                       "
        "       return false;              "
        "   }                              "
        "                                  "
        "   10 == 10;                      "
        "   10 != 9;                       "
        "   \"foobar\"                     "
        "   \"foo bar\"                    "
        "   [1, 2];                        "
        "   {\"foo\": \"bar\"}             "
        ;

        std::vector<expect> results = {
                { lexer::tokens::type::LET,          "let"      },
                { lexer::tokens::type::IDENT,        "five"     },
                { lexer::tokens::type::ASSIGN,       "="        },
                { lexer::tokens::type::INT,          "5"        },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::LET,          "let"      },
                { lexer::tokens::type::IDENT,        "ten"      },
                { lexer::tokens::type::ASSIGN,       "="        },
                { lexer::tokens::type::INT,          "10"       },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::LET,          "let"      },
                { lexer::tokens::type::IDENT,        "add"      },
                { lexer::tokens::type::ASSIGN,       "="        },
                { lexer::tokens::type::FUNCTION,     "fn"       },
                { lexer::tokens::type::LPAREN,       "("        },
                { lexer::tokens::type::IDENT,        "x"        },
                { lexer::tokens::type::COMMA,        ","        },
                { lexer::tokens::type::IDENT,        "y"        },
                { lexer::tokens::type::RPAREN,       ")"        },
                { lexer::tokens::type::LBRACE,       "{"        },
                { lexer::tokens::type::IDENT,        "x"        },
                { lexer::tokens::type::PLUS,         "+"        },
                { lexer::tokens::type::IDENT,        "y"        },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::RBRACE,       "}"        },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::LET,          "let"      },
                { lexer::tokens::type::IDENT,        "result"   },
                { lexer::tokens::type::ASSIGN,       "="        },
                { lexer::tokens::type::IDENT,        "add"      },
                { lexer::tokens::type::LPAREN,       "("        },
                { lexer::tokens::type::IDENT,        "five"     },
                { lexer::tokens::type::COMMA,        ","        },
                { lexer::tokens::type::IDENT,        "ten"      },
                { lexer::tokens::type::RPAREN,       ")"        },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::BANG,         "!"        },
                { lexer::tokens::type::MINUS,        "-"        },
                { lexer::tokens::type::SLASH,        "/"        },
                { lexer::tokens::type::ASTERISK,     "*"        },
                { lexer::tokens::type::INT,          "5"        },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::INT,          "5"        },
                { lexer::tokens::type::LT,           "<"        },
                { lexer::tokens::type::INT,          "10"       },
                { lexer::tokens::type::GT,           ">"        },
                { lexer::tokens::type::INT,          "5"        },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::IF,           "if"       },
                { lexer::tokens::type::LPAREN,       "("        },
                { lexer::tokens::type::INT,          "5"        },
                { lexer::tokens::type::LT,           "<"        },
                { lexer::tokens::type::INT,          "10"       },
                { lexer::tokens::type::RPAREN,       ")"        },
                { lexer::tokens::type::LBRACE,       "{"        },
                { lexer::tokens::type::RETURN,       "return"   },
                { lexer::tokens::type::TRUE,         "true"     },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::RBRACE,       "}"        },
                { lexer::tokens::type::ELSE,         "else"     },
                { lexer::tokens::type::LBRACE,       "{"        },
                { lexer::tokens::type::RETURN,       "return"   },
                { lexer::tokens::type::FALSE,        "false"    },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::RBRACE,       "}"        },
                { lexer::tokens::type::INT,          "10"       },
                { lexer::tokens::type::EQ,           "=="       },
                { lexer::tokens::type::INT,          "10"       },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::INT,          "10"       },
                { lexer::tokens::type::NOT_EQ,       "!="       },
                { lexer::tokens::type::INT,          "9"        },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::STRING,       "foobar"   },
                { lexer::tokens::type::STRING,       "foo bar"  },
                { lexer::tokens::type::LBRACKET,     "["        },
                { lexer::tokens::type::INT,          "1"        },
                { lexer::tokens::type::COMMA,        ","        },
                { lexer::tokens::type::INT,          "2"        },
                { lexer::tokens::type::RBRACKET,     "]"        },
                { lexer::tokens::type::SEMICOLON,    ";"        },
                { lexer::tokens::type::LBRACE,       "{"        },
                { lexer::tokens::type::STRING,       "foo"      },
                { lexer::tokens::type::COLON,        ":"        },
                { lexer::tokens::type::STRING,       "bar"      },
                { lexer::tokens::type::RBRACE,       "}"        },
                { lexer::tokens::type::END_OF_FILE,  "EOF"      },

        };
        auto lst = lexer::tokens::get_list( tt, input.begin( ), input.end( ) );

        REQUIRE( lst.size( ) == results.size( ) );
        size_t id = 0;

        for( auto &token: lst ) {
            REQUIRE( token.name    == results[id].t );
            REQUIRE( token.literal == results[id].literal );
            id++;
        }
    }

}
