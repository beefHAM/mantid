#ifndef MULTIPLYTEST_H_
#define MULTIPLYTEST_H_

#include <cxxtest/TestSuite.h>
#include <cmath>

#include "WorkspaceCreationHelper.hh"
#include "MantidAlgorithms/Multiply.h"
#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidDataObjects/Workspace2D.h"
#include "MantidDataObjects/Workspace1D.h"

using namespace Mantid::API;
using namespace Mantid::Kernel;
using namespace Mantid::Algorithms;
using namespace Mantid::DataObjects;

class MultiplyTest : public CxxTest::TestSuite
{
public:

  void testInit()
  {
    Multiply alg;
    TS_ASSERT_THROWS_NOTHING(alg.initialize());
    TS_ASSERT(alg.isInitialized());
    TS_ASSERT_THROWS_NOTHING(
    alg.setPropertyValue("InputWorkspace_1","test_in21");
    alg.setPropertyValue("InputWorkspace_2","test_in22");    
    alg.setPropertyValue("OutputWorkspace","test_out2");
    );

  }

  void testExec1D1D()
  {
    int sizex = 5;
    // Register the workspace in the data service
    AnalysisDataService* ADS = AnalysisDataService::Instance();
    Workspace1D_sptr work_in1 = WorkspaceCreationHelper::Create1DWorkspaceFib(sizex);
    Workspace1D_sptr work_in2 = WorkspaceCreationHelper::Create1DWorkspaceFib(sizex);
    ADS->add("test_in11", work_in1);
    ADS->add("test_in12", work_in2);

    Multiply alg;

    alg.initialize();
    alg.setPropertyValue("InputWorkspace_1","test_in11");
    alg.setPropertyValue("InputWorkspace_2","test_in12");    
    alg.setPropertyValue("OutputWorkspace","test_out1");
    alg.execute();

    Workspace_sptr work_out1;
    TS_ASSERT_THROWS_NOTHING(work_out1 = ADS->retrieve("test_out1"));

    checkData(work_in1, work_in2, work_out1);

    ADS->remove("test_out1");
    ADS->remove("test_in11");
    ADS->remove("test_in12");

  }

  void testExec2D2D()
  {
    int sizex = 2,sizey=4;
    // Register the workspace in the data service
    AnalysisDataService* ADS = AnalysisDataService::Instance();
    Workspace2D_sptr work_in1 = WorkspaceCreationHelper::Create2DWorkspace123(sizex,sizey);
    Workspace2D_sptr work_in2 = WorkspaceCreationHelper::Create2DWorkspace154(sizex,sizey);

    Multiply alg;

    ADS->add("test_in21", work_in1);
    ADS->add("test_in22", work_in2);
    alg.initialize();
    alg.setPropertyValue("InputWorkspace_1","test_in21");
    alg.setPropertyValue("InputWorkspace_2","test_in22");    
    alg.setPropertyValue("OutputWorkspace","test_out2");
    TS_ASSERT_THROWS_NOTHING(alg.execute());
    TS_ASSERT( alg.isExecuted() );
    Workspace_sptr work_out1;
    TS_ASSERT_THROWS_NOTHING(work_out1 = ADS->retrieve("test_out2"));

    checkData(work_in1, work_in2, work_out1);

    ADS->remove("test_in21");
    ADS->remove("test_in22");
    ADS->remove("test_out2");

  }

  void checkData( Workspace_sptr work_in1,  Workspace_sptr work_in2, Workspace_sptr work_out1)
  {
    for (int i = 0; i < work_out1->size(); i++)
    {
      double sig1 = work_in1->dataY(i/work_in1->blocksize())[i%work_in1->blocksize()];
      double sig2 = work_in2->dataY(i/work_in1->blocksize())[i%work_in1->blocksize()];
      double sig3 = work_out1->dataY(i/work_in1->blocksize())[i%work_in1->blocksize()];
      TS_ASSERT_DELTA(work_in1->dataX(i/work_in1->blocksize())[i%work_in1->blocksize()],
        work_out1->dataX(i/work_in1->blocksize())[i%work_in1->blocksize()], 0.0001);
      TS_ASSERT_DELTA(sig1 * sig2, sig3, 0.0001);
      double err1 = work_in1->dataE(i/work_in1->blocksize())[i%work_in1->blocksize()];
      double err2 = work_in2->dataE(i/work_in2->blocksize())[i%work_in1->blocksize()];
      // (Sa/a)2 + (Sb/b)2 = (Sc/c)2 
      //  So after taking proportions, squaring, summing, 
      //  and taking the square root, you get a proportional error to the product c. Multiply that proportional error by c to get the actual standard deviation Sc. 
      double err3(sig3 * sqrt(((err1/sig1)*(err1/sig1)) + ((err2/sig2)*(err2/sig2))));     
      TS_ASSERT_DELTA(err3, work_out1->dataE(i/work_in1->blocksize())[i%work_in1->blocksize()], 0.0001);
    }
  }


};

#endif /*MULTIPLYTEST_H_*/
