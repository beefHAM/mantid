// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef EXPRESSIONTEST_H_
#define EXPRESSIONTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidAPI/Expression.h"

using namespace Mantid;
using namespace Mantid::API;

class ExpressionTest : public CxxTest::TestSuite {
public:
  void testIt() {
    Expression expr;
    TS_ASSERT_THROWS_NOTHING(expr.parse("a+b*c-1"));
    TS_ASSERT_EQUALS(expr.str(), "a+b*c-1");
    TS_ASSERT(expr.isFunct());
    TS_ASSERT_EQUALS(expr.name(), "+");
    TS_ASSERT_EQUALS(expr.operator_name(), "");
    const std::vector<Expression> &terms = expr.terms();
    TS_ASSERT_EQUALS(terms.size(), 3);
    TS_ASSERT_EQUALS(terms[0].name(), "a");
    TS_ASSERT_EQUALS(terms[0].operator_name(), "");
    TS_ASSERT_EQUALS(terms[1].name(), "*");
    TS_ASSERT_EQUALS(terms[1].operator_name(), "+");
    TS_ASSERT_EQUALS(terms[2].name(), "1");
    TS_ASSERT_EQUALS(terms[2].operator_name(), "-");
  }

  void testBrackets() {
    Expression expr;
    TS_ASSERT_THROWS_NOTHING(expr.parse("(a+b)*c"));
    TS_ASSERT_EQUALS(expr.str(), "(a+b)*c");
    TS_ASSERT(expr.isFunct());
    TS_ASSERT_EQUALS(expr.name(), "*");
    TS_ASSERT_EQUALS(expr.operator_name(), "");
    const std::vector<Expression> &terms = expr.terms();
    TS_ASSERT_EQUALS(terms.size(), 2);
    TS_ASSERT_EQUALS(terms[0].name(), "+");
    TS_ASSERT_EQUALS(terms[0].operator_name(), "");
    TS_ASSERT_EQUALS(terms[1].name(), "c");
    TS_ASSERT_EQUALS(terms[1].operator_name(), "*");
  }

  void testMismatchedBrackets() {
    Expression expr;
    TS_ASSERT_THROWS(expr.parse("b*(c+a"), const std::runtime_error &);
    TS_ASSERT_THROWS(expr.parse("b*c+a)"), const std::runtime_error &);
  }

  void testFunctions() {
    Expression expr;
    TS_ASSERT_THROWS_NOTHING(expr.parse("sin(x)+cos(x)"));
    TS_ASSERT_EQUALS(expr.str(), "sin(x)+cos(x)");
    TS_ASSERT(expr.isFunct());
    TS_ASSERT_EQUALS(expr.name(), "+");
    TS_ASSERT_EQUALS(expr.operator_name(), "");
    const std::vector<Expression> &terms = expr.terms();
    TS_ASSERT_EQUALS(terms.size(), 2);
    TS_ASSERT_EQUALS(terms[0].name(), "sin");
    TS_ASSERT_EQUALS(terms[0].operator_name(), "");
    TS_ASSERT(terms[0].isFunct());
    TS_ASSERT_EQUALS(terms[0].size(), 1);
    TS_ASSERT_EQUALS(terms[1].name(), "cos");
    TS_ASSERT_EQUALS(terms[1].operator_name(), "+");
    TS_ASSERT(terms[1].isFunct());
    TS_ASSERT_EQUALS(terms[1].size(), 1);
  }

  void testFunctions2() {
    Expression expr;
    TS_ASSERT_THROWS_NOTHING(expr.parse("sin(x,y) + cos(x,y)"));
    TS_ASSERT_EQUALS(expr.str(), "sin(x,y)+cos(x,y)");
    TS_ASSERT(expr.isFunct());
    TS_ASSERT_EQUALS(expr.name(), "+");
    TS_ASSERT_EQUALS(expr.operator_name(), "");
    const std::vector<Expression> &terms = expr.terms();
    TS_ASSERT_EQUALS(terms.size(), 2);
    TS_ASSERT_EQUALS(terms[0].name(), "sin");
    TS_ASSERT_EQUALS(terms[0].operator_name(), "");
    TS_ASSERT(terms[0].isFunct());
    TS_ASSERT_EQUALS(terms[0].size(), 2);
    TS_ASSERT_EQUALS(terms[1].name(), "cos");
    TS_ASSERT_EQUALS(terms[1].operator_name(), "+");
    TS_ASSERT(terms[1].isFunct());
    TS_ASSERT_EQUALS(terms[1].size(), 2);
  }

  void testNestedFunctions() {
    Expression expr;
    TS_ASSERT_THROWS_NOTHING(expr.parse("sin(  cos(x)  )"));
    TS_ASSERT_EQUALS(expr.str(), "sin(cos(x))");
    TS_ASSERT(expr.isFunct());
    TS_ASSERT_EQUALS(expr.name(), "sin");
    TS_ASSERT_EQUALS(expr.operator_name(), "");
    const std::vector<Expression> &terms = expr.terms();
    TS_ASSERT_EQUALS(terms.size(), 1);
    TS_ASSERT_EQUALS(terms[0].name(), "cos");
    TS_ASSERT_EQUALS(terms[0].operator_name(), "");
    TS_ASSERT(terms[0].isFunct());
    TS_ASSERT_EQUALS(terms[0].size(), 1);
  }

  void testString() {
    Expression expr;
    TS_ASSERT_THROWS_NOTHING(expr.parse("x = \" y + z\""));
    TS_ASSERT_EQUALS(expr.str(), "x=\" y + z\"");
    TS_ASSERT(expr.isFunct());
    TS_ASSERT_EQUALS(expr.name(), "=");
    TS_ASSERT_EQUALS(expr.operator_name(), "");
    const std::vector<Expression> &terms = expr.terms();
    TS_ASSERT_EQUALS(terms.size(), 2);
    TS_ASSERT_EQUALS(terms[0].name(), "x");
    TS_ASSERT_EQUALS(terms[0].operator_name(), "");
    TS_ASSERT(!terms[0].isFunct());
    TS_ASSERT_EQUALS(terms[1].name(), "\" y + z\"");
    TS_ASSERT_EQUALS(terms[1].operator_name(), "=");
    TS_ASSERT(!terms[1].isFunct());
  }

  void testQuotes() {
    Expression expr;
    TS_ASSERT_THROWS_NOTHING(expr.parse("\"(2)\" "));
    TS_ASSERT_EQUALS(expr.str(), "\"(2)\"");

    Expression expression;
    TS_ASSERT_THROWS_NOTHING(expression.parse("abc\"xyz\"(1,2)"));
  }

  void testOperators() {
    Expression expr({",", "="});
    TS_ASSERT_THROWS_NOTHING(expr.parse("x=-1\"(2)\",y=2a+b*(x+y) "));

    TS_ASSERT_EQUALS(expr[0][1].name(), "-1\"(2)\"");
    TS_ASSERT_EQUALS(expr[1][1].name(), "2a+b*");
    TS_ASSERT_EQUALS(expr[1][1][0].name(), "x+y");
  }

  void testUnariOperators() {
    Expression e1;
    TS_ASSERT_THROWS_NOTHING(e1.parse("x =-1"));
    TS_ASSERT_EQUALS(e1.str(), "x= -1");
    TS_ASSERT_EQUALS(e1.size(), 2);
    TS_ASSERT_EQUALS(e1.name(), "=");
    TS_ASSERT_EQUALS(e1[0].name(), "x");
    TS_ASSERT_EQUALS(e1[1].name(), "-");

    Expression e2;
    TS_ASSERT_THROWS_NOTHING(e2.parse("x =-  (1-2)"));
    TS_ASSERT_EQUALS(e2.str(), "x= -(1-2)");
    TS_ASSERT_EQUALS(e2.size(), 2);
    TS_ASSERT_EQUALS(e2.name(), "=");
    TS_ASSERT_EQUALS(e2[0].name(), "x");
    TS_ASSERT_EQUALS(e2[1].name(), "-");

    Expression e3;
    TS_ASSERT_THROWS_NOTHING(e3.parse("x =-1+2"));
    TS_ASSERT_EQUALS(e3.str(), "x= -1+2");
    TS_ASSERT_EQUALS(e3.size(), 2);
    TS_ASSERT_EQUALS(e3.name(), "=");
    TS_ASSERT_EQUALS(e3[0].name(), "x");
    TS_ASSERT_EQUALS(e3[1].name(), "+");
    TS_ASSERT_EQUALS(e3[1][0].name(), "-");

    Expression e4;
    TS_ASSERT_THROWS_NOTHING(e4.parse("x ==+1"));
    TS_ASSERT_EQUALS(e4.str(), "x== +1");
    TS_ASSERT_EQUALS(e4.size(), 2);
    TS_ASSERT_EQUALS(e4.name(), "==");
    TS_ASSERT_EQUALS(e4[0].name(), "x");
    TS_ASSERT_EQUALS(e4[1].name(), "+");

    std::vector<std::string> bin_ops{"="};
    std::unordered_set<std::string> un_ops{"!", "%%"};

    Expression e5(bin_ops, un_ops);
    TS_ASSERT_THROWS_NOTHING(e5.parse("x=!1"));
    TS_ASSERT_EQUALS(e5.str(), "x= !1");
    TS_ASSERT_EQUALS(e5.size(), 2);
    TS_ASSERT_EQUALS(e5.name(), "=");
    TS_ASSERT_EQUALS(e5[0].name(), "x");
    TS_ASSERT_EQUALS(e5[1].name(), "!");

    Expression e6(bin_ops, un_ops);
    TS_ASSERT_THROWS_NOTHING(e6.parse("x= ! 1"));
    TS_ASSERT_EQUALS(e6.str(), "x= !1");
    TS_ASSERT_EQUALS(e6.size(), 2);
    TS_ASSERT_EQUALS(e6.name(), "=");
    TS_ASSERT_EQUALS(e6[0].name(), "x");
    TS_ASSERT_EQUALS(e6[1].name(), "!");

    Expression e7(bin_ops, un_ops);
    TS_ASSERT_THROWS_NOTHING(e7.parse("x=%%1"));
    TS_ASSERT_EQUALS(e7.str(), "x= %%1");
    TS_ASSERT_EQUALS(e7.size(), 2);
    TS_ASSERT_EQUALS(e7.name(), "=");
    TS_ASSERT_EQUALS(e7[0].name(), "x");
    TS_ASSERT_EQUALS(e7[1].name(), "%%");

    Expression e8(bin_ops, un_ops);
    TS_ASSERT_THROWS_NOTHING(e8.parse("x= %%1"));
    TS_ASSERT_EQUALS(e8.str(), "x= %%1");
    TS_ASSERT_EQUALS(e8.size(), 2);
    TS_ASSERT_EQUALS(e8.name(), "=");
    TS_ASSERT_EQUALS(e8[0].name(), "x");
    TS_ASSERT_EQUALS(e8[1].name(), "%%");

    std::vector<std::string> bin_ops1{"=="};
    std::unordered_set<std::string> un_ops1{"!", "%%"};

    Expression e9(bin_ops1, un_ops1);
    TS_ASSERT_THROWS_NOTHING(e9.parse("x==!1"));
    TS_ASSERT_EQUALS(e9.str(), "x== !1");
    TS_ASSERT_EQUALS(e9.size(), 2);
    TS_ASSERT_EQUALS(e9.name(), "==");
    TS_ASSERT_EQUALS(e9[0].name(), "x");
    TS_ASSERT_EQUALS(e9[1].name(), "!");

    Expression e10(bin_ops1, un_ops1);
    TS_ASSERT_THROWS_NOTHING(e10.parse("x== !1"));
    TS_ASSERT_EQUALS(e10.str(), "x== !1");
    TS_ASSERT_EQUALS(e10.size(), 2);
    TS_ASSERT_EQUALS(e10.name(), "==");
    TS_ASSERT_EQUALS(e10[0].name(), "x");
    TS_ASSERT_EQUALS(e10[1].name(), "!");

    Expression e11(bin_ops1, un_ops1);
    TS_ASSERT_THROWS_NOTHING(e11.parse("x==%%1"));
    TS_ASSERT_EQUALS(e11.str(), "x== %%1");
    TS_ASSERT_EQUALS(e11.size(), 2);
    TS_ASSERT_EQUALS(e11.name(), "==");
    TS_ASSERT_EQUALS(e11[0].name(), "x");
    TS_ASSERT_EQUALS(e11[1].name(), "%%");

    Expression e12(bin_ops1, un_ops1);
    TS_ASSERT_THROWS_NOTHING(e12.parse("x== %%1"));
    TS_ASSERT_EQUALS(e12.str(), "x== %%1");
    TS_ASSERT_EQUALS(e12.size(), 2);
    TS_ASSERT_EQUALS(e12.name(), "==");
    TS_ASSERT_EQUALS(e12[0].name(), "x");
    TS_ASSERT_EQUALS(e12[1].name(), "%%");

    Expression e13(bin_ops1, un_ops1);
    TS_ASSERT_THROWS(e13.parse("x==%%"), const std::runtime_error &);

    Expression e14(bin_ops1, un_ops1);
    TS_ASSERT_THROWS(e14.parse("x==%% "), const std::runtime_error &);

    std::vector<std::string> bin_ops2{"-", "--"};
    std::unordered_set<std::string> un_ops2{"-", "--"};

    Expression e15(bin_ops2, un_ops2);
    TS_ASSERT_THROWS_NOTHING(e15.parse("x--1"));
    TS_ASSERT_EQUALS(e15.str(), "x--1");
    TS_ASSERT_EQUALS(e15.size(), 2);
    TS_ASSERT_EQUALS(e15.name(), "--");
    TS_ASSERT_EQUALS(e15[0].name(), "x");
    TS_ASSERT_EQUALS(e15[1].name(), "1");

    Expression e16(bin_ops2, un_ops2);
    TS_ASSERT_THROWS_NOTHING(e16.parse("x---1"));
    TS_ASSERT_EQUALS(e16.str(), "x-- -1");
    TS_ASSERT_EQUALS(e16.size(), 2);
    TS_ASSERT_EQUALS(e16.name(), "--");
    TS_ASSERT_EQUALS(e16[0].name(), "x");
    TS_ASSERT_EQUALS(e16[1].name(), "-");

    Expression e17(bin_ops2, un_ops2);
    TS_ASSERT_THROWS_NOTHING(e17.parse("x-- -1"));
    TS_ASSERT_EQUALS(e17.str(), "x-- -1"); // this is not the same as input
    TS_ASSERT_EQUALS(e17.size(), 2);
    TS_ASSERT_EQUALS(e17.name(), "--");
    TS_ASSERT_EQUALS(e17[0].name(), "x");
    TS_ASSERT_EQUALS(e17[1].name(), "-");

    Expression e18(bin_ops2, un_ops2);
    TS_ASSERT_THROWS_NOTHING(e18.parse("x- --1"));
    TS_ASSERT_EQUALS(e18.str(), "x- --1"); // this is not the same as input
    TS_ASSERT_EQUALS(e18.size(), 2);
    TS_ASSERT_EQUALS(e18.name(), "-");
    TS_ASSERT_EQUALS(e18[0].name(), "x");
    TS_ASSERT_EQUALS(e18[1].name(), "--");

    Expression e19(bin_ops2, un_ops2);
    TS_ASSERT_THROWS_NOTHING(e19.parse("x-- --1"));
    TS_ASSERT_EQUALS(e19.str(), "x-- --1");
    TS_ASSERT_EQUALS(e19.size(), 2);
    TS_ASSERT_EQUALS(e19.name(), "--");
    TS_ASSERT_EQUALS(e19[0].name(), "x");
    TS_ASSERT_EQUALS(e19[1].name(), "--");

    Expression e20(bin_ops2, un_ops2);
    TS_ASSERT_THROWS_NOTHING(e20.parse("x----1"));
    TS_ASSERT_EQUALS(e20.str(), "x-- --1");
    TS_ASSERT_EQUALS(e20.size(), 2);
    TS_ASSERT_EQUALS(e20.name(), "--");
    TS_ASSERT_EQUALS(e20[0].name(), "x");
    TS_ASSERT_EQUALS(e20[1].name(), "--");

    Expression e21(bin_ops2, un_ops2);
    TS_ASSERT_THROWS_NOTHING(e21.parse("x- -1"));
    TS_ASSERT_EQUALS(e21.str(), "x- -1"); // this is not the same as input
    TS_ASSERT_EQUALS(e21.size(), 2);
    TS_ASSERT_EQUALS(e21.name(), "-");
    TS_ASSERT_EQUALS(e21[0].name(), "x");
    TS_ASSERT_EQUALS(e21[1].name(), "-");
  }

  void testNestedUnary() {
    Expression expression;
    TS_ASSERT_THROWS_NOTHING(expression.parse("- - -1"));
    TS_ASSERT_EQUALS(expression.str(),
                     "-( -( -1))"); // this is not the same as input
    TS_ASSERT_EQUALS(expression.size(), 1);
    TS_ASSERT_EQUALS(expression.name(), "-");
    TS_ASSERT_EQUALS(expression[0].name(), "-");
    TS_ASSERT_EQUALS(expression[0][0].name(), "-");
    TS_ASSERT_EQUALS(expression[0][0][0].name(), "1");

    Expression e1;
    TS_ASSERT_THROWS(e1.parse("---1"), const std::runtime_error &);
  }

  void testRemoveBrackets() {
    Expression expression;
    expression.parse("(((a+b+sin(x))))");
    TS_ASSERT_EQUALS(expression.str(), "(((a+b+sin(x))))");
    TS_ASSERT_EQUALS(expression.bracketsRemoved().str(), "a+b+sin(x)");
    TS_ASSERT_EQUALS(expression.bracketsRemoved().name(), "+");
    TS_ASSERT_EQUALS(expression.bracketsRemoved().bracketsRemoved().str(),
                     "a+b+sin(x)");
    TS_ASSERT_EQUALS(expression.bracketsRemoved().bracketsRemoved().name(),
                     "+");
  }

  void testBrackets1() {
    Expression expression;
    expression.parse("2*(a+b)+(1-sin(x-y))");
    TS_ASSERT_EQUALS(expression.str(), "2*(a+b)+(1-sin(x-y))");
  }

  void testBrackets2() {
    Expression expression;
    expression.parse("2*(a+b)-(1-s)");
    TS_ASSERT_EQUALS(expression.str(), "2*(a+b)-(1-s)");
  }

  void testBrackets3() {
    Expression expression;
    expression.parse("2*(a+b)-(1-sin(x-y))");
    TS_ASSERT_EQUALS(expression.str(), "2*(a+b)-(1-sin(x-y))");
  }

  void testGetVariables() {
    Expression expression;
    expression.parse("a+b*sin(x)*fun1(fun2(a+c))");
    std::unordered_set<std::string> vars = expression.getVariables();
    TS_ASSERT_EQUALS(vars.size(), 4);
    TS_ASSERT(vars.find("a") != vars.end());
    TS_ASSERT(vars.find("b") != vars.end());
    TS_ASSERT(vars.find("c") != vars.end());
    TS_ASSERT(vars.find("x") != vars.end());
  }

  void testToList() {
    Expression expression;
    expression.parse("x");
    expression.toList();
    TS_ASSERT_EQUALS(expression.name(), ",");
  }

  void testEmptyString() {
    Expression expression;
    expression.parse("");
    TS_ASSERT_EQUALS(expression.name(), "EMPTY");
  }

  void testEmptyStringInBrackets() {
    {
      Expression expression;
      expression.parse("()");
      TS_ASSERT_EQUALS(expression.name(), "EMPTY");
    }
    {
      Expression expression;
      expression.parse(" (  ) ");
      TS_ASSERT_EQUALS(expression.name(), "EMPTY");
    }
    {
      Expression expression;
      expression.parse(" ((  ) )  ");
      TS_ASSERT_EQUALS(expression.name(), "EMPTY");
    }
  }

  void testEndOnOperator() {
    {
      Expression expression;
      TS_ASSERT_THROWS(expression.parse("x+y+z +  "),
                       const std::runtime_error &);
    }
    {
      Expression expression;
      TS_ASSERT_THROWS(expression.parse("x*z-"), const std::runtime_error &);
    }
    {
      Expression expression;
      TS_ASSERT_THROWS(expression.parse("x*z="), const std::runtime_error &);
    }
    {
      Expression expression;
      TS_ASSERT_THROWS_NOTHING(expression.parse("x;y;z;"));
      TS_ASSERT_EQUALS(expression.size(), 3);
      TS_ASSERT_EQUALS(expression[2].str(), "z");
    }
    {
      Expression expression;
      TS_ASSERT_THROWS_NOTHING(expression.parse("z,y,x,"));
      TS_ASSERT_EQUALS(expression.size(), 3);
      TS_ASSERT_EQUALS(expression[2].str(), "x");
    }
  }

  void test_tie_expression_with_brackets() {
    Expression expression;
    expression.parse("ties=(a0=2,a1=4*(2+2))");
    TS_ASSERT_EQUALS(expression.str(), "ties=(a0=2,a1=4*(2+2))");
    TS_ASSERT_EQUALS(expression[1].name(), ",");
  }

  void test_tie_expression_with_brackets_1() {
    Expression expression;
    expression.parse("ties=(a0 = 2,  Height = Sigma/(0.5 * 0.5))");
    TS_ASSERT_EQUALS(expression.str(), "ties=(a0=2,Height=Sigma/(0.5*0.5))");
    TS_ASSERT_EQUALS(expression[1].name(), ",");
  }

  void test_bad_brackets() {
    Expression expression;
    try {
      expression.parse("x+(y");
      TS_FAIL("Expected an exception.");
    } catch (Expression::ParsingError &error) {
      TS_ASSERT_EQUALS(std::string(error.what()),
                       "Unmatched bracket at\n\n(y\n^\n");
    }
  }

  void test_bad_brackets_1() {
    Expression expression;
    try {
      expression.parse("x+sin(y-3");
      TS_FAIL("Expected an exception.");
    } catch (Expression::ParsingError &error) {
      TS_ASSERT_EQUALS(std::string(error.what()),
                       "Unmatched bracket at\n\nsin(y-3\n   ^\n");
    }
  }

  void test_bad_brackets_2() {
    Expression expression;
    try {
      expression.parse("x+sinsinsinsinsin(y-3");
      TS_FAIL("Expected an exception.");
    } catch (Expression::ParsingError &error) {
      TS_ASSERT_EQUALS(
          std::string(error.what()),
          "Unmatched bracket at\n\n...nsinsinsin(y-3\n             ^\n");
    }
  }

  void test_bad_brackets_3() {
    Expression expression;
    try {
      expression.parse("x+sinsinsinsinsin(y-3*1+2+3+4+5+6+7+0");
      TS_FAIL("Expected an exception.");
    } catch (Expression::ParsingError &error) {
      TS_ASSERT_EQUALS(std::string(error.what()),
                       "Unmatched bracket "
                       "at\n\n...nsinsinsin(y-3*1+2+3+4+"
                       "5+...\n             ^\n");
    }
  }

  void test_bad_brackets_4() {
    Expression expression;
    try {
      expression.parse("x+y+z)");
      TS_FAIL("Expected an exception.");
    } catch (Expression::ParsingError &error) {
      TS_ASSERT_EQUALS(std::string(error.what()),
                       "Unmatched bracket at\n\nx+y+z)\n^\n");
    }
  }

  void test_syntax_error() {
    Expression expression;
    try {
      expression.parse("x+y+");
      TS_FAIL("Expected an exception.");
    } catch (Expression::ParsingError &error) {
      TS_ASSERT_EQUALS(
          std::string(error.what()),
          "A binary operator isn't followed by a value at\n\nx+y+\n   ^\n");
    }
  }

  void test_syntax_error_1() {
    Expression expression;
    try {
      expression.parse("x+y++++z");
      TS_FAIL("Expected an exception.");
    } catch (Expression::ParsingError &error) {
      TS_ASSERT_EQUALS(std::string(error.what()),
                       "Unrecognized operator at\n\nx+y++++z\n   ^\n");
    }
  }

  void test_syntax_error_2() {
    Expression expression;
    try {
      expression.parse("x+y+ + + +");
      TS_FAIL("Expected an exception.");
    } catch (Expression::ParsingError &error) {
      TS_ASSERT_EQUALS(std::string(error.what()),
                       "A binary operator isn't "
                       "followed by a value at\n\nx+y+ "
                       "+ + +\n         ^\n");
    }
  }

  void test_syntax_error_3() {
    Expression expression;
    try {
      expression.parse("x+y+ )");
      TS_FAIL("Expected an exception.");
    } catch (Expression::ParsingError &error) {
      TS_ASSERT_EQUALS(std::string(error.what()),
                       "Unmatched bracket at\n\nx+y+ )\n^\n");
    }
  }
};

#endif /*EXPRESSIONTEST_H_*/
