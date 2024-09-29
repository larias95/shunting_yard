/*
 * author: Luis Enrique Arias Curbelo
 * repo:   https://github.com/larias95/shunting_yard
 */

#pragma once

#include <cassert>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace sy {

class Evaluable_t {
public:
    typedef double (*handler_t)(const vector<double>& args);

    int const arity;
    handler_t const handler;

    double evaluate(const vector<double>& args) const {
        assert(args.size() == arity);
        return handler(args);
    }

    static Evaluable_t* Function(int arity, handler_t handler) {
        return new Evaluable_t(arity, handler);
    }

protected:
    Evaluable_t(int arity, handler_t handler):
    arity(arity),
    handler(handler) {

    }
};

class Operator_t : public Evaluable_t {
public:
    enum assoc_t {
        LEFT,
        RIGHT,
    };

    enum position_t {
        PREFIX,
        POSTFIX,
    };

    int const precedence;
    assoc_t const associativity;

    static Operator_t* Unary(int precedence, position_t position, handler_t handler) {
        return new Operator_t(1, precedence, position == position_t::PREFIX ? assoc_t::RIGHT : assoc_t::LEFT, handler);
    }

    static Operator_t* Binary(int precedence, assoc_t associativity, handler_t handler) {
        return new Operator_t(2, precedence, associativity, handler);
    }

private:
    Operator_t(int arity, int precedence, assoc_t associativity, handler_t handler):
    Evaluable_t(arity, handler),
    precedence(precedence),
    associativity(associativity) {

    }
};

struct entity_t {
    enum content_t {
        NONE,
        VALUE,
        FUNCTION,
        OPERATOR,
    };

    content_t content;
    bool is_readonly;

    union {
        double value;
        Evaluable_t* function;
        Operator_t* operator_;
    };
} const NO_ENTITY {
    entity_t::content_t::NONE,
};

class ParsingContext {
public:
    ParsingContext* set(const string& key, double value, bool as_readonly=true) {
        check_key_is_assignable(key);

        entity_t entity;
        entity.content = entity_t::content_t::VALUE;
        entity.is_readonly = as_readonly;
        entity.value = value;
        entities[key] = entity;

        return this;
    }

    ParsingContext* set(const string& key, Evaluable_t* function, bool as_readonly=true) {
        check_key_is_assignable(key);

        entity_t entity;
        entity.content = entity_t::content_t::FUNCTION;
        entity.is_readonly = as_readonly;
        entity.function = function;
        entities[key] = entity;
        
        return this;
    }

    ParsingContext* set(const string& key, Operator_t* operator_, bool as_readonly=true) {
        check_key_is_assignable(key);

        entity_t entity;
        entity.content = entity_t::content_t::OPERATOR;
        entity.is_readonly = as_readonly;
        entity.operator_ = operator_;
        entities[key] = entity;
        
        return this;
    }

    entity_t get(const string& key) const {
        auto result = entities.find(key);

        if (result == entities.end())
            throw runtime_error("Context has no entity " + key + ".");

        return result->second;
    }

private:
    unordered_map<string, entity_t> entities;

    void check_key_is_assignable(const string& key) const {
        auto result = entities.find(key);

        if (result != entities.end() && result->second.is_readonly)
            throw runtime_error("Entity " + key + " is readonly.");
    }
};

}
