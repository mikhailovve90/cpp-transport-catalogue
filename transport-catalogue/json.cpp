#include "json.h"

using namespace std;

namespace json {

bool operator==(const Node& lhs, const Node& rhs) {
    return lhs.GetValue() == rhs.GetValue();
}

bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
}

bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.get_root() == rhs.get_root();
}
bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs==rhs);
}

bool Node::is_int() const {
    if(value_.index() == 4) {
        return true;
    }
    return false;
}
bool Node::is_double() const {
    if(value_.index() >= 4 && value_.index() <= 5) {
        return true;
    }
    return false;
}
bool Node::is_bool() const {
    if(value_.index() == 3) {
        return true;
    }
    return false;
}
bool Node::is_pure_double() const {
    if(value_.index() == 5) {
        return true;
    }
    return false;
}
bool Node::is_null() const {
    if(value_.index() == 0) {
        return true;
    }
    return false;
}
bool Node::is_string() const {
    if(value_.index() == 6) {
        return true;
    }
    return false;
}
bool Node::is_array() const {
    if(value_.index() == 1) {
        return true;
    }
    return false;
}
bool Node::is_map() const {
    if(value_.index() == 2) {
        return true;
    }
    return false;
}

int Node::as_int() const {
    if(this->is_int()) {
        return std::get<int>(value_);
    }
    throw std::logic_error("Node not contain int");
}

bool Node::as_bool() const {
    if(this->is_bool()) {
        return std::get<bool>(value_);
    }
    throw std::logic_error("Node not contain bool");

}

double Node::as_double() const {
    if(this->is_pure_double()) {
        return std::get<double>(value_);
    }
    if(this->is_int()) {
        return static_cast<double>(std::get<int>(value_));
    }
    throw std::logic_error("Node not contain double or int");
}

const std::string& Node::as_string() const {
    if(this->is_string()) {
        return std::get<std::string>(value_);
    }
    throw std::logic_error("Node not contain string");
}



const Array& Node::as_array() const {
    if(this->is_array()) {
        return std::get<Array>(value_);
    }
    throw std::logic_error("Node not contain array");
}

const Dict& Node::as_map() const {
    if(this->is_map()) {
        return std::get<Dict>(value_);
    }
    throw std::logic_error("Node not contain map");
}

Node::Node(const Array& array_)
    : value_(move(array_)) {
}

Node::Node(const Dict& map)
    : value_(move(map)) {
}

Node::Node(int value)
    : value_(value) {
}

Node::Node(const std::string& value)
    : value_(move(value)) {
}

Node::Node(double value)
    : value_(value) {
}

Node::Node(bool b)
    : value_(b) {
}

Node::Node() {}
Node::Node(void*) {}

void render_value(const bool value, std::ostream& out) {
    if(value) {
        out << "true";
    } else {
        out << "false";
    }
}

void render_value(const int value, std::ostream& out) {
    out << value;
}

void render_value(const double value, std::ostream& out) {
    out << value;
}

void render_value(std::nullptr_t, std::ostream& out) {
    out << "null";
}

void render_value(const std::string& str, std::ostream& out) {
    using namespace std::literals;
    out << "\"";

    for(const char ch : str) {
        switch (ch) {
        case '\\':
            out << "\\\\";
            break;
        case '"':
            out << "\\\"";
            break;
        case '\n':
            out << "\\n";
            break;
        case '\t':
            out << "\t";
            break;
        case '\r':
            out << "\\r";
            break;
        default:
            out << ch;
            break;
        }
    }
    out << "\"";
}

void render_value(const Array& arr, std::ostream& out) {
    bool flag = false;
    out << "[";
    for(const auto  value : arr) {
        if(!flag) {
            render_node(value, out);
            flag = true;
        } else {
            out << ",";
            render_node(value, out);
        }
    }
    out << "]";
}

void render_value(const std::map<std::string, Node>& dict, std::ostream& out) {
    bool flag = false;
    out << "{";
    for(const auto [key, value] : dict) {
        if(!flag) {
            out << "\"" << key << "\": ";
            render_node(value, out);
            flag = true;
        } else {
            out << ",\"" << key << "\": ";
            render_node(value, out);
        }
    }
    out << "}";
}

void render_node(const Node& node, std::ostream& out) {
    std::visit(
    [&out](const auto& value) {
        render_value(value, out);
    },
    node.GetValue());
}

namespace {

Number LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return std::stoi(parsed_num);
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return std::stod(parsed_num);
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

// Считывает содержимое строкового литерала JSON-документа
//    :
std::string LoadString(std::istream& input) {
    using namespace std::literals;

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
            case 'n':
                s.push_back('\n');
                break;
            case 't':
                s.push_back('\t');
                break;
            case 'r':
                s.push_back('\r');
                break;
            case '"':
                s.push_back('"');
                break;
            case '\\':
                s.push_back('\\');
                break;
            default:
                // Встретили неизвестную escape-последовательность
                throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return s;
}

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
    Array result;
    bool flag_of_correct = false;

    char c;
    while(input >> c) {
        if(c == ']') {
            flag_of_correct = true;
            break;
        }

        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    if(!flag_of_correct) {
        throw ParsingError("Array parsing error");
    }

    return Node(move(result));
}

Node LoadDict(istream& input) {
    Dict result;
    bool flag_of_correct = false;
    char c;

    while(input >> c) {
        if(c == '}') {
            flag_of_correct = true;
            break;
        }
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input);
        input >> c;
        result.insert({move(key), LoadNode(input)});

    }

    if(!flag_of_correct) {
        throw ParsingError("Dict parsing error");
    }
    return Node(move(result));
}

Node LoadNull(istream& input) {
    char c;
    std::string is_null;
    while(input >> c ) {
        if(c==',' || c=='}' || c==']' || c == ' ') {
            input.putback(c);
            break;
        }
        is_null.push_back(c);
    }

    if(is_null != "ull") {
        throw ParsingError("null parsing error");
    }

    return Node();
}

Node LoadTrue(istream& input) {
    char c;
    std::string is_true;
    while(input >> c) {
        if(c==',' || c=='}' || c==']' || c == ' ') {
            input.putback(c);
            break;
        }
        is_true.push_back(c);
    }

    if(is_true != "rue") {

        throw ParsingError("true parsing error");
    }

    return Node(true);
}

Node LoadFalse(istream& input) {
    char c;
    std::string is_false;
    while(input >> c) {
        if(c==',' || c=='}' || c==']' || c == ' ') {
            input.putback(c);
            break;
        }
        is_false.push_back(c);
    }

    if(is_false!= "alse") {
        throw ParsingError("false parsing error");
    }

    return Node(false);
}


Node LoadNode(istream& input) {
    char c;
    input >> c;

    if (c == '[') {
        return LoadArray(input);
    } else if (c == '{') {
        return LoadDict(input);
    } else if (c == '"') {
        return Node(LoadString(input));
    } else if (c == 'n') {
        return LoadNull(input);
    } else if (c == 't') {
        return LoadTrue(input);
    } else if (c == 'f') {
        return LoadFalse(input);
    } else {
        input.putback(c);
        Number result = LoadNumber(input);
        if(result.index() == 0) {
            return Node(std::get<int>(result));
        } else  {
            return Node(std::get<double>(result));
        }
    }
}

}  // namespace

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::get_root() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void print(const Document& doc, std::ostream& output) {
    render_node(doc.get_root(), output);
}

}  // namespace json
