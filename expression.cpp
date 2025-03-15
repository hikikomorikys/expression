#include "expression.hpp"
#include <sstream>

//конструктор для числового значения
template <typename T>
Expression<T>::Expression(T val) : type(Type::CONSTANT), value(val) {}

//конструктор для переменной
template <typename T>
Expression<T>::Expression(const std::string& var) : type(Type::VARIABLE), variable(var) {}

//конструктор для бинарных операций
template <typename T>
Expression<T>::Expression(char op, Ptr left, Ptr right) 
    : type(Type::OPERATION), op(op), left(std::move(left)), right(std::move(right)) {}

//конструктор копирования
template <typename T>
Expression<T>::Expression(const Expression& other) 
    : type(other.type), value(other.value), variable(other.variable), op(other.op), 
      left(other.left), right(other.right), func(other.func) {}

//конструктор перемещения
template <typename T>
Expression<T>::Expression(Expression&& other) noexcept 
    : type(std::move(other.type)), value(std::move(other.value)), 
      variable(std::move(other.variable)), op(std::move(other.op)), 
      left(std::move(other.left)), right(std::move(other.right)), 
      func(std::move(other.func)) {}

//оператор копирования
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

//оператор перемещения
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
    return std::make_shared<Expression>('+', std::make_shared<Expression>(*this), std::make_shared<Expression>(rhs));
}
template <typename T>
typename Expression<T>::Ptr Expression<T>::operator-(const Expression& rhs) const{
    return std::make_shared<Expression>('-', std::make_shared<Expression>(*this), std::make_shared<Expression>(rhs));
}
template <typename T>
typename Expression<T>::Ptr Expression<T>::operator*(const Expression& rhs) const{
    return std::make_shared<Expression>('*', std::make_shared<Expression>(*this), std::make_shared<Expression>(rhs));
}
template <typename T>
typename Expression<T>::Ptr Expression<T>::operator/(const Expression& rhs) const{
    return std::make_shared<Expression>('/', std::make_shared<Expression>(*this), std::make_shared<Expression>(rhs));
}
template <typename T>
typename Expression<T>::Ptr Expression<T>::pow(const Expression& rhs) const{
    return std::make_shared<Expression>('^', std::make_shared<Expression>(*this), std::make_shared<Expression>(rhs));
}

//функции sin, cos, ln, exp
template <typename T>
typename Expression<T>::Ptr Expression<T>::sin(Ptr arg){
    auto expr = std::make_shared<Expression>(*arg);
    expr->type = Type::FUNCTION;
    expr->func = "sin";
    return expr;
}
template <typename T>
typename Expression<T>::Ptr Expression<T>::cos(Ptr arg){
    auto expr = std::make_shared<Expression>(*arg);
    expr->type = Type::FUNCTION;
    expr->func = "cos";
    return expr;
}
template <typename T>
typename Expression<T>::Ptr Expression<T>::ln(Ptr arg){
    auto expr = std::make_shared<Expression>(*arg);
    expr->type = Type::FUNCTION;
    expr->func = "ln";
    return expr;
}
template <typename T>
typename Expression<T>::Ptr Expression<T>::exp(Ptr arg){
    auto expr = std::make_shared<Expression>(*arg);
    expr->type = Type::FUNCTION;
    expr->func = "exp";
    return expr;
}

//символьное дифференцирование
template <typename T>
typename Expression<T>::Ptr Expression<T>::differentiate(const std::string& var) const{
    if (type == Type::CONSTANT) return std::make_shared<Expression>(T(0));
    if (type == Type::VARIABLE) return std::make_shared<Expression>(variable == var ? T(1) : T(0));
    if (type == Type::OPERATION){
        if (op == '+') return left->differentiate(var) + right->differentiate(var);
        if (op == '-') return left->differentiate(var) - right->differentiate(var);
        if (op == '*') return left->differentiate(var) * right + left * right->differentiate(var);
        if (op == '/'){
            auto num = left->differentiate(var) * right - left * right->differentiate(var);
            auto denom = right * right;
            return num / denom;
        }
        if (op == '^'){
            // d/dx (f^g) = f^g * (g * f'/f + g' * ln(f))
            auto f = left, g = right;
            auto df = f->differentiate(var), dg = g->differentiate(var);
            return (f->pow(*g)) * ((*g) * df / *f + dg * ln(f));
        }
    }
    if (type == Type::FUNCTION){
        if (func == "sin") return cos(left) * left->differentiate(var);
        if (func == "cos") return std::make_shared<Expression<T>>("-1") * sin(left) * left->differentiate(var);
        if (func == "ln") return left->differentiate(var) / left;
        if (func == "exp") return exp(left) * left->differentiate(var);
    }
    return nullptr;
}

//вычисление выражения
template <typename T>
T Expression<T>::evaluate(const std::map<std::string, T>& variables) const{
    if (type == Type::CONSTANT) return value;
    if (type == Type::VARIABLE) return variables.at(variable);
    if (type == Type::OPERATION) {
        T leftVal = left->evaluate(variables);
        T rightVal = right->evaluate(variables);
        if (op == '+') return leftVal + rightVal;
        if (op == '-') return leftVal - rightVal;
        if (op == '*') return leftVal * rightVal;
        if (op == '/') return leftVal / rightVal;
        if (op == '^') return std::pow(leftVal, rightVal);
    }
    if (type == Type::FUNCTION){
        T arg = left->evaluate(variables);
        if (func == "sin") return std::sin(arg);
        if (func == "cos") return std::cos(arg);
        if (func == "ln") return std::log(arg);
        if (func == "exp") return std::exp(arg);
    }
    return T(0);
}

//преобразование в строку
template <typename T>
std::string Expression<T>::toString() const{
    if (type == Type::CONSTANT) return std::to_string(value);
    if (type == Type::VARIABLE) return variable;
    if (type == Type::OPERATION) return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    if (type == Type::FUNCTION) return func + "(" + left->toString() + ")";
    return "";
}