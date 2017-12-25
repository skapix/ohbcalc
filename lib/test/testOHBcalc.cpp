#include "ohbcalc.h"
#include "ohbException.h"
#include <gtest/gtest.h>

using namespace std;
// TODO: test on big-endian machine

class OHBCalcTest : public ::testing::Test
{
protected:
  OHBCalc calculator;

  void testEval(const string &expression, int64_t result)
  {
    const string trace = string("Calculating ") + expression;
    SCOPED_TRACE(trace.c_str());
    EXPECT_EQ(calculator.eval(expression), result);
  }
  void testEval(const string &checkedExpression, const string &expression)
  {
    const string trace = string("Check: ") + checkedExpression + " == " + expression;
    SCOPED_TRACE(trace.c_str());
    EXPECT_EQ(calculator.eval(expression), calculator.eval(checkedExpression));
  }

};

TEST_F(OHBCalcTest, case_insensetivity)
{
  testEval("1i", "1I");
  testEval("1b", "1B");
  testEval("1i", "1b");
  testEval("10h", "10H");
  testEval("10h", "0x10");
  testEval("123o", "123O");
  testEval("0123", "123o");
  testEval("ABCDEFH", "abcdefh");
  testEval("aH", "Ah");
}


TEST_F(OHBCalcTest, solo_number)
{
  testEval("3", 3);
  testEval("243", 243);
  testEval("1894", 1894);
  testEval("(1893)", 1893);

}

TEST_F(OHBCalcTest, with_unary_operator)
{
  testEval("-5", -5);
  testEval("(-10)", -10);
  testEval("(-9103)", -9103);
  testEval("~(2113)", ~2113);
  testEval("!2113", 0);
  testEval("!0", 1);
}

TEST_F(OHBCalcTest, skip_spaces)
{
  testEval("  (  1     ) ", 1);
  testEval("(  1   + 2   )", 3);
  testEval("  1   + 2   ", 3);
}

TEST_F(OHBCalcTest, many_unary_operators)
{
  testEval("3---------3", 0);
  testEval("--3---~--~--3", 0);
}

TEST_F(OHBCalcTest, all_operations)
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

TEST_F(OHBCalcTest, brackets_evaluation)
{
  testEval("(1)", 1);
  testEval("(1+2)", 3);
  testEval("(1+2+3)*4", 24);
  testEval("2*(1+2+3)", 12);
  testEval("(1+2+3)*(4+3-2)", 30);
}

TEST_F(OHBCalcTest, precedence)
{
  testEval("1+2-3*4", -9);
  testEval("1*2 + 3*4", 14);
  testEval("1+2 * 3+4", 11);
  testEval("1<<2+3*4", 1 << 14); // operation sequence order: 3, 2, 1
  testEval("1*2+3<<4", 80); // 1, 2, 3
  testEval("1 << 2 + 1 << 3", 64); // 2, 1, 3
  testEval("0 + 1 << 2 + 1 << 3 + 1 - 1", 64);

  testEval("1 << 1 + 1 + 1 << 1", 16);
  testEval("1 << 1 + 2 * 1 << 1", 16);
  testEval("1 << 1 + 2 * 1 ** 1 * 1 << 1", 16);
}

TEST_F(OHBCalcTest, correct_decoding)
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

  testEval("\"\"", 0);
  testEval("\"\\00\"", 0);
  testEval("\"123\"", '1' * 256 * 256 + '2' * 256 + '3');
  testEval("\"\\12\"", 0x12);
  testEval("\"\\03\"", 3);
  testEval("\"\\t\"", '\t');
  testEval("\"\\r\"", '\r');
  testEval("\"\\n\"", '\n');
  testEval("\"\\r\\n\"", '\r' * 256 + '\n');
  testEval("\"\\\\\"\"", '\\' * 256 + '"');
  testEval("\"e\\\"q\"", 'e' * 256 * 256 + '"' * 256 + 'q');
}

TEST_F(OHBCalcTest, chars)
{

}

TEST_F(OHBCalcTest, mixing_types)
{
  testEval("10i+101i-1b", 6);
  testEval("10h+101h-1h", 0x101 + 15);
  testEval("10i+2Fh-0x3", 46);
}

TEST_F(OHBCalcTest, recurse_brackets)
{
  testEval("(((4)))", 4);
  testEval("3 * (((2 - 3) + 4 * 5) / 2 + 3) ", 36);
  testEval("(1000i + 11o) & ( E0h + 0xE)", 0);

  testEval("3 * (((2 - 3) + 4 * 5) / 2 ) - 4 * (1 + 2 * (-1 + 1i) )", 27 - 4);
  testEval("(AH ^ (0x5F - 1010111i)) * (17o + (24h-24)*11i)", 102);
}
