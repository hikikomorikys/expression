#include <iostream>
#include <map>
#include <cmath>
#include "expression.hpp"

#define TEST_CASE(name, expr) \
    do { \
        std::cout << (expr ? "OK: " : "FAIL: ") << name << std::endl; \
    } while (0)

using Expr = Expression<double>::Ptr;

void testNumericExpression() {
    Expr e = std::make_shared<Expression<double>>(5.8);
    TEST_CASE("Numeric Expression", e->toString() == "5.800000");
}

void testVariableExpression() {
    Expr e = std::make_shared<Expression<double>>("x");
    TEST_CASE("Variable Expression", e->toString() == "x");
}

void testArithmeticOperations() {
    Expr x = std::make_shared<Expression<double>>("x");
    Expr y = std::make_shared<Expression<double>>("y");
    
    Expr sum = x + y;
    Expr sub = x - y;
    Expr mul = x * y;
    Expr div = x / y;
    Expr power = x ^ y;

    TEST_CASE("Addition", sum->toString() == "(x + y)");
    TEST_CASE("Subtraction", sub->toString() == "(x - y)");
    TEST_CASE("Multiplication", mul->toString() == "(x * y)");
    TEST_CASE("Division", div->toString() == "(x / y)");
    TEST_CASE("Power", power->toString() == "(x ^ y)");
}

void testEvaluation() {
    Expr x = std::make_shared<Expression<double>>("x");
    Expr y = std::make_shared<Expression<double>>("y");
    Expr expr = (x * y) + x;
    
    std::map<std::string, double> vars = {{"x", 2.0}, {"y", 3.0}};
    double result = expr->evaluate(vars);
    
    TEST_CASE("Expression Evaluation", std::abs(result - 8.0) < 1e-6);
}

void testDifferentiation() {
    Expr x = std::make_shared<Expression<double>>("x");
    Expr expr = x * Expression<double>::sin(x);
    Expr derivative = expr->differentiate("x");

    TEST_CASE("Differentiation", derivative->toString() == "(sin(x) + (x * cos(x)))");
}

int main() {
    testNumericExpression();
    testVariableExpression();
    testArithmeticOperations();
    testEvaluation();
    testDifferentiation();
    return 0;
}
