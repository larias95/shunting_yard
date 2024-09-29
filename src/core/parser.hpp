/*
 * author: Luis Enrique Arias Curbelo
 * repo:   https://github.com/larias95/shunting_yard
 */

#pragma once

#include <algorithm>
#include <cassert>
#include <stack>
#include <stdexcept>
#include <utility>
#include <vector>

using namespace std;

#include "lexer.hpp"
#include "context.hpp"

namespace sy {

#define ENSURE_TOKENS_SEQUENCE(seq) \
    assert((seq).size() > 0 && (seq).back().kind == token_t::kind_t::END);

#define THROW_INVALID_TOKEN(tok) \
    throw runtime_error("Invalid token " + (tok).str() + ".");

inline void _check_type_0 /* eps, comma, binary operator, prefix unary operator */ (
    const token_t& token,
    const ParsingContext* context
) {
    if (token.kind == token_t::kind_t::NUMBER ||
        token.kind == token_t::kind_t::IDENTIFIER ||
        token.kind == token_t::kind_t::LPARENT)
        return;
    
    if (token.kind == token_t::kind_t::OPERATOR) {
        Operator_t* op = context->get(token.text).operator_;

        if (op->arity == 1 && op->associativity == Operator_t::assoc_t::RIGHT)
            return;
    }

    THROW_INVALID_TOKEN(token);
}

inline void _check_type_1 /* number, value, right parent., postfix unary operator */ (
    const token_t& token,
    const ParsingContext* context
) {
    if (token.kind == token_t::kind_t::RPARENT ||
        token.kind == token_t::kind_t::COMMA ||
        token.kind == token_t::kind_t::END)
        return;
    
    if (token.kind == token_t::kind_t::OPERATOR) {
        Operator_t* op = context->get(token.text).operator_;

        if (op->arity == 2)
            return;
        
        if (op->arity == 1 && op->associativity == Operator_t::assoc_t::LEFT)
            return;
    }

    THROW_INVALID_TOKEN(token);
}

inline void _check_type_2 /* function */ (
    const token_t& token,
    const ParsingContext* context
) {
    if (token.kind == token_t::kind_t::LPARENT)
        return;

    THROW_INVALID_TOKEN(token);
}

inline void _check_type_3 /* left parent. */ (
    const token_t& token,
    const ParsingContext* context
) {
    if (token.kind == token_t::kind_t::NUMBER ||
        token.kind == token_t::kind_t::IDENTIFIER ||
        token.kind == token_t::kind_t::LPARENT ||
        token.kind == token_t::kind_t::RPARENT)
        return;
    
    if (token.kind == token_t::kind_t::OPERATOR) {
        Operator_t* op = context->get(token.text).operator_;

        if (op->arity == 1 && op->associativity == Operator_t::assoc_t::RIGHT)
            return;
    }

    THROW_INVALID_TOKEN(token);
}

inline void _check_relative_position(
    const vector<token_t>& tokens,
    const ParsingContext* context
) {
    auto head = tokens.begin();
    
    _check_type_0(*head, context);

    auto end = tokens.end() - 1;

    for (; head != end; ++head) {
        auto next = head + 1;

        switch (head->kind) {
            case token_t::kind_t::NUMBER:
            case token_t::kind_t::RPARENT:
                _check_type_1(*next, context);
                break;
            
            case token_t::kind_t::OPERATOR: {
                Operator_t* op = context->get(head->text).operator_;

                if (op->arity == 1 && op->associativity == Operator_t::assoc_t::LEFT)
                    _check_type_1(*next, context);
                else
                    _check_type_0(*next, context);
                break;
            }
            
            case token_t::kind_t::IDENTIFIER:
                if (context->get(head->text).content == entity_t::content_t::VALUE)
                    _check_type_1(*next, context);
                else
                    _check_type_2(*next, context);
                break;

            case token_t::kind_t::LPARENT:
                _check_type_3(*next, context);
                break;
            
            case token_t::kind_t::COMMA:
            case token_t::kind_t::END:
                _check_type_0(*next, context);
                break;
            
            default:
                THROW_INVALID_TOKEN(*head);
        }
    }
}

inline bool _should_pop(const entity_t& head, const entity_t& top) {
    if (top.content == entity_t::content_t::NONE)
        return false;
    
    if (head.operator_->precedence > top.operator_->precedence)
        return false;
    
    if (head.operator_->precedence == top.operator_->precedence)
        return head.operator_->associativity == Operator_t::assoc_t::LEFT;
    
    return true;
}

inline void to_rpn(
    const vector<token_t>& tokens,
    const ParsingContext* context,
    vector<token_t>& rpn
) {
    ENSURE_TOKENS_SEQUENCE(tokens);

    _check_relative_position(tokens, context);

    stack<pair<token_t, entity_t>> op_stack;

    for (const token_t& token : tokens)
        switch (token.kind) {
            case token_t::kind_t::NUMBER:
                rpn.push_back(token);
                break;
            
            case token_t::kind_t::OPERATOR: {
                entity_t entity = context->get(token.text);

                while (!op_stack.empty() && _should_pop(entity, op_stack.top().second)) {
                    rpn.push_back(op_stack.top().first);
                    op_stack.pop();
                }

                op_stack.push(make_pair(token, entity));

                break;
            }

            case token_t::kind_t::IDENTIFIER: {
                entity_t entity = context->get(token.text);

                if (entity.content == entity_t::content_t::VALUE)
                    rpn.push_back(token);
                
                else // entity_t::content_t::FUNCTION
                    op_stack.push(make_pair(token, entity));

                break;
            }
            
            case token_t::kind_t::LPARENT:
                op_stack.push(make_pair(token, NO_ENTITY));
                break;
            
            case token_t::kind_t::RPARENT:
            case token_t::kind_t::COMMA:
                while (!op_stack.empty() && op_stack.top().first.kind != token_t::kind_t::LPARENT) {
                    rpn.push_back(op_stack.top().first);
                    op_stack.pop();
                }

                if (op_stack.empty())
                    THROW_INVALID_TOKEN(token);
                
                if (token.kind == token_t::kind_t::RPARENT) {
                    op_stack.pop();

                    if (!op_stack.empty() && op_stack.top().first.kind == token_t::kind_t::IDENTIFIER) {
                        rpn.push_back(op_stack.top().first);
                        op_stack.pop();
                    }
                }

                break;
            
            case token_t::kind_t::END:
                while (!op_stack.empty()) {
                    if (op_stack.top().first.kind == token_t::kind_t::LPARENT)
                        THROW_INVALID_TOKEN(op_stack.top().first);
                    
                    rpn.push_back(op_stack.top().first);
                    op_stack.pop();
                }

                rpn.push_back(token);
                break;
            
            default:
                THROW_INVALID_TOKEN(token);
        }
}

inline void rpn_eval(
    const vector<token_t>& rpn,
    const ParsingContext* context,
    vector<double>& results
) {
    ENSURE_TOKENS_SEQUENCE(rpn);

    stack<double> args_stack;

    for (const token_t& token : rpn)
        switch (token.kind) {
            case token_t::kind_t::NUMBER:
                args_stack.push(std::stod(token.text));
                break;
            
            case token_t::kind_t::OPERATOR:
            case token_t::kind_t::IDENTIFIER: {
                entity_t entity = context->get(token.text);

                switch (entity.content) {
                    case entity_t::content_t::VALUE:
                        args_stack.push(entity.value);
                        break;
                    
                    case entity_t::content_t::FUNCTION:
                    case entity_t::content_t::OPERATOR: {
                        Evaluable_t* op = (entity.content == entity_t::content_t::FUNCTION)
                                        ? entity.function : entity.operator_;
                        
                        if (args_stack.size() < op->arity)
                            throw runtime_error("Too few arguments for " + token.str() + ".");

                        vector<double> args;

                        while (args.size() != op->arity) {
                            args.push_back(args_stack.top());
                            args_stack.pop();
                        }

                        std::reverse(args.begin(), args.end());
                        args_stack.push(op->evaluate(args));
                        break;
                    }
                }
                break;
            }
            
            case token_t::kind_t::END:
                if (args_stack.size() != 1)
                    throw runtime_error("RPN sequence could not be reduced to a single value.");
                
                results.push_back(args_stack.top());
                args_stack.pop();
                break;
            
            default:
                THROW_INVALID_TOKEN(token);
        }
}

}
