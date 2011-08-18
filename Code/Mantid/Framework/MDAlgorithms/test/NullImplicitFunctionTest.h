#ifndef NULL_IMPLICIT_FUNCTION_TEST_H_
#define NULL_IMPLICIT_FUNCTION_TEST_H_

#include <cxxtest/TestSuite.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "MantidMDAlgorithms/NullImplicitFunction.h"

class NullImplicitFunctionTest: public CxxTest::TestSuite
{
private:

class MockPoint3D: public Mantid::API::Point3D
{
  public:
  MOCK_CONST_METHOD0(getX, double());
  MOCK_CONST_METHOD0(getY, double());
  MOCK_CONST_METHOD0(getZ, double());
};

public:

void testGetName()
{
  using namespace Mantid::MDAlgorithms;
  NullImplicitFunction function;

  TSM_ASSERT_EQUALS("The static and dynamic names do not align", NullImplicitFunction::functionName(), function.getName());
}

void testEvaluateReturnsTrue()
{
  using namespace Mantid::MDAlgorithms;
  NullImplicitFunction function;
  MockPoint3D mockPoint;
  EXPECT_CALL(mockPoint, getX()).Times(0);
  EXPECT_CALL(mockPoint, getY()).Times(0);
  EXPECT_CALL(mockPoint, getZ()).Times(0);
  TS_ASSERT(function.evaluate(&mockPoint));
}

void testEvaluateCoordReturnsTrue()
{
  using namespace Mantid::MDAlgorithms;
  NullImplicitFunction function;
  Mantid::coord_t coords[3];
  bool masks[3];
  TS_ASSERT(function.evaluate(coords, masks, 3));
}

void testToXMLEmpty()
{
  using namespace Mantid::MDAlgorithms;
  NullImplicitFunction function;

  TSM_ASSERT_EQUALS("The xml string should be empty for any instance of this type", std::string() , function.toXMLString());
}

};
#endif
