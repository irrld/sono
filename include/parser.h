#pragma once

#include <string>
#include "lexer.h"
#include "error_handler.h"
#include "astree.h"
#include "grammar.h"

enum class ParseType {
  Root,
  FunctionDecl,
  ParamVarDecl,
  CompoundStatement,
  DeclStatement,
  VarDecl,
  StringLiteral,
  FloatLiteral,
  DoubleLiteral,
  Int32Literal,
  Int64Literal,
  CharacterLiteral,
  BooleanLiteral,
  ImplicitCastExpr,
  BinaryOperator,
  UnaryOperator
};

struct ParseData {
  std::string value_;
  std::string line_;
  int line_number_;
  int char_index_;
};

class Parser {
public:
  using ParseNode = ASTNode<ParseType, ParseData>;
  Parser(const std::string& file, Lexer& lexer, ErrorHandler& error_handler);
  ~Parser();

  std::shared_ptr<ParseNode> Parse();

  void ParseFunction();
  void ParseStatement();
  void ParseParameters();

  std::shared_ptr<Lexeme> Next() {
    return current_lexeme_ = lexer_.Next();
  }
  bool CheckPunctuation(const std::string& value);
  bool CheckPunctuation(std::shared_ptr<Lexeme> lexeme, const std::string& value);


private:
  std::shared_ptr<ParseNode> root_;
  std::shared_ptr<ParseNode> current_node_;

  std::string file_;
  Lexer& lexer_;
  ErrorHandler& error_handler_;
  std::shared_ptr<Lexeme> current_lexeme_;
  std::shared_ptr<Grammar> grammar_;
  // module_name, vector<symbol_name>
  std::unordered_map<std::string, std::vector<std::string>> symbol_table;



};