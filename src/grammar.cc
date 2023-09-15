#include "grammar.h"
#include "json.h"

using namespace nlohmann;

Grammar::Grammar(const std::string& file) {
  json json;
  std::ifstream file_stream(file);
  file_stream >> json;
  file_stream.close();

  for (const auto& item : json["rules"].items()) {
    const std::string& key = item.key();
    Process(key, item.value());
  }
}

Grammar::~Grammar() {

}

void Grammar::Print() {
  for (const auto& symbol : symbols_) {
    if (symbol->is_terminal_) {
      std::cout << "terminal ";
    } else {
      std::cout << "non-terminal ";
    }
    std::cout << symbol->name << " -> ";
    for (int i = 0; i < symbol->or_.size(); ++i) {
      std::cout << symbol->or_[i];
      if (i != symbol->or_.size() - 1) {
        std::cout << " | ";
      }
    }
    std::cout << std::endl;
  }
}

void Grammar::Process(const std::string& key, json& j) {
  if (j.is_string()) {
    std::string str = j.get<std::string>();
    bool is_terminal = true;
    for (const auto& item : Split(str)) {
      if (item[0] == '<') {
        is_terminal = false;
        break;
      }
    }
    auto value = std::make_shared<GrammarValue>(key, is_terminal);
    value->or_.push_back(str);
    symbols_.push_back(value);
  } else if (!j["$or"].is_null()) {
    json _or = j["$or"];
    auto value = std::make_shared<GrammarValue>(key, true);
    bool is_terminal = true;
    for (const auto& item : _or) {
      if (item.is_string()) {
        std::string str = item.get<std::string>();
        for (const auto& split_item : Split(str)) {
          if (split_item[0] == '<') {
            is_terminal = false;
            break;
          }
        }
        value->or_.push_back(str);
      } else if (item.is_object() && !item["$type"].is_null()) {
        std::string str = item["$type"].get<std::string>();
        value->or_.push_back(str);
        if (!is_terminal) {
          // todo throw error
        }
      }
    }
    value->is_terminal_ = is_terminal;
    symbols_.push_back(value);
  } else if (!j["$type"].is_null()) {
    std::string str = j["$type"].get<std::string>();
    auto value = std::make_shared<GrammarValue>(key, true);
    value->or_.push_back(str);
    symbols_.push_back(value);
  }
}

