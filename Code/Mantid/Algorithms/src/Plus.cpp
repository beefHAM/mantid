//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------

#include <cmath>
#include "MantidAlgorithms/Plus.h"
#include "MantidAlgorithms/BinaryOpHelper.h"
#include "MantidKernel/PropertyWithValue.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidKernel/Exception.h" 
#include "MantidAPI/TripleIterator.h" 

// Register the class into the algorithm factory
DECLARE_NAMESPACED_ALGORITHM(Mantid::Algorithms,Plus)
using namespace Mantid::DataObjects;
using namespace Mantid::API;
using namespace Mantid::Kernel;

namespace Mantid
{
  namespace Algorithms
  {
    // Get a reference to the logger
    Logger& Plus::g_log = Logger::get("Plus");

    /** Initialisation method. 
    * Defines input and output workspaces
    * 
    */
    void Plus::init()
    {
      declareProperty(new WorkspaceProperty<Workspace>("InputWorkspace_1","",Direction::Input));
      declareProperty(new WorkspaceProperty<Workspace>("InputWorkspace_2","",Direction::Input));
      declareProperty(new WorkspaceProperty<Workspace>("OutputWorkspace","",Direction::Output));    
    }

    /** Executes the algorithm
    * 
    *  @throw runtime_error Thrown if algorithm cannot execute
    */
    void Plus::exec()
    {
      // get input workspace, dynamic cast not needed
      Workspace_sptr in_work1 = getProperty("InputWorkspace_1");
      Workspace_sptr in_work2 = getProperty("InputWorkspace_2");

      //create a BinaryOpHelper
      BinaryOpHelper boHelper;
      if (!boHelper.checkSizeCompatability(in_work1,in_work2))
      {
        throw std::invalid_argument("The size of the two workspaces are not compatible for algorithm plus"  );
      }

      if (!boHelper.checkXarrayCompatability(in_work1,in_work2))
      {
        g_log.error("The x arrays of the workspaces are not identical");
        throw std::invalid_argument("The x arrays of the workspaces are not identical");
      }

      Workspace_sptr out_work = boHelper.createOutputWorkspace(in_work1,in_work2);
/*
      Workspace2D_sptr workin1=boost::dynamic_pointer_cast<Workspace2D>(in_work1);
      Workspace2D_sptr workin2=boost::dynamic_pointer_cast<Workspace2D>(in_work2);
      Workspace2D_sptr workout=boost::dynamic_pointer_cast<Workspace2D>(out_work);

      for( int i=0;i<workin1->getHistogramNumber();i++)
        for(int j=0; j<workin1->blocksize();j++)
        {
          double& e1=workin1->dataE(i)[j];
          double& e2=workin2->dataE(i)[j];
          workout->dataY(i)[j]=workin1->dataY(i)[j]+workin2->dataY(i)[j];
          workout->dataX(i)[j]=workin1->dataX(i)[j];
          workout->dataE(i)[j]=sqrt((e2*e2)+(e1*e1));
        }
*/    

      triple_iterator<Workspace> ti_out(*out_work);
      triple_iterator<Workspace> ti_in1(*in_work1);
      triple_iterator<Workspace> ti_in2(*in_work2);
      std::transform(ti_in1.begin(),ti_in1.end(),ti_in2.begin(),ti_out.begin(),Plus_fn());

/*
      for( ti_in1=ti_in1.begin();ti_in1!=ti_in1.end();++ti_in1)
      {
      (*ti_out)[0]=(*ti_in1)[0];
      (*ti_out)[1]=0;//(*ti_in1)[1]+(*ti_in2)[1];
      (*ti_out)[2]=0;// sqrt(  ((*ti_in1)[2]*(*ti_in1)[2]) + ((*ti_in2)[2]*(*ti_in2)[2]));

//        ++ti_in2;
        ++ti_out;
      }
*/



      // Assign it to the output workspace property
      setProperty("OutputWorkspace",out_work);

      return;
    }


    /** Performs the addition with Gaussian errors within the transform function
    * @param a The triple ref of the first workspace data item
    * @param b The triple ref of the second workspace data item
    * @returns A triple ref of the result with Gaussian errors
    */
    TripleRef<double&>
      Plus::Plus_fn::operator() (const TripleRef<double&>& a,const TripleRef<double&>& b) 
    {           
      ret_sig=a[1]+b[1];
      //Gaussian errors for the moment
      ret_err=sqrt((a[2]*a[2])+(b[2]*b[2]));     
      return TripleRef<double&>(a[0],ret_sig,ret_err);      
    }
  }
}
