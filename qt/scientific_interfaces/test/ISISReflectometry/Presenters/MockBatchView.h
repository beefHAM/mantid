#ifndef MANTID_CUSTOMINTERFACES_MOCKBATCHVIEW_H_
#define MANTID_CUSTOMINTERFACES_MOCKBATCHVIEW_H_
#include "MantidKernel/WarningSuppressions.h"
#include "MantidQtWidgets/Common/Batch/MockJobTreeView.h"
#include "Views/IBatchView.h"
#include <gmock/gmock.h>

GNU_DIAG_OFF_SUGGEST_OVERRIDE

namespace MantidQt {
namespace CustomInterfaces {

class MockBatchView : public IBatchView {
public:
  MOCK_METHOD1(subscribe, void(BatchViewSubscriber *));
  MOCK_METHOD1(setProgress, void(int));
  MOCK_METHOD0(jobs, MantidQt::MantidWidgets::Batch::IJobTreeView &());
  MOCK_METHOD0(resetFilterBox, void());

  MOCK_METHOD0(invalidSelectionForCopy, void());
  MOCK_METHOD0(invalidSelectionForPaste, void());
  MOCK_METHOD0(invalidSelectionForCut, void());

  MOCK_METHOD0(mustSelectRow, void());
  MOCK_METHOD0(mustSelectGroup, void());
  MOCK_METHOD0(mustNotSelectGroup, void());
  MOCK_METHOD0(mustSelectGroupOrRow, void());
};
} // namespace CustomInterfaces
} // namespace MantidQt
GNU_DIAG_ON_SUGGEST_OVERRIDE
#endif // MANTID_CUSTOMINTERFACES_MOCKBATCHVIEW_H_