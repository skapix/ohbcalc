#include "ohbcalc.h"
#include "ohbException.h"
#include <gtest/gtest.h>

using namespace std;
// TODO: test on big-endian machine

class HexCalcTest : public ::testing::Test
{
protected:
  OHBCalc calculator;

  void testEval(const string &expression, int64_t result)
  {
    const string trace = string("Calculating ") + expression;
    SCOPED_TRACE(trace.c_str());
    EXPECT_EQ(calculator.eval(expression), result);
  }

  void testPositionAtError(const string &expression, size_t pos)
  {
    const string trace = string("Catching error from: ") + expression;
    SCOPED_TRACE(trace.c_str());
    try {
      calculator.eval(expression);
      GTEST_FAIL();
    }
    catch (const OHBException& error)
    {
      ASSERT_EQ(error.getPos(), pos);
    }

  }
};


TEST_F(HexCalcTest, solo_number)
{
  testEval("3", 3);
  testEval("243", 243);
  testEval("1894", 1894);
  testEval("(1893)", 1893);

}

TEST_F(HexCalcTest, with_unary_operator)
{
  testEval("-5", -5);
  testEval("(-10)", -10);
  testEval("(-9103)", -9103);
  testEval("~(2113)", ~2113);
  testEval("!2113", 0);
  testEval("!0", 1);
}

TEST_F(HexCalcTest, skip_spaces)
{
  testEval("  (  1     ) ", 1);
  testEval("(  1   + 2   )", 3);
  testEval("  1   + 2   ", 3);
}

TEST_F(HexCalcTest, many_unary_operators)
{
  testEval("3---------3", 0);
  testEval("--3---~--~--3", 0);
}

TEST_F(HexCalcTest, all_operations)
{
  testEval("19 << 2", 19 << 2);
  testEval("1094 << 5", 1094 << 5);
  testEval("19 >> 2", 19 >> 2);
  testEval("2 >> 2", 2 >> 2);
  testEval("19 ** 2", 19 * 19);
  testEval("2 ** 5", 32);
  testEval("19 + 2", 19 + 2);
  testEval("190123 + 90123312", 190123 + 90123312);
  testEval("19 - 2", 19 - 2);
  testEval("19209859519 - 1248512", 19209859519 - 1248512);
  testEval("19 * 2", 19 * 2);
  testEval("190001 * 22220", static_cast<int64_t>(190001) * 22220);
  testEval("19 / 2", 19 / 2);
  testEval("19930111 / 1245", 19930111 / 1245);
  testEval("19 % 2", 19 % 2);
  testEval("19930111 % 1245", 19930111 % 1245);
  testEval("19 | 2", 19 | 2);
  testEval("1234567890 | 123", 1234567890 | 123);
  testEval("19 ^ 2", 19 ^ 2);
  testEval("1901 ^ 25", 1901 ^ 25);
  testEval("19 & 2", 19 & 2);
  testEval("1901 & 25", 1901 & 25);

  testEval("19 < 2", 19 < 2);
  testEval("2 < 2", 2 < 2);
  testEval("19 > 2", 19 > 2);
  testEval("2 > 2", 2 > 2);
  testEval("19 <= 2", 19 <= 2);
  testEval("2 <= 2", 2 <= 2);
  testEval("19 >= 2", 19 >= 2);
  testEval("2 >= 2", 2 >= 2);
  testEval("19 != 2", 19 != 2);
  testEval("19 != 19", false);
  testEval("19 == 2", 19 == 2);
  testEval("19 == 19", true);
  testEval("19 || 2", 1);
  testEval("0 || 1", 1);
  testEval("0 || 0", 0);
  testEval("19 && 2", 1);
  testEval("0 && 1", 0);
  testEval("0 && 0", 0);
}

TEST_F(HexCalcTest, brackets_evaluation)
{
  testEval("(1)", 1);
  testEval("(1+2)", 3);
  testEval("(1+2+3)*4", 24);
  testEval("2*(1+2+3)", 12);
  testEval("(1+2+3)*(4+3-2)", 30);
}

TEST_F(HexCalcTest, precedence)
{
  testEval("1+2-3*4", -9);
  testEval("1*2 + 3*4", 14);
  testEval("1+2 * 3+4", 11);
  testEval("1<<2+3*4", 1 << 14); // operation sequence order: 3, 2, 1
  testEval("1*2+3<<4", 80); // 1, 2, 3
}

TEST_F(HexCalcTest, correct_decoding)
{
  testEval("0i", 0);
  testEval("1010101010111i", 5463);
  testEval("1010101010111b", 5463);
  testEval("1010101010111010101011110I", 22377822);
  testEval("1010101010111010101011110B", 22377822);
  testEval("AAAAh", 0xAAAA);
  testEval("1234567890ABCDEFH", 1311768467294899695ull);
  testEval("0x11", 17);
  testEval("0xABCD", 0xABCD);
  testEval("25o", 21);
  testEval("025", 21);
  testEval("253477o", 87871);
  testEval("0253477", 87871);

}

TEST_F(HexCalcTest, mixing_types)
{
  testEval("10i+101i-1b", 6);
  testEval("10h+101h-1h", 0x101 + 15);
  testEval("10i+2Fh-0x3", 46);
}

TEST_F(HexCalcTest, recurse_brackets)
{
  testEval("(((4)))", 4);
  testEval("3 * (((2 - 3) + 4 * 5) / 2 + 3) ", 36);
  testEval("(1000i + 11o) & ( E0h + 0xE)", 0);

  testEval("3 * (((2 - 3) + 4 * 5) / 2 ) - 4 * (1 + 2 * (-1 + 1i) )", 27 - 4);
  testEval("(AH ^ (0x5F - 1010111i)) * (17o + (24h-24)*11i)", 102);
}


TEST_F(HexCalcTest, parse_number)
{
  testPositionAtError("12k4", 2);
  testPositionAtError("1234q", 4);
  testPositionAtError("z1", 0);
  testPositionAtError("101012i", 5);
  testPositionAtError("1FBGCh", 3);
  testPositionAtError("1234F", 4);
  testPositionAtError("012348", 5);
  testPositionAtError("0x012L48", 5);

  testPositionAtError("123 + 2q6", 7);
  testPositionAtError("123h + 2q6", 8);
  testPositionAtError("123op", 4);
  testPositionAtError("123op1", 3);
  testPositionAtError("(123+(456+234*(123+123q)))", 22);
  testPositionAtError("(123+456+67q8)", 11);
  testPositionAtError("(123+45f6+678)", 7);
  testPositionAtError("1+t2", 2);

}

TEST_F(HexCalcTest, unknown_op)
{
  testPositionAtError("123h # 123", 5);
  testPositionAtError("123h 8 123", 5);
  testPositionAtError("123h?123", 4);
}

TEST_F(HexCalcTest, unfinished)
{
  testPositionAtError("123h + ", 7);
}

TEST_F(HexCalcTest, divizion_by_zero)
{
  testPositionAtError("1/0", 1);
  testPositionAtError("2+1/(4-2*2)", 3);
  testPositionAtError("(2+1)/(4-2*2)", 5);
  testPositionAtError("2*(4/(2-2)", 4);
}


TEST_F(HexCalcTest, wrong_brackets)
{
  testPositionAtError("()", 1);
  testPositionAtError("((1)", 4);
  testPositionAtError("(1))", 3);
  testPositionAtError("(2+)3", 3);
}