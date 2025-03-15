#include "expression.hpp"
#include <cmath>
#include <sstream>
#include <stdexcept>

template <typename T>
Expression<T>::Expression(T value)
    : op_(0), value_(value), variable_(""), lhs_(nullptr), rhs_(nullptr) {}

template <typename T>
Expression<T>::Expression(const std::string& variable)
    : op_(0), value_(0), variable_(variable), lhs_(nullptr), rhs_(nullptr) {}

template <typename T>
Expression<T>::Expression(char op, Ptr lhs, Ptr rhs)
    : op_(op), value_(0), variable_(""), lhs_(lhs), rhs_(rhs) {}

template <typename T>
std::string Expression<T>::toString() const {
    std::ostringstream os;
    os.precision(6);
    os << std::fixed;
    if (op_ == 0) {
        if (!variable_.empty()) {
            return variable_;
        }
        return std::to_string(value_);
    }
    if (op_ == 's') {
        return "sin(" + lhs_->toString() + ")";
    } else if (op_ == 'c') {
        return "cos(" + lhs_->toString() + ")";
    } else if (op_ == 'l') {
        return "ln(" + lhs_->toString() + ")";
    } else if (op_ == 'e') {
        return "exp(" + lhs_->toString() + ")";
    }
    else if (op_ == '+') {
        return "(" + lhs_->toString() + " + " + rhs_->toString() + ")";
    }
    else if (op_ == '*') {
        return "(" + lhs_->toString() + " * " + rhs_->toString() + ")";
    }
    else if (op_ == '+') {
        return lhs_->toString() + " + " + rhs_->toString();
    } else if (op_ == '*') {
        return "(" + lhs_->toString() + " * " + rhs_->toString() + ")";
    }
    return "(" + lhs_->toString() + " " + op_ + " " + rhs_->toString() + ")";
}

template <typename T>
T Expression<T>::evaluate(const std::map<std::string, T>& variables) const {
    if (op_ == 0) {
        if (!variable_.empty()) {
            auto it = variables.find(variable_);
            if (it == variables.end()) throw std::runtime_error("Variable not found: " + variable_);
            return it->second;
        } else {
            return value_;
        }
    }
    T lhs_val = lhs_->evaluate(variables);
    switch (op_) {
        case '+':
            return lhs_val + rhs_->evaluate(variables);
        case '-':
            return lhs_val - rhs_->evaluate(variables);
        case '*':
            return lhs_val * rhs_->evaluate(variables);
        case '/': {
            T rhs_val = rhs_->evaluate(variables);
            if (rhs_val == T(0)) throw std::runtime_error("Division by zero.");
            return lhs_val / rhs_val;
        }
        case '^':
            return std::pow(lhs_val, rhs_->evaluate(variables));
        case 's':
            return std::sin(lhs_val);
        case 'c':
            return std::cos(lhs_val);
        case 'l':
            if (lhs_val <= T(0)) throw std::runtime_error("ln domain error.");
            return std::log(lhs_val);
        case 'e':
            return std::exp(lhs_val);
        default:
            throw std::runtime_error("Unknown operator in evaluation.");
    }
}

template <typename T>
std::shared_ptr<Expression<T>> Expression<T>::differentiate(const std::string& variableName) const {
    if (op_ == 0) {
        if (!variable_.empty()) {
            return variable_ == variableName
                ? std::make_shared<Expression<T>>(1.0)
                : std::make_shared<Expression<T>>(0.0);
        } else {
            return std::make_shared<Expression<T>>(0.0);
        }
    } else if (op_ == '+') {
        return std::make_shared<Expression<T>>(
            '+',
            lhs_->differentiate(variableName),
            rhs_->differentiate(variableName)
        );
    } else if (op_ == '-') {
        return std::make_shared<Expression<T>>(
            '-',
            lhs_->differentiate(variableName),
            rhs_->differentiate(variableName)
        );
    } else if (op_ == '*') {
        auto left = std::make_shared<Expression<T>>(
            '*',
            lhs_->differentiate(variableName),
            rhs_
        );
        auto right = std::make_shared<Expression<T>>(
            '*',
            lhs_,
            rhs_->differentiate(variableName)
        );
        return std::make_shared<Expression<T>>(
            '+',
            right,
            left
        );
    } else if (op_ == '/') {
        return std::make_shared<Expression<T>>(
            '/',
            std::make_shared<Expression<T>>(
                '-',
                std::make_shared<Expression<T>>(
                    '*',
                    lhs_->differentiate(variableName),
                    rhs_
                ),
                std::make_shared<Expression<T>>(
                    '*',
                    lhs_,
                    rhs_->differentiate(variableName)
                )
            ),
            std::make_shared<Expression<T>>(
                '*', rhs_, rhs_
            )
        );
    } else if (op_ == 's') {
        auto inner_diff = lhs_->differentiate(variableName);
        return std::make_shared<Expression<T>>(
            '*',
            Expression<T>::cos(lhs_),
            inner_diff
        );
    } else if (op_ == 'c') {
        return std::make_shared<Expression<T>>(
            '*',
            std::make_shared<Expression<T>>(
                '-',
                std::make_shared<Expression<T>>(0.0),
                Expression<T>::sin(lhs_)
            ),
            lhs_->differentiate(variableName)
        );
    } else if (op_ == 'l') {
        return std::make_shared<Expression<T>>(
            '/',
            lhs_->differentiate(variableName),
            lhs_
        );
    } else if (op_ == 'e') {
        return std::make_shared<Expression<T>>(
            '*',
            Expression<T>::exp(lhs_),
            lhs_->differentiate(variableName)
        );
    } else if (op_ == '^') {
        auto u_diff = lhs_->differentiate(variableName);
        auto v_diff = rhs_->differentiate(variableName);
        return std::make_shared<Expression<T>>(
            '*',
            std::make_shared<Expression<T>>(
                '^', lhs_, rhs_
            ),
            std::make_shared<Expression<T>>(
                '+',
                std::make_shared<Expression<T>>(
                    '*', v_diff, Expression<T>::ln(lhs_)
                ),
                std::make_shared<Expression<T>>(
                    '*',
                    rhs_,
                    std::make_shared<Expression<T>>(
                        '/',
                        u_diff,
                        lhs_
                    )
                )
            )
        );
    } else {
        throw std::runtime_error("Unknown operator in differentiation.");
    }
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::sin(Ptr arg) {
    return std::make_shared<Expression<T>>('s', arg, nullptr);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::cos(Ptr arg) {
    return std::make_shared<Expression<T>>('c', arg, nullptr);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::ln(Ptr arg) {
    return std::make_shared<Expression<T>>('l', arg, nullptr);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::exp(Ptr arg) {
    return std::make_shared<Expression<T>>('e', arg, nullptr);
}

template class Expression<double>;
