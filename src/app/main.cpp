/*
 * author: Luis Enrique Arias Curbelo
 * repo:   https://github.com/larias95/shunting_yard
 */

#include <exception>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "lexer.hpp"
#include "my_context.hpp"
#include "parser.hpp"

using namespace sy;

void print_tokens(const vector<token_t>& tokens) {
    for (const token_t& token : tokens)
        cout << token.str() << " ";
    cout << endl;
}

int main() {
    ParsingContext* context = get_context();

    string line;
    vector<token_t> tokens;
    vector<token_t> rpn;
    vector<double> results;

    while (cout << ">> ", getline(cin, line), line != "exit") {
        try {
            tokenize(line, tokens);
            // print_tokens(tokens);

            to_rpn(tokens, context, rpn);
            // print_tokens(rpn);

            rpn_eval(rpn, context, results);

            cout << results[0] << endl;
        }
        catch (exception& ex) {
            cout << ex.what() << endl;
        }
        
        tokens.clear();
        rpn.clear();
        results.clear();
    }

    return 0;
}
