# Create egg link to binary output directory
if(WIN32)
  # handled separately below
  set(_exclude_on_install "resources\.pyc?")
else()
  set(_exclude_on_install "")
endif()
add_python_package(workbench
  EXECUTABLE
  EXCLUDE_FROM_INSTALL ${_exclude_on_install}
  INSTALL_LIB_DIRS "${WORKBENCH_LIB_DIR}"
  INSTALL_BIN_DIR "${WORKBENCH_BIN_DIR}"
)

set(_images_qrc_file ${CMAKE_CURRENT_LIST_DIR}/resources.qrc)

if (WIN32)

  function (CREATE_QRC_FILE_TEMPLATE BUILD_TYPE)
    # Set path and file content variables
    set(_paths_qrc_file ${CMAKE_CURRENT_BINARY_DIR}/resources.qrc.${BUILD_TYPE})
    set(_qt_conf_file ${CMAKE_CURRENT_BINARY_DIR}/qt.conf.${BUILD_TYPE})
    set(_qrc_file_contents "<!DOCTYPE RCC><RCC version=\"1.0\">\n<qresource prefix=\"/qt/etc\">\n<file alias=\"qt.conf\">qt.conf.${BUILD_TYPE}</file>\n</qresource>\n</RCC>\n")

    # Write qt.conf.BUILD_TYPE file and get resources file name
    if (${BUILD_TYPE} STREQUAL "build")
      file (WRITE ${_qt_conf_file} "[Paths]\nPrefix = ${THIRD_PARTY_DIR}/lib/qt5\n\n[Platforms]\nWindowsArguments = dpiawareness=1\n")
      set(_output_res_py ${CMAKE_CURRENT_LIST_DIR}/workbench/app/resources.py)
    elseif(${BUILD_TYPE} STREQUAL "install")
      file (WRITE ${_qt_conf_file} "[Paths]\nPrefix = ../lib/qt5\n\n[Platforms]\nWindowsArguments = dpiawareness=1\n")
      file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/qt.conf.webengine "[Paths]\nPrefix = ../\n")
      set(_output_res_py ${CMAKE_CURRENT_BINARY_DIR}/resources.py.install)
    endif()

    # Write resources.qrc.BUILD_TYPE
    file (WRITE ${_paths_qrc_file} ${_qrc_file_contents})

    # Add command to generate resources.py file
    add_custom_command(OUTPUT ${_output_res_py}
                      COMMAND ${PYRCC5_CMD} -o ${_output_res_py} ${_images_qrc_file} ${_paths_qrc_file}
                      COMMENT "Generating workbench resources module for ${BUILD_TYPE}: ${_output_res_py}"
                      DEPENDS ${_images_qrc_file} ${_paths_qrc_file})
  endfunction()

  # Run function for each build type
  foreach(BUILD_TYPE "build" "install")
    CREATE_QRC_FILE_TEMPLATE(${BUILD_TYPE})
  endforeach()
  add_custom_target(workbench_resources ALL
                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/resources.py.install ${CMAKE_CURRENT_LIST_DIR}/workbench/app/resources.py)
  # Install rule for package's resources.py
  install ( FILES ${CMAKE_CURRENT_BINARY_DIR}/resources.py.install
            DESTINATION bin/workbench/app RENAME resources.py)
else()
  set (_output_res_py ${CMAKE_CURRENT_LIST_DIR}/workbench/app/resources.py)
  add_custom_command(OUTPUT ${_output_res_py}
                     COMMAND ${PYRCC5_CMD} -o ${_output_res_py} ${_images_qrc_file}
                     COMMENT "Generating workbench resources module"
                     DEPENDS ${_images_qrc_file})
  add_custom_target(workbench_resources ALL DEPENDS ${_output_res_py})
endif()

# Dependency chain
add_dependencies(workbench workbench_resources mantidqt)

# ctest targets
set(TEST_FILES
    workbench/config/test/test_user.py
    workbench/test/test_import.py
    workbench/plotting/plotscriptgenerator/test/test_plotscriptgeneratoraxes.py
    workbench/plotting/plotscriptgenerator/test/test_plotscriptgeneratorfigure.py
    workbench/plotting/plotscriptgenerator/test/test_plotscriptgeneratorlines.py
    workbench/plotting/plotscriptgenerator/test/test_plotscriptgeneratorutils.py
    workbench/plotting/plotscriptgenerator/test/test_plotscriptgenerator.py
    workbench/plotting/test/test_figureerrorsmanager.py
    workbench/plotting/test/test_figureinteraction.py
    workbench/plotting/test/test_figuremanager.py
    workbench/plotting/test/test_figurewindow.py
    workbench/plotting/test/test_globalfiguremanager.py
    workbench/plugins/test/test_editor.py
    workbench/plugins/test/test_exception_handler.py
    workbench/plugins/test/test_jupyterconsole.py
    workbench/plugins/test/test_workspacewidget.py
    workbench/utils/test/test_workspacehistorygeneration.py
    workbench/widgets/plotselector/test/test_plotselector_model.py
    workbench/widgets/plotselector/test/test_plotselector_presenter.py
    workbench/widgets/plotselector/test/test_plotselector_view.py
    workbench/widgets/settings/test/test_settings_presenter.py
    workbench/widgets/settings/test/test_settings_view.py
    workbench/widgets/settings/general/test/test_general_settings.py
    workbench/projectrecovery/test/test_projectrecovery.py
    workbench/projectrecovery/test/test_projectrecoveryloader.py
    workbench/projectrecovery/test/test_projectrecoverysaver.py
    workbench/projectrecovery/recoverygui/test/test_projectrecoverymodel.py
    workbench/projectrecovery/recoverygui/test/test_projectrecoverypresenter.py
    workbench/projectrecovery/recoverygui/test/test_projectrecoverywidgetview.py
    workbench/projectrecovery/recoverygui/test/test_recoveryfailureview.py)

set(PYUNITTEST_RUN_SERIAL ON)
set(PYUNITTEST_QT_API pyqt5)
pyunittest_add_test (${CMAKE_CURRENT_SOURCE_DIR}
  workbench ${TEST_FILES}
)

# Install MantidWorkbench for OSX
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set ( MAC_BUNDLE_EXECUTABLE MantidWorkbench )
  set ( MAC_BUNDLE_IDENTIFIER "org.mantidproject.MantidWorkbench" )
  set ( MAC_BUNDLE_ICON MantidWorkbench.icns )
  set ( MAC_BUNDLE_NAME MantidWorkbench )
  configure_file ( ${CMAKE_CURRENT_SOURCE_DIR}/../../../installers/MacInstaller/Info.plist.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
                   @ONLY )

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/Info.plist DESTINATION MantidWorkbench.app/Contents/ )
install (CODE "
    execute_process(COMMAND ${CMAKE_SOURCE_DIR}/installers/MacInstaller/make_package.rb
                            \${CMAKE_INSTALL_PREFIX}/MantidWorkbench.app ${PYTHON_EXECUTABLE} ${ParaView_DIR}
                    RESULT_VARIABLE install_name_tool_result OUTPUT_VARIABLE _out ERROR_VARIABLE _out COMMAND_ECHO STDOUT)
    if(NOT install_name_tool_result EQUAL 0)
      message(\"\${_out}\")
      message(FATAL_ERROR \"Package script failed!!!\n\")
    endif()
")

endif()
