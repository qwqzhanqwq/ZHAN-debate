find_package(Qt6 6.4 REQUIRED COMPONENTS Gui Qml Quick)
qt_standard_project_setup()

qt_add_executable(zhan-debate WIN32 MACOSX_BUNDLE
    main.cpp
)

set(qml_files
    qml/Main.qml
    qml/Theme.qml
    qml/Header.qml
    qml/StagePane.qml
    qml/Clock.qml
    qml/ProgressRule.qml
    qml/FreeDebatePanel.qml
    qml/SidePanel.qml
    qml/Rundown.qml
    qml/ControlBar.qml
    qml/KeyButton.qml
    qml/SideBadge.qml
    qml/StateIndicator.qml
    qml/MicroLabel.qml
)

# QML 文件放在 qml/ 子目录，但在模块中位于根目录 // QML files live in qml/ but sit at the module root
foreach(file IN LISTS qml_files)
    get_filename_component(name "${file}" NAME)
    set_source_files_properties("${file}" PROPERTIES QT_RESOURCE_ALIAS "${name}")
endforeach()
set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(zhan-debate
    URI ZhanDebate
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    QML_FILES ${qml_files}
    SOURCES
        DebateController.cpp
        DebateController.h
)

set(resource_dir "${PROJECT_SOURCE_DIR}/resources")
qt_add_resources(zhan-debate "assets"
    PREFIX "/"
    BASE "${resource_dir}"
    FILES
        "${resource_dir}/fonts/Inter-Regular.ttf"
        "${resource_dir}/fonts/Inter-Medium.ttf"
        "${resource_dir}/fonts/Inter-Bold.ttf"
        "${resource_dir}/fonts/InterDisplay-SemiBold.ttf"
        "${resource_dir}/icons/zhan-debate.png"
)

if(WIN32)
    # 用绝对路径引用图标，避免 rc.exe 按构建目录解析相对路径
    # Reference the icon by absolute path so rc.exe does not resolve it against the build directory
    set(ZHAN_ICON "${resource_dir}/icons/zhan-debate.ico")
    configure_file("${PROJECT_SOURCE_DIR}/packaging/windows/zhan-debate.rc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/zhan-debate.rc" @ONLY)
    target_sources(zhan-debate PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/zhan-debate.rc")
endif()

target_link_libraries(zhan-debate PRIVATE debate_core Qt6::Gui Qt6::Qml Qt6::Quick)
target_compile_definitions(zhan-debate PRIVATE ZHAN_VERSION="${PROJECT_VERSION}")
zhan_set_warnings(zhan-debate)

include(GNUInstallDirs)
install(TARGETS zhan-debate
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(UNIX AND NOT APPLE)
    install(FILES "${PROJECT_SOURCE_DIR}/packaging/linux/zhan-debate.desktop"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES "${resource_dir}/icons/zhan-debate.png"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps)
endif()
