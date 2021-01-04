#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <stack>

class State {
public:
	State() : hash_table({}) {}
	State(std::unordered_map<int, int> hash_table_);

	void SetValue(int pos, int value);
	State Concatenate(State s, int k);
	State Sum(State s);
	State Star(int k);
	int FetchAnswer(int pos);

	std::unordered_map<int, int> hash_table;
};

State::State(std::unordered_map<int, int> hash_table_) {
	hash_table = hash_table_;
}

void State::SetValue(int key, int value) {
	if (hash_table.find(key) == hash_table.end()) {
		hash_table[key] = value;
	}
	hash_table[key] = std::min(hash_table[key], value);
}

State State::Concatenate(State s, int k) {
	State result;
	for (auto remainder : hash_table) {
		for (auto s_remainder : s.hash_table) {
			result.SetValue((remainder.first + s_remainder.first) % k,
				remainder.second + s_remainder.second);
		}
	}
	return result;
}

State State::Sum(State s) {
	State result;
	for (auto remainder : hash_table) {
		result.SetValue(remainder.first, remainder.second);
	}
	for (auto remainder : s.hash_table) {
		result.SetValue(remainder.first, remainder.second);
	}
	return result;
}

State State::Star(int k) {
	if (hash_table.empty()) {
		throw "Can't implement \'*\'";
	}
	std::vector<int> items(k, INT_MAX);
	for (auto remainder : hash_table) {
		for (int i = 0; i < k; ++i) {
			int weight = (remainder.first * i) % k;
			int cost = remainder.second * i;
			if (/*items[weight] == INT_MAX || */items[weight] > cost) {
				items[weight] = cost;
			}
		}
	}
	std::vector<int> temp(k, INT_MAX);
	std::vector<std::vector<int>> cache(items.size(), std::move(temp));
	for (int weight = 0; weight < k; ++weight) {
		cache[weight][weight] = items[weight];
	}
	for (int weight = 1; weight < k; ++weight) {
		int i = weight;
		int cost = items[weight];
		for (int j = 0; j < k; ++j) {
			if (cache[i - 1][j] == INT_MAX) {
				cache[i][j] = cache[i - 1][j];
			}
		}
		if (cost != INT_MAX) {
			for (int j = 0; j < k; ++j) {
				int old_j = j - weight;
				if (old_j < 0) {
					old_j += k;
				}
				if (cache[i - 1][old_j] != INT_MAX) {
					if (/*cache[i][j] == INT_MAX || */cache[i][j] > cache[i - 1][old_j] + cost) {
						cache[i][j] = cache[i - 1][old_j] + cost;
					}
				}
			}
		}
	}
	State result;
	for (int i = 0; i < items.size(); ++i) {
		for (int j = 0; j < k; ++j) {
			if (cache[i][j] != INT_MAX) {
				result.SetValue(j % k, cache[i][j]);
			}
		}
	}
	return result;
}

int State::FetchAnswer(int key) {
	if (hash_table.find(key) == hash_table.end()) {
		return INT_MAX;
	}
	return hash_table[key];
}

class Parser {
public:
	Parser(std::string reg_expr, int k);

	int GetAnswer(int key);

private:
	State Calculate();

	std::string reg_expr;
	int k;
	State final_state;
};

Parser::Parser(std::string reg_expr_, int k_) {
	reg_expr = reg_expr_;
	k = k_;
	final_state = {};
}

State Parser::Calculate() {
	std::stack<State> st;
	std::string reg_expr = this->reg_expr;
	for (auto c : reg_expr) {
		if (c == 'a' || c == 'b' || c == 'c') {
			std::unordered_map<int, int> unmp = { {1, 1} };
			st.push(State(unmp));
		}
		else if (c == '1') {
			std::unordered_map<int, int> unmp = { {0, 0} };
			st.push(State(unmp));
		}
		else if (c == '*') {
			if (st.empty()) {
				throw "Can't implement \'*\'";
			}
			State current_state = st.top();
			st.pop();
			st.push(current_state.Star(k));
		}
		else if (c == '+') {
			if (st.size() < 2) {
				throw "Can't implement \'+\'";
			}
			State first_state = st.top();
			st.pop();
			State second_state = st.top();
			st.pop();
			st.push(first_state.Sum(second_state));
		}
		else if (c == '.') {
			if (st.size() < 2) {
				throw "Can't implement \'+\'";
			}
			State first_state = st.top();
			st.pop();
			State second_state = st.top();
			st.pop();
			st.push(first_state.Concatenate(second_state, k));
		}
		else {
			throw "Error while parsing the regular expression";
		}
	}
	if (st.size() == 1) {
		return st.top();
	}
	throw "Error while parsing the regular expression";
}

int Parser::GetAnswer(int key) {
	if (final_state.hash_table.empty()) {
		final_state = Calculate();
	}
	return final_state.FetchAnswer(key);
}

int main() {
	std::string alpha;
	std::cin >> alpha;
	int k, l;
	std::cin >> k >> l;
	Parser parser(alpha, k);
	int answer = parser.GetAnswer(l);
	if (answer != INT_MAX) {
		std::cout << answer << std::endl;
	}
	else {
		std::cout << "INF" << std::endl;
	}
	return 0;
}
