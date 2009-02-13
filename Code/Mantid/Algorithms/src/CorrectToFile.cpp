//-----------------------------
// Includes
//----------------------------
#include "MantidAlgorithms/CorrectToFile.h"
#include "MantidKernel/FileValidator.h"
#include "MantidKernel/ListValidator.h"
#include "MantidKernel/UnitFactory.h"

using namespace Mantid::API;
using namespace Mantid::Algorithms;

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(CorrectToFile)

// Get a reference to the logger. It is used to print out information, warning and error messages
Mantid::Kernel::Logger& CorrectToFile::g_log = Mantid::Kernel::Logger::get("CorrectToFile");

void CorrectToFile::init()
{
  declareProperty(new API::WorkspaceProperty<>("WorkspaceToCorrect","",Kernel::Direction::Input));
  declareProperty("Filename","",new Kernel::FileValidator());

  std::vector<std::string> propOptions = Kernel::UnitFactory::Instance().getKeys();
  propOptions.push_back("SpectraNumber");
  declareProperty("FirstColumnValue", "Wavelength", new Kernel::ListValidator(propOptions) );

  std::vector<std::string> operations(1, std::string("Divide"));
  operations.push_back("Multiply");
  declareProperty("WorkspaceOperation", "Divide", new Kernel::ListValidator(operations));
  declareProperty(new API::WorkspaceProperty<>("OutputWorkspace","",Kernel::Direction::Output));
}

void CorrectToFile::exec()
{
  //First load in rkh file for correction
  Algorithm_sptr loadRKH = createSubAlgorithm("LoadRKH");
  std::string rkhfile = getProperty("Filename");
  loadRKH->setPropertyValue("Filename", rkhfile);
  loadRKH->setPropertyValue("OutputWorkspace", "rkhout");
  std::string columnValue = getProperty("FirstColumnValue");
  loadRKH->setPropertyValue("FirstColumnValue", columnValue);

  try
  {
    loadRKH->execute();
  }
  catch(std::runtime_error & err)
  {
    g_log.error() << "Unable to successfully run the LoadRKH sub algorithm.";
    throw std::runtime_error("Error executing LoadRKH as a sub algorithm.");
  }

  //Now get input workspace for this algorithm
  MatrixWorkspace_const_sptr toCorrect = getProperty("WorkspaceToCorrect");
  MatrixWorkspace_const_sptr rkhInput = loadRKH->getProperty("OutputWorkspace");

  // Check that the workspace to rebin has the same units as the one that we are matching to
  // However, just print a warning if it isn't, don't abort (since user provides the file's unit)
  if (toCorrect->getAxis(0)->unit() != rkhInput->getAxis(0)->unit())
  {
    g_log.warning("Unit on input workspace is different to that specified in 'FirstColumnValue' property");
  }

  // Get references to the correction factors
  const std::vector<double> &Xcor = rkhInput->readX(0);
  const std::vector<double> &Ycor = rkhInput->readY(0);
  const std::vector<double> &Ecor = rkhInput->readE(0);

  // Create the output workspace
  MatrixWorkspace_sptr outputWS = WorkspaceFactory::Instance().create(toCorrect);
  const bool histogramData = outputWS->isHistogramData();
  const std::string operation = getProperty("WorkspaceOperation");
  const bool divide = (operation == "Divide") ? true : false;
  double Yfactor,Efactor;

  MatrixWorkspace::iterator outIt(*outputWS);
  for (MatrixWorkspace::const_iterator inIt(*toCorrect); inIt != inIt.end(); ++inIt,++outIt)
  {
    const double currentX = histogramData ? (inIt->X()+inIt->X2())/2.0 : inIt->X();
    // Find out the index of the first correction point after this value
    std::vector<double>::const_iterator pos = std::lower_bound(Xcor.begin(),Xcor.end(),currentX);
    const int index = pos-Xcor.begin();
    if ( index == Xcor.size() )
    {
      // If we're past the end of the correction factors vector, use the last point
      Yfactor = Ycor[index-1];
      Efactor = Ycor[index-1];
    }
    else if (index)
    {
      // Calculate where between the two closest points our current X value is
      const double fraction = (currentX-Xcor[index-1])/(Xcor[index]-Xcor[index-1]);
      // Now linearly interpolate to find the correction factors to use
      Yfactor = Ycor[index-1] + fraction*(Ycor[index]-Ycor[index-1]);
      Efactor = Ecor[index-1] + fraction*(Ecor[index]-Ecor[index-1]);
    }
    else
    {
      // If we're before the start of the correction factors vector, use the first point
      Yfactor = Ycor[0];
      Efactor = Ycor[0];
    }

    // Now do the correction on the current point
    if (divide)
    {
      outIt->Y() = inIt->Y()/Yfactor;
      outIt->E() = inIt->E()/Efactor;
    }
    else
    {
      outIt->Y() = inIt->Y()*Yfactor;
      outIt->E() = inIt->E()*Efactor;
    }
  }

  //Set the resulting workspace
  setProperty("Outputworkspace", outputWS);
}
