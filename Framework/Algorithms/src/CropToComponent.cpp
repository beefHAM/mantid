#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAlgorithms/CropToComponent.h"
#include "MantidGeometry/IDetector.h"
#include "MantidGeometry/Instrument.h"
#include "MantidKernel/ArrayProperty.h"

namespace {

void getDetectors(
    Mantid::API::MatrixWorkspace_sptr workspace,
    const std::vector<std::string> &componentNames,
    std::vector<Mantid::Geometry::IDetector_const_sptr> &detectors) {
  auto instrument = workspace->getInstrument();
  for (const auto &componentName : componentNames) {
    instrument->getDetectorsInBank(detectors, componentName);
  }
}

void getDetectorIDs(
    std::vector<Mantid::Geometry::IDetector_const_sptr> &detectors,
    std::vector<Mantid::detid_t> &detectorIDs) {
  auto numberOfDetectors = static_cast<int>(detectors.size());
  PARALLEL_FOR_NO_WSP_CHECK()
  for (int index = 0; index < numberOfDetectors; ++index) {
    auto det = detectors[index];
    detectorIDs[index] = det->getID();
  }
}
}

namespace Mantid {
namespace Algorithms {

using Mantid::Kernel::Direction;
using Mantid::API::WorkspaceProperty;

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(CropToComponent)

//----------------------------------------------------------------------------------------------

/// Algorithms name for identification. @see Algorithm::name
const std::string CropToComponent::name() const { return "CropToComponent"; }

/// Algorithm's version for identification. @see Algorithm::version
int CropToComponent::version() const { return 1; }

/// Algorithm's category for identification. @see Algorithm::category
const std::string CropToComponent::category() const {
  return "Transforms\\Splitting";
}

/// Algorithm's summary for use in the GUI and help. @see Algorithm::summary
const std::string CropToComponent::summary() const {
  return "Crops a workspace to a set of components.";
}

//----------------------------------------------------------------------------------------------
/** Initialize the algorithm's properties.
 */
void CropToComponent::init() {
  declareProperty(
      Kernel::make_unique<WorkspaceProperty<Mantid::API::MatrixWorkspace>>(
          "InputWorkspace", "", Direction::Input),
      "An input workspace.");
  declareProperty(
      Kernel::make_unique<WorkspaceProperty<Mantid::API::MatrixWorkspace>>(
          "OutputWorkspace", "", Direction::Output),
      "An output workspace.");
  declareProperty(
      Kernel::make_unique<Mantid::Kernel::ArrayProperty<std::string>>(
          "ComponentNames"),
      "List of component names which are used to crop the workspace."
      "to.");
  declareProperty("OrderByDetId", false,
                  "Whether to order the elements of "
                  "the component by increasing detector ID.");
}

//----------------------------------------------------------------------------------------------
/** Execute the algorithm.
 */
void CropToComponent::exec() {
  // Get the names of the components
  std::vector<std::string> componentNames = getProperty("ComponentNames");
  Mantid::API::MatrixWorkspace_sptr inputWorkspace =
      getProperty("InputWorkspace");

  // Get all detectors
  std::vector<Mantid::Geometry::IDetector_const_sptr> detectors;
  getDetectors(inputWorkspace, componentNames, detectors);

  // Get the detector IDs from the Detectors
  std::vector<detid_t> detectorIDs(detectors.size());
  getDetectorIDs(detectors, detectorIDs);

  const bool orderByDetID = getProperty("OrderByDetId");
  if (orderByDetID) {
    std::sort(detectorIDs.begin(), detectorIDs.end());
  }

  // Run ExtractSpectra in order to obtain the cropped workspace
  auto extract_alg = Mantid::API::AlgorithmManager::Instance().createUnmanaged(
      "ExtractSpectra");
  extract_alg->setChild(true);
  extract_alg->initialize();
  extract_alg->setProperty("InputWorkspace", inputWorkspace);
  extract_alg->setProperty("OutputWorkspace", "dummy");
  extract_alg->setProperty("DetectorList", detectorIDs);
  extract_alg->execute();
  Mantid::API::MatrixWorkspace_sptr outputWorkspace =
      extract_alg->getProperty("OutputWorkspace");

  // Set the output
  setProperty("OutputWorkspace", outputWorkspace);
}

std::map<std::string, std::string> CropToComponent::validateInputs() {
  std::map<std::string, std::string> result;
  Mantid::API::MatrixWorkspace_sptr inputWorkspace =
      getProperty("InputWorkspace");
  std::vector<std::string> componentNames = getProperty("ComponentNames");

  // Make sure that the component exists on the input workspace
  auto instrument = inputWorkspace->getInstrument();
  for (auto &componentName : componentNames) {
    auto detector = instrument->getComponentByName(componentName);
    if (!detector) {
      std::string message =
          "The component name " + componentName +
          " does not exist on the workspace. Specify a valid component.";
      result["ComponentNames"] = message;
      break;
    }
  }
  return result;
}

} // namespace Algorithms
} // namespace Mantid
