#ifndef MANTIDQT_MANTIDWIDGETS_LOGVALUESELECTOR_H_
#define MANTIDQT_MANTIDWIDGETS_LOGVALUESELECTOR_H_

#include "ui_LogValueSelector.h"
#include "WidgetDllOption.h"
#include "MantidQtAPI/MantidWidget.h"

namespace MantidQt {
namespace MantidWidgets {

/** LogValueSelector : Select a log name and mean/min/max/first/last

  Copyright &copy; 2016 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
  National Laboratory & European Spallation Source

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
class EXPORT_OPT_MANTIDQT_MANTIDWIDGETS LogValueSelector
    : public MantidQt::API::MantidWidget {
  Q_OBJECT
public:
  /// Constructor
  LogValueSelector(QWidget *parent);

  /// Get selected log text
  QString getLog() const;

  /// Get selected function text
  QString getFunction() const;

  /// Whether checkbox is shown or not
  bool isCheckboxShown() const;

  /// Control whether checkbox is shown
  void setCheckboxShown(bool visible);

  /// Get a pointer to log combo box
  QComboBox *getLogComboBox() const;

private slots:
  /// Set enabled/disabled
  void setEnabled(int checkstate);

private:
  /// Set up connections
  void doConnect();

  /// User interface
  Ui::LogValueSelector m_ui;
};

} // namespace MantidWidgets
} // namespace MantidQt

#endif /* MANTIDQT_MANTIDWIDGETS_LOGVALUESELECTOR_H_ */