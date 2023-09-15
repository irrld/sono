#include "error_handler.h"

ErrorHandler::ErrorHandler() {
}

ErrorHandler::~ErrorHandler() {
}

void ErrorHandler::PushError(std::string file, std::string line, int line_number, int char_index, std::string message) {
  ErrorData error;
  error.file = file;
  error.line = line;
  error.line_number = line_number;
  error.char_index = char_index;
  error.message = message;
  errors_.push_back(error);
}

bool ErrorHandler::PrintErrors() {
  for (int i = 0; i < errors_.size(); i++) {
    std::cout << errors_[i].file << ":" << errors_[i].line_number << ":" << errors_[i].char_index << ": error: " << errors_[i].message << std::endl;
    std::cout << errors_[i].line << std::endl;
    for (int j = 0; j < errors_[i].char_index - 1; j++) {
      std::cout << " ";
    }
    std::cout << "^" << std::endl;
  }
  return errors_.size() > 0;
}
