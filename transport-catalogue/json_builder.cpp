#include "json_builder.h"

json::Builder::KeyItemContext json::Builder::Key(std::string str) {
    if(root_.is_null()) {
        if(nodes_stack_.back()->is_map()) {
            Node* str_nd = new Node(str);
            nodes_stack_.emplace_back(str_nd);
            return {*this};
        } else throw std::logic_error("Key do not call without open map");
    }
    throw std::logic_error("Only build may be call after comlete object");
}

json::Builder& json::Builder::Value(json::Node node) {
    if(root_.is_null()) {
        if(nodes_stack_.empty()) {
            root_ = std::move(node);
            return *this;
        }

        if(nodes_stack_.back()->is_string()) {
            std::string key = nodes_stack_.back()->as_string();
            delete nodes_stack_.back();
            nodes_stack_.pop_back();
            nodes_stack_.back()->as_map()[key] = node;
            return *this;
        } else if (nodes_stack_.back()->is_array()) {
            nodes_stack_.back()->as_array().push_back(node);
            return *this;
        } else {
            root_ = std::move(node);
            return *this;
        }
    }
    throw std::logic_error("Only build may be call after comlete object");
}

json::Builder::DictItemContext json::Builder::StartDict() {
    if(root_.is_null()) {
        std::map<std::string, Node> nd;
        Node* str_map = new Node(nd);
        nodes_stack_.push_back(str_map);
        return {*this};
    }
    throw std::logic_error("Only build may be call after comlete object");
}

json::Builder::ArrayItemContext json::Builder::StartArray() {
    if(root_.is_null()) {
        std::vector<Node> nd;
        Node* str_arr = new Node(nd);
        nodes_stack_.push_back(str_arr);
        return {*this};
    }
    throw std::logic_error("Only build may be call after complete object");
}

json::Builder& json::Builder::EndDict() {
    if(root_.is_null()) {
        if(nodes_stack_.back()->is_map()) {
            std::map<std::string, Node> node = std::move(nodes_stack_.back()->as_map());
            delete nodes_stack_.back();
            nodes_stack_.pop_back();
            if(nodes_stack_.empty()) {
                root_ = node;
                return *this;
            }
            if(nodes_stack_.back()->is_string()) {
                std::string key = nodes_stack_.back()->as_string();
                delete nodes_stack_.back();
                nodes_stack_.pop_back();
                nodes_stack_.back()->as_map()[key] = node;
                return *this;
            } else if (nodes_stack_.back()->is_array()) {
                nodes_stack_.back()->as_array().push_back(node);
                return *this;
            }
        } else throw std::logic_error("EndDict call only for Dict(std::map<std::string, Node>)");
    }
    throw std::logic_error("Only build may be call after complete object");
}

json::Builder& json::Builder::EndArray() {
    if(root_.is_null()) {
        if(nodes_stack_.back()->is_array()) {
            std::vector<Node> arr = std::move(nodes_stack_.back()->as_array());
            delete nodes_stack_.back();
            nodes_stack_.pop_back();

            if(nodes_stack_.empty()) {
                root_ = arr;
                return *this;
            }

            if(nodes_stack_.back()->is_string()) {
                std::string key = nodes_stack_.back()->as_string();
                delete nodes_stack_.back();
                nodes_stack_.pop_back();
                nodes_stack_.back()->as_map()[key] = arr;
                return *this;
            } else if (nodes_stack_.back()->is_array()) {
                nodes_stack_.back()->as_array().push_back(arr);
                return *this;
            }
        } else throw std::logic_error("EndArray call only for Array(std::vector<Node>)");
    }
    throw std::logic_error("Only build may be call after complete object");
}



json::Builder::KeyItemContext json::Builder::ItemContext::Key(std::string str) {
    return builder_.Key(std::move(str));
}

json::Builder& json::Builder::ItemContext::Value(json::Node node) {
    builder_.Value(std::move(node));
    return builder_;
}

json::Builder::DictItemContext json::Builder::ItemContext::StartDict() {
    return builder_.StartDict();
}

json::Builder::ArrayItemContext json::Builder::ItemContext::StartArray() {
    return builder_.StartArray();
}

json::Builder& json::Builder::ItemContext::EndDict() {
    return builder_.EndDict();
}

json::Builder& json::Builder::ItemContext::EndArray() {
    return builder_.EndArray();
}

json::Builder::DictItemContext json::Builder::KeyItemContext::Value(json::Node node) {
        builder_.Value(std::move(node));
        return builder_;
}

json::Builder::ArrayItemContext json::Builder::ArrayItemContext::Value(json::Node node) {
    builder_.Value(std::move(node));
    return builder_;
}
