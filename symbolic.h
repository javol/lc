#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

class Symbol;
enum Op { ADD, SUB, MUL, DIV, AND, OR, XOR, NOT, UNDEFINED };
std::vector<std::string> op_strings = { " + ", " - ", " * ", " / ", " & ", " | ", " ^ ", " ! ", " NOP " };

class Expression
{
public:
	Expression();
	Expression(const Symbol&);
	Expression eval();

	std::string print();

	Expression operator+(const Expression&);
	Expression operator-(const Expression&);
	Expression operator*(const Expression&);
	Expression operator/(const Expression&);
	Expression operator&(const Expression&);
	Expression operator|(const Expression&);
	Expression operator^(const Expression&);

	Expression operator=(const Symbol&);

	std::vector<Symbol> symbols;
	Op op;
};

class Symbol
{
public:
	Symbol();
	Symbol(const Expression&);
	Symbol(const std::string&);
	Symbol(uint64_t);
	Symbol(int64_t);
	std::string print();
	// a symbol can be a numeric, a string or an expression
	enum Type { SIGNED_NUMERIC, UNSIGNED_NUMERIC, STRING, EXPRESSION, UNDEFINED } type;
	uint64_t 		uvalue;
	int64_t 		svalue;
	std::string 	name;
	Expression 		expr;
};

class SymbolicEnv
{
public:
	void add(const std::string&, const Symbol&);
	void remove(const std::string&);
	Symbol& operator[](const std::string& name);
	void print();
private:
	std::unordered_map<std::string, Symbol> env;
};
