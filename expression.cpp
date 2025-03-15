#include "expression.hpp"
#include <sstream>
#include <stdexcept>
#include <cmath>

template <typename T>
typename Expression<T>::Ptr add(typename Expression<T>::Ptr lhs, typename Expression<T>::Ptr rhs){
    return std::make_shared<Expression<T>>('+', lhs, rhs);
}

template <typename T>
typename Expression<T>::Ptr subtract(typename Expression<T>::Ptr lhs, typename Expression<T>::Ptr rhs){
    return std::make_shared<Expression<T>>('-', lhs, rhs);
}

template <typename T>
typename Expression<T>::Ptr multiply(typename Expression<T>::Ptr lhs, typename Expression<T>::Ptr rhs){
    return std::make_shared<Expression<T>>('*', lhs, rhs);
}

template <typename T>
typename Expression<T>::Ptr divide(typename Expression<T>::Ptr lhs, typename Expression<T>::Ptr rhs){
    return std::make_shared<Expression<T>>('/', lhs, rhs);
}

template <typename T>
typename Expression<T>::Ptr power(typename Expression<T>::Ptr lhs, typename Expression<T>::Ptr rhs){
    return std::make_shared<Expression<T>>('^', lhs, rhs);
}

//fromString
template <typename T>
typename Expression<T>::Ptr Expression<T>::fromString(const std::string& str){
    std::istringstream iss(str);
    char op;
    T value;

    if (iss >> value){
        return std::make_shared<Expression<T>>(value);
    }

    if (!str.empty() && std::isalpha(str[0])){
        return std::make_shared<Expression<T>>(str);
    }
    std::string left, right;
    iss.clear();
    iss.str(str);
    if (std::getline(iss, left, ' ') && iss >> op && std::getline(iss, right)){
        return std::make_shared<Expression<T>>(op, fromString(left), fromString(right));
    }
    throw std::runtime_error("Ошибка парсинга выражения");
}

//конструкторы и операторы копирования/перемещения
template <typename T>
Expression<T>::Expression(T val) : type(Type::CONSTANT), value(val) {}

template <typename T>
Expression<T>::Expression(const std::string& var) : type(Type::VARIABLE), variable(var) {}

template <typename T>
Expression<T>::Expression(char op, Ptr left, Ptr right) 
    : type(Type::OPERATION), op(op), left(std::move(left)), right(std::move(right)) {}

template <typename T>
Expression<T>::Expression(const Expression& other)
    : type(other.type), value(other.value), variable(other.variable),
      op(other.op), left(other.left), right(other.right), func(other.func) {}

template <typename T>
Expression<T>::Expression(Expression&& other) noexcept
    : type(std::move(other.type)), value(std::move(other.value)),
      variable(std::move(other.variable)), op(std::move(other.op)),
      left(std::move(other.left)), right(std::move(other.right)),
      func(std::move(other.func)) {}

template <typename T>
Expression<T>& Expression<T>::operator=(const Expression& other){
    if (this != &other) {
        type = other.type;
        value = other.value;
        variable = other.variable;
        op = other.op;
        left = other.left;
        right = other.right;
        func = other.func;
    }
    return *this;
}
template <typename T>
Expression<T>& Expression<T>::operator=(Expression&& other) noexcept{
    type = std::move(other.type);
    value = std::move(other.value);
    variable = std::move(other.variable);
    op = std::move(other.op);
    left = std::move(other.left);
    right = std::move(other.right);
    func = std::move(other.func);
    return *this;
}

//арифметические операции
template <typename T>
typename Expression<T>::Ptr Expression<T>::operator+(const Expression& rhs) const{
    return add(std::make_shared<Expression>(*this), std::make_shared<Expression>(rhs));
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::operator-(const Expression& rhs) const{
    return subtract(std::make_shared<Expression>(*this), std::make_shared<Expression>(rhs));
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::operator*(const Expression& rhs) const{
    return multiply(std::make_shared<Expression>(*this), std::make_shared<Expression>(rhs));
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::operator/(const Expression& rhs) const{
    return divide(std::make_shared<Expression>(*this), std::make_shared<Expression>(rhs));
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::pow(const Expression& rhs) const{
    return power(std::make_shared<Expression>(*this), std::make_shared<Expression>(rhs));
}

//функции sin, cos, ln, exp
template <typename T>
typename Expression<T>::Ptr Expression<T>::sin(Ptr arg){
    auto expr = std::make_shared<Expression>(*arg);
    expr->type = Type::FUNCTION;
    expr->func = "sin";
    expr->left = arg;
    return expr;
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::cos(Ptr arg){
    auto expr = std::make_shared<Expression>(*arg);
    expr->type = Type::FUNCTION;
    expr->func = "cos";
    expr->left = arg;
    return expr;
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::ln(Ptr arg){
    auto expr = std::make_shared<Expression>(*arg);
    expr->type = Type::FUNCTION;
    expr->func = "ln";
    expr->left = arg;
    return expr;
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::exp(Ptr arg){
    auto expr = std::make_shared<Expression>(*arg);
    expr->type = Type::FUNCTION;
    expr->func = "exp";
    expr->left = arg;
    return expr;
}

//дифференцирование
template <typename T>
typename Expression<T>::Ptr Expression<T>::differentiate(const std::string& var) const{
    if (type == Type::CONSTANT) return std::make_shared<Expression>(T(0));
    if (type == Type::VARIABLE) return std::make_shared<Expression>(variable == var ? T(1) : T(0));

    if (type == Type::OPERATION){
        auto dLeft = left->differentiate(var);
        auto dRight = right->differentiate(var);
        if (op == '+') return add(dLeft, dRight);
        if (op == '-') return subtract(dLeft, dRight);
        if (op == '*') return add(multiply(dLeft, right), multiply(left, dRight));
        if (op == '/') return divide(subtract(multiply(dLeft, right), multiply(left, dRight)), power(right, std::make_shared<Expression>(2.0)));
        if (op == '^'){
            auto f = left, g = right;
            auto df = dLeft, dg = dRight;
            return multiply(power(f, g), add(multiply(dg, ln(f)), divide(multiply(g, df), f)));
        }
    }

    if (type == Type::FUNCTION){
        if (func == "sin") return multiply(cos(left), left->differentiate(var));
        if (func == "cos") return multiply(std::make_shared<Expression>(-1), multiply(sin(left), left->differentiate(var)));
        if (func == "ln") return divide(left->differentiate(var), left);
        if (func == "exp") return multiply(exp(left), left->differentiate(var));
    }
    return nullptr;
}

template class Expression<double>;
template typename Expression<double>::Ptr add<double>(typename Expression<double>::Ptr, typename Expression<double>::Ptr);
template typename Expression<double>::Ptr subtract<double>(typename Expression<double>::Ptr, typename Expression<double>::Ptr);
template typename Expression<double>::Ptr multiply<double>(typename Expression<double>::Ptr, typename Expression<double>::Ptr);
template typename Expression<double>::Ptr divide<double>(typename Expression<double>::Ptr, typename Expression<double>::Ptr);
template typename Expression<double>::Ptr power<double>(typename Expression<double>::Ptr, typename Expression<double>::Ptr);
