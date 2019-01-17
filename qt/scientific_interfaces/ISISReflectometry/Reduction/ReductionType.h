// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTID_CUSTOMINTERFACES_REDUCTIONTYPE_H_
#define MANTID_CUSTOMINTERFACES_REDUCTIONTYPE_H_
#include "../DllConfig.h"
#include <boost/optional.hpp>
namespace MantidQt {
namespace CustomInterfaces {
enum class ReductionType { DivergentBeam, NonFlatSample, Normal };

inline ReductionType reductionTypeFromString(std::string const &reductionType) {
  if (reductionType == "Normal")
    return ReductionType::Normal;
  else if (reductionType == "DivergentBeam")
    return ReductionType::DivergentBeam;
  else if (reductionType == "NonFlatSample")
    return ReductionType::NonFlatSample;
  else
    throw std::runtime_error("Unexpected reduction type.");
}
} // namespace CustomInterfaces
} // namespace MantidQt
#endif // MANTID_CUSTOMINTERFACES_REDUCTIONTYPE_H_