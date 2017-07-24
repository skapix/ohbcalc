#include "hexCalc.h"
#include <gtest/gtest.h>
#include <limits>

using namespace std;
// TODO: add testing little/big endian
// TODO: check, that binaryOperations with equal Precedence have equal Associativity

class HexCalcTest : public ::testing::Test
{
protected:
  HexCalc calculator;

  void testEval(const string &expression, int64_t result)
  {
    const string trace = string("Calculating ") + expression;
    SCOPED_TRACE(trace.c_str());
    EXPECT_EQ(calculator.eval(expression), result);
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
  testEval("19 >> 2", 19 >> 2);
  testEval("19 ** 2", 19 * 19);
  testEval("19 + 2", 19 + 2);
  testEval("19 - 2", 19 - 2);
  testEval("19 * 2", 19 * 2);
  testEval("19 / 2", 19 / 2);
  testEval("19 % 2", 19 % 2);
  testEval("19 | 2", 19 | 2);
  testEval("19 ^ 2", 19 ^ 2);
  testEval("19 & 2", 19 & 2);
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
  testEval("1010101010111i", 5463);
  testEval("1010101010111010101011110I", 22377822);
  testEval("AAAAh", 0xAAAA);
  testEval("1234567890ABCDEFH", 1311768467294899695);

}

TEST_F(HexCalcTest, mixing_types)
{
  testEval("10i+101i-1i", 6);
  testEval("10h+101h-1h", 0x101 + 15);
  testEval("10i+2Fh-3", 46);
}