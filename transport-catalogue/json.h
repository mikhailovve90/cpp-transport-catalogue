#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <deque>
namespace json {

std::string& text_to_json_format(const std::string& text);

class Node;
using Array = std::vector<Node>;
using Dict = std::map<std::string, Node>;
using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
using Number = std::variant<int, double>;
// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
  public:
    using runtime_error::runtime_error;
};

class Node {
  public:
    Node(const Array& array_);
    Node(const Dict& map);
    Node(int value);
    Node(bool b);
    Node(const std::string& value);
    Node(double value);
    Node();
    Node(void*);

    const Value& GetValue() const {
        return value_;
    }

    const Array& AsArray() const;
    const Dict& AsMap() const;
    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const;
    const std::string& AsString() const;

    bool IsInt() const;
    bool IsDouble() const;
    bool IsBool() const;
    bool IsPureDouble() const;
    bool IsNull() const;
    bool IsString() const;
    bool IsArray() const;
    bool IsMap() const;

  private:
    Value value_;
};

void PrintNode(const Node& node, std::ostream& out);
// Шаблон, подходящий для вывода double и int


void PrintValue(const int value, std::ostream& out);

void PrintValue(const bool value, std::ostream& out);

void PrintValue(const double value, std::ostream& out);

// Перегрузка функции PrintValue для вывода значений null
void PrintValue(std::nullptr_t, std::ostream& out);

void PrintValue(const std::string& str, std::ostream& out);

void PrintValue(const std::vector<Node>& arr, std::ostream& out);

void PrintValue(const std::map<std::string, Node>& dict, std::ostream& out);

bool operator==(const Node& lhs, const Node& rhs);
bool operator!=(const Node& lhs, const Node& rhs);


class Document {
  public:
    explicit Document(Node root);

    const Node& GetRoot() const;

  private:
    Node root_;
};

bool operator==(const Document& lhs, const Document& rhs);
bool operator!=(const Document& lhs, const Document& rhs);
Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json
