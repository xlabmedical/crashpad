set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS REQUIRED Core Widgets Network)
find_package(LibArchive CONFIG REQUIRED)
add_subdirectory(xQt)


add_library(CrashUI STATIC
    CrashUploadDialog.cpp
    CrashUploadProgressDialog.cpp
    MedicAttachmentUtil.cpp
    CrashUploadHandler.cpp
)

target_include_directories(CrashUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(CrashUI PUBLIC
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    LibArchive::LibArchive
    xQt::xQt
    crashpad_compat
    crashpad_minidump
    crashpad_snapshot
    crashpad_util
    mini_chromium
)
