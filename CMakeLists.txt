cmake_minimum_required(VERSION 3.19)
project(OS-Scheduler LANGUAGES CXX)

# Set install prefix to a user-writable location for packaging
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Default install path" FORCE)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/backend
    ${CMAKE_CURRENT_SOURCE_DIR}/frontend    
)

# Add the Qt resource files
qt_add_resources(RESOURCES resources.qrc)

# Add the new backend files here
set(backend_files
    backend/scheduler.cpp
    backend/scheduler.h
    backend/FCFS_Scheduler.cpp
    backend/FCFS_Scheduler.h
    backend/priority_scheduler.cpp
    backend/priority_scheduler.h
    backend/process.cpp
    backend/process.h
    backend/RR_SCHEDULER.cpp
    backend/RR_SCHEDULER.h
    backend/SJF_Scheduler.cpp
    backend/SJF_Scheduler.h
)

# Add the new frontend files here
set(frontend_files
    frontend/mainwindow.cpp
    frontend/mainwindow.h
    frontend/mainwindow.ui
    frontend/toggle.h
    frontend/toggle.cpp
    frontend/processProgress.cpp
    frontend/processProgress.h
)

qt_add_executable(OS-Scheduler
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${frontend_files}
    ${backend_files}
    ${RESOURCES}
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

########################################################
##              CPack Configuration                   ## 
########################################################

# Set application version
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_PATCH 3)

# Set the package name, dependency on Qt
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "OS-Scheduler")
set(CPACK_GENERATOR "ZIP")
install(CODE "execute_process(COMMAND windeployqt \"\$<TARGET_FILE:OS-Scheduler>\" --release)")

include(CPack)