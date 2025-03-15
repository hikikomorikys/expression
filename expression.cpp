#include "expression.hpp"
#include <sstream>
#include <stdexcept>
#include <cmath>

// Конструктор для константы
template <typename T>
Expression<T>::Expression(T val) 
    : type(Type::CONSTANT), value(val) {}

// Конструктор для переменной
template <typename T>
Expression<T>::Expression(const std::string& var) 
    : type(Type::VARIABLE), variable(var) {}

// Конструктор для операции
template <typename T>
Expression<T>::Expression(char op, Ptr left, Ptr right)
    : type(Type::OPERATION), op(op), left(std::move(left)), right(std::move(right)) {}

// Конструктор копирования
template <typename T>
Expression<T>::Expression(const Expression& other)
    : type(other.type), value(other.value), variable(other.variable),
      op(other.op), left(other.left), right(other.right), func(other.func) {}

// Конструктор перемещения
template <typename T>
Expression<T>::Expression(Expression&& other) noexcept
    : type(std::move(other.type)), value(std::move(other.value)),
      variable(std::move(other.variable)), op(std::move(other.op)),
      left(std::move(other.left)), right(std::move(other.right)),
      func(std::move(other.func)) {}

// Оператор копирующего присваивания
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

// Оператор перемещающего присваивания
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

// Оценка выражения
template <typename T>
T Expression<T>::evaluate(const std::map<std::string, T>& variables) const {
    switch (type) {
        case Type::CONSTANT:
            return value;
            
        case Type::VARIABLE: {
            auto it = variables.find(variable);
            if (it != variables.end()) {
                return it->second;
            }
            throw std::runtime_error("Переменная не найдена: " + variable);
        }
            
        case Type::OPERATION: {
            T left_val = left->evaluate(variables);
            T right_val = right->evaluate(variables);
            switch (op) {
                case '+': return left_val + right_val;
                case '-': return left_val - right_val;
                case '*': return left_val * right_val;
                case '/': return left_val / right_val;
                case '^': return std::pow(left_val, right_val);
                default:
                    throw std::runtime_error("Неизвестная операция");
            }
        }
            
        case Type::FUNCTION:
            throw std::runtime_error("Функции не реализованы для evaluate.");
            
        default:
            throw std::runtime_error("Ошибка при вычислении выражения.");
    }
}

// Преобразование выражения в строку
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
            
        default:
            throw std::runtime_error("Ошибка при преобразовании в строку.");
    }
    
    return oss.str();
}

// Дифференцирование выражения
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
            case '+':
                return std::make_shared<Expression<T>>('+', dl, dr);
                
            case '-':
                return std::make_shared<Expression<T>>('-', dl, dr);
                
            case '*':
                return std::make_shared<Expression<T>>('+',
                    std::make_shared<Expression<T>>('*', dl, right),
                    std::make_shared<Expression<T>>('*', left, dr)
                );
                
            case '/':
                return std::make_shared<Expression<T>>('/',
                    std::make_shared<Expression<T>>('-',
                        std::make_shared<Expression<T>>('*', dl, right),
                        std::make_shared<Expression<T>>('*', left, dr)
                    ),
                    std::make_shared<Expression<T>>('*', right, right)
                );
                
            case '^':
                return std::make_shared<Expression<T>>('*',
                    std::make_shared<Expression<T>>('*', right,
                        std::make_shared<Expression<T>>('^', left,
                            std::make_shared<Expression<T>>(right->evaluate({{var, 1.0}}) - T(1))
                        )
                    ),
                    dl
                );
                
            default:
                throw std::runtime_error("Неизвестная операция для дифференцирования.");
        }
    }
    throw std::runtime_error("Ошибка при вычислении производной.");
}
template class Expression<double>;
