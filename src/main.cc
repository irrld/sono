#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "lexer.h"
#include "error_handler.h"
#include "parser.h"


int main() {
  Grammar grammar{"grammar.json"};
  //grammar.Print();

  std::filesystem::path path = "main.sls";
  std::ifstream stream;
  stream.open(path, std::ios::in);
  std::stringstream buffer;
  buffer << stream.rdbuf();
  stream.close();

  std::string absolute_path = std::filesystem::absolute(path).string();

  // todo preprocessed input stream class
  ErrorHandler error_handler{};
  Lexer lexer{absolute_path, buffer, error_handler};

  /*Lexeme lexeme = lexer.Next();
  while (lexeme.Type() != TokenTypeEndOfFile) {
    std::cout << debugTokenNames[lexeme.Type()] << ": " << lexeme.Value() << std::endl;
    std::cout << lexeme.Line() + " (" << lexeme.LineNumber() << ":" << lexeme.CharIndex() << ")" << std::endl;
    for (int i = 0; i < lexeme.CharIndex() - 1; ++i) {
      std::cout << " ";
    }
    std::cout << "^" << std::endl;

    lexeme = lexer.Next();
  }*/
  Parser parser{absolute_path, lexer, error_handler};

  parser.Parse();

  if (error_handler.PrintErrors()) {
    return 1;
  }

  return 0;
}
