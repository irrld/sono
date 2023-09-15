#pragma once

#include <string>
#include <type_traits>
#include <vector>
#include <memory>
#include "lexer.h"
#include "json.h"

struct GrammarValue {
  std::string name;
  std::vector<std::string> or_;
  bool is_terminal_;

  GrammarValue(std::string name, bool is_terminal) :
                 name(name), is_terminal_(is_terminal) {}
};

class Grammar {
public:
  Grammar(const std::string& file);
  ~Grammar();

  void Print();

private:
  std::vector<std::shared_ptr<GrammarValue>> symbols_;

  void Process(const std::string& key, nlohmann::json& json);

  std::vector<std::string> Split(const std::string& str) {
    std::vector<std::string> terms;
    std::istringstream f{str};
    std::string tmp_str;
    while (getline(f, tmp_str, ' ')) {
      terms.push_back(tmp_str);
    }
    return terms;
  }
};