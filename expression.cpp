#include "expression.hpp"
#include <sstream>
#include <cmath>
#include <stdexcept>

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
Expression<T>& Expression<T>::operator=(const Expression& other) {
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
Expression<T>& Expression<T>::operator=(Expression&& other) noexcept {
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
typename Expression<T>::Ptr Expression<T>::operator+(const Expression& rhs) const {
    return add(left, rhs.left);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::operator-(const Expression& rhs) const {
    return subtract(left, rhs.left);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::operator*(const Expression& rhs) const {
    return multiply(left, rhs.left);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::operator/(const Expression& rhs) const {
    return divide(left, rhs.left);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::pow(const Expression& rhs) const {
    return power(left, rhs.left);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::sin(Ptr arg) {
    auto expr = std::make_shared<Expression>('s', arg, nullptr);
    expr->func = "sin";
    return expr;
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::cos(Ptr arg) {
    auto expr = std::make_shared<Expression>('c', arg, nullptr);
    expr->func = "cos";
    return expr;
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::ln(Ptr arg) {
    auto expr = std::make_shared<Expression>('l', arg, nullptr);
    expr->func = "ln";
    return expr;
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::exp(Ptr arg) {
    auto expr = std::make_shared<Expression>('e', arg, nullptr);
    expr->func = "exp";
    return expr;
}

template <typename T>
T Expression<T>::evaluate(const std::map<std::string, T>& vars) const {
    if (type == Type::CONSTANT) return value;
    if (type == Type::VARIABLE) {
        auto it = vars.find(variable);
        if (it != vars.end()) return it->second;
        throw std::runtime_error("Variable not found: " + variable);
    }
    if (type == Type::OPERATION) {
        T left_val = left->evaluate(vars);
        T right_val = right->evaluate(vars);
        switch (op) {
            case '+': return left_val + right_val;
            case '-': return left_val - right_val;
            case '*': return left_val * right_val;
            case '/': return left_val / right_val;
            case '^': return std::pow(left_val, right_val);
            default: throw std::runtime_error("Unknown operation.");
        }
    }
    throw std::runtime_error("Evaluation error.");
}

template <typename T>
std::string Expression<T>::toString() const {
    std::ostringstream oss;
    switch (type) {
        case Type::CONSTANT: oss << value; break;
        case Type::VARIABLE: oss << variable; break;
        case Type::OPERATION:
            oss << "(" << left->toString() << " " << op << " " << right->toString() << ")";
            break;
        case Type::FUNCTION:
            oss << func << "(" << left->toString() << ")";
            break;
    }
    return oss.str();
}
template class Expression<double>;
