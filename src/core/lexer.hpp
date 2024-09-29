/*
 * author: Luis Enrique Arias Curbelo
 * repo:   https://github.com/larias95/shunting_yard
 */

#pragma once

#include <algorithm>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace sy {

struct token_t {
    enum kind_t {
        NUMBER,       // 1, 2, 3.45, 0.98, ...
        OPERATOR,     // ~, +, -, *, /, %, ^, _, !, ...
        IDENTIFIER,   // f, g, sqrt, log, x, y, pi, e, ...
        LPARENT,      // (
        RPARENT,      // )
        COMMA,        // ,
        END,          // end of a sequence of tokens
        UNKNOWN = -1, // otherwise
    };

    kind_t kind;
    string text;
    int    column;
    
    token_t(kind_t kind, string text, int column):
    kind(kind),
    text(text),
    column(column) {

    }

    string str() const;
} const END_TOKEN(token_t::kind_t::END, "", -1);

inline const char* name(token_t::kind_t token_kind) {
    switch (token_kind) {
        case token_t::kind_t::NUMBER: return "NUMBER";
        case token_t::kind_t::OPERATOR: return "OPERATOR";
        case token_t::kind_t::IDENTIFIER: return "IDENTIFIER";
        case token_t::kind_t::LPARENT: return "LPARENT";
        case token_t::kind_t::RPARENT: return "RPARENT";
        case token_t::kind_t::COMMA: return "COMMA";
        case token_t::kind_t::END: return "END";
        case token_t::kind_t::UNKNOWN: return "UNKNOWN";
    }
    return NULL;
}

inline string token_t::str() const {
    stringstream ss;
    ss << "<" << name(kind) << " " << text << " (col: " << column << ")>";
    return ss.str();
}

inline void tokenize(const string& line, vector<token_t>& tokens) {
    static vector<pair<token_t::kind_t, regex>> patterns {
        make_pair(token_t::kind_t::NUMBER, regex("^[0-9]+(\\.[0-9]+)?")),
        make_pair(token_t::kind_t::OPERATOR, regex("^[~+\\-*/%^_!]")),
        make_pair(token_t::kind_t::IDENTIFIER, regex("^[a-zA-Z][a-zA-Z0-9]*")),
        make_pair(token_t::kind_t::LPARENT, regex("^\\(")),
        make_pair(token_t::kind_t::RPARENT, regex("^\\)")),
        make_pair(token_t::kind_t::COMMA, regex("^,")),
    };

    auto head = line.begin();

    while (head != line.end()) {
        if (*head == ' ')
            ++head;
        
        else {
            token_t::kind_t token_kind = token_t::kind_t::UNKNOWN;
            string longest_prefix;

            for (auto& pattern : patterns) {
                smatch prefix;

                if (regex_search(head, line.end(), prefix, pattern.second) && prefix[0].length() > longest_prefix.length()) {
                    token_kind = pattern.first;
                    longest_prefix = prefix[0].str();
                }
            }

            if (token_kind == token_t::kind_t::UNKNOWN)
                throw runtime_error("Invalid symbol " + string(head, head+1) + ".");
            
            tokens.push_back(token_t(token_kind, longest_prefix, distance(line.begin(), head) + 1));
            head += longest_prefix.length();
        }
    }

    tokens.push_back(END_TOKEN);
}

}
