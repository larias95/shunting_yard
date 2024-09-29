/*
 * author: Luis Enrique Arias Curbelo
 * repo:   https://github.com/larias95/shunting_yard
 */

#pragma once

#include <cmath>
#include <vector>

using namespace std;

#include "context.hpp"

using namespace sy;

/* functions: */

inline double _abs(const vector<double>& args) {
    return std::abs(args[0]);
}

inline double _sqrt(const vector<double>& args) {
    return std::sqrt(args[0]);
}

inline double _exp(const vector<double>& args) {
    return std::exp(args[0]);
}

inline double _log(const vector<double>& args) {
    return std::log(args[0]);
}

inline double _sin(const vector<double>& args) {
    return std::sin(args[0]);
}

inline double _cos(const vector<double>& args) {
    return std::cos(args[0]);
}

inline double _tan(const vector<double>& args) {
    return std::tan(args[0]);
}

inline double _min(const vector<double>& args) {
    return std::min(args[0], args[1]);
}

inline double _max(const vector<double>& args) {
    return std::max(args[0], args[1]);
}

/* operators: */

inline double _neg(const vector<double>& args) {
    return -args[0];
}

inline double _add(const vector<double>& args) {
    return args[0] + args[1];
}

inline double _sub(const vector<double>& args) {
    return args[0] - args[1];
}

inline double _mul(const vector<double>& args) {
    return args[0] * args[1];
}

inline double _div(const vector<double>& args) {
    return args[0] / args[1];
}

inline double _rem(const vector<double>& args) {
    return std::fmod(args[0], args[1]);
}

inline double _pow(const vector<double>& args) {
    return std::pow(args[0], args[1]);
}

inline double _log_b(const vector<double>& args) {
    return std::log(args[1]) / std::log(args[0]);
}

inline double _factorial(const vector<double>& args) {
    return std::tgamma(args[0] + 1);
}

inline ParsingContext* get_context() {
    
    ParsingContext* context = (new ParsingContext)

    // constants:
    ->set("e", 2.718281828459045235360287471352662498L)
    ->set("phi", 1.618033988749894848204586834365638118L)
    ->set("pi", 3.141592653589793238462643383279502884L)

    // functions:
    ->set("abs", Evaluable_t::Function(1, _abs))
    ->set("sqrt", Evaluable_t::Function(1, _sqrt))
    ->set("exp", Evaluable_t::Function(1, _exp))
    ->set("log", Evaluable_t::Function(1, _log))
    ->set("sin", Evaluable_t::Function(1, _sin))
    ->set("cos", Evaluable_t::Function(1, _cos))
    ->set("tan", Evaluable_t::Function(1, _tan))
    ->set("min", Evaluable_t::Function(2, _min))
    ->set("max", Evaluable_t::Function(2, _max))

    // operators:
    ->set("~", Operator_t::Unary(10, Operator_t::position_t::PREFIX, _neg))
    ->set("+", Operator_t::Binary(8, Operator_t::assoc_t::LEFT, _add))
    ->set("-", Operator_t::Binary(8, Operator_t::assoc_t::LEFT, _sub))
    ->set("*", Operator_t::Binary(9, Operator_t::assoc_t::LEFT, _mul))
    ->set("/", Operator_t::Binary(9, Operator_t::assoc_t::LEFT, _div))
    ->set("%", Operator_t::Binary(9, Operator_t::assoc_t::LEFT, _rem))
    ->set("^", Operator_t::Binary(10, Operator_t::assoc_t::RIGHT, _pow))
    ->set("_", Operator_t::Binary(10, Operator_t::assoc_t::RIGHT, _log_b))
    ->set("!", Operator_t::Unary(11, Operator_t::position_t::POSTFIX, _factorial));

    return context;
}
