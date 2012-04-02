#ifndef MULTIDOMAINFUNCTIONTEST_H_
#define MULTIDOMAINFUNCTIONTEST_H_

#include "MantidAPI/FunctionDomain1D.h"
#include "MantidAPI/FunctionValues.h"
#include "MantidAPI/MultiDomainFunction.h"
#include "MantidAPI/JointDomain.h"
#include "MantidAPI/IFunction1D.h"
#include "MantidAPI/ParamFunction.h"
#include "MantidAPI/FunctionFactory.h"

#include <cxxtest/TestSuite.h>
#include <boost/make_shared.hpp>
#include <algorithm>
#include <iostream>

using namespace Mantid;
using namespace Mantid::API;

class MultiDomainFunctionTest_Function: public virtual IFunction1D, public virtual ParamFunction
{
public:
  MultiDomainFunctionTest_Function():IFunction1D(),ParamFunction()
  {
    this->declareParameter("A",0);
    this->declareParameter("B",0);
  }
  virtual std::string name() const {return "MultiDomainFunctionTest_Function";}
protected:
  virtual void function1D(double* out, const double* xValues, const size_t nData)const
  {
    const double A = getParameter(0);
    const double B = getParameter(1);

    for(size_t i = 0; i < nData; ++i)
    {
      double x = xValues[i];
      out[i] = A + B * x;
    }
  }
  virtual void functionDeriv1D(Jacobian* out, const double* xValues, const size_t nData)
  {
    for(size_t i = 0; i < nData; ++i)
    {
      double x = xValues[i];
      out->set(i,0,x);
      out->set(i,1,1.0);
    }
  }
};

DECLARE_FUNCTION(MultiDomainFunctionTest_Function);

class MultiDomainFunctionTest : public CxxTest::TestSuite
{
public:
  MultiDomainFunctionTest()
  {
    multi.addFunction(boost::make_shared<MultiDomainFunctionTest_Function>());
    multi.addFunction(boost::make_shared<MultiDomainFunctionTest_Function>());
    multi.addFunction(boost::make_shared<MultiDomainFunctionTest_Function>());

    multi.getFunction(0)->setParameter("A",0);
    multi.getFunction(0)->setParameter("B",1);

    multi.getFunction(1)->setParameter("A",1);
    multi.getFunction(1)->setParameter("B",2);

    multi.getFunction(2)->setParameter("A",2);
    multi.getFunction(2)->setParameter("B",3);

    domain.addDomain(boost::make_shared<FunctionDomain1DVector>(0,1,9));
    domain.addDomain(boost::make_shared<FunctionDomain1DVector>(1,2,10));
    domain.addDomain(boost::make_shared<FunctionDomain1DVector>(2,3,11));

  }

  void test_calc_domain0_only()
  {
    multi.setDomainIndex(0,0);
    multi.setDomainIndices(1,std::vector<size_t>());
    multi.setDomainIndices(2,std::vector<size_t>());
    //multi.setDomainIndex(1,1);
    //multi.setDomainIndex(2,2);

    //std::vector<size_t> ii;
    //ii.push_back(0);
    //ii.push_back(1);
    //multi.setDomainIndices(1,ii);
    //ii.clear();
    //ii.push_back(0);
    //ii.push_back(2);
    //multi.setDomainIndices(2,ii);

    FunctionValues values(domain);
    multi.function(domain,values);

    const double A = multi.getFunction(0)->getParameter("A");
    const double B = multi.getFunction(0)->getParameter("B");
    const FunctionDomain1D& d = static_cast<const FunctionDomain1D&>(domain.getDomain(0));
    for(size_t i = 0; i < 9; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d[i]);
    }
    for(size_t i = 9; i < 19; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), 0);
    }
    for(size_t i = 19; i < 30; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), 0);
    }

  }

  void test_calc_domain1_only()
  {
    multi.setDomainIndex(0,1);
    multi.setDomainIndices(1,std::vector<size_t>());
    multi.setDomainIndices(2,std::vector<size_t>());

    FunctionValues values(domain);
    multi.function(domain,values);

    const double A = multi.getFunction(0)->getParameter("A");
    const double B = multi.getFunction(0)->getParameter("B");
    const FunctionDomain1D& d = static_cast<const FunctionDomain1D&>(domain.getDomain(1));
    for(size_t i = 0; i < 9; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), 0);
    }
    for(size_t i = 9; i < 19; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d[i-9]);
    }
    for(size_t i = 19; i < 30; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), 0);
    }
  }

  void test_calc_domain2_only()
  {
    multi.setDomainIndex(0,2);
    multi.setDomainIndices(1,std::vector<size_t>());
    multi.setDomainIndices(2,std::vector<size_t>());

    FunctionValues values(domain);
    multi.function(domain,values);

    const double A = multi.getFunction(0)->getParameter("A");
    const double B = multi.getFunction(0)->getParameter("B");
    const FunctionDomain1D& d = static_cast<const FunctionDomain1D&>(domain.getDomain(2));
    for(size_t i = 0; i < 9; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), 0);
    }
    for(size_t i = 9; i < 19; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), 0);
    }
    for(size_t i = 19; i < 30; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d[i-19]);
    }

  }

  void test_calc_all_domains()
  {
    multi.clearDomainIndices();
    multi.setDomainIndices(1,std::vector<size_t>());
    multi.setDomainIndices(2,std::vector<size_t>());

    FunctionValues values(domain);
    multi.function(domain,values);

    const double A = multi.getFunction(0)->getParameter("A");
    const double B = multi.getFunction(0)->getParameter("B");
    const FunctionDomain1D& d0 = static_cast<const FunctionDomain1D&>(domain.getDomain(0));
    for(size_t i = 0; i < 9; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d0[i]);
    }
    const FunctionDomain1D& d1 = static_cast<const FunctionDomain1D&>(domain.getDomain(1));
    for(size_t i = 9; i < 19; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d1[i-9]);
    }
    const FunctionDomain1D& d2 = static_cast<const FunctionDomain1D&>(domain.getDomain(2));
    for(size_t i = 19; i < 30; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d2[i-19]);
    }

  }

  void test_calc()
  {
    multi.setDomainIndex(0,0);
    std::vector<size_t> ii;
    ii.push_back(0);
    ii.push_back(1);
    multi.setDomainIndices(1,ii);
    ii.clear();
    ii.push_back(0);
    ii.push_back(2);
    multi.setDomainIndices(2,ii);

    FunctionValues values(domain);
    multi.function(domain,values);

    double A = multi.getFunction(0)->getParameter("A") + 
      multi.getFunction(1)->getParameter("A") + 
      multi.getFunction(2)->getParameter("A");
    double B = multi.getFunction(0)->getParameter("B") + 
      multi.getFunction(1)->getParameter("B") + 
      multi.getFunction(2)->getParameter("B");
    const FunctionDomain1D& d0 = static_cast<const FunctionDomain1D&>(domain.getDomain(0));
    for(size_t i = 0; i < 9; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d0[i]);
    }

    A = multi.getFunction(1)->getParameter("A");
    B = multi.getFunction(1)->getParameter("B");
    const FunctionDomain1D& d1 = static_cast<const FunctionDomain1D&>(domain.getDomain(1));
    for(size_t i = 9; i < 19; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d1[i-9]);
    }

    A = multi.getFunction(2)->getParameter("A");
    B = multi.getFunction(2)->getParameter("B");
    const FunctionDomain1D& d2 = static_cast<const FunctionDomain1D&>(domain.getDomain(2));
    for(size_t i = 19; i < 30; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d2[i-19]);
    }

  }

  void test_attribute()
  {
    multi.clearDomainIndices();
    multi.setLocalAttributeValue(0,"domains","i");
    multi.setLocalAttributeValue(1,"domains","0,1");
    multi.setLocalAttributeValue(2,"domains","0,2");

    FunctionValues values(domain);
    multi.function(domain,values);

    double A = multi.getFunction(0)->getParameter("A") + 
      multi.getFunction(1)->getParameter("A") + 
      multi.getFunction(2)->getParameter("A");
    double B = multi.getFunction(0)->getParameter("B") + 
      multi.getFunction(1)->getParameter("B") + 
      multi.getFunction(2)->getParameter("B");
    const FunctionDomain1D& d0 = static_cast<const FunctionDomain1D&>(domain.getDomain(0));
    for(size_t i = 0; i < 9; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d0[i]);
    }

    A = multi.getFunction(1)->getParameter("A");
    B = multi.getFunction(1)->getParameter("B");
    const FunctionDomain1D& d1 = static_cast<const FunctionDomain1D&>(domain.getDomain(1));
    for(size_t i = 9; i < 19; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d1[i-9]);
    }

    A = multi.getFunction(2)->getParameter("A");
    B = multi.getFunction(2)->getParameter("B");
    const FunctionDomain1D& d2 = static_cast<const FunctionDomain1D&>(domain.getDomain(2));
    for(size_t i = 19; i < 30; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d2[i-19]);
    }

  }

  void test_attribute_in_FunctionFactory()
  {
    std::string ini = "composite=MultiDomainFunction;"
      "name=MultiDomainFunctionTest_Function,A=0,B=1,$domains=i;"
      "name=MultiDomainFunctionTest_Function,A=1,B=2,$domains=(0,1);"
      "name=MultiDomainFunctionTest_Function,A=2,B=3,$domains=(0,2)"
      ;
    auto mfun = boost::dynamic_pointer_cast<CompositeFunction>(FunctionFactory::Instance().createInitialized(ini));

    FunctionValues values(domain);
    mfun->function(domain,values);

    double A = mfun->getFunction(0)->getParameter("A") + 
      mfun->getFunction(1)->getParameter("A") + 
      mfun->getFunction(2)->getParameter("A");
    double B = mfun->getFunction(0)->getParameter("B") + 
      mfun->getFunction(1)->getParameter("B") + 
      mfun->getFunction(2)->getParameter("B");
    const FunctionDomain1D& d0 = static_cast<const FunctionDomain1D&>(domain.getDomain(0));
    double checksum = 0;
    for(size_t i = 0; i < 9; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d0[i]);
      checksum += values.getCalculated(i);
    }
    TS_ASSERT_DIFFERS(checksum,0);

    checksum = 0;
    A = mfun->getFunction(1)->getParameter("A");
    B = mfun->getFunction(1)->getParameter("B");
    const FunctionDomain1D& d1 = static_cast<const FunctionDomain1D&>(domain.getDomain(1));
    for(size_t i = 9; i < 19; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d1[i-9]);
      checksum += values.getCalculated(i);
    }
    TS_ASSERT_DIFFERS(checksum,0);

    checksum = 0;
    A = mfun->getFunction(2)->getParameter("A");
    B = mfun->getFunction(2)->getParameter("B");
    const FunctionDomain1D& d2 = static_cast<const FunctionDomain1D&>(domain.getDomain(2));
    for(size_t i = 19; i < 30; ++i)
    {
      TS_ASSERT_EQUALS(values.getCalculated(i), A + B * d2[i-19]);
      checksum += values.getCalculated(i);
    }
    TS_ASSERT_DIFFERS(checksum,0);

  }

private:
  MultiDomainFunction multi;
  JointDomain domain;
};

#endif /*MULTIDOMAINFUNCTIONTEST_H_*/
