/*WIKI*
The algorithms will attach an OrientedLattice object to a sample in the workspace. For MD workspaces, you can select to which sample to attach it. If nothing entered, it will attach to all. If bad number is enetered, it will attach to first sample.

If UB matrix elements are entered, lattice parameters and orientation vectors are ignored. The algorithm will throw an exception if the determinant is 0.
If the UB matrix is all zeros (default), it will calculate it from lattice parameters and orientation vectors. The algorithm will throw an exception if u and v are collinear, or one of them is very small in magnitude.
*WIKI*/

#include "MantidCrystal/SetUB.h"
#include "MantidKernel/System.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/ArrayLengthValidator.h"
#include "MantidGeometry/Crystal/OrientedLattice.h"
#include "MantidAPI/IMDEventWorkspace.h"

using namespace Mantid::Kernel;
using namespace Mantid::API;
using Mantid::Geometry::OrientedLattice;

namespace Mantid
{
namespace Crystal
{

  // Register the algorithm into the AlgorithmFactory
  DECLARE_ALGORITHM(SetUB)
  


  //----------------------------------------------------------------------------------------------
  /** Constructor
   */
  SetUB::SetUB()
  {
  }
    
  //----------------------------------------------------------------------------------------------
  /** Destructor
   */
  SetUB::~SetUB()
  {
  }
  

  //----------------------------------------------------------------------------------------------
  /// Algorithm's name for identification. @see Algorithm::name
  const std::string SetUB::name() const
  {
    return "SetUB";
  }
  
  /// Algorithm's version for identification. @see Algorithm::version
  int SetUB::version() const
  {
    return 1;
  }
  
  /// Algorithm's category for identification. @see Algorithm::category
  const std::string SetUB::category() const
  {
    return "Crystal";
  }

  //----------------------------------------------------------------------------------------------
  /// Sets documentation strings for this algorithm
  void SetUB::initDocs()
  {
    this->setWikiSummary("Set the UB matrix, given either lattice parametersand orientation vectors or the UB matrix elements");
    this->setOptionalMessage("Set the UB matrix, given either lattice parametersand orientation vectors or the UB matrix elements");
  }

  //----------------------------------------------------------------------------------------------
  /** Initialize the algorithm's properties.
   */
  void SetUB::init()
  {
    BoundedValidator<double> *mustBePositive = new BoundedValidator<double>();
    mustBePositive->setLower(0.0);
    BoundedValidator<double> *reasonableAngle = new BoundedValidator<double>();
    reasonableAngle->setLower(5.0);
    reasonableAngle->setUpper(175.0);
    ArrayLengthValidator<double> *mustBe3D = new ArrayLengthValidator<double>(3);
    ArrayLengthValidator<double> *threeVthree = new ArrayLengthValidator<double>(9);
    std::vector<double>  zeroes(9,0.),u0(3,0),v0(3,0);
    u0[0]=1.;
    v0[1]=1.;
    this->declareProperty(new WorkspaceProperty<Workspace>("Workspace","",Direction::InOut), "An input workspace.");
    this->declareProperty(new PropertyWithValue<double>("a",1.0,mustBePositive,Direction::Input),"Lattice parameter a");
    this->declareProperty(new PropertyWithValue<double>("b",1.0,mustBePositive->clone(),Direction::Input),"Lattice parameter b");
    this->declareProperty(new PropertyWithValue<double>("c",1.0,mustBePositive->clone(),Direction::Input),"Lattice parameter c");
    this->declareProperty(new PropertyWithValue<double>("alpha",90.0,reasonableAngle,Direction::Input),"Lattice parameter alpha (degrees)");
    this->declareProperty(new PropertyWithValue<double>("beta",90.0,reasonableAngle->clone(),Direction::Input),"Lattice parameter beta (degrees)");
    this->declareProperty(new PropertyWithValue<double>("gamma",90.0,reasonableAngle->clone(),Direction::Input),"Lattice parameter gamma(degrees) ");
    this->declareProperty(new ArrayProperty<double>("u",u0,mustBe3D),"Vector along k_i, when goniometer is at 0");
    this->declareProperty(new ArrayProperty<double>("v",v0,mustBe3D->clone()),"In plane vector perpendicular to k_i, when goniometer is at 0");
    this->declareProperty(new ArrayProperty<double>("UB",zeroes,threeVthree->clone()),"UB Matrix");
    this->declareProperty(new PropertyWithValue<int>("MDSampleNumber",EMPTY_INT(),Direction::Input),"For an MD workspace, the sample number to wich to attach an oriented lattice (starting from 0). No number, or negative number, means that it will copy to all samples" );

  }

  //----------------------------------------------------------------------------------------------
  /** Execute the algorithm.
    @throws :: |B.u|=0 or |B.v|=0 or u||v
   */
  void SetUB::exec()
  {
    Mantid::Geometry::OrientedLattice o;
    std::vector<double> UBvec=getProperty("UB");
    Mantid::Kernel::DblMatrix UBMatrix(UBvec),zeroMatrix(3,3);
    if (UBMatrix==zeroMatrix)
    {
      double a,b,c,alpha,beta,gamma;
      a=getProperty("a");
      b=getProperty("b");
      c=getProperty("c");
      alpha=getProperty("alpha");
      beta=getProperty("beta");
      gamma=getProperty("gamma");
      std::vector<double> u = getProperty("u");
      std::vector<double> v = getProperty("v");

      o=Mantid::Geometry::OrientedLattice(a,b,c,alpha,beta,gamma);
      o.setUFromVectors(Mantid::Kernel::V3D(u[0],u[1],u[2]),Mantid::Kernel::V3D(v[0],v[1],v[2]));
    }
    else
    {
      if (UBMatrix.determinant()==0) throw std::invalid_argument("UB matrix determinant is 0");
      else o.setUB(UBMatrix);
    }

    //now attach the oriented lattice to the workspace
    Workspace_sptr ws=this->getProperty("Workspace");

    //Sample copy;
    IMDEventWorkspace_sptr mdws=boost::dynamic_pointer_cast<IMDEventWorkspace>(ws);
    if (mdws != NULL)
    {
      int sampleNumber=getProperty("MDSampleNumber");
      if ((sampleNumber==EMPTY_INT()) || (sampleNumber<0)) //copy to all samples
      {
        for(uint16_t i=0;i<mdws->getNumExperimentInfo();i++)
          mdws->getExperimentInfo(i)->mutableSample().setOrientedLattice(new OrientedLattice(o));
      }
      else //copy to a single sample
      {
        if (static_cast<uint16_t>(sampleNumber)>(mdws->getNumExperimentInfo()-1))
        {
          g_log.warning()<<"Number greater than the number of last sample in the workspace ("<<(mdws->getNumExperimentInfo()-1)<<"). Will use sample number 0 instead\n";
          sampleNumber=0;
        }
        mdws->getExperimentInfo(static_cast<uint16_t>(sampleNumber))->mutableSample().setOrientedLattice(new OrientedLattice(o));
      }
    }
    else //peaks workspace or matrix workspace
    {
      ExperimentInfo_sptr ei=boost::dynamic_pointer_cast<ExperimentInfo>(ws);
      if (!ei) throw std::invalid_argument("Wrong type of workspace");
      ei->mutableSample().setOrientedLattice(new OrientedLattice(o));
    }
    this->setProperty("Workspace",ws);
  }

} // namespace Mantid
} // namespace Crystal
