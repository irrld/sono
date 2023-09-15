#pragma once

#include <iostream>

struct ErrorData {
  std::string file;
  std::string line;
  int line_number;
  int char_index;
  std::string message;
};
class ErrorHandler {
public:
  ErrorHandler();
  ~ErrorHandler();

  void PushError(std::string file, std::string line, int line_number, int char_index, std::string message);

  bool PrintErrors();
private:
  std::vector<ErrorData> errors_;
};