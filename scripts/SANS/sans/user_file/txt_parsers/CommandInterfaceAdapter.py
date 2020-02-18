# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2020 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
from sans.user_file.txt_parsers.ParsedDictConverter import ParsedDictConverter


class CommandInterfaceAdapter(ParsedDictConverter):
    def __init__(self, instrument, processed_state):
        super(CommandInterfaceAdapter, self).__init__(instrument=instrument)
        self._processed_state = processed_state

    def _get_input_dict(self):
        return self._processed_state
