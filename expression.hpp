#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP
#include <iostream>
#include <memory>
#include <string>
#include <map>

template <typename T>
class Expression{
public:
    using Ptr = std::shared_ptr<Expression>;
    Expression(T value);
    Expression(const std::string& var);
    Expression(char op, Ptr left, Ptr right);
    Expression(const Expression& other);
    Expression(Expression&& other) noexcept;
    Expression& operator=(const Expression& other);
    Expression& operator=(Expression&& other) noexcept;
    Ptr operator+(const Expression& rhs) const;
    Ptr operator-(const Expression& rhs) const;
    Ptr operator*(const Expression& rhs) const;
    Ptr operator/(const Expression& rhs) const;
    Ptr pow(const Expression& rhs) const;
    static Ptr sin(Ptr arg);
    static Ptr cos(Ptr arg);
    static Ptr ln(Ptr arg);
    static Ptr exp(Ptr arg);
    Ptr differentiate(const std::string& var) const;
    T evaluate(const std::map<std::string, T>& variables) const;
    std::string toString() const;
    static Ptr fromString(const std::string& str);  // Объявление метода
private:
    enum class Type { CONSTANT, VARIABLE, OPERATION, FUNCTION };
    Type type;
    T value;
    std::string variable;
    char op;
    Ptr left, right;
    std::string func;
    static Ptr add(Ptr lhs, Ptr rhs);
    static Ptr subtract(Ptr lhs, Ptr rhs);
    static Ptr multiply(Ptr lhs, Ptr rhs);
    static Ptr divide(Ptr lhs, Ptr rhs);
    static Ptr power(Ptr lhs, Ptr rhs);
};
extern template class Expression<double>;
#endif
