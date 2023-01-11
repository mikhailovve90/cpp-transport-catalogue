#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

class Node;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node final
    : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
public:
    using variant::variant;
    using Value = variant;

    bool is_int() const;
    int as_int() const;
    bool is_pure_double() const;
    bool is_double() const;
    double as_double() const;
    bool is_bool() const;
    bool as_bool() const;
    bool is_null() const;
    bool is_array() const;
    const Array& as_array() const;
    Array& as_array();
    bool is_string() const;
    const std::string& as_string() const;
    bool is_map() const;
    const Dict& as_map() const;
    Dict& as_map();

    bool operator==(const Node& rhs) const {
        return get_value() == rhs.get_value();
    }

    const Value& get_value() const;
};

inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}

class Document {
public:
    explicit Document(Node root)
        : root_(std::move(root)) {
    }

    const Node& get_root() const;

private:
    Node root_;
};

inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.get_root() == rhs.get_root();
}

inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}

Document Load(std::istream& input);

void Print(const Document& doc, std::ostream& output);

}  // namespace json
