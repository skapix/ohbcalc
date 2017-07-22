#include "hexCalc.h"
#include <gtest/gtest.h>
#include <string>

using namespace std;
// TODO: add testing little/big endian
// TODO: check, that operations with equal Precedence have equal Associativity


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
};


TEST_F(HexCalcTest, solo_binary_operation)
{
  testExpression("1+2", 3, 3, "3", "11", "\3");
  testExpression("6+9", 15, 15, "F", "1111", "\x0f");
  testExpression("9+11", 20, 20, "14", "10100", "\x14");
  testExpression("0-0", 0, 0, "0", "0", "");
  testExpression("0+1965", 1965, 1965, "7AD", "11110101101", "\x07\xAD");

}