// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
// @author Ronald Fowler
#include "MantidDataHandling/SetScalingPSD.h"
#include "LoadRaw/isisraw.h"
#include "MantidAPI/Axis.h"
#include "MantidAPI/FileProperty.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidGeometry/Instrument/ComponentInfo.h"
#include "MantidGeometry/Instrument/DetectorInfo.h"
#include "MantidGeometry/Instrument/ParameterMap.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/BoundedValidator.h"
#include "MantidKernel/V3D.h"

#include <cmath>
#include <fstream>

namespace Mantid {
namespace DataHandling {

// Register the algorithm into the algorithm factory
DECLARE_ALGORITHM(SetScalingPSD)

using namespace Kernel;
using namespace API;
using namespace Geometry;

/// Empty default constructor
SetScalingPSD::SetScalingPSD() : Algorithm(), m_scalingOption(0) {}

/** Initialisation method.
 *
 */
void SetScalingPSD::init() {
  // Declare required input parameters for algorithm
  declareProperty(
      std::make_unique<FileProperty>(
          "ScalingFilename", "", FileProperty::Load,
          std::vector<std::string>{".sca", ".raw"}),
      "The name of the scaling calibrations file to read, including its\n"
      "full or relative path. The file extension must be either .sca or\n"
      ".raw (filenames are case sensitive on linux)");
  declareProperty(
      std::make_unique<WorkspaceProperty<>>("Workspace", "", Direction::InOut),
      "The name of the workspace to apply the scaling to. This must be\n"
      "associated with an instrument appropriate for the scaling file");

  auto mustBePositive = boost::make_shared<BoundedValidator<int>>();
  mustBePositive->setLower(0);
  declareProperty(
      "ScalingOption", 0, mustBePositive,
      "Control scaling calculation - 0 => use average of left and right\n"
      "scaling (default). 1 => use maximum scaling. 2 => maximum + 5%");
}

/** Executes the algorithm.
 *
 *  @throw runtime_error Thrown if algorithm cannot execute
 */
void SetScalingPSD::exec() {
  // Retrieve the filename and output workspace name from the properties
  m_filename = getPropertyValue("ScalingFilename");
  m_workspace = getProperty("Workspace");
  m_scalingOption = getProperty("ScalingOption");
  std::vector<Kernel::V3D> truepos;
  processScalingFile(m_filename, truepos);
  // calculateDetectorShifts(truepos);
}

/** Read the scaling information from a file (e.g. merlin_detector.sca) or from
 * the RAW file (.raw)
 *  @param scalingFile :: Name of scaling file .sca
 *  @param truepos :: V3D vector of actual positions as read from the file
 *  @return False if unable to open file, True otherwise
 */
bool SetScalingPSD::processScalingFile(const std::string &scalingFile,
                                       std::vector<Kernel::V3D> &truepos) {
  // Read the scaling information from a text file (.sca extension) or from a
  // raw file (.raw)
  // This is really corrected positions as (r,theta,phi) for each detector
  // Compare these with the instrument values to determine the change in
  // position and the scaling
  // which may be necessary for each pixel if in a tube.
  // movePos is used to updated positions
  std::map<int, Kernel::V3D> posMap;
  std::map<int, double> scaleMap;
  std::map<int, double>::iterator its;

  const auto &detectorInfo = m_workspace->detectorInfo();
  if (scalingFile.find(".sca") != std::string::npos ||
      scalingFile.find(".SCA") != std::string::npos) {
    // read a .sca text format file
    // format consists of a short header followed by one line per detector

    std::ifstream sFile(scalingFile.c_str());
    if (!sFile) {
      g_log.error() << "Unable to open scaling file " << scalingFile << '\n';
      return false;
    }
    std::string str;
    // skip header line should be <filename> generated by <prog>
    getline(sFile, str);
    int detectorCount;
    getline(sFile, str); // get detector count line
    std::istringstream detCountStream(str);
    detCountStream >> detectorCount;
    if (detectorCount < 1) {
      g_log.error("Bad detector count in scaling file");
      throw std::runtime_error("Bad detector count in scaling file");
    }
    truepos.reserve(detectorCount);
    getline(sFile, str); // skip title line
    int detIdLast = -10;
    Kernel::V3D truPosLast, detPosLast;

    Progress prog(this, 0.0, 0.5, detectorCount);
    // Now loop through lines, one for each detector/monitor. The latter are
    // ignored.

    while (getline(sFile, str)) {
      if (str.empty() || str[0] == '#')
        continue;
      std::istringstream istr(str);

      // read 6 values from the line to get the 3 (l2,theta,phi) of interest
      int detIndex, code;
      double l2, theta, phi, offset;
      istr >> detIndex >> offset >> l2 >> code >> theta >> phi;

      // sanity check on angles - l2 should be +ve but sample file has a few -ve
      // values
      // on monitors
      if (theta > 181.0 || theta < -1 || phi < -181 || phi > 181) {
        g_log.error("Position angle data out of range in .sca file");
        throw std::runtime_error(
            "Position angle data out of range in .sca file");
      }
      Kernel::V3D truPos;
      // use abs as correction file has -ve l2 for first few detectors
      truPos.spherical(fabs(l2), theta, phi);
      truepos.push_back(truPos);
      try {
        // detIndex is what Mantid usually calls detectorID
        size_t index = detectorInfo.indexOf(detIndex);
        Kernel::V3D detPos = detectorInfo.position(index);
        Kernel::V3D shift = truPos - detPos;

        // scaling applied to dets that are not monitors and have sequential IDs
        if (detIdLast == detIndex - 1 && !detectorInfo.isMonitor(index)) {
          Kernel::V3D diffI = detPos - detPosLast;
          Kernel::V3D diffT = truPos - truPosLast;
          double scale = diffT.norm() / diffI.norm();
          // Wish to store the scaling in a map, if we already have a scaling
          // for this detector (i.e. from the other side) we average the two
          // values. End of tube detectors only have one scaling estimate.
          scaleMap[detIndex] = scale;
          its = scaleMap.find(detIndex - 1);
          if (its == scaleMap.end())
            scaleMap[detIndex - 1] = scale;
          else
            its->second = 0.5 * (its->second + scale);
        }
        detIdLast = detIndex;
        detPosLast = detPos;
        truPosLast = truPos;
        posMap[detIndex] = shift;
        prog.report();
      } catch (std::out_of_range &) {
        continue;
      }
    }
  } else if (scalingFile.find(".raw") != std::string::npos ||
             scalingFile.find(".RAW") != std::string::npos) {
    std::vector<int> detID;
    std::vector<Kernel::V3D> truepos;
    getDetPositionsFromRaw(scalingFile, detID, truepos);
    //
    int detectorCount = static_cast<int>(detID.size());
    if (detectorCount < 1) {
      g_log.error("Failed to read any detectors from RAW file");
      throw std::runtime_error("Failed to read any detectors from RAW file");
    }
    int detIdLast = -10;
    Kernel::V3D truPosLast, detPosLast;
    Progress prog(this, 0.0, 0.5, detectorCount);
    for (int i = 0; i < detectorCount; i++) {
      int detIndex = detID[i];
      try {
        // detIndex is what Mantid usually calls detectorID
        size_t index = detectorInfo.indexOf(detIndex);
        Kernel::V3D detPos = detectorInfo.position(index);
        Kernel::V3D shift = truepos[i] - detPos;

        if (detIdLast == detIndex - 1 && !detectorInfo.isMonitor(index)) {
          Kernel::V3D diffI = detPos - detPosLast;
          Kernel::V3D diffT = truepos[i] - truPosLast;
          double scale = diffT.norm() / diffI.norm();
          scaleMap[detIndex] = scale;
          its = scaleMap.find(detIndex - 1);
          if (its == scaleMap.end()) {
            scaleMap[detIndex - 1] = scale;
          } else {
            if (m_scalingOption == 0)
              its->second = 0.5 * (its->second + scale); // average of two
            else if (m_scalingOption == 1) {
              if (its->second < scale)
                its->second = scale; // max
            } else if (m_scalingOption == 2) {
              if (its->second < scale)
                its->second = scale;
              its->second *= 1.05; // max+5%
            } else
              its->second = 3.0; // crazy test value
          }
        }
        detIdLast = detID[i];
        detPosLast = detPos;
        truPosLast = truepos[i];
        posMap[detIndex] = shift;
        prog.report();
      } catch (std::out_of_range &) {
        continue;
      }
    }
  }
  movePos(m_workspace, posMap, scaleMap);
  return true;
}

/**
 * Move all the detectors to their actual positions, as stored in posMap and set
 * their scaling as in scaleMap
 * @param WS :: pointer to the workspace
 * @param posMap :: A map of integer detector ID to position shift
 * @param scaleMap :: A map of integer detectorID to scaling (in Y)
 */
void SetScalingPSD::movePos(API::MatrixWorkspace_sptr &WS,
                            std::map<int, Kernel::V3D> &posMap,
                            std::map<int, double> &scaleMap) {

  auto &detectorInfo = WS->mutableDetectorInfo();
  auto &componentInfo = WS->mutableComponentInfo();

  double maxScale = -1e6, minScale = 1e6, aveScale = 0.0;
  int scaleCount = 0;
  Progress prog(this, 0.5, 1.0, static_cast<int>(detectorInfo.size()));

  for (size_t i = 0; i < detectorInfo.size(); ++i) {
    int idet = detectorInfo.detectorIDs()[i];

    // Check if we have a shift, else do nothing.
    auto itPos = posMap.find(idet);
    if (itPos == posMap.end())
      continue;

    // Do a relative move
    const auto newPosition = detectorInfo.position(i) + itPos->second;
    detectorInfo.setPosition(i, newPosition);

    // Set the "sca" instrument parameter
    auto itScale = scaleMap.find(idet);
    if (itScale != scaleMap.end()) {
      const double scale = itScale->second;
      if (minScale > scale)
        minScale = scale;
      if (maxScale < scale)
        maxScale = scale;
      aveScale += fabs(1.0 - scale);
      componentInfo.setScaleFactor(i, V3D(1.0, scale, 1.0));
      ++scaleCount;
    }
    prog.report();
  }
  g_log.debug() << "Range of scaling factors is " << minScale << " to "
                << maxScale << "\n";
  if (0 != scaleCount) {
    g_log.debug() << "Average abs scaling fraction is " << aveScale / scaleCount
                  << "\n";
  } else {
    g_log.debug() << "Average abs scaling fraction cannot ba calculated "
                     "because the scale count is 0! Its value before dividing "
                     "by the count is "
                  << aveScale << "\n";
  }
}

/** Read detector true positions from raw file
 * @param rawfile :: Name of raw file
 * @param detID :: Vector of detector numbers
 * @param pos :: V3D of detector positions corresponding to detID
 */
void SetScalingPSD::getDetPositionsFromRaw(std::string rawfile,
                                           std::vector<int> &detID,
                                           std::vector<Kernel::V3D> &pos) {
  (void)rawfile; // Avoid compiler warning

  // open raw file
  ISISRAW iraw(nullptr);
  if (iraw.readFromFile(m_filename.c_str(), false) != 0) {
    g_log.error("Unable to open file " + m_filename);
    throw Exception::FileError("Unable to open File:", m_filename);
  }
  // get detector information
  const int numDetector = iraw.i_det;    // number of detectors
  const int *const rawDetID = iraw.udet; // detector IDs
  const float *const r = iraw.len2;      // distance from sample
  const float *const angle = iraw.tthe;  // angle between indicent beam and
                                         // direction from sample to detector
                                         // (two-theta)
  const float *const phi = iraw.ut;
  // Is ut01 (=phi) present? Sometimes an array is present but has wrong values
  // e.g.all 1.0 or all 2.0
  bool phiPresent = iraw.i_use > 0 && phi[0] != 1.0 && phi[0] != 2.0;
  if (!phiPresent) {
    g_log.error("Unable to get Phi values from the raw file");
  }
  detID.reserve(numDetector);
  pos.reserve(numDetector);
  Kernel::V3D point;
  for (int i = 0; i < numDetector; ++i) {
    point.spherical(r[i], angle[i], phi[i]);
    pos.push_back(point);
    detID.push_back(rawDetID[i]);
  }
}

} // namespace DataHandling
} // namespace Mantid
