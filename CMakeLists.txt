cmake_minimum_required(VERSION 3.16)

project(Dictionary VERSION 0.1 LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql)

qt_standard_project_setup(REQUIRES 6.8)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appDictionary
    main.cpp
)
add_library(dict-core STATIC
    core/kana.cpp
    core/kana.h
    core/csvreader.cpp
    core/csvreader.h
    core/common.cpp
    core/common.h
    core/entry.h
    core/dictionary.h
    core/dictionary.cpp
    core/third_party/sqlite3.c core/third_party/sqlite3.h
    core/dict_types.h
    core/dict_types.c
    core/dict_utf8.h
    core/dict_utf8.c
    core/dict_kana.h



)
set_source_files_properties(core/third_party/sqlite3.c
    PROPERTIES COMPILE_FLAGS "-w")
target_link_libraries(dict-core PUBLIC Qt6::Core Qt6::Sql)
target_include_directories(dict-core PUBLIC core)

qt_add_qml_module(appDictionary
    URI Dictionary
    QML_FILES
        Main.qml
    SOURCES
        core/entrymodel.h
        core/entrymodel.cpp

)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appDictionary PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDictionary
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appDictionary PRIVATE Qt6::Quick dict-core)

install(TARGETS appDictionary
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
qt_add_executable(importer tool/importer/main.cpp)
target_link_libraries(importer PRIVATE dict-core)
set_target_properties(importer PROPERTIES WIN32_EXECUTABLE FALSE)