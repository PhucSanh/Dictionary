cmake_minimum_required(VERSION 3.16)

project(Dictionary VERSION 0.1 LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick)

qt_standard_project_setup(REQUIRES 6.8)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

add_library(dict-core STATIC

    core/third_party/sqlite3.c core/third_party/sqlite3.h
    core/dict_types.h
    core/dict_types.c
    core/dict_utf8.h
    core/dict_utf8.c
    core/dict_kana.h
    core/dict_kana.c
    core/dict_db.h
    core/dict_db.c
    core/dict_conjugate.h
    core/dict_conjugate.c
    core/dict_deinflect.h
    core/dict_deinflect.c

)
target_include_directories(dict-core PUBLIC core core/third_party)

set_source_files_properties(core/third_party/sqlite3.c
    PROPERTIES COMPILE_FLAGS "-w")
target_compile_definitions(dict-core PRIVATE
    SQLITE_ENABLE_FTS5
    SQLITE_THREADSAFE=1
    SQLITE_DEFAULT_FOREIGN_KEYS=1
    SQLITE_OMIT_DEPRECATED
)
qt_add_executable(appDictionary app/main.cpp)
target_include_directories(appDictionary PRIVATE app)
qt_add_qml_module(appDictionary
    URI Dictionary
    QML_FILES
        app/Main.qml
        app/SearchPage.qml
        app/DetailPage.qml
        app/FlashcardPage.qml
        app/EntryDelegate.qml
        app/CategoryDialog.qml
    SOURCES
        app/domain/Category.h
        app/domain/Entry.h
        app/domain/Note.h
        app/domain/ConjugationForm.h
        app/domain/Flashcard.h
        app/domain/SearchResult.h
        app/domain/IDictionaryRepository.h
        app/domain/IUserRepository.h
        app/domain/ITextAnalyzer.h

        app/data/DbPaths.h
        app/data/DbPaths.cpp
        app/data/DictDbConnection.h
        app/data/DictDbConnection.cpp
        app/data/DictMapping.h
        app/data/DictMapping.cpp
        app/data/DictionaryRepository.h
        app/data/DictionaryRepository.cpp
        app/data/UserRepository.h
        app/data/UserRepository.cpp
        app/data/CoreTextAnalyzer.h
        app/data/CoreTextAnalyzer.cpp

        app/entrymodel.h
        app/entrymodel.cpp
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
qt_add_executable(importer
    tool/importer/main.cpp
    tool/importer/csvreader.h  tool/importer/csvreader.cpp
    tool/importer/common.h     tool/importer/common.cpp
)
target_link_libraries(importer PRIVATE dict-core Qt6::Core)
set_target_properties(importer PROPERTIES WIN32_EXECUTABLE FALSE)