#include "ohbcalc.h"
#include "ohbException.h"
#include <gtest/gtest.h>

using namespace std;

class OHBTestPos : public ::testing::Test
{
protected:
  OHBCalc calculator;

  void testPositionAtError(const string &expression, size_t pos)
  {
    const string trace = string("Catching error from: ") + expression;
    SCOPED_TRACE(trace.c_str());
    try
    {
      calculator.eval(expression);
      GTEST_FAIL();
    }
    catch (const OHBException& error)
    {
      ASSERT_EQ(error.getPos(), pos);
    }

  }
};

TEST_F(OHBTestPos, parse_number)
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


  testPositionAtError("0 + 1 << 2 + 1p << 3 + 1 - 1", 14);
  testPositionAtError("0 + 1 << 2 + 1 p< 3 + 1 - 1", 15);

}

TEST_F(OHBTestPos, unknown_op)
{
  testPositionAtError("123h # 123", 5);
  testPositionAtError("123h 8 123", 5);
  testPositionAtError("123h?123", 4);
}

TEST_F(OHBTestPos, unfinished)
{
  testPositionAtError("123h + ", 7);
}

TEST_F(OHBTestPos, divizion_by_zero)
{
  testPositionAtError("1/0", 1);
  testPositionAtError("2+1/(4-2*2)", 3);
  testPositionAtError("(2+1)/(4-2*2)", 5);
  testPositionAtError("2*(4/(2-2)", 4);
  testPositionAtError("1/0+2", 1);

}


TEST_F(OHBTestPos, wrong_brackets)
{
  testPositionAtError("()", 1);
  testPositionAtError("((1)", 4);
  testPositionAtError("(1))", 3);
  testPositionAtError("(2+)3", 3);
}