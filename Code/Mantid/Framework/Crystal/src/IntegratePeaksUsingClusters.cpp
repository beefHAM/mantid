/*WIKI*
Handles integration of arbitary single crystal peaks shapes.

Uses connected component analysis to integrate peaks in an PeaksWorkspace over an MDHistoWorkspace of data.
*WIKI*/

#include "MantidCrystal/IntegratePeaksUsingClusters.h"
#include "MantidCrystal/ConnectedComponentLabeling.h"
#include "MantidCrystal/PeakBackground.h"
#include "MantidAPI/IMDHistoWorkspace.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidAPI/IMDIterator.h"
#include "MantidAPI/AlgorithmManager.h"
#include "MantidKernel/CompositeValidator.h"
#include "MantidKernel/MandatoryValidator.h"
#include "MantidKernel/BoundedValidator.h"
#include "MantidKernel/ListValidator.h"
#include "MantidKernel/Utils.h"
#include "MantidDataObjects/PeaksWorkspace.h"
#include <boost/make_shared.hpp>
#include <map>
#include <algorithm>
#include <boost/tuple/tuple.hpp>

using namespace Mantid::API;
using namespace Mantid::Kernel;
using namespace Mantid::DataObjects;

namespace
{
  typedef boost::tuple<double, double> SignalErrorSQPair;
  typedef std::map<size_t, SignalErrorSQPair > LabelIdIntensityMap;
  typedef std::map<V3D, size_t> PositionToLabelIdMap;

  class IsNearPeak
  {
  public:
    IsNearPeak(const V3D& coordinates, const double& thresholdDistance ) : m_coordinates(coordinates), m_thresholdDistance(thresholdDistance)
    {}
    bool operator()( const PositionToLabelIdMap::value_type& v ) const 
    { 
      return  v.first.distance(m_coordinates) < m_thresholdDistance; 
    }
  private:
    const V3D m_coordinates;
    const double m_thresholdDistance;
  };

}

namespace Mantid
{
  namespace Crystal
  {

    // Register the algorithm into the AlgorithmFactory
    DECLARE_ALGORITHM(IntegratePeaksUsingClusters)



    //----------------------------------------------------------------------------------------------
    /** Constructor
    */
    IntegratePeaksUsingClusters::IntegratePeaksUsingClusters()
    {
    }

    //----------------------------------------------------------------------------------------------
    /** Destructor
    */
    IntegratePeaksUsingClusters::~IntegratePeaksUsingClusters()
    {
    }


    //----------------------------------------------------------------------------------------------
    /// Algorithm's name for identification. @see Algorithm::name
    const std::string IntegratePeaksUsingClusters::name() const { return "IntegratePeaksUsingClusters";};

    /// Algorithm's version for identification. @see Algorithm::version
    int IntegratePeaksUsingClusters::version() const { return 1;};

    /// Algorithm's category for identification. @see Algorithm::category
    const std::string IntegratePeaksUsingClusters::category() const { return "MDAlgorithms";}

    //----------------------------------------------------------------------------------------------
    /// Sets documentation strings for this algorithm
    void IntegratePeaksUsingClusters::initDocs()
    {
      this->setWikiSummary("Integrate single crystal peaks using connected component analysis");
      this->setOptionalMessage(this->getWikiSummary());
    }

    //----------------------------------------------------------------------------------------------
    /** Initialize the algorithm's properties.
    */
    void IntegratePeaksUsingClusters::init()
    {
      declareProperty(new WorkspaceProperty<IMDHistoWorkspace>("InputWorkspace","",Direction::Input), "Input md workspace.");
      declareProperty(new WorkspaceProperty<IPeaksWorkspace>("PeaksWorkspace","", Direction::Input),"A PeaksWorkspace containing the peaks to integrate.");
      
      auto positiveValidator = boost::make_shared<BoundedValidator<double> >();
      positiveValidator->setLower(0);
      
      auto compositeValidator = boost::make_shared<CompositeValidator>();
      compositeValidator->add(positiveValidator);
      compositeValidator->add(boost::make_shared<MandatoryValidator<double > >());

      declareProperty(new PropertyWithValue<double>("RadiusEstimate", 0.0, compositeValidator, Direction::Input), "Estimate of Peak Radius. Points beyond this radius will not be considered, so caution towards the larger end.");
      
      declareProperty(new PropertyWithValue<double>("Threshold", 0, positiveValidator->clone(), Direction::Input), "Threshold signal above which to consider peaks");
      declareProperty(new WorkspaceProperty<IPeaksWorkspace>("OutputWorkspace","",Direction::Output), "An output integrated peaks workspace."); 
      declareProperty(new WorkspaceProperty<IMDHistoWorkspace>("OutputWorkspaceMD","",Direction::Output), "MDHistoWorkspace containing the clusters.");
    }

    //----------------------------------------------------------------------------------------------
    /** Execute the algorithm.
    */
    void IntegratePeaksUsingClusters::exec()
    {
      IMDHistoWorkspace_sptr mdWS = getProperty("InputWorkspace");
      IPeaksWorkspace_sptr inPeakWS = getProperty("PeaksWorkspace");
      IPeaksWorkspace_sptr peakWS = getProperty("OutputWorkspace");
      if (peakWS != inPeakWS)
      {
        auto cloneAlg = createChildAlgorithm("CloneWorkspace");
        cloneAlg->setProperty("InputWorkspace", inPeakWS);
        cloneAlg->setPropertyValue("OutputWorkspace", "out_ws");
        cloneAlg->execute();
        {
          Workspace_sptr temp = cloneAlg->getProperty("OutputWorkspace");
          peakWS = boost::dynamic_pointer_cast<IPeaksWorkspace>(temp);
        }
      }

      const SpecialCoordinateSystem mdCoordinates = mdWS->getSpecialCoordinateSystem();
      if(mdCoordinates == None)
      {
        throw std::invalid_argument("The coordinate system of the input MDWorkspace cannot be established. Run SetSpecialCoordinates on InputWorkspace.");
      }

      const double threshold = getProperty("Threshold");
      const double radiusEstimate = getProperty("RadiusEstimate");
      PeakBackground background(peakWS, radiusEstimate, threshold, NoNormalization, mdCoordinates);
      //HardThresholdBackground background(threshold, normalization);

      ConnectedComponentLabeling analysis;
      IMDHistoWorkspace_sptr clusters = analysis.execute(mdWS, &background);

      /*
      Note that the following may be acheived better inside the clustering utility at the same time as the 
      cluster workspace is populated.

      Accumulate intesity values for each peak cluster and key by label_id
      */
      LabelIdIntensityMap labelMap;
      PositionToLabelIdMap positionMap;

      for(size_t i = 0; i < clusters->getNPoints(); ++i)
      {
        const size_t& label_id = static_cast<size_t>(clusters->getSignalAt(i));

        const double& signal = mdWS->getSignalAt(i);
        double errorSQ = mdWS->getErrorAt(i);
        errorSQ *=errorSQ;
        if(label_id >= analysis.getStartLabelId())
        {
          if(labelMap.find(label_id) != labelMap.end())
          {
            SignalErrorSQPair current = labelMap[label_id];
            labelMap[label_id] = SignalErrorSQPair(current.get<0>() + signal, current.get<1>() + errorSQ);
          }
          else
          {
            labelMap[label_id] = SignalErrorSQPair(signal, errorSQ);

            const VMD& center = mdWS->getCenter(i);
            V3D temp(center[0], center[1], center[2]);
            positionMap[temp] = label_id; //Record charcteristic position of the cluster.
          }
        }
      }


      for(size_t i =0; i < peakWS->getNumberPeaks(); ++i)
      {
        IPeak& peak = peakWS->getPeak(i);
        V3D coords;
        if(mdCoordinates==QLab)
        {
          coords= peakWS->getPeak(i).getQLabFrame();
        }
        else if(mdCoordinates==QSample)
        {
          coords= peakWS->getPeak(i).getQSampleFrame();
        }
        else if(mdCoordinates==Mantid::API::HKL)
        {
          coords= peakWS->getPeak(i).getHKL();
        }

        /* Now find the label corresponding to these coordinates. Use the characteristic coordinates of the coordinates
        recorded earlier to do this. A better implemention would be a direct lookup.
        */
        IsNearPeak nearPeak(coords, radiusEstimate);
        auto iterator = std::find_if(positionMap.begin(), positionMap.end(), nearPeak);
        if(iterator != positionMap.end())
        {
          peak.setIntensity(labelMap[ iterator->second ].get<0>());
          peak.setSigmaIntensity(labelMap[ iterator->second ].get<1>());
        }
      }

      setProperty("OutputWorkspace", peakWS);
      setProperty("OutputWorkspaceMD", clusters);
    }



  } // namespace Crystal
} // namespace Mantid