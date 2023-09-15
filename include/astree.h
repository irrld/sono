#pragma once

#include <type_traits>
#include <vector>
#include <string>

template<class T>
concept EnumType = std::is_enum_v<T>;

static int sIdCounter = 0;

template<EnumType Type, typename DataType>
class ASTNode {
public:
  using SharedPtr = std::shared_ptr<ASTNode<Type, DataType>>;

  ASTNode(Type type, DataType data, const std::string& name) : type_(type), data_(data), name_(name) {
    id_ = sIdCounter++;
  }
  virtual ~ASTNode() {}

  void AddChild(SharedPtr child) { childs_.push_back(child); }
  void SetParent(SharedPtr parent) { parent_ = parent; }
  SharedPtr GetParent() const { return parent_; }
  std::vector<SharedPtr> GetChilds() { return childs_; }
  int GetId() const { return id_; }
  const std::string& GetName() const { return name_; }
  Type GetType() const { return type_; }
  DataType& GetData() const { return data_; }

  static SharedPtr Create(Type type, DataType data, const std::string& name) {
    return std::make_shared<ASTNode<Type, DataType>>(type, data, name);
  }
private:
  int id_;
  Type type_;
  DataType data_;
  std::string name_;
  SharedPtr parent_;
  std::vector<SharedPtr> childs_;


};