#include "hexCalc.h"
#include <gtest/gtest.h>
#include <limits>

using namespace std;
// TODO: add testing little/big endian
// TODO: check, that binaryOperations with equal Precedence have equal Associativity

const static uint64_t maxUint64 = numeric_limits<uint64_t>::max();
const static string stringOnes[] = {
  "",
  "11111111",
  "1111111111111111",
  "111111111111111111111111",
  "11111111111111111111111111111111",
  "1111111111111111111111111111111111111111",
  "111111111111111111111111111111111111111111111111",
  "11111111111111111111111111111111111111111111111111111111",
  "1111111111111111111111111111111111111111111111111111111111111111"
};

const static string charOnes[] = {
  "",
  "\xFF",
  "\xFF\xFF",
  "\xFF\xFF\xFF",
  "\xFF\xFF\xFF\xFF",
  "\xFF\xFF\xFF\xFF\xFF",
  "\xFF\xFF\xFF\xFF\xFF\xFF",
  "\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
  "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
};

class HexCalcTest : public ::testing::Test
{
protected:
  HexCalc calculator;
  void testExpression(const string &expression, int64_t rInt, uint64_t rUint,
                      const string &rHex, const string &rBin, const string &rChars)
  {
    const string calculating = "Calculating ";
    calculator.eval(expression);
    SCOPED_TRACE(calculating + expression);
    EXPECT_EQ(calculator.getInt64(), rInt);
    EXPECT_EQ(calculator.getUint64(), rUint);
    EXPECT_EQ(calculator.getHex(), rHex);
    EXPECT_EQ(calculator.getBinary(), rBin);
    EXPECT_EQ(calculator.getChars(), rChars);
  }

  void testEval(const string &expression, int64_t result)
  {
    const string calculating = "Calculating ";
    calculator.eval(expression);
    SCOPED_TRACE(calculating + expression);
    EXPECT_EQ(calculator.getInt64(), result);
  }
};

TEST_F(HexCalcTest, solo_binary_operation)
{
  testExpression("1+2", 3, 3, "3", "11", "\3");
  testExpression("6+9", 15, 15, "F", "1111", "\x0f");
  testExpression("9+11", 20, 20, "14", "10100", "\x14");
  testExpression("0-0", 0, 0, "0", "0", "");
  testExpression("0+1965", 1965, 1965, "7AD", "11110101101", "\x07\xAD");
}

TEST_F(HexCalcTest, skip_spaces)
{
  testExpression("  1   + 2   ", 3, 3, "3", "11", "\3");
}

TEST_F(HexCalcTest, solo_number)
{
  testExpression("3", 3, 3, "3", "11", "\3");
  testExpression("-3", -3, maxUint64-2, "FFFFFFFFFFFFFFFD", stringOnes[7] + "11111101", charOnes[7] + "\xFD");
}

TEST_F(HexCalcTest, mixing)
{
  testExpression("3---------3", 0, 0, "0", "0", "");
  testExpression("--3---~--~--3", 0, 0, "0", "0", "");
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

TEST_F(HexCalcTest, brackets)
{
  testEval("(1)", 1);
  testEval("(1+2)", 3);
  testEval("(1+2+3)*4", 24);
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
  testEval("1234567890ABCDEFH", 1311768467294899695);

}

TEST_F(HexCalcTest, mixing_types)
{
  testEval("10i+101i-1i", 6);
  testEval("10h+101h-1h", 0x101 + 15);
  testEval("10i+2Fh-3", 46);
}