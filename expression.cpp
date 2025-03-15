#include "expression.hpp"
#include <sstream>
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
typename Expression<T>::Ptr Expression<T>::operator+(Ptr rhs) const {
    return std::make_shared<Expression>('+', this->shared_from_this(), rhs);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::operator-(Ptr rhs) const {
    return std::make_shared<Expression>('-', this->shared_from_this(), rhs);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::operator*(Ptr rhs) const {
    return std::make_shared<Expression>('*', this->shared_from_this(), rhs);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::operator/(Ptr rhs) const {
    return std::make_shared<Expression>('/', this->shared_from_this(), rhs);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::pow(Ptr rhs) const {
    return std::make_shared<Expression>('^', this->shared_from_this(), rhs);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::sin(Ptr arg) {
    auto expr = std::make_shared<Expression>("sin");
    expr->type = Type::FUNCTION;
    expr->func = "sin";
    expr->left = arg;
    return expr;
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::cos(Ptr arg) {
    auto expr = std::make_shared<Expression>("cos");
    expr->type = Type::FUNCTION;
    expr->func = "cos";
    expr->left = arg;
    return expr;
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::ln(Ptr arg) {
    auto expr = std::make_shared<Expression>("ln");
    expr->type = Type::FUNCTION;
    expr->func = "ln";
    expr->left = arg;
    return expr;
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::exp(Ptr arg) {
    auto expr = std::make_shared<Expression>("exp");
    expr->type = Type::FUNCTION;
    expr->func = "exp";
    expr->left = arg;
    return expr;
}

template <typename T>
T Expression<T>::evaluate(const std::map<std::string, T>& variables) const {
    switch (type) {
        case Type::CONSTANT:
            return value;
        case Type::VARIABLE:
            if (variables.count(variable))
                return variables.at(variable);
            throw std::runtime_error("Variable '" + variable + "' not found.");
        case Type::OPERATION: {
            T left_val = left->evaluate(variables);
            T right_val = right->evaluate(variables);
            switch (op) {
                case '+': return left_val + right_val;
                case '-': return left_val - right_val;
                case '*': return left_val * right_val;
                case '/': return left_val / right_val;
                case '^': return std::pow(left_val, right_val);
                default: throw std::runtime_error("Unknown operation.");
            }
        }
        case Type::FUNCTION:
            if (func == "sin") return std::sin(left->evaluate(variables));
            if (func == "cos") return std::cos(left->evaluate(variables));
            if (func == "ln") return std::log(left->evaluate(variables));
            if (func == "exp") return std::exp(left->evaluate(variables));
            throw std::runtime_error("Unknown function.");
    }
    return T{};
}

template <typename T>
std::string Expression<T>::toString() const {
    std::ostringstream oss;
    switch (type) {
        case Type::CONSTANT:
            oss << value;
            break;
        case Type::VARIABLE:
            oss << variable;
            break;
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
