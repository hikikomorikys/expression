#include <iostream>
#include <map>
#include <cmath>
#include <complex>
#include "expression.hpp"

#define TEST_CASE(name, expr) \
    do { \
        std::cout << (expr ? "OK" : "FAIL") << name << std::endl; \
    } while (0)

using Expr = Expression<double>::Ptr;

void testNumericExpression(){
    Expr e = std::make_shared<Expression<double>>(5.8);
    TEST_CASE("Numeric Expression", e->toString() == "5.800000");
}

void testVariableExpression(){
    Expr e = std::make_shared<Expression<double>>("x");
    TEST_CASE("Variable Expression", e->toString() == "x");
}

void testArithmeticOperations()
{
    Expr x = std::make_shared<Expression<double>>("x");
    Expr y = std::make_shared<Expression<double>>("y");
    Expr sum = *x + *y;
    Expr sub = *x - *y;
    Expr mul = *x * *y;
    Expr div = *x / *y;
    Expr power = x->pow(*y);
    TEST_CASE("Addition", sum->toString() == "(x + y)");
    TEST_CASE("Subtraction", sub->toString() == "(x - y)");
    TEST_CASE("Multiplication", mul->toString() == "(x * y)");
    TEST_CASE("Division", div->toString() == "(x / y)");
    TEST_CASE("Power", power->toString() == "(x ^ y)");
}

void testFunctions()
{
    Expr x = std::make_shared<Expression<double>>("x");
    Expr sin_expr = Expression<double>::sin(x);
    Expr cos_expr = Expression<double>::cos(x);
    Expr ln_expr = Expression<double>::ln(x);
    Expr exp_expr = Expression<double>::exp(x);
    TEST_CASE("Sin Function", sin_expr->toString() == "sin(x)");
    TEST_CASE("Cos Function", cos_expr->toString() == "cos(x)");
    TEST_CASE("Ln Function", ln_expr->toString() == "ln(x)");
    TEST_CASE("Exp Function", exp_expr->toString() == "exp(x)");
}

void testEvaluation()
{
    Expr x = std::make_shared<Expression<double>>("x");
    Expr y = std::make_shared<Expression<double>>("y");
    Expr expr = *x * *y + *x;
    std::map<std::string, double> values = {{"x", 2.0}, {"y", 3.0}};
    double result = expr->evaluate(values);
    TEST_CASE("Expression Evaluation", std::abs(result - 8.0) < 1e-6);
}

void testDifferentiation()
{
    Expr x = std::make_shared<Expression<double>>("x");
    Expr y = std::make_shared<Expression<double>>("y");
    Expr expr = *x * Expression<double>::sin(x);
    Expr derivative = expr->differentiate("x");
    TEST_CASE("Differentiation", derivative->toString() == "(x * cos(x) + sin(x))");
}

int main()
{
    testNumericExpression();
    testVariableExpression();
    testArithmeticOperations();
    testFunctions();
    testEvaluation();
    testDifferentiation();
    return 0;
}
