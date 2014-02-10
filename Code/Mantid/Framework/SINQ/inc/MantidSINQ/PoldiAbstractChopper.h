#ifndef POLDIABSTRACTCHOPPER_H
#define POLDIABSTRACTCHOPPER_H

#include "MantidSINQ/DllConfig.h"

#include "MantidDataObjects/TableWorkspace.h"

#include "MantidKernel/V2D.h"

#include <utility>

namespace Mantid
{
namespace Poldi
{

/** PoldiAbstractChopper :
 *
  Abstract representation of the POLDI chopper

    @author Michael Wedel, Paul Scherrer Institut - SINQ
    @date 10/02/2014

    Copyright © 2014 PSI-MSS

  This file is part of Mantid.

  Mantid is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  Mantid is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  File change history is stored at: <https://github.com/mantidproject/mantid>
  Code Documentation is available at: <http://doxygen.mantidproject.org>
*/

using namespace Kernel;
using namespace API;

class MANTID_SINQ_DLL PoldiAbstractChopper
{
public:
    virtual ~PoldiAbstractChopper() {}

    virtual void loadConfiguration(DataObjects::TableWorkspace_sptr chopperConfigurationWorkspace,
                                   DataObjects::TableWorkspace_sptr chopperSlitWorkspace,
                                   DataObjects::TableWorkspace_sptr chopperSpeedWorkspace) = 0;

    virtual void setRotationSpeed(double rotationSpeed) = 0;

    virtual std::vector<double> slitPositions() = 0;
    virtual std::vector<double> slitTimes() = 0;

    virtual double rotationSpeed() = 0;
    virtual double cycleTime() = 0;
    virtual double zeroOffset() = 0;

    virtual double distanceFromSample() = 0;

protected:
    PoldiAbstractChopper() {}

};
}
}

#endif // POLDIABSTRACTCHOPPER_H
