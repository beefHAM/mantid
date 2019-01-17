// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTID_CUSTOMINTERFACES_SUMMATIONTYPE_H_
#define MANTID_CUSTOMINTERFACES_SUMMATIONTYPE_H_
#include <boost/optional.hpp>
#include <stdexcept>
#include <string>

namespace MantidQt {
namespace CustomInterfaces {

enum class SummationType { SumInLambda, SumInQ };

inline SummationType summationTypeFromString(std::string const &summationType) {
  if (summationType == "SumInLambda")
    return SummationType::SumInLambda;
  else if (summationType == "SumInQ")
    return SummationType::SumInQ;
  else
    throw std::runtime_error("Unexpected summation type.");
}
} // namespace CustomInterfaces
} // namespace MantidQt
#endif // MANTID_CUSTOMINTERFACES_SUMMATIONTYPE_H_