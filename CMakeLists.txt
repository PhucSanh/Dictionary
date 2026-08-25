cmake_minimum_required(VERSION 3.16)

project(Dictionary VERSION 0.1 LANGUAGES C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick)
find_package(Qt6 QUIET COMPONENTS TextToSpeech)

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
target_include_directories(appDictionary PRIVATE app/ViewModel app)
qt_add_qml_module(appDictionary
    URI Dictionary
    QML_FILES
        # ---------- View ----------
        app/View/Main.qml
        app/View/SearchPage.qml
        app/View/DetailPage.qml
        app/View/FlashcardPage.qml
        app/View/EntryDelegate.qml
        app/View/CategoryDialog.qml
    SOURCES
        # ---------- ViewModel ----------
        app/ViewModel/EntryModel.h
        app/ViewModel/EntryModel.cpp
        app/ViewModel/SpeechViewModel.h
        app/ViewModel/SpeechViewModel.cpp

        # ---------- Model / Entities ----------
        app/Model/Entities/Entry.h
        app/Model/Entities/Note.h
        app/Model/Entities/Category.h
        app/Model/Entities/ConjugationForm.h
        app/Model/Entities/Flashcard.h
        app/Model/Entities/SearchResult.h

        # ---------- Model / Interfaces ----------
        app/Model/Interfaces/IDictionaryRepository.h
        app/Model/Interfaces/IUserRepository.h
        app/Model/Interfaces/ITextAnalyzer.h
        app/Model/Interfaces/ISpeechService.h

        # ---------- Model / Services ----------
        app/Model/Services/QtSpeechService.h
        app/Model/Services/QtSpeechService.cpp

        # ---------- Model / Repositories ----------
        app/Model/Repositories/DictionaryRepository.h
        app/Model/Repositories/DictionaryRepository.cpp
        app/Model/Repositories/UserRepository.h
        app/Model/Repositories/UserRepository.cpp
        app/Model/Repositories/CoreTextAnalyzer.h
        app/Model/Repositories/CoreTextAnalyzer.cpp

        # ---------- Model / Database ----------
        app/Model/Database/DbPaths.h
        app/Model/Database/DbPaths.cpp
        app/Model/Database/DictDbConnection.h
        app/Model/Database/DictDbConnection.cpp
        app/Model/Database/DictMapping.h
        app/Model/Database/DictMapping.cpp
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

if (TARGET Qt6::TextToSpeech)
    target_link_libraries(appDictionary PRIVATE Qt6::TextToSpeech)
    target_compile_definitions(appDictionary PRIVATE DICT_HAS_TTS)
    message(STATUS "Qt TextToSpeech: FOUND - chuc nang phat am duoc bat")
else()
    message(STATUS "Qt TextToSpeech: NOT FOUND - nut phat am se bi an. "
                   "Cai them module Qt Speech bang Qt Maintenance Tool de bat.")
endif()

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