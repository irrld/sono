#include "parser.h"

Parser::Parser(const std::string& file, Lexer& lexer, ErrorHandler& error_handler)
    : file_(file),
      lexer_(lexer),
      error_handler_(error_handler) {
}

Parser::~Parser() {
}

std::shared_ptr<Parser::ParseNode> Parser::Parse() {
  root_ = ParseNode::Create(ParseType::Root, {}, "root");
  current_node_ = root_;
  do {
    current_lexeme_ = lexer_.Next();
    if (current_lexeme_->Type() == TokenTypeEndOfFile) {
      break;
    }
    if (current_lexeme_->Type() == TokenTypeComment) {
      continue;
    }
    if (current_lexeme_->Type() == TokenTypeInvalid) {
      error_handler_.PushError(current_lexeme_->File(), current_lexeme_->Line(), current_lexeme_->LineNumber(), current_lexeme_->CharIndex(), "unexpected invalid token");
      continue;
    }
    // todo use grammar to parse
    if (current_lexeme_->Type() == TokenTypeKeyword) {
      if (current_lexeme_->Value() == "fun") {
        ParseFunction();
      }
    }
//    auto lexeme = *current_lexeme_.get();
//    std::cout << debugTokenNames[lexeme.Type()] << ": " << lexeme.Value() << std::endl;
//    std::cout << lexeme.Line() + " (" << lexeme.LineNumber() << ":" << lexeme.CharIndex() << ")" << std::endl;
/*    for (int i = 0; i < lexeme.CharIndex() - 1; ++i) {
      std::cout << " ";
    }
    std::cout << "^" << std::endl;*/
  } while (current_lexeme_->Type() != TokenTypeEndOfFile);
  return root_;
}

void Parser::ParseFunction() {
  auto func_name = Next();
  if (func_name->Type() != TokenTypeIdentifier) {
    error_handler_.PushError(func_name->File(), func_name->Line(), func_name->LineNumber(), func_name->CharIndex(), "expected function name");
    return;
  }
  std::cout << "function: " << func_name->Value() << std::endl;
  ParseParameters();
  Next();
  if (!CheckPunctuation("{")) {
    error_handler_.PushError(func_name->File(), func_name->Line(), func_name->LineNumber(), func_name->CharIndex(), "expected left brace");
    return;
  }
  while (true) {
    auto statement = Next();
    if (CheckPunctuation("}")) {
      break;
    }
    ParseStatement();
  }
}

void Parser::ParseStatement() {
  std::vector<std::shared_ptr<Lexeme>> lexemes;
  while (true) {
    if (current_lexeme_->Type() == TokenTypeEndOfFile) {
      error_handler_.PushError(current_lexeme_->File(), current_lexeme_->Line(), current_lexeme_->LineNumber(), current_lexeme_->CharIndex(), "unexpected end of file");
      return;
    }
    if (CheckPunctuation(";")) {
      break;
    }
    lexemes.push_back(current_lexeme_);
    Next();
  }
  if (lexemes.size() == 0) {
    return;
  }
  if (lexemes.size() == 1) {
    error_handler_.PushError(lexemes[0]->File(), lexemes[0]->Line(), lexemes[0]->LineNumber(), lexemes[0]->CharIndex(), "expected statement");
    return;
  }
  if (lexemes.size() == 6) {
    // var name: type = value;
    if (lexemes[0]->Type() == TokenTypeKeyword && lexemes[0]->Value() == "var"
        && lexemes[1]->Type() == TokenTypeIdentifier && lexemes[3]->Type() == TokenTypeIdentifier
        && lexemes[4]->Type() == TokenTypeOperator && lexemes[4]->Value() == "="
        && (lexemes[5]->Type() == TokenTypeIdentifier || IsTokenConstant(lexemes[5]->Type()))) {
      std::cout << "var declaration: " << lexemes[1]->Value() << " with type " << lexemes[3]->Value() << std::endl;
      std::cout << "assignment: " << lexemes[1]->Value() << " = " << lexemes[5]->Value() << " <- " << debugTokenNames[lexemes[5]->Type()] << std::endl;
      return;
    }
  } else if (lexemes.size() == 4) {
    if (lexemes[0]->Type() == TokenTypeKeyword && lexemes[0]->Value() == "var"
        && lexemes[1]->Type() == TokenTypeIdentifier && lexemes[3]->Type() == TokenTypeIdentifier) {
      std::cout << "var declaration: " << lexemes[1]->Value() << " " << lexemes[3]->Value() << std::endl;
      return;
    }
  } else if (lexemes.size() == 3) {
    if (lexemes[0]->Type() == TokenTypeIdentifier && lexemes[1]->Type() == TokenTypeOperator && lexemes[1]->Value() == "="
        && (lexemes[2]->Type() == TokenTypeIdentifier || IsTokenConstant(lexemes[2]->Type()))) {
      std::cout << "assignment: " << lexemes[0]->Value() << " = " << lexemes[2]->Value() << std::endl;
      return;
    }
  }

  if (CheckPunctuation(lexemes[lexemes.size() - 1], ")")) {
    // function call
    std::vector<std::string> identifiers;
    std::string full_call;
    bool dot = false;
    int start_index = 0;
    for (int i = 0; i < lexemes.size(); ++i) {
      const auto& item = lexemes[i];
      if (CheckPunctuation(item, "(")) {
        start_index = i;
        break;
      }
      if (dot) {
        if (CheckPunctuation(item, ".")) {
          full_call += item->Value();
        } else {
          error_handler_.PushError(item->File(), item->Line(), item->LineNumber(), item->CharIndex(), "unexpected punctuation");
        }
        dot = false;
      }

      if (item->Type() == TokenTypeIdentifier) {
        identifiers.push_back(item->Value());
        full_call += item->Value();
        dot = true;
      }
    }

    const auto& last_lexeme = lexemes[lexemes.size() - 1];
    if (start_index == 0) {
      error_handler_.PushError(last_lexeme->File(), last_lexeme->Line(), last_lexeme->LineNumber(), last_lexeme->CharIndex(), "expected open paranthesis");
      return;
    }
    std::cout << "function call: " << full_call << std::endl;
    for (int i = start_index + 1, j = 0; i < lexemes.size() - 1; ++i, j++) {
      const auto& item = lexemes[i];
      if (j % 2 == 1) {
        if (!CheckPunctuation(item, ",")) {
          error_handler_.PushError(item->File(), item->Line(), item->LineNumber(), item->CharIndex(), "expected comma");
          break;
        }
        continue;
      }

      if (lexemes[2]->Type() == TokenTypeIdentifier || IsTokenConstant(lexemes[2]->Type())) {
        std::cout << "param: " << item->Value() << std::endl;
      }
    }
    if (!CheckPunctuation(last_lexeme, ")")) {
      error_handler_.PushError(last_lexeme->File(), last_lexeme->Line(), last_lexeme->LineNumber(), last_lexeme->CharIndex(), "expected close parenthesis");
      return;
    }
  }
}

void Parser::ParseParameters() {
  auto open_params = Next();
  if (!CheckPunctuation("(")) {
    error_handler_.PushError(open_params->File(), open_params->Line(), open_params->LineNumber(), open_params->CharIndex(), "expected left parenthesis");
    return;
  }
  bool expected_comma = false;
  while (true) {
    auto param_type = Next();
    if (CheckPunctuation(")")) {
      break;
    }
    if (expected_comma) {
      auto comma = param_type;
      if (!CheckPunctuation(",")) {
        error_handler_.PushError(comma->File(), comma->Line(), comma->LineNumber(), comma->CharIndex(), "expected comma");
        break;
      }
      param_type = Next();
      expected_comma = false;
    }
    if (param_type->Type() != TokenTypeIdentifier) {
      error_handler_.PushError(param_type->File(), param_type->Line(), param_type->LineNumber(), param_type->CharIndex(), "expected parameter type");
      break;
    }
    auto param_name = Next();
    if (param_name->Type() != TokenTypeIdentifier) {
      error_handler_.PushError(param_type->File(), param_name->Line(), param_name->LineNumber(), param_name->CharIndex(), "expected parameter name");
      break;
    }
    expected_comma = true;
    std::cout << "parameter: " << param_type->Value() << " " << param_name->Value() << std::endl;
  }
}

bool Parser::CheckPunctuation(const std::string& value) {
  if (current_lexeme_->Type() == TokenTypePunctuation && current_lexeme_->Value() == value) {
    return true;
  }
  return false;
}

bool Parser::CheckPunctuation(std::shared_ptr<Lexeme> lexeme, const std::string& value) {
  if (lexeme->Type() == TokenTypePunctuation && lexeme->Value() == value) {
    return true;
  }
  return false;
}