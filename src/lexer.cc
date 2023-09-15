#include "lexer.h"

Lexer::Lexer(std::string file, std::stringstream& source, ErrorHandler& error_handler) : file_(file), source_(source), error_handler_(error_handler) {
  line_number_ = 1;
  char_index_ = 0;
  UpdateLine();
}

std::shared_ptr<Lexeme> Lexer::Next() {
  char c;
  while (true) {
    if (source_.tellg() == EOF) {
      return std::make_shared<Lexeme>(file_, "", TokenTypeEndOfFile, line_, line_number_, char_index_);
    }
    source_.get(c);
    char_index_++;
    if (c == '\n') {
      line_number_++;
      char_index_ = 0;
      UpdateLine();
      continue;
    }
    if (!IsEmpty(c)) {
      break;
    }
  }
  // handle comments
  if (c == '/') {
    if (source_.peek() == '/') {
      std::string comment = "/";
      int start_char_index = char_index_;
      int start_line_number = line_number_;
      std::string start_line = line_;
      while (true) {
        if (source_.tellg() == EOF) {
          break;
        }
        source_.get(c);
        char_index_++;
        if (c == '\n') {
          line_number_++;
          char_index_ = 0;
          UpdateLine();
          break;
        }
        comment += c;
      }
      return std::make_shared<Lexeme>(file_, comment, TokenTypeComment, start_line, start_line_number, start_char_index);
    } else if (source_.peek() == '*') {
      std::string comment = "/";
      int start_char_index = char_index_;
      int start_line_number = line_number_;
      std::string start_line = line_;
      bool terminated = false;
      while (true) {
        if (source_.tellg() == EOF) {
          break;
        }
        source_.get(c);
        char_index_++;
        if (c == '\n') {
          line_number_++;
          char_index_ = 0;
          UpdateLine();
          comment += ' ';
        } else {
          comment += c;
        }
        if (c == '*' && source_.peek() == '/') {
          source_.get(c);
          char_index_++;
          comment += c;
          terminated = true;
          break;
        }
      }
      if (!terminated) {
        error_handler_.PushError(file_, start_line, start_line_number, start_char_index, "Unterminated comment");
      }
      return std::make_shared<Lexeme>(file_, comment, TokenTypeComment, start_line, start_line_number, start_char_index);
    }
  }

  if (IsOperator(c)) {
    return std::make_shared<Lexeme>(file_, std::string{c}, TokenTypeOperator, line_, line_number_, char_index_);
  } else if (IsPunctuation(c)) {
    return std::make_shared<Lexeme>(file_, std::string{c}, TokenTypePunctuation, line_, line_number_, char_index_);
  } else if (IsDigit(c)) {
    std::string lexeme;
    lexeme += c;
    int start_char_index = char_index_;
    int start_line_number = line_number_;
    std::string start_line = line_;
    bool is_float = false;
    bool is_long = false;
    bool is_floating_number = false;
    bool should_continue = false;
    while (true) {
      if (source_.tellg() == EOF) {
        break;
      }
      source_.get(c);
      char_index_++;
      if (c == '\n') {
        line_number_++;
        char_index_ = 0;
        UpdateLine();
        break;
      }
      if (c == '\'') {
        // ' can be used as a separator for numbers like 1'000'000
        should_continue = true;
        continue;
      }
      if (c == ';' || IsEmpty(c)) {
        char_index_--;
        source_.unget();
        break;
      }
      if (c == '.') {
        if (is_floating_number) {
          std::string cstr{c};
          error_handler_.PushError(file_, start_line, start_line_number, start_char_index, "Invalid suffix '" + cstr + "'character in floating constant");
        }
        is_floating_number = true;
      } else if (c == 'f' || c == 'F') {
        is_float = true;
        is_floating_number = true;
      } else if (c == 'l' || c == 'L') {
        is_long = true;
      } else if (!IsDigit(c)) {
        char_index_--;
        source_.unget();
        break;
      }
      lexeme += c;
      should_continue = false;
    }
    TokenType type;
    if (is_floating_number) {
      if (is_float) {
        type = TokenTypeNumberFloat;
      } else {
        type = TokenTypeNumberDouble;
      }
    } else {
      if (is_long) {
        type = TokenTypeNumberInt64;
      } else {
        type = TokenTypeNumberInt32;
      }
    }
    if (should_continue) {
      error_handler_.PushError(file_, start_line, start_line_number, start_char_index, "Leading separators are not allowed");
    }
    return std::make_shared<Lexeme>(file_, lexeme, type, start_line, start_line_number, start_char_index);
  } else if (c == '"') {
    std::string lexeme = "";
    int start_char_index = char_index_;
    int start_line_number = line_number_;
    std::string start_line = line_;
    bool is_escaped = false;
    while (true) {
      if (source_.tellg() == EOF) {
        break;
      }
      source_.get(c);
      char_index_++;
      if (c == '\n') {
        line_number_++;
        char_index_ = 0;
        UpdateLine();
        break;
      }
      if (c == '"') {
        is_escaped = true;
        break;
      }
      lexeme += c;
    }
    if (!is_escaped) {
      error_handler_.PushError(file_, start_line, start_line_number, start_char_index, "Unterminated string literal");
    }
    return std::make_shared<Lexeme>(file_, lexeme, TokenTypeStringLiteral, start_line, start_line_number, start_char_index);
  } else if (c == '\'') {
    int start_char_index = char_index_;
    int start_line_number = line_number_;
    std::string start_line = line_;
    char character;
    source_.get(character);
    char_index_++;
    if (c == '\n') {
      line_number_++;
      char_index_ = 0;
      UpdateLine();
      error_handler_.PushError(file_, start_line, start_line_number, start_char_index, "Unexpected end of line in character constant");
    }
    source_.get(c);
    char_index_++;
    if (c != '\'') {
      error_handler_.PushError(file_, start_line, start_line_number, start_char_index, "Unterminated character constant");
    }
    return std::make_shared<Lexeme>(file_, std::string{character}, TokenTypeCharacterLiteral, line_, line_number_, char_index_);
  } else {
    std::string lexeme;
    lexeme += c;
    int start_char_index = char_index_;
    int start_line_number = line_number_;
    std::string start_line = line_;
    while (true) {
      source_.get(c);
      char_index_++;
      if (c == '\n') {
        line_number_++;
        char_index_ = 0;
        UpdateLine();
        break;
      }
      if (IsEmpty(c)) {
        break;
      }
      if (IsPunctuation(c) || IsOperator(c)) {
        char_index_--;
        source_.unget();
        break;
      }
      lexeme += c;
    }
    if (IsKeyword(lexeme)) {
      return std::make_shared<Lexeme>(file_, lexeme, TokenTypeKeyword, start_line, start_line_number, start_char_index);
    } else {
      if (!IsIdentifier(lexeme)) {
        error_handler_.PushError(file_, start_line, start_line_number, start_char_index, "Invalid identifier name");
      }
      return std::make_shared<Lexeme>(file_, lexeme, TokenTypeIdentifier, start_line, start_line_number, start_char_index);
    }
  }
}

void Lexer::UpdateLine() {
  line_ = "";
  int pos = source_.tellg();
  char c;
  if (char_index_ > 0) {
    source_.seekg(pos - char_index_);
  }
  do {
    if (source_.eof()) {
      return;
    }
    source_.get(c);
    if (c == '\n') {
      source_.seekg(pos);
      break;
    }
    line_ += c;
  } while (true);
  source_.seekg(pos);
}