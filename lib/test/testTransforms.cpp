#include "ohbcalc.h"
#include "ohbTransform.h"
#include <gtest/gtest.h>
#include <limits>

using namespace std;


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


void testRepresentation(int64_t rInt,
                        const string &rHex, const string &rBin, const string &rChars)
{
  SCOPED_TRACE(string("Checking different representation of number ") + std::to_string(rInt));
  EXPECT_EQ(toHex(rInt), rHex);
  EXPECT_EQ(toBinary(rInt), rBin);
  EXPECT_EQ(toChars(rInt), rChars);
}


TEST(Representation, allKinds)
{
  testRepresentation(3, "3", "11", "\3");
  testRepresentation(15, "F", "1111", "\x0f");
  testRepresentation(20, "14", "10100", "\x14");
  testRepresentation(0, "0", "0", "");
  testRepresentation(1965, "7AD", "11110101101", "\x07\xAD");
  testRepresentation(-1871, "FFFFFFFFFFFFF8B1", stringOnes[6] + "1111100010110001",  charOnes[6] + "\xF8\xB1");
}
