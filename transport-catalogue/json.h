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

    const Array& as_array() const;
    const Dict& as_map() const;
    int as_int() const;
    bool as_bool() const;
    double as_double() const;
    const std::string& as_string() const;

    bool is_int() const;
    bool is_double() const;
    bool is_bool() const;
    bool is_pure_double() const;
    bool is_null() const;
    bool is_string() const;
    bool is_array() const;
    bool is_map() const;

  private:
    Value value_;
};

void render_node(const Node& node, std::ostream& out);
// Шаблон, подходящий для вывода double и int


void render_value(const int value, std::ostream& out);

void render_value(const bool value, std::ostream& out);

void render_value(const double value, std::ostream& out);

// Перегрузка функции render_value для вывода значений null
void render_value(std::nullptr_t, std::ostream& out);

void render_value(const std::string& str, std::ostream& out);

void render_value(const std::vector<Node>& arr, std::ostream& out);

void render_value(const std::map<std::string, Node>& dict, std::ostream& out);

bool operator==(const Node& lhs, const Node& rhs);
bool operator!=(const Node& lhs, const Node& rhs);


class Document {
  public:
    explicit Document(Node root);

    const Node& get_root() const;

  private:
    Node root_;
};

bool operator==(const Document& lhs, const Document& rhs);
bool operator!=(const Document& lhs, const Document& rhs);
Document Load(std::istream& input);

void print(const Document& doc, std::ostream& output);

}  // namespace json
