#include "ohbcalc.h"
#include "ohbTransform.h"
#include <gtest/gtest.h>
#include <limits>

using namespace std;

// TODO: test Octal

void testRepresentation(int64_t rInt,
                        const string &rHex, const string &rBin, const string &rChars, bool leadingZero = false)
{
  SCOPED_TRACE(string("Checking different representation of number ") + std::to_string(rInt));
  EXPECT_EQ(toHex(rInt, leadingZero), rHex);
  EXPECT_EQ(toBinary(rInt, leadingZero), rBin);
  EXPECT_EQ(toChars(rInt, leadingZero), rChars);
}


TEST(Representation, allKinds)
{
  testRepresentation(3, "3", "11", "\3");
  testRepresentation(15, "F", "1111", "\x0f");
  testRepresentation(20, "14", "10100", "\x14");
  testRepresentation(0, "0", "0", "");
  testRepresentation(1965, "7AD", "11110101101", "\x07\xAD");
  testRepresentation(-1871, "FFFFFFFFFFFFF8B1", std::string(53, '1') + "00010110001",
                     std::string(6, '\xFF') + "\xF8\xB1");
}


TEST(Representation, allKindsLeadingZero)
{
  testRepresentation(3, std::string(15, '0') + "3", std::string(62, '0') + "11",  std::string(7, '\0') + "\3", true);
  testRepresentation(15, std::string(15, '0') + "F", std::string(60, '0') + "1111", std::string(7, '\0') + "\x0f", true);
  testRepresentation(20, std::string(14, '0') + "14", std::string(59, '0') + "10100",  std::string(7, '\0') + "\x14", true);
  testRepresentation(0, std::string(16, '0'), std::string(64, '0'), std::string(8, '\0'), true);
  testRepresentation(1965, std::string(13, '0') + "7AD", std::string(53, '0') + "11110101101",
                     std::string(6, '\0') + "\x07\xAD", true);
  testRepresentation(-1871, std::string(13, 'F') + "8B1", std::string(53, '1') + "00010110001",
                     std::string(6, '\xFF') + "\xF8\xB1", true);
}