#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include "error_handler.h"

inline bool IsEmpty(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f' || c == EOF;
}

inline bool IsOperator(char c) {
  return c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^' || c == '!' || c == '<' || c == '>' || c == '&' || c == '|' || c == '~' || c == '?';
}

inline bool IsKeyword(const std::string& str) {
  return str == "if" || str == "else" || str == "while" || str == "for" || str == "return" ||
  str == "break" || str == "continue" || str == "true" || str == "false" || str == "null" ||
  str == "fun" || str == "enum" || str == "class" || str == "import" || str == "var" ||
  str == "module" || str == "extern" || str == "static";
}

inline bool IsPunctuation(char c) {
  return c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == ':' || c == ';' || c == '.';
}

inline bool IsDigit(char c) {
  return c >= '0' && c <= '9';
}

const std::regex kIdentifierPattern("[_a-zA-Z][_a-zA-Z0-9]{0,2048}");

inline bool IsDigitType(char c) {
  return c == 'f' || c == 'F' || c == 'l' || c == 'L';
}

inline bool IsIdentifier(const std::string& identifier) {
  return std::regex_match(identifier, kIdentifierPattern);
}

enum TokenType {
  TokenTypeIdentifier,
  TokenTypeKeyword,
  TokenTypeOperator,
  TokenTypePunctuation,
  TokenTypeEndOfFile,
  TokenTypeCharacterLiteral,
  TokenTypeStringLiteral,
  TokenTypeNumberDouble,
  TokenTypeNumberFloat,
  TokenTypeNumberInt32,
  TokenTypeNumberInt64,
  TokenTypeBoolean,
  TokenTypeComment,
  TokenTypeInvalid,
};

static std::unordered_map<TokenType, std::string> debugTokenNames = {
    {TokenTypeIdentifier,       "Identifier"},
    {TokenTypeKeyword,          "Keyword"},
    {TokenTypeOperator,         "Operator"},
    {TokenTypePunctuation,      "Punctuation"},
    {TokenTypeEndOfFile,        "EndOfFile"},
    {TokenTypeCharacterLiteral, "CharacterLiteral"},
    {TokenTypeStringLiteral,    "StringLiteral"},
    {TokenTypeNumberDouble,     "Double"},
    {TokenTypeNumberFloat,      "Float"},
    {TokenTypeNumberInt32,      "Int32"},
    {TokenTypeNumberInt64,      "Int64"},
    {TokenTypeBoolean,          "Boolean"},
    {TokenTypeComment,          "Comment"},
    {TokenTypeInvalid,          "Invalid"},
};


inline bool IsTokenConstant(TokenType token) {
    return token == TokenTypeCharacterLiteral || token == TokenTypeStringLiteral || token == TokenTypeNumberDouble || token == TokenTypeNumberFloat || token == TokenTypeNumberInt32 || token == TokenTypeNumberInt64 || token == TokenTypeBoolean;
}

class Lexeme {
public:
  Lexeme(const std::string& file, const std::string& value, TokenType type, const std::string& line, int line_number, int char_index)
      : file_(file),
        value_(value),
        type_(type),
        line_(line),
        line_number_(line_number),
        char_index_(char_index) {}

  const std::string& Value() const {
    return value_;
  }

  TokenType Type() const {
    return type_;
  }

  int LineNumber() const {
    return line_number_;
  }

  int CharIndex() const {
    return char_index_;
  }

  const std::string& Line() const {
    return line_;
  }

  const std::string& File() const {
    return file_;
  }
private:
  std::string file_;
  std::string value_;
  TokenType type_;
  std::string line_;

  int line_number_;
  int char_index_;

};

class Lexer {
public:
  Lexer(std::string file, std::stringstream& source, ErrorHandler& error_handler);

  std::shared_ptr<Lexeme> Next();

  void UpdateLine();
private:
  std::string file_;
  std::stringstream& source_;
  ErrorHandler& error_handler_;
  std::string line_;
  int line_number_;
  int char_index_;
};

