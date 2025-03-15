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
template <typename T>
std::string Expression<T>::toString() const {
    std::ostringstream os;
    os.precision(6);
    os << std::fixed;
    if (op_ == 0) {
        if (!variable_.empty()) {
            os << variable_;
        } else {
            os << value_;
        }
    } else {
        if (op_ == 's' || op_ == 'c' || op_ == 'l' || op_ == 'e') {
            os << op_ << "(" << lhs_->toString() << ")";
        } else {
            os << "(" << lhs_->toString() << " " << op_ << " " << rhs_->toString() << ")";
        }
    }
    return os.str();
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
        case '+': return lhs_val + rhs_->evaluate(variables);
        case '-': return lhs_val - rhs_->evaluate(variables);
        case '*': return lhs_val * rhs_->evaluate(variables);
        case '/': return lhs_val / rhs_->evaluate(variables);
        case '^': return std::pow(lhs_val, rhs_->evaluate(variables));
        case 's': return std::sin(lhs_val);
        case 'c': return std::cos(lhs_val);
        case 'l':
            if (lhs_val <= 0) throw std::runtime_error("ln domain error");
            return std::log(lhs_val);
        case 'e': return std::exp(lhs_val);
        default:
            throw std::runtime_error("Unknown operator");
    }
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::differentiate(const std::string& variable) const {
    if (op_ == 0) {
        if (variable_ == variable) {
            return std::make_shared<Expression<T>>(1.0);
        } else {
            return std::make_shared<Expression<T>>(0.0);
        }
    }

    if (op_ == '+') {
        return lhs_->differentiate(variable) + rhs_->differentiate(variable);
    } else if (op_ == '-') {
        return lhs_->differentiate(variable) - rhs_->differentiate(variable);
    } else if (op_ == '*') { 
        return (lhs_->differentiate(variable) * rhs_) + (lhs_ * rhs_->differentiate(variable));
    } else if (op_ == '/') {
        return ((lhs_->differentiate(variable) * rhs_) - (lhs_ * rhs_->differentiate(variable)))
               / (rhs_ * rhs_);
    } else if (op_ == '^') {
        if (!rhs_->variable_.empty()) {
            auto ln_u = Expression<T>::ln(lhs_);
            auto v_prime = rhs_->differentiate(variable);
            auto u_prime = lhs_->differentiate(variable);
            auto term1 = v_prime * ln_u;
            auto term2 = rhs_ * (u_prime / lhs_);
            return std::make_shared<Expression<T>>('^', lhs_, rhs_) * (term1 + term2);
        } 
        else {
            auto new_exp = lhs_ ^ (rhs_ - std::make_shared<Expression<T>>(1.0));
            return rhs_ * new_exp * lhs_->differentiate(variable);
        }
    } else if (op_ == 's') {
        return Expression<T>::cos(lhs_) * lhs_->differentiate(variable);
    } else if (op_ == 'c') {
        return Expression<T>::sin(lhs_) * std::make_shared<Expression<T>>(-1.0) * lhs_->differentiate(variable);
    } else if (op_ == 'l') {
        return lhs_->differentiate(variable) / lhs_;
    } else if (op_ == 'e') {
        return Expression<T>::exp(lhs_) * lhs_->differentiate(variable);
    }
    throw std::runtime_error("Неизвестная операция");
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

template <typename T>
typename Expression<T>::Ptr Expression<T>::fromString(const std::string& str) {
    if (str == "x" || str == "y") {
        return std::make_shared<Expression<T>>(str);
    }
    try {
        double value = std::stod(str);
        return std::make_shared<Expression<T>>(value);
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid input string");
    }
}
template class Expression<double>;
