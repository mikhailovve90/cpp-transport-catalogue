#pragma once

#include "json.h"
#include <utility>
namespace json {


class Builder {
  public:
    class DictItemContext;
    class KeyItemContext;
    class ArrayItemContext;

    class ItemContext {
      public:
        ItemContext(Builder& builder) : builder_(builder) {}

        KeyItemContext Key(std::string str) {
            return builder_.Key(std::move(str));
        }

        Builder& Value(json::Node node) {
            builder_.Value(std::move(node));
            return builder_;
        }

        DictItemContext StartDict() {
            return builder_.StartDict();
        }

        ArrayItemContext StartArray() {
            return builder_.StartArray();
        }

        Builder& EndDict() {
            return builder_.EndDict();
        }

        Builder& EndArray() {
            return builder_.EndArray();
        }

        Builder& builder_;
    };


    class KeyItemContext : public ItemContext {
      public:
        KeyItemContext(Builder& builder) : ItemContext(builder) {}

        KeyItemContext Key(std::string str) = delete;

        DictItemContext Value(json::Node node) {
            builder_.Value(std::move(node));
            return builder_;
        }

        Builder& EndDict() = delete;

        Builder& EndArray() = delete;
    };

    class DictItemContext : public ItemContext {
      public:
        DictItemContext(Builder& builder) : ItemContext(builder) {}

        ItemContext Value(json::Node node) = delete;
        ItemContext StartDict() = delete;
        ItemContext StartArray() = delete;
        ItemContext EndArray() = delete;
    };

    class ArrayItemContext : public ItemContext {
      public:
        ArrayItemContext(Builder& builder) : ItemContext(builder) {}
        KeyItemContext Key(std::string str) = delete;
        Builder& EndDict() = delete;

        ArrayItemContext Value(json::Node node) {
            builder_.Value(std::move(node));
            return builder_;
        }
    };



    KeyItemContext Key(std::string str);
    Builder& Value(json::Node node);
    //Builder& StartDict();
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndDict();
    Builder& EndArray();

    json::Node Build() {
        if(!root_.is_null() && nodes_stack_.empty()) {
            return root_;
        }
        throw std::logic_error("Build not call for nullptr object");
    }

    ~Builder() {
        for(size_t i = 0; i < nodes_stack_.size(); ++i) {
            delete nodes_stack_[i];
        }
        nodes_stack_.clear();
    }

  private:
    // То что будем создавать
    Node root_;
    //указатели на текущий создаваемый объект
    std::vector<Node*> nodes_stack_;
};
}
