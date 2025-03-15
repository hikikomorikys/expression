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

template <typename T>
T Expression<T>::evaluate(const std::map<std::string, T>& variables) const{
    if (type == Type::CONSTANT) {
        return value;
    }
    if (type == Type::VARIABLE){
        auto it = variables.find(variable);
        if (it != variables.end()){
            return it->second;
        }
        throw std::runtime_error("Переменная не найдена: " + variable);
    }
    if (type == Type::OPERATION){
        T left_val = left->evaluate(variables);
        T right_val = right->evaluate(variables);
        switch (op){
            case '+': return left_val + right_val;
            case '-': return left_val - right_val;
            case '*': return left_val * right_val;
            case '/': return left_val / right_val;
            case '^': return std::pow(left_val, right_val);
            default: throw std::runtime_error("Неизвестная операция");
        }
    }
    throw std::runtime_error("Ошибка вычисления выражения");
}

template <typename T>
std::string Expression<T>::toString() const{
    std::ostringstream oss;
    if (type == Type::CONSTANT){
        oss << value;
    } else if (type == Type::VARIABLE){
        oss << variable;
    } else if (type == Type::OPERATION){
        oss << "(" << left->toString() << " " << op << " " << right->toString() << ")";
    }
    return oss.str();
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::differentiate(const std::string& var) const {
    if (type == Type::CONSTANT) {
        return std::make_shared<Expression<T>>(T(0));
    }
    if (type == Type::VARIABLE) {
        return std::make_shared<Expression<T>>(variable == var ? T(1) : T(0));
    }
    if (type == Type::OPERATION) {
        Ptr dl = left->differentiate(var);
        Ptr dr = right->differentiate(var);
        switch (op) {
            case '+': return std::make_shared<Expression<T>>('+', dl, dr);
            case '-': return std::make_shared<Expression<T>>('-', dl, dr);
            case '*': return std::make_shared<Expression<T>>('+',
                          std::make_shared<Expression<T>>('*', dl, right),
                          std::make_shared<Expression<T>>('*', left, dr));
            case '/': return std::make_shared<Expression<T>>('/',
                          std::make_shared<Expression<T>>('-',
                              std::make_shared<Expression<T>>('*', dl, right),
                              std::make_shared<Expression<T>>('*', left, dr)),
                          std::make_shared<Expression<T>>('*', right, right));
            case '^':
                return std::make_shared<Expression<T>>('*',
                           std::make_shared<Expression<T>>('*', right,
                               std::make_shared<Expression<T>>('^', left,
                                   std::make_shared<Expression<T>>('-', right, std::make_shared<Expression<T>>(T(1))))),
                           dl);
            default: throw std::runtime_error("Неизвестная операция в дифференцировании");
        }
    }
    throw std::runtime_error("Ошибка вычисления производной");
}
template class Expression<double>;
