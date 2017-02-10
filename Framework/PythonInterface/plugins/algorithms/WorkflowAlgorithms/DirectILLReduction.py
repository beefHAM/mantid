# -*- coding: utf-8 -*-

from __future__ import (absolute_import, division, print_function)

import DirectILLReduction_common as common
from mantid.api import (AlgorithmFactory, DataProcessorAlgorithm, InstrumentValidator, ITableWorkspaceProperty,
                        MatrixWorkspaceProperty, PropertyMode, WorkspaceProperty, WorkspaceUnitValidator)
from mantid.kernel import (CompositeValidator, Direction, FloatArrayProperty, FloatBoundedValidator, Property,
                           StringListValidator)
from mantid.simpleapi import (BinWidthAtX, ConvertToPointData, ConvertUnits, CorrectKiKf, DetectorEfficiencyCorUser,
                              Divide, GroupDetectors, MedianBinWidth, Rebin, SofQWNormalisedPolygon, Transpose)
import numpy
from scipy import constants


def _createDetectorGroups(ws):
    """Find detectors with (almost) same theta and group them. Masked
    detectors are ignored.
    """
    numHistograms = ws.getNumberHistograms()
    assignedDets = list()
    groups = list()
    for i in range(numHistograms):
        if i in assignedDets:
            continue
        det1 = ws.getDetector(i)
        if det1.isMasked():
            continue
        currentGroup = [det1.getID()]
        twoTheta1 = ws.detectorTwoTheta(det1)
        for j in range(i + 1, numHistograms):
            if j in assignedDets:
                continue
            det2 = ws.getDetector(j)
            if det2.isMasked():
                continue
            twoTheta2 = ws.detectorTwoTheta(det2)
            if abs(twoTheta1 - twoTheta2) < 0.01 / 180.0 * constants.pi:
                currentGroup.append(det2.getID())
                assignedDets.append(j)
        groups.append(currentGroup)
    return groups


def _deltaQ(ws):
    """Estimate a q bin width for a S(theta, w) workspace."""
    deltaTheta = _medianDeltaTheta(ws)
    wavelength = ws.run().getProperty('wavelength').value
    return 2.0 * constants.pi / wavelength * deltaTheta


def _groupsToGroupingPattern(groups):
    """Return a grouping pattern suitable for the GroupDetectors algorithm."""
    pattern = ''
    for group in groups:
        for index in group:
            pattern += str(index) + '+'
        pattern = pattern[:-1] + ','
    return pattern[:-1]


def _medianDeltaTheta(ws):
    """Calculate the median theta spacing for a S(theta, w) workspace."""
    thetas = list()
    spectrumInfo = ws.spectrumInfo()
    for i in range(ws.getNumberHistograms()):
        if (not spectrumInfo.isMasked(i) and spectrumInfo.hasDetectors(i) and
                not spectrumInfo.isMonitor(i)):
            det = ws.getDetector(i)
            twoTheta = ws.detectorTwoTheta(det)
            thetas.append(twoTheta)
    if not thetas:
        raise RuntimeError('No usable detectors for median DTheta ' +
                           'calculation.')
    dThetas = numpy.diff(thetas)
    return numpy.median(dThetas[dThetas > numpy.radians(0.1)])


def _minMaxQ(ws):
    """Estimate the start and end q bins for a S(theta, w) workspace."""
    Ei = ws.run().getProperty('Ei').value * 1e-3 * constants.e  # in Joules
    xs = ws.readX(0)
    minW = xs[0] * 1e-3 * constants.e  # in Joules
    maxEf = Ei - minW
    # In Ånströms
    maxQ = numpy.sqrt(2.0 * constants.m_n / constants.hbar**2 *
                     (Ei + maxEf - 2 * numpy.sqrt(Ei * maxEf) * -1.0)) * 1e-10
    minQ = 0.0
    return (minQ, maxQ)


def _rebin(ws, params, wsNames, algorithmLogging):
    """Rebin a workspace."""
    rebinnedWSName = wsNames.withSuffix('rebinned')
    rebinnedWS = Rebin(InputWorkspace=ws,
                       OutputWorkspace=rebinnedWSName,
                       Params=params,
                       EnableLogging=algorithmLogging)
    return rebinnedWS


class DirectILLReduction(DataProcessorAlgorithm):
    """A data reduction workflow algorithm for the direct geometry TOF spectrometers at ILL."""

    def __init__(self):
        """Initialize an instance of the algorithm."""
        DataProcessorAlgorithm.__init__(self)

    def category(self):
        """Return the algorithm's category."""
        return 'Workflow\\Inelastic'

    def name(self):
        """Return the algorithm's name."""
        return 'DirectILLReduction'

    def summary(self):
        """Return a summary of the algorithm."""
        return 'Data reduction workflow for the direct geometry time-of-flight spectrometers at ILL.'

    def version(self):
        """Return the algorithm's version."""
        return 1

    def PyExec(self):
        """Executes the data reduction workflow."""
        report = common.Report()
        subalgLogging = False
        if self.getProperty(common.PROP_SUBALG_LOGGING).value == common.SUBALG_LOGGING_ON:
            subalgLogging = True
        wsNamePrefix = self.getProperty(common.PROP_OUTPUT_WS).valueAsStr
        cleanupMode = self.getProperty(common.PROP_CLEANUP_MODE).value
        wsNames = common.NameSource(wsNamePrefix, cleanupMode)
        wsCleanup = common.IntermediateWSCleanup(cleanupMode, subalgLogging)

        # Get input workspace.
        mainWS = self._inputWS(wsNames, wsCleanup, subalgLogging)

        # Vanadium normalization.
        # TODO Absolute normalization.
        mainWS = self._normalizeToVana(mainWS, wsNames, wsCleanup,
                                       subalgLogging)

        # Convert units from TOF to energy.
        mainWS = self._convertTOFToDeltaE(mainWS, wsNames, wsCleanup,
                                          subalgLogging)

        # KiKf conversion.
        mainWS = self._correctByKiKf(mainWS, wsNames,
                                     wsCleanup, subalgLogging)

        # Detector efficiency correction.
        mainWS = self._correctByDetectorEfficiency(mainWS, wsNames,
                                                   wsCleanup, subalgLogging)

        mainWS = self._groupDetectors(mainWS, wsNames, wsCleanup,
                                      subalgLogging)

        # Rebinning.
        mainWS = self._rebinInW(mainWS, wsNames, wsCleanup, report,
                                subalgLogging)

        mainWS = self._convertVerticalAxisAxis(mainWS, wsNames, wsCleanup, subalgLogging)

        self._finalize(mainWS, wsCleanup, report)

    def PyInit(self):
        """Initialize the algorithm's input and output properties."""
        inputWorkspaceValidator = CompositeValidator()
        inputWorkspaceValidator.add(InstrumentValidator())
        inputWorkspaceValidator.add(WorkspaceUnitValidator('TOF'))
        positiveFloat = FloatBoundedValidator(lower=0)

        # Properties.
        self.declareProperty(MatrixWorkspaceProperty(
            name=common.PROP_INPUT_WS,
            defaultValue='',
            validator=inputWorkspaceValidator,
            optional=PropertyMode.Optional,
            direction=Direction.Input),
            doc='Input workspace.')
        self.declareProperty(WorkspaceProperty(name=common.PROP_OUTPUT_WS,
                                               defaultValue='',
                                               direction=Direction.Output),
                             doc='The output of the algorithm.')
        self.declareProperty(name=common.PROP_CLEANUP_MODE,
                             defaultValue=common.CLEANUP_ON,
                             validator=StringListValidator([
                                 common.CLEANUP_ON,
                                 common.CLEANUP_OFF]),
                             direction=Direction.Input,
                             doc='What to do with intermediate workspaces.')
        self.declareProperty(name=common.PROP_SUBALG_LOGGING,
                             defaultValue=common.SUBALG_LOGGING_OFF,
                             validator=StringListValidator([
                                 common.SUBALG_LOGGING_OFF,
                                 common.SUBALG_LOGGING_ON]),
                             direction=Direction.Input,
                             doc='Enable or disable subalgorithms to ' +
                                 'print in the logs.')
        self.declareProperty(MatrixWorkspaceProperty(
            name=common.PROP_VANA_WS,
            defaultValue='',
            validator=inputWorkspaceValidator,
            direction=Direction.Input,
            optional=PropertyMode.Optional),
            doc='Reduced vanadium workspace.')
        self.declareProperty(MatrixWorkspaceProperty(
            name=common.PROP_DIAGNOSTICS_WS,
            defaultValue='',
            direction=Direction.Input,
            optional=PropertyMode.Mandatory),
            doc='Detector diagnostics workspace obtained from another ' +
                'reduction run.')
        self.declareProperty(name=common.PROP_REBINNING_MODE_W,
                             defaultValue=common.REBIN_AUTO_ELASTIC_PEAK,
                             validator=StringListValidator([
                                 common.REBIN_AUTO_ELASTIC_PEAK,
                                 common.REBIN_AUTO_MEDIAN_BIN_WIDTH,
                                 common.REBIN_MANUAL_W,
                                 common.REBIN_W_OFF]),
                             direction=Direction.Input,
                             doc='Energy rebinnin mode.')
        self.setPropertyGroup(common.PROP_REBINNING_MODE_W, common.PROPGROUP_REBINNING)
        self.declareProperty(FloatArrayProperty(name=common.PROP_REBINNING_PARAMS_W),
                             doc='Manual energy rebinning parameters.')
        self.setPropertyGroup(common.PROP_REBINNING_PARAMS_W, common.PROPGROUP_REBINNING)
        self.declareProperty(name=common.PROP_VERTICAL_AXIS_UNITS,
                             defaultValue=common.UNITS_SPECTRUM_NUMBER,
                             validator=StringListValidator([
                                 common.UNITS_SPECTRUM_NUMBER,
                                 common.UNITS_SCATTERING_ANGLE]),
                             direction=Direction.Input,
                             doc='Unit for the horizontal axis for ' + common.PROP_OUTPUT_WS)

    def validateInputs(self):
        """Check for issues with user input."""
        # TODO
        return dict()

    def _applyDiagnostics(self, mainWS, wsNames, wsCleanup, subalgLogging):
        """Mask workspace according to diagnostics."""
        diagnosticsWS = self.getProperty(common.PROP_DIAGNOSTICS_WS).value
        if not diagnosticsWS:
            return mainWS
        maskedWSName = wsNames.withSuffix('diagnostics_applied')
        maskedWS = CloneWorkspace(InputWorkspace=mainWS,
                                  OutputWorkspace=maskedWSName,
                                  EnableLogging=subalgLogging)
        wsCleanup.cleanup(mainWS)
        diagnosticsWS = self.getProperty(common.PROP_DIAGNOSTICS_WS).value
        MaskDetectors(Workspace=maskedWS,
                      MaskedWorkspace=diagnosticsWS,
                      EnableLogging=subalgLogging)
        wsCleanup.cleanup(mainWS)
        return maskedWS

    def _convertTOFToDeltaE(self, mainWS, wsNames, wsCleanup, subalgLogging):
        """Convert the X axis units from time-of-flight to energy transfer."""
        energyConvertedWSName = wsNames.withSuffix('energy_converted')
        energyConvertedWS = ConvertUnits(InputWorkspace=mainWS,
                                         OutputWorkspace=energyConvertedWSName,
                                         Target='DeltaE',
                                         EMode='Direct',
                                         EnableLogging=subalgLogging)
        wsCleanup.cleanup(mainWS)
        return energyConvertedWS

    def _convertVerticalAxis(self, mainWS, wsNames, wsCleanup, subalgLogging):
        """If requested, convert the spectrum axis to theta."""
        verticalUnit = self.getProperty(common.PROP_VERTICAL_AXIS_UNITS).value
        if verticalUnit == common.UNITS_SPECTRUM_NUMBER:
            return mainWS
        thetaWSName = wsNames.withSuffix('in_scattering_angles')
        thetaWS = ConvertSpectrumAxis(InputWorkspace=mainWS,
                                      OutputWorkspace=thetaWSName,
                                      Target='Theta',
                                      EMode='Direct',
                                      EnableLogging=subalgLogging)
        wsCleanup.cleanup(mainWS)
        return thetaWS

    def _correctByDetectorEfficiency(self, mainWS, wsNames, wsCleanup,
                                     subalgLogging):
        """Apply detector efficiency corrections."""
        correctedWSName = wsNames.withSuffix('detector_efficiency_corrected')
        correctedWS = \
            DetectorEfficiencyCorUser(InputWorkspace=mainWS,
                                      OutputWorkspace=correctedWSName,
                                      EnableLogging=subalgLogging)
        wsCleanup.cleanup(mainWS)
        return correctedWS

    def _correctByKiKf(self, mainWS, wsNames, wsCleanup, subalgLogging):
        """Apply the k_i / k_f correction."""
        correctedWSName = wsNames.withSuffix('kikf')
        correctedWS = CorrectKiKf(InputWorkspace=mainWS,
                                  OutputWorkspace=correctedWSName,
                                  EnableLogging=subalgLogging)
        wsCleanup.cleanup(mainWS)
        return correctedWS

    def _finalize(self, outWS, wsCleanup, report):
        """Do final cleanup and set the output property."""
        self.setProperty(common.PROP_OUTPUT_WS, outWS)
        wsCleanup.finalCleanup()
        report.toLog(self.log())

    def _groupDetectors(self, mainWS, wsNames, wsCleanup, subalgLogging):
        """Group detectors with similar thetas."""
        groups = _createDetectorGroups(mainWS)
        groupingPattern = _groupsToGroupingPattern(groups)
        groupedWSName = wsNames.withSuffix('grouped_detectors')
        groupedWS = GroupDetectors(InputWorkspace=mainWS,
                                   OutputWorkspace=groupedWSName,
                                   GroupingPattern=groupingPattern,
                                   KeepUngroupedSpectra=False,
                                   Behaviour='Average',
                                   EnableLogging=subalgLogging)
        wsCleanup.cleanup(mainWS)
        return groupedWS

    def _inputWS(self, wsNames, wsCleanup, subalgLogging):
        """Return the raw input workspace."""
        mainWS = self.getProperty(common.PROP_INPUT_WS).value
        wsCleanup.protect(mainWS)
        return mainWS

    def _normalizeToVana(self, mainWS, wsNames, wsCleanup, subalgLogging):
        """Normalize to vanadium workspace."""
        vanaWS = self.getProperty(common.PROP_VANA_WS).value
        # TODO integrate vana, ComputeCalibrationCoefVan
        vanaNormalizedWSName = wsNames.withSuffix('vanadium_normalized')
        vanaNormalizedWS = Divide(LHSWorkspace=mainWS,
                                  RHSWorkspace=vanaWS,
                                  OutputWorkspace=vanaNormalizedWSName,
                                  EnableLogging=subalgLogging)
        wsCleanup.cleanup(mainWS)
        return vanaNormalizedWS

    def _rebinInW(self, mainWS, wsNames, wsCleanup, report,
                  subalgLogging):
        """Rebin the horizontal axis of a workspace."""
        mode = self.getProperty(common.PROP_REBINNING_MODE_W).value
        if mode == common.REBIN_AUTO_ELASTIC_PEAK:
            binWidth = BinWidthAtX(InputWorkspace=mainWS,
                                   X=0.0,
                                   Rounding='10^n',
                                   EnableLogging=subalgLogging)
            params = [binWidth]
            report.notice('Rebinned energy axis to bin width {}.'
                          .format(binWidth))
        elif mode == common.REBIN_AUTO_MEDIAN_BIN_WIDTH:
            binWidth = MedianBinWidth(InputWorkspace=mainWS,
                                      Rounding='10^n',
                                      EnableLogging=subalgLogging)
            params = [binWidth]
            report.notice('Rebinned energy axis to bin width {}.'
                          .format(binWidth))
        elif mode == common.REBIN_MANUAL_W:
            params = self.getProperty(common.PROP_REBINNING_PARAMS_W).value
        else:
            raise RuntimeError('Unknown ' + common.PROP_REBINNING_MODE_W)
        rebinnedWS = _rebin(mainWS, params, wsNames, subalgLogging)
        wsCleanup.cleanup(mainWS)
        return rebinnedWS


AlgorithmFactory.subscribe(DirectILLReduction)
