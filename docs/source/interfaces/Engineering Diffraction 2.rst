.. _Engineering_Diffraction_2-ref:

Engineering Diffraction 2
=========================

.. contents:: Table of Contents
    :local:

Interface Overview
------------------

This custom interface will integrate several tasks related to engineering
diffraction. In its current state it provides functionality for creating
and loading calibration files and focusing ENGINX run files.

Functionality for performing single peak fitting on focused run files is currently in progress.

This interface is under active development.

General Options
^^^^^^^^^^^^^^^
RB Number
    The reference number for the output paths (usually an experiment reference
    number at ISIS). Leaving this field empty will result in no user directories
    being created, and only the general directory will be used for file storage.

Instrument
    Select the instrument (ENGINX or IMAT). Currently only ENGINX is fully
    supported.

?
    Show this documentation page.

Settings
    Provides a range of options that apply across the entire interface, currently
    providing the option to change the default output directory and force the
    recalculation of the vanadium correction files.

Close
    Close the interface.

Red Stars
    Red stars next to browse boxes and other fields indicate that the file
    could not be found. Hover over the star to see more information.

Calibration
-----------

This tab currently provides a graphical interface to create new calibrations, using the
:ref:`EnggCalibrate<algm-EnggCalibrate>` algorithm, and visualise them.
It also allows for the loading of GSAS parameter files created by the calibration process
to load a previously created calibration into the interface.

When loading an existing calibration, the fields for creating a new calibration will be
automatically filled, allowing the recreation of the workspaces and plots generated by
creating a new calibration.

The "Plot Output" check-box will plot vanadium curves and ceria peaks for new calibrations.
Four plots will be generated (for ENGINX), one of each plot for each of the detector banks.

Creating a new calibration file generates 3 GSAS instrument parameter files,
one covering all banks and separate ones for each individual bank. All 3 files are written
to the same directory:

`<CHOSEN_OUTPUT_DIRECTORY>/Calibration/`

If an RB number has been specified the files will also be saved to a user directory
in the base directory:

`<CHOSEN_OUTPUT_DIRECTORY>/User/<RB_NUMBER>/Calibration/`

Cropping
^^^^^^^^

The interface also provides the ability to restrict a new calibration to one of the two banks
or to a custom list of spectra.

Parameters
^^^^^^^^^^

Vanadium Number
    The run number or file path used to correct the calibration and experiment runs.

Calibration Sample Number
    The run number for the calibration sample run (such as ceria) used to calibrate
    experiment runs.

Path
    The path to the GSAS parameter file to be loaded.

Bank/Spectra
    Select a bank to crop to or specify a custom spectra will be entered.

Custom Spectra
    A comma separated list of spectra to restrict the calibration to. Can be provided as single spectrum numbers
    or ranges using hyphens (e.g. 14-150, 405, 500-600).

Focus
-----

This tab allows for the focusing of data files by making use of the :ref:`EnggFocus<algm-EnggFocus>` algorithm.

Files can be selected by providing run numbers or selecting the files manually using the browse button.

In order to use the tab, a new or existing calibration must be created or loaded (see above).

The interface allows for two kinds of focusing:

- **Normal Focusing:**
    Run numbers can be entered and both banks will be focused for each workspace.
    The output workspaces will have a prefix for the run they are for and a suffix denoting which bank they are for.

- **Cropped Focusing:**
    The entered workspace can be cropped to one of the two banks or to a user defined set of spectra.
    Workspaces cropped using custom spectra lists will have the suffix "cropped".

Ticking the "Plot Focused Workspace" checkbox will create a plot of the focused workspace when the algorithm is
complete. The number of plots that are generated is dependent on the type of focusing done. Normal focusing generates
a plot for each bank and cropped focusing generates a plot for the single bank or one for the chosen spectra.

Clicking the focus button will begin the focusing algorithm for the selected run files. The button and plotting checkbox
will be disabled until the fitting algorithm is complete.

The focused output files are saved in NeXus, GSS, and raw XYE format to:

`<CHOSEN_OUTPUT_DIRECTORY>/Focus/`

If an RB number has been specified the files will also be saved to a user directory:

`<CHOSEN_OUTPUT_DIRECTORY>/User/<RB_NUMBER>/Focus/`

Parameters
^^^^^^^^^^

Sample Run Number
    The run numbers of or file paths to the data files to be focused.
    
Bank/Spectra
    Select which bank to restrict the focusing to or allow for the entry of custom spectra. 

Custom Spectra
    A comma separated list of spectra to restrict the calibration to. Can be provided as single spectrum numbers
    or ranges using hyphens (e.g. 14-150, 405, 500-600).

Fitting
-------

**This tab is currently a work in progress!**

This tab will allow for single peak fitting of focused run files.

Focused run files can be loaded from the file system into mantid from the interface, which will keep track of all the
workspaces that it has created from these files.

The plan for the rest of the functionality is to allow for loaded workspaces to be plotted in the interface. Peaks
could then be selected by clicking on the plot or by using a text field to enter peak centres in d-spacing.
Once the peaks have been selected, they would be fitted using the :ref:`Pseudo-Voigt <func-PseudoVoigt>` and
:ref:`BackToBackExponential <func-BackToBackExponential>` fit functions.

The output from the fitting functions will be stored in a multidimensional file format, along with the sample logs for
the runs that have been fitted.

Parameters
^^^^^^^^^^

Focused Run Files
    A comma separated list of files to load. Selecting files from the file system using the browse button will do this
    for you.

Peak Positions
    A comma separated list of peak positions to be used when performing the fit.
