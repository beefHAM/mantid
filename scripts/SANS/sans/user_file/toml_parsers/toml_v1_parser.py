# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2020 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +

from sans.common.enums import SANSInstrument, ReductionMode, DetectorType, RangeStepType
from sans.state.IStateParser import IStateParser
from sans.state.StateObjects.StateCalculateTransmission import get_calculate_transmission
from sans.state.StateObjects.StateConvertToQ import StateConvertToQ
from sans.state.StateObjects.StateMaskDetectors import get_mask_builder
from sans.state.StateObjects.StateMoveDetectors import get_move_builder
from sans.state.StateObjects.StateReductionMode import StateReductionMode
from sans.state.StateObjects.StateScale import StateScale
from sans.state.StateObjects.StateWavelength import StateWavelength
from sans.user_file.toml_parsers.toml_v1_schema import TomlSchemaV1Validator


class TomlV1Parser(IStateParser):
    def __init__(self, dict_to_parse, data_info, schema_validator=None):
        self._validator = schema_validator if schema_validator else TomlSchemaV1Validator(dict_to_parse)
        self._validator.validate()

        self._implementation = self._get_impl(dict_to_parse, data_info)
        self._implementation.parse_all()

    @staticmethod
    def _get_impl(*args):
        # Wrapper which can replaced with a mock
        return _TomlV1ParserImpl(*args)

    def get_state_adjustment(self):
        pass

    def get_state_calculate_transmission(self):
        return self._implementation.calculate_transmission

    def get_state_compatibility(self):
        pass

    def get_state_convert_to_q(self):
        return self._implementation.convert_to_q

    def get_state_data(self):
        return self._implementation.data_info

    def get_state_mask_detectors(self):
        pass

    def get_state_move_detectors(self):
        return self._implementation.move

    def get_state_normalize_to_monitor(self):
        pass

    def get_state_reduction_mode(self):
        return self._implementation.reduction_mode

    def get_state_save(self):
        pass

    def get_state_scale(self):
        return self._implementation.scale

    def get_state_slice_event(self):
        pass

    def get_state_wavelength(self):
        return self._implementation.wavelength

    def get_state_wavelength_and_pixel_adjustment(self):
        pass


class _TomlV1ParserImpl(object):
    # TODO this should not be in the TOML parser so we should unpick it at a later stage
    def __init__(self, input_dict, data_info):
        self._input = input_dict
        self.data_info = data_info
        self._create_state_objs()

    def parse_all(self):
        self._parse_instrument_configuration()
        self._parse_detector_configuration()
        self._parsed_binning()

    def _create_state_objs(self):
        self.convert_to_q = StateConvertToQ()
        self.calculate_transmission = get_calculate_transmission(instrument=self.instrument)
        self.move = get_move_builder(data_info=self.data_info).build()
        self.reduction_mode = StateReductionMode()
        self.scale = StateScale()
        self.wavelength = StateWavelength()

    def _parse_instrument_configuration(self):
        inst_config_dict = self._get_val(["instrument", "configuration"])

        self.calculate_transmission.incident_monitor = self._get_val("norm_monitor", inst_config_dict)
        self.calculate_transmission.transmission_monitor = self._get_val("trans_monitor", inst_config_dict)

        self.convert_to_q.q_resolution_collimation_length = self._get_val("collimation_length", inst_config_dict)
        self.convert_to_q.gravity_extra_length = self._get_val("gravity_extra_length", inst_config_dict)
        self.convert_to_q.q_resolution_a2 = self._get_val("sample_aperture_diameter", inst_config_dict)

        self.move.sample_offset = self._get_val("sample_offset", inst_config_dict)

    def _parse_detector_configuration(self):
        det_config_dict = self._get_val(["detector", "configuration"])

        self.scale.scale = self._get_val("rear_scale", det_config_dict)

        reduction_mode_key = self._get_val(["detector", "configuration", "selected_detector"])
        reduction_mode = ReductionMode(reduction_mode_key) if reduction_mode_key else ReductionMode.NOT_SET
        self.reduction_mode.reduction_mode = reduction_mode

        def update_translations(det_type, values: dict):
            if values:
                self.move.detectors[det_type.value].x_translation_correction = values["x"]
                self.move.detectors[det_type.value].y_translation_correction = values["y"]
                self.move.detectors[det_type.value].z_translation_correction = values["z"]

        update_translations(DetectorType.HAB, self._get_val("front_centre", det_config_dict))
        update_translations(DetectorType.LAB, self._get_val("rear_centre", det_config_dict))

    def _parsed_binning(self):
        binning_dict = self._get_val(["binning"])

        self.wavelength.wavelength_low = self._get_val(["wavelength", "start"], binning_dict)
        self.wavelength.wavelength_step = self._get_val(["wavelength", "step"], binning_dict)
        self.wavelength.wavelength_high = self._get_val(["wavelength", "stop"], binning_dict)
        step_type = RangeStepType(self._get_val(["wavelength", "type"], binning_dict))
        self.wavelength.wavelength_step_type = step_type


    @property
    def instrument(self):
        # Use impl so that it throws as mandatory key we later rely on
        try:
            instrument = self._get_val_impl(["instrument", "name"], dict_to_parse=self._input)
        except KeyError:
            raise RuntimeError("instrument.name is missing")
        return SANSInstrument(instrument)

    def _get_val(self, keys, dict_to_parse=None):
        """
        Gets a nested value within the specified dictionary
        :param keys: A list of keys to iterate through the dictionary
        :param dict_to_parse: (Optional) The dict to parse, if None parses the input dict
        :return: The corresponding value
        """
        if isinstance(keys, str):
            keys = [keys]

        try:
            return self._get_val_impl(keys=keys, dict_to_parse=dict_to_parse)
        except KeyError:
            return None

    def _get_val_impl(self, keys, dict_to_parse):
        if dict_to_parse is None:
            dict_to_parse = self._input

        assert isinstance(dict_to_parse, dict)

        val = dict_to_parse[keys[0]]
        if isinstance(val, dict) and len(keys) > 1:
            return self._get_val_impl(keys=keys[1:], dict_to_parse=val)
        return val
