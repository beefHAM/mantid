// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2019 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +

/*
 * NexusGeometrySave::saveInstrument :
 * Save methods to save geometry and metadata from memory
 * to disk in Nexus file format for Instrument 2.0.
 *
 * @author Takudzwa Makoni, RAL (UKRI), ISIS
 * @date 07/08/2019
 */

#ifndef MANTID_NEXUSGEOMETRY_NEXUSGEOMETRYSAVE_H_
#define MANTID_NEXUSGEOMETRY_NEXUSGEOMETRYSAVE_H_

#include "MantidAPI/MatrixWorkspace.h"
#include "MantidNexusGeometry/AbstractLogger.h"
#include "MantidNexusGeometry/DllConfig.h"
#include <memory>
#include <string>
#include <vector>

namespace Mantid {

namespace Kernel {

class ProgressBase;
class V3D;
class Quat;

} // namespace Kernel

namespace Geometry {
class ComponentInfo;
class DetectorInfo;
} // namespace Geometry
namespace API {
class MatrixWorkspace;
}

namespace NexusGeometry {
namespace NexusGeometrySave {

/// Container defining mappings needed for writing spectra information to
/// NXDetectors
struct MANTID_NEXUSGEOMETRY_DLL SpectraMappings {
  std::vector<int32_t> detector_index;
  std::vector<int32_t> detector_count;
  std::vector<int32_t> detector_list;
  int number_spec = 0;
  size_t number_dets = 0;
};

MANTID_NEXUSGEOMETRY_DLL void
saveInstrument(const Geometry::ComponentInfo &compInfo,
               const Geometry::DetectorInfo &detInfo,
               const std::string &fullPath, const std::string &rootName,
               AbstractLogger &logger, bool append = false,
               Kernel::ProgressBase *reporter = nullptr);

MANTID_NEXUSGEOMETRY_DLL void
saveInstrument(const Mantid::API::MatrixWorkspace &ws,
               const std::string &fullPath, const std::string &rootName,
               AbstractLogger &logger, bool append = false,
               Kernel::ProgressBase *reporter = nullptr);

MANTID_NEXUSGEOMETRY_DLL void saveInstrument(
    const std::pair<std::unique_ptr<Geometry::ComponentInfo>,
                    std::unique_ptr<Geometry::DetectorInfo>> &instrPair,
    const std::string &fullPath, const std::string &rootName,
    AbstractLogger &logger, bool append = false,
    Kernel::ProgressBase *reporter = nullptr);
} // namespace NexusGeometrySave
} // namespace NexusGeometry
} // namespace Mantid

#endif /* MANTID_NEXUSGEOMETRY_NEXUSGEOMETRYSAVE_H_ */
