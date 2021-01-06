#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <utility>
#include <memory>
#include <list>
#include <algorithm>

class Rule {
public:
    Rule(const std::string& left, const std::string& right);
    Rule(const std::string& str);

    std::string left;
    std::vector<std::vector<std::string>> right;
    bool succeeded;
};

Rule::Rule(const std::string& left, const std::string& right) : left(left) {
    this->right.push_back(std::vector<std::string>(1, right));
}

Rule::Rule(const std::string& str) {
    size_t i = 0;
    while (i < str.size() && std::isspace(str[i])) {
        ++i;
    }
    while (i < str.size() && std::isalnum(str[i])) {
        left.push_back(str[i]);
        ++i;
    }
    if (!(succeeded = !left.empty())) {
        return;
    }
    while (i < str.size() && std::isspace(str[i])) {
        ++i;
    }
    if (!(succeeded = str.substr(i, 2) == "->")) {
        return;
    }
    i += 2;
    while (i < str.size() && std::isspace(str[i])) {
        ++i;
    }
    while (i < str.size()) {
        bool added = false;
        while (i < str.size() && str[i] != '|') {
            std::string fragment;
            while (i < str.size() && !std::isspace(str[i]) && str[i] != '|') {
                fragment.push_back(str[i]);
                ++i;
            }
            if (!fragment.empty()) {
                if (!added) {
                    right.push_back(std::vector<std::string>());
                    added = true;
                }
                right.back().push_back(fragment);
            }
            while (i < str.size() && std::isspace(str[i])) {
                ++i;
            }
        }
        if (i < str.size()) {
            ++i;
        }
    }
}

class Grammar {
public:
    Grammar(std::istream& is);

    template <typename Iterator>
    Iterator LeftRules(const std::string& symbol, Iterator it) const;
    bool IsTerminal(const std::string& symbol) const;

    std::vector<std::shared_ptr<Rule>> rules;
    std::vector<std::string> terminals;
};

Grammar::Grammar(std::istream& is) {
    std::string line;
    while (std::getline(is, line)) {
        line.erase(line.find_last_not_of("\r\n") + 1);
        if (line.size()) {
            auto rule = std::shared_ptr<Rule>(new Rule(line));
            if (rule) {
                rules.push_back(rule);
            }
        }
    }
    std::set<std::string> nonterminals;
    std::set<std::string> charset;
    for (auto r : rules) {
        nonterminals.insert(r->left);
        for (auto alternative : r->right) {
            for (const std::string& ch : alternative) {
                charset.insert(ch);
            }
        }
    }
    for (auto symbol : charset) {
        if (nonterminals.find(symbol) == nonterminals.end()) {
            terminals.push_back(symbol);
        }
    }
}

template <typename Iterator>
Iterator Grammar::LeftRules(const std::string& symbol, Iterator it) const {
    for (auto r : rules) {
        if (r->left == symbol) {
            *it++ = r;
        }
    }
    return it;
}

bool Grammar::IsTerminal(const std::string& symbol) const {
    return std::find(terminals.begin(), terminals.end(), symbol) != terminals.end();
}

class State {
public:
    State(std::shared_ptr<const Rule> rule, size_t right, size_t i, size_t j);

    std::shared_ptr<const Rule> rule;
    size_t right_index;
    size_t dot_index;
    size_t pos_i, pos_j;
    char made_from;
};

State::State(std::shared_ptr<const Rule> rule, size_t right_index, size_t pos_i, size_t pos_j)
    : rule(rule), right_index(right_index), dot_index(0), pos_i(pos_i), pos_j(pos_j), made_from(0) {}

std::ostream& operator<<(std::ostream& os, const State& st) {
    auto right = st.rule->right[st.right_index];
    size_t right_len = right.size();
    os << '(';
    os << st.rule->left << " -> ";
    size_t i;
    for (i = 0; i < right_len; ++i) {
        if (i == st.dot_index) {
            os << "\' ";
        }
        os << right[i];
        if (i < right_len - 1) {
            os << ' ';
        }
    }
    if (i == st.dot_index) {
        os << " \'";
    }
    os << ", [" << st.pos_i << " , " << st.pos_j << "]) ";
    switch (st.made_from) {
    case 'P':
        os << "predict";
        break;
    case 'S':
        os << "scann";
        break;
    case 'C':
        os << "complete";
        break;
    default:
        os << "start state";
    }
    return os;
}

bool operator==(const State& state1, const State& state2) {
    return state1.rule->left == state2.rule->left
        && state1.rule->right == state2.rule->right
        && state1.right_index == state2.right_index
        && state1.dot_index == state2.dot_index
        && state1.pos_i == state2.pos_i
        && state1.pos_j == state2.pos_j;
}

class EarleyParser {
public:
    EarleyParser(const Grammar& grammar);

    void AddState(State& state, size_t size);
    void Predict(State& state);
    void Scan(const State& state, const std::vector<std::string>& input);
    void Complete(const State& state);
    bool Succeeded() const;
    template <typename Iterator>
    bool Parse(Iterator begin, Iterator end, std::ostream& os);
    friend std::ostream& operator<<(std::ostream& os, const EarleyParser& ch);

    const Grammar& grammar;
    std::vector<std::list<State>> table;
};

EarleyParser::EarleyParser(const Grammar& grammar) : grammar(grammar),
    table(1, std::list<State>(1,
        State(std::shared_ptr<Rule>(new Rule("$", grammar.rules.front()->left)),
            0, 0, 0))) {}

void EarleyParser::AddState(State& state, size_t size) {
    if (size < table.size()) {
        auto it = std::find(table[size].begin(), table[size].end(), state);
        if (it == table[size].end()) {
            table[size].push_back(state);
        }
    }
    else {
        table.push_back(std::list<State>(1, state));
    }
}

void EarleyParser::Predict(State& state) {
    std::vector<std::shared_ptr<const Rule>> rules;
    grammar.LeftRules(state.rule->right[state.right_index][state.dot_index], std::back_inserter(rules));
    for (auto rule : rules) {
        for (size_t i = 0; i < rule->right.size(); ++i) {
            auto prediction = State(rule, i, state.pos_j, state.pos_j);
            prediction.made_from = 'P';
            AddState(prediction, state.pos_j);
        }
    }
}

void EarleyParser::Scan(const State& state, const std::vector<std::string>& input) {
    auto word = input[state.pos_j];
    if (word == state.rule->right[state.right_index][state.dot_index]) {
        auto scanned = State(state.rule, state.right_index, state.pos_i, state.pos_j + 1);
        scanned.dot_index = state.dot_index + 1;
        scanned.made_from = 'S';
        AddState(scanned, state.pos_j + 1);
    }
}

void EarleyParser::Complete(const State& state) {
    auto list = table[state.pos_i];
    size_t i = state.pos_i;
    size_t j = state.pos_j;
    for (auto it : list) {
        if (it.pos_j == i
            && it.dot_index != it.rule->right[state.right_index].size()
            && it.rule->right[it.right_index][it.dot_index] == state.rule->left)
        {
            auto completed = State(it.rule, it.right_index, it.pos_i, j);
            completed.dot_index = it.dot_index + 1;
            completed.made_from = 'C';
            AddState(completed, j);
        }
    }
}

bool EarleyParser::Succeeded() const {
    auto list = table[table.size() - 1];
    return std::find_if(list.begin(), list.end(),
        [](const State& st)->bool {
        return st.rule->left == "$" && st.dot_index == st.rule->right[st.right_index].size(); })
        != list.end();
}

template <typename Iterator>
bool EarleyParser::Parse(Iterator begin, Iterator end, std::ostream& os) {
    std::vector<std::string> input;
    std::copy(begin, end, std::back_inserter(input));
    for (size_t i = 0; i <= input.size(); ++i) {
        if (table.size() > i) {
            for (auto it = table[i].begin(); it != table[i].end(); ++it) {
                auto st = *it;
                if (st.dot_index != st.rule->right[st.right_index].size()
                    && !grammar.IsTerminal(st.rule->right[st.right_index][st.dot_index])) {
                    Predict(st);
                }
                else if (st.dot_index != st.rule->right[st.right_index].size()) {
                    if (i < input.size()) {
                        Scan(st, input);
                    }
                }
                else {
                    Complete(st);
                }
            }
            if (&os) {
                os << *this;
                os << '\n';
            }
        }
    }
    return Succeeded();
}

std::ostream& operator<<(std::ostream& os, const EarleyParser& ch) {
    for (size_t i = 0; i < ch.table.size(); ++i) {
        os << "State" << i << ": ";
        os << '[';
        size_t j = 0;
        for (auto st : ch.table[i]) {
            os << st;
            if (j < ch.table[i].size() - 1) {
                os << "," << std::endl;
            }
            ++j;
        }
        os << ']' << std::endl;
    }
    return os;
}


int main() {
    std::string filename = "grammar.txt";
    std::ifstream ifs(filename);
    if (ifs) {
        Grammar grammar(ifs);
        std::string word[] = { "a", "c", "b" };
        const size_t length = sizeof(word) / sizeof(word[0]);
        if (EarleyParser(grammar).Parse(word, word + length, std::cout)) {
            std::cout << "True" << std::endl;
        }
        else {
            std::cout << "False" << std::endl;
        }
    }
    else {
        std::cerr << "Error while opening the file." << std::endl;
    }
    return 0;
}
