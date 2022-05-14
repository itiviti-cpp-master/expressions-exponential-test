#include "exponential.h"

#include <cmath>
#include <gtest/gtest.h>
#include <limits>
#include <sstream>
#include <type_traits>

using namespace ::testing;

class ExponentialTest : public testing::Test
{
protected:
    static inline const Exponential zero{};

    static inline const Exponential one{1};
    static inline const Exponential googol{1, 100};
    static inline const Exponential trillion{1'000'000'000'000ll};
    static inline const Exponential nano{1, -9};

    static inline const Exponential negative_one{-1};
    static inline const Exponential negative_googol{-1, 100};
    static inline const Exponential negative_trillion{-1'000'000'000'000ll};
    static inline const Exponential negative_nano{-1, -9};
};

TEST_F(ExponentialTest, traits)
{
    EXPECT_EQ(sizeof(Exponential), sizeof(double) * 2);
    EXPECT_TRUE(std::is_trivially_copyable_v<Exponential>);
}

TEST_F(ExponentialTest, construct)
{
    EXPECT_EQ(zero.significand(), 0);
    EXPECT_EQ(zero.exponent(), 0);

    EXPECT_EQ(one.significand(), 1);
    EXPECT_EQ(one.exponent(), 0);

    EXPECT_EQ(googol.significand(), 1);
    EXPECT_EQ(googol.exponent(), 100);

    EXPECT_EQ(trillion.significand(), 1);
    EXPECT_EQ(trillion.exponent(), 12);

    EXPECT_EQ(nano.significand(), 1);
    EXPECT_EQ(nano.exponent(), -9);

    const Exponential x{42, -43};

    EXPECT_EQ(x.significand(), 42);
    EXPECT_EQ(x.exponent(), -43);

    const auto max = std::numeric_limits<int64_t>::max();
    const auto min = std::numeric_limits<int64_t>::min();

    const Exponential y{max, max};

    EXPECT_EQ(y.significand(), max);
    EXPECT_EQ(y.exponent(), max);

    const Exponential z{min, min};

    EXPECT_EQ(z.significand(), min);
    EXPECT_EQ(z.exponent(), min);
}

TEST_F(ExponentialTest, contruct_negative)
{
    EXPECT_EQ(negative_one.significand(), -1);
    EXPECT_EQ(negative_one.exponent(), 0);

    EXPECT_EQ(negative_googol.significand(), -1);
    EXPECT_EQ(negative_googol.exponent(), 100);

    EXPECT_EQ(negative_trillion.significand(), -1);
    EXPECT_EQ(negative_trillion.exponent(), 12);

    EXPECT_EQ(negative_nano.significand(), -1);
    EXPECT_EQ(negative_nano.exponent(), -9);
}

TEST_F(ExponentialTest, double_cast)
{
    EXPECT_DOUBLE_EQ(static_cast<double>(zero), 0.0);
    EXPECT_DOUBLE_EQ(static_cast<double>(one), 1.0);
    EXPECT_DOUBLE_EQ(static_cast<double>(negative_one), -1.0);
    EXPECT_DOUBLE_EQ(static_cast<double>(nano), 1e-9);
    EXPECT_DOUBLE_EQ(static_cast<double>(negative_nano), -1e-9);
    EXPECT_DOUBLE_EQ(static_cast<double>(trillion), 1e12);
    EXPECT_DOUBLE_EQ(static_cast<double>(negative_trillion), -1e12);
    EXPECT_DOUBLE_EQ(static_cast<double>(googol), 1e100);
    EXPECT_DOUBLE_EQ(static_cast<double>(negative_googol), -1e100);
}

TEST_F(ExponentialTest, equals)
{
#define CHECK_EQ(e, value) \
    EXPECT_EQ(e, e);       \
    EXPECT_EQ(e, value);   \
    EXPECT_EQ(value, e)

    CHECK_EQ(zero, 0);
    CHECK_EQ(one, 1);
    CHECK_EQ(negative_one, -1);
    CHECK_EQ(trillion, 1'000'000'000'000);
    CHECK_EQ(negative_trillion, -1'000'000'000'000);

#undef CHECK_EQ
}

TEST_F(ExponentialTest, not_equals)
{
#define CHECK_NE(e, value) \
    EXPECT_NE(e, value);   \
    EXPECT_NE(value, e)

    CHECK_NE(zero, 1);
    CHECK_NE(one, 0);
    CHECK_NE(negative_one, 0);
    CHECK_NE(trillion, 10101010101);
    CHECK_NE(negative_trillion, -1);
    CHECK_NE(one, negative_one);

#undef CHECK_NE
}

TEST_F(ExponentialTest, negate)
{
    const Exponential x = googol;

    EXPECT_EQ(-x, 0 - x);
    EXPECT_EQ(-(-x), x);
}

TEST_F(ExponentialTest, add)
{
    EXPECT_EQ(1 + zero, zero + 1);
    EXPECT_EQ(1 + zero, 1);
    EXPECT_EQ(trillion + 1, 1 + trillion);
    EXPECT_EQ(trillion + 1, 1'000'000'000'001ll);
    EXPECT_EQ(googol + 0, googol);

    EXPECT_EQ(1 + negative_one, 0);
    EXPECT_EQ(-1 + negative_one, -2);

    const Exponential x = 2;
    const Exponential y = 3;
    const Exponential z = 5;

    EXPECT_EQ(x + y, y + x);
    EXPECT_EQ((x + y) + z, x + (y + z));

    const Exponential w{5, 100};
    const auto h = w + w;

    EXPECT_EQ(h.significand(), 1);
    EXPECT_EQ(h.exponent(), 101);
}

TEST_F(ExponentialTest, subtract)
{
    EXPECT_EQ(1 - zero, 1);
    EXPECT_EQ(0 - one, -1);
    EXPECT_EQ(trillion - 1, 999'999'999'999ll);
    EXPECT_EQ(1 - trillion, -999'999'999'999ll);

    EXPECT_EQ(1 - negative_one, 2);
    EXPECT_EQ(-1 - negative_one, 0);
    EXPECT_EQ(one - 1, 0);

    const Exponential x = 2;
    const Exponential y = 3;
    const Exponential z = 5;

    EXPECT_EQ((x - y) + z, x - (y - z));

    const Exponential w{5, 100};
    const auto h = 0 - w - w;

    EXPECT_EQ(h.significand(), -1);
    EXPECT_EQ(h.exponent(), 101);
}

TEST_F(ExponentialTest, multiple)
{
    EXPECT_EQ(0 * zero, 0);
    EXPECT_EQ(zero * 0, 0);
    EXPECT_EQ(googol * 0, 0);

    EXPECT_EQ(one * 1, 1);
    EXPECT_EQ(1 * negative_one, -1);
    EXPECT_EQ(trillion * nano, 1'000);

    const Exponential g{1, -100};
    EXPECT_EQ(googol * g, 1);

    const Exponential googol_square{1, 200};
    EXPECT_EQ(googol * googol, googol_square);

    const Exponential x = 2;
    const Exponential y = 3;
    const Exponential z = 5;

    EXPECT_EQ(x * y, y * x);

    EXPECT_EQ((x * y) * z, x * (y * z));

    const Exponential a = 1490116119384765625ll;
    const Exponential b = 67108864ll;
    const Exponential c{1, 26};

    EXPECT_EQ(a * b, c);
}

TEST_F(ExponentialTest, distributivity)
{
    const Exponential x = 2;
    const Exponential y = 3;
    const Exponential z = 5;

    EXPECT_EQ((x + y) * z, x * z + y * z);
    EXPECT_EQ(z * (x + y), x * z + y * z);
    EXPECT_EQ(z * (x + y), z * x + z * y);
    EXPECT_EQ((x + y) * z, z * x + z * y);
}

TEST_F(ExponentialTest, divide)
{
    EXPECT_EQ(zero / 1, 0);
    EXPECT_EQ(one / 1, 1);
    EXPECT_EQ(googol / 1, googol);

    EXPECT_EQ(googol / nano, Exponential(1, 109));
    EXPECT_EQ(nano / googol, Exponential(1, -109));
    EXPECT_EQ(nano / (2 * googol), Exponential(5, -110));
    EXPECT_EQ(one / 3, Exponential(333333333333333333, -18));

    const Exponential googol_square{1, 200};
    EXPECT_EQ(googol_square / googol, googol);

    EXPECT_EQ(googol / -1, negative_googol);

    const Exponential a = 1490116119384765625ll;
    const Exponential b = 67108864ll;
    const Exponential c{1, 26};

    EXPECT_EQ(c / a, b);
    EXPECT_EQ(c / b, a);
}

TEST_F(ExponentialTest, string)
{
    static constexpr auto str = [](const Exponential & n) {
        return n.str();
    };

    EXPECT_EQ(str(zero), "0");

    EXPECT_EQ(str(one), "1");
    EXPECT_EQ(str(googol), "1e100");
    EXPECT_EQ(str(trillion), "1e12");
    EXPECT_EQ(str(nano), "1e-9");

    EXPECT_EQ(str(negative_one), "-1");
    EXPECT_EQ(str(negative_googol), "-1e100");
    EXPECT_EQ(str(negative_trillion), "-1e12");
    EXPECT_EQ(str(negative_nano), "-1e-9");

    EXPECT_EQ(str(Exponential(42)), "42");
    EXPECT_EQ(str(Exponential(420)), "42e1");
    EXPECT_EQ(str(Exponential(42, -1)), "42e-1");
}

TEST_F(ExponentialTest, print)
{
    static constexpr auto str = [](const Exponential & n) {
        std::stringstream ss;
        ss << n;
        return ss.str();
    };

    EXPECT_EQ(str(zero), "0");

    EXPECT_EQ(str(one), "1");
    EXPECT_EQ(str(googol), "1e100");
    EXPECT_EQ(str(trillion), "1e12");
    EXPECT_EQ(str(nano), "1e-9");

    EXPECT_EQ(str(negative_one), "-1");
    EXPECT_EQ(str(negative_googol), "-1e100");
    EXPECT_EQ(str(negative_trillion), "-1e12");
    EXPECT_EQ(str(negative_nano), "-1e-9");

    EXPECT_EQ(str(Exponential(42)), "42");
    EXPECT_EQ(str(Exponential(420)), "42e1");
    EXPECT_EQ(str(Exponential(42, -1)), "42e-1");
}
