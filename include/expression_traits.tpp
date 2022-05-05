#pragma once

#include "exponential.h"
#include "expression_traits.h"

#include <random>

template <>
struct ExpressionTraits<Exponential> : BaseExpressionTraits<Exponential>
{
private:
    static bool checkAdditive(const Exponential & left, const Exponential & right)
    {
        double e = left.exponent() - right.exponent();
        double d = std::pow(10, e < 0 ? -e : e);
        double a = left.significand();
        double b = right.significand();
        return checkBounds(a * d) && checkBounds(b * d) && checkBounds((a + b) * d) && checkBounds((a - b) * d);
    }

    static bool checkMultiplicative(const Exponential & left, const Exponential & right)
    {
        double a = left.significand();
        double b = right.significand();
        double c = left.exponent();
        double d = right.exponent();
        return checkBounds(a * b) && checkBounds(c + d) && checkBounds(c - d);
    }

public:
    static Exponential randomNumber(std::mt19937 & rng)
    {
        std::uniform_int_distribution<int> dist(-100, 100);
        return Exponential(dist(rng), dist(rng));
    }

    static inline const UnaryOperation UNARY_OPERATIONS[]{
            UnaryOperation(std::negate{}, operator-),
    };

    static inline const BinaryOperation BINARY_OPERATIONS[]{
            BinaryOperation(std::plus{}, operator+, checkAdditive),
            BinaryOperation(std::minus{}, operator-, checkAdditive),
            BinaryOperation(std::multiplies{}, operator*, checkMultiplicative),
            BinaryOperation(std::divides{}, operator/, [](const Exponential & left, const Exponential & right) {
                return right != 0 && checkMultiplicative(left, right);
            }),
    };
};

using Number = Exponential;
