cmake_minimum_required(VERSION 3.19)
project(OS-Scheduler LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/backend
    ${CMAKE_CURRENT_SOURCE_DIR}/frontend    
)

# Add the new backend files here
set(backend_files
    # Example backend files
    # backend/file1.cpp
)

# Add the new frontend files here
set(frontend_files
    frontend/mainwindow.cpp
    frontend/mainwindow.h
    frontend/mainwindow.ui
    frontend/toggle.h
    frontend/toggle.cpp
)

qt_add_executable(OS-Scheduler
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${frontend_files}
    ${backend_files}





)

target_link_libraries(OS-Scheduler
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS OS-Scheduler
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET OS-Scheduler
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
