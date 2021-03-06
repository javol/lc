#include <iostream>
#include <algorithm>
#include <sstream>

#include "symbolic.h"

using std::cout;
using std::cerr;
using std::endl;

Expression::Expression() {}

Expression::Expression(const Symbol& x) 
{
	op = UNDEFINED;
	symbols.clear();
	symbols.push_back(x);
}

Expression Expression::eval()
{
}

bool Expression::is_numeric() const
{
	if (symbols.size() == 1 && symbols[0].is_numeric()) return true;
	else return false;
}

template<Op op>
Expression op_func(const Expression& e, const Expression& x)
{
	Expression expr;
	expr.op = op;
	expr.symbols.push_back(e);
	expr.symbols.push_back(x);
	return expr;
}

Expression Expression::operator+(const Expression& x) { return op_func<ADD>(*this, x); }
Expression Expression::operator-(const Expression& x) { return op_func<SUB>(*this, x); }
Expression Expression::operator*(const Expression& x) { return op_func<MUL>(*this, x); }
Expression Expression::operator/(const Expression& x) { return op_func<DIV>(*this, x); }
Expression Expression::operator&(const Expression& x) { return op_func<AND>(*this, x); }
Expression Expression::operator|(const Expression& x) { return op_func<OR>(*this, x); }
Expression Expression::operator^(const Expression& x) { return op_func<XOR>(*this, x); }

Expression Expression::operator=(const Symbol& x) 
{ 
	Expression expr(*this);
	expr.symbols.push_back(x); 
	return expr;
}

std::string Expression::print()
{
	if (symbols.size() == 1) return symbols[0].print();
	if (symbols.size() == 2) return std::string("(") + symbols[0].print() + op_strings[op] + symbols[1].print() + ")";
	return "";
}

Symbol::Symbol() : type(UNDEFINED) {}
Symbol::Symbol(const std::string& x) : type(STRING), name(x) {}
Symbol::Symbol(int128_t x) : type(NUMERIC), value(x) {}
Symbol::Symbol(const Expression& x) : type(EXPRESSION), expr(x) {}
bool Symbol::is_numeric() const
{
	if (type == NUMERIC) return true;
	else if (type == EXPRESSION) return expr.is_numeric();
	else return false;
}
std::string Symbol::print()
{
	if (type == NUMERIC) 
	{
		std::stringstream stream;
		if (value > 0xFFFFFF) stream << "0x" << std::hex;
		stream << value;
		return stream.str();
	}
	else if (type == STRING) return name;
	else if (type == EXPRESSION) return expr.print();
}

void SymbolicEnv::add(const std::string& name, const Symbol& s) { env[name] = s; }
void SymbolicEnv::remove(const std::string& name) { env.erase(name); }
Symbol& SymbolicEnv::operator[](const std::string& name) 
{ 
	if (env.count(name)) return env[name];
	env[name] = Symbol(name);
	return env[name]; 
}
void SymbolicEnv::print()
{
	for (auto x : env)
		if (x.first[0] == '*' || x.first == "SP")
			cout << x.first << " = " << Expression(x.second).print() << endl;
}

std::vector<std::string> split(const std::string& str)
{
	std::istringstream ss(str);
	std::vector<std::string> result;
	do
	{
		std::string x;
		ss >> x;
		x.erase( std::remove_if( x.begin(), x.end(), ::isspace ), x.end() );
		if (!x.empty())
			result.push_back(x);
	} while(ss);
	return result;
}


bool is_numeric(const std::string& s)
{
	if (s[0] == '-' || s[0] == '+') return true;
	char* p;
	long converted = strtoull(s.c_str(), &p, 10);
	return *p ? false : true;
}

int main()
{
	std::string line;
    std::vector<std::string> input;
    while (std::getline(std::cin, line))
       input.push_back(line);

	SymbolicEnv env;

	for (auto& line : input)
	{
		auto tokens = split(line);
		if (tokens[0][0] == '*')
			tokens[0] = std::string("*") + env[tokens[0].substr(1, std::string::npos)].print();
		Symbol& target = env[tokens[0]];
		// i1 = i2
		if (tokens.size() == 3)
		{
			if (is_numeric(tokens[2])) target = Symbol(std::stoull(tokens[2]));
			else if (tokens[2][0] == '*')
				target = env[std::string("*") + env[tokens[2].substr(1, std::string::npos)].print()];
			else target = env[tokens[2]];
		}
		// i1 = i2 + 2
		else if (tokens.size() == 5)
		{
			Symbol op1 = is_numeric(tokens[2]) ? (tokens[2][0] == '-' ? Symbol(std::stoll(tokens[2])) : Symbol(std::stoull(tokens[2]))) : env[tokens[2]];
			Symbol op2 = is_numeric(tokens[4]) ? (tokens[4][0] == '-' ? Symbol(std::stoll(tokens[4])) : Symbol(std::stoull(tokens[4]))) : env[tokens[4]];
			if (tokens[3] == "+") target = Expression(op1) + Expression(op2);
			if (tokens[3] == "-") target = Expression(op1) - Expression(op2);
			if (tokens[3] == "*") target = Expression(op1) * Expression(op2);
			if (tokens[3] == "/") target = Expression(op1) / Expression(op2);
			if (tokens[3] == "&") target = Expression(op1) & Expression(op2);
			if (tokens[3] == "|") target = Expression(op1) | Expression(op2);
			if (tokens[3] == "^") target = Expression(op1) ^ Expression(op2);
		}
		else cerr << "Can't handle expression: " << line << endl;
	}

	env.print();

	return 0;
}