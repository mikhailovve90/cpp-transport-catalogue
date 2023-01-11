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

    bool is_int() const {
        return std::holds_alternative<int>(*this);
    }
    int as_int() const {
        using namespace std::literals;
        if (!is_int()) {
            throw std::logic_error("Not an int"s);
        }
        return std::get<int>(*this);
    }

    bool is_pure_double() const {
        return std::holds_alternative<double>(*this);
    }
    bool is_double() const {
        return is_int() || is_pure_double();
    }
    double as_double() const {
        using namespace std::literals;
        if (!is_double()) {
            throw std::logic_error("Not a double"s);
        }
        return is_pure_double() ? std::get<double>(*this) : as_int();
    }

    bool is_bool() const {
        return std::holds_alternative<bool>(*this);
    }
    bool as_bool() const {
        using namespace std::literals;
        if (!is_bool()) {
            throw std::logic_error("Not a bool"s);
        }

        return std::get<bool>(*this);
    }

    bool is_null() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }

    bool is_array() const {
        return std::holds_alternative<Array>(*this);
    }


    const Array& as_array() const {
        using namespace std::literals;
        if (!is_array()) {
            throw std::logic_error("Not an array"s);
        }

        return std::get<Array>(*this);
    }

    Array& as_array() {
        using namespace std::literals;
        if (!is_array()) {
            throw std::logic_error("Not an array"s);
        }

        return std::get<Array>(*this);
    }

    bool is_string() const {
        return std::holds_alternative<std::string>(*this);
    }
    const std::string& as_string() const {
        using namespace std::literals;
        if (!is_string()) {
            throw std::logic_error("Not a string"s);
        }

        return std::get<std::string>(*this);
    }

    bool is_map() const {
        return std::holds_alternative<Dict>(*this);
    }
    const Dict& as_map() const {
        using namespace std::literals;
        if (!is_map()) {
            throw std::logic_error("Not a dict"s);
        }

        return std::get<Dict>(*this);
    }

    Dict& as_map() {
        using namespace std::literals;
        if (!is_map()) {
            throw std::logic_error("Not a dict"s);
        }
        return std::get<Dict>(*this);
    }

    bool operator==(const Node& rhs) const {
        return get_value() == rhs.get_value();
    }

    const Value& get_value() const {
        return *this;
    }
};

inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}

class Document {
public:
    explicit Document(Node root)
        : root_(std::move(root)) {
    }

    const Node& get_root() const {
        return root_;
    }

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
