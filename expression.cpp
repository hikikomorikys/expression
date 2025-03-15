#include "expression.hpp"
#include <cmath>
#include <sstream>
#include <stdexcept>

template <typename T>
typename Expression<T>::Ptr Expression<T>::add(Ptr lhs, Ptr rhs){
    return std::make_shared<Expression<T>>('+', lhs, rhs);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::subtract(Ptr lhs, Ptr rhs){
    return std::make_shared<Expression<T>>('-', lhs, rhs);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::multiply(Ptr lhs, Ptr rhs){
    return std::make_shared<Expression<T>>('*', lhs, rhs);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::divide(Ptr lhs, Ptr rhs){
    return std::make_shared<Expression<T>>('/', lhs, rhs);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::power(Ptr lhs, Ptr rhs){
    return std::make_shared<Expression<T>>('^', lhs, rhs);
}

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
    if (std::getline(iss, left, ' ') && iss >> op && std::getline(iss, right)) {
        return std::make_shared<Expression<T>>(op, fromString(left), fromString(right));
    }
    throw std::runtime_error("Ошибка парсинга выражения");
}

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
    if (this != &other){
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
template class Expression<double>;
