#pragma once
#include <memory>
#include <string>
#include <map>

template <typename T>
class Expression {
public:
    using Ptr = std::shared_ptr<Expression<T>>;
    Expression(T value);
    Expression(const std::string& variable);
    Expression(char op, Ptr lhs, Ptr rhs);
    std::string toString() const;
    T evaluate(const std::map<std::string, T>& variables) const;
    Ptr differentiate(const std::string& variable) const;
    static Ptr fromString(const std::string& str);
    static Ptr sin(Ptr arg);
    static Ptr cos(Ptr arg);
    static Ptr ln(Ptr arg);
    static Ptr exp(Ptr arg);
    friend Ptr operator+(Ptr lhs, Ptr rhs) {
        return std::make_shared<Expression<T>>('+', lhs, rhs);
    }
    friend Ptr operator-(Ptr lhs, Ptr rhs) {
        return std::make_shared<Expression<T>>('-', lhs, rhs);
    }
    friend Ptr operator*(Ptr lhs, Ptr rhs) {
        return std::make_shared<Expression<T>>('*', lhs, rhs);
    }
    friend Ptr operator/(Ptr lhs, Ptr rhs) {
        return std::make_shared<Expression<T>>('/', lhs, rhs);
    }
    friend Ptr operator^(Ptr lhs, Ptr rhs) {
        return std::make_shared<Expression<T>>('^', lhs, rhs);
    }
private:
    char op_;
    T value_;
    std::string variable_;
    Ptr lhs_;
    Ptr rhs_;
};
