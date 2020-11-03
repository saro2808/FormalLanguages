#include <iostream>
#include <string>
#include <stack>

struct Expression {
	std::string str;
	int product_length;
};

//class ExprArray {
//public:
//	std::string* expr;
//	int size;
//	ExprArray() {
//		expr = new std::string[100];
//	}
//	~ExprArray() {
//		delete[] expr;
//	}
//};

// Postfix to infix
std::string ReadInput() {
	std::stack<Expression> stack;
	std::string c;
	while (1) {
		c = getchar();
		if (c == "\n") {
			break;
		}
		if (c == "+") {
			Expression temp = stack.top();
			stack.pop();
			temp.str = stack.top().str + "+" + temp.str;
			stack.pop();
			temp.product_length = 0;
			stack.push(temp);
		}
		else if (c == ".") {
			Expression temp = stack.top();
			stack.pop();
			Expression temp1 = stack.top();
			stack.pop();
			if (temp.product_length == 0) {
				temp.str = "(" + temp.str + ")";
				temp.product_length = 1;
			}
			if (temp1.product_length == 0) {
				temp1.str = "(" + temp1.str + ")";
				temp1.product_length = 1;
			}
			temp.str = temp1.str + "." + temp.str;
			temp.product_length += temp1.product_length;
			stack.push(temp);
		}
		else if (c == "*") {
			Expression temp = stack.top();
			stack.pop();
			if (temp.product_length != 1) {
				temp.str = "(" + temp.str + ")";
			}
			temp.str += "*";
			temp.product_length = 1;
			stack.push(temp);
		}
		else {
			Expression temp = {c, 1};
			stack.push(temp);
		}
	}
	return stack.top().str;
}

// a0 + a1+ ... + an -> (a0, a1, ... , an)
/*ExprArray SumToList(std::string expr) {
	int size = 0;
	for (int i = 0; expr[i] != '\0'; ++i) {
		if (expr[i] == '+') {
			++size;
		}
	}
	std::string* res = new std::string[size];
	int real_size = 0;
	int balance = 0;
	int start = 0;
	int i = start;
	while (expr[i] != '\0') {
		if (expr[i] == '(') {
			++balance;
		}
		else if (expr[i] == ')') {
			--balance;
		}
		if (balance == 0 && (expr[i + 1] == '+' || expr[i + 1] == '\0')) {
			res[real_size] = expr.substr(start, i);
			start = i + 2;
			++real_size;
		}
		++i;
	}
	ExprArray result;
	result.expr = res;
	result.size = real_size;
	return result;
}*/

int Count(std::string expr) {
	int size = 0;
	for (int i = 0; i < expr.length(); ++i) {
		if (expr[i] == '+') {
			++size;
		}
	}
	int *FirstLevelPluses = new int[size];
	int real_size = 0;
	FirstLevelPluses[0] = -1;
	++real_size;
	int balance = 0;
	for (int i = 0; i < expr.length; ++i) {
		if (expr[i] == '(') {
			++balance;
		}
		else if (expr[i] == ')') {
			--balance;
		}
		if (balance == 0 && (expr[i + 1] == '+' || expr[i + 1] == '\0')) {
			FirstLevelPluses[real_size] = i + 1;
			++real_size;
			++i;
		}
	}
	// Now array of '+'s is ready
	for (int i = 0; i < real_size - 1; ++i) {

	}
	delete[] FirstLevelPluses;
}

int main() {
	std::cout << ReadInput() << std::endl;
	/*ExprArray res;
	res = SumToList(ReadInput());
	for (int i = 0; i < res.size; ++i) {
		std::cout << res.expr[i] << std::endl;
	}*/
	system("pause");
	return 0;
}
