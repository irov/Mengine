OPTION(MENGINE_BUILD_ONLY_EXECUTABLE_MODULE "Mengine build only executable module" OFF)
OPTION(MENGINE_BUILD_MENGINE_MASTER_RELEASE "Mengine build master release" OFF)
OPTION(MENGINE_USE_PRECOMPILED_HEADER "Mengine use precompiled header" OFF)
OPTION(MENGINE_USE_SCRIPT_SERVICE "Mengine use script" ON)
OPTION(MENGINE_EXTERNAL_PDB "Mengine external pdb" OFF)
OPTION(MENGINE_EXTERNAL_SOURCE "Mengine external sources" OFF)
OPTION(MENGINE_USE_SUBFOLDER "Mengine use subfolder" OFF)
OPTION(MENGINE_LOGGER_DEBUG "Mengine logger enable" ON)

SET(MENGINE_EXTERNAL_PDB_PATH "" CACHE STRING "Mengine external source path")
SET(MENGINE_EXTERNAL_PROJECT_NAME "" CACHE STRING "Mengine external project name")
SET(MENGINE_EXTERNAL_SOURCE_PATH "" CACHE STRING "Mengine external source path")
SET(MENGINE_EXTERNAL_TEMP_PATH "" CACHE STRING "Mengine external temp path")
SET(MENGINE_SUBFOLDER_NAME "Mengine" CACHE STRING "Mengine subfolder name")
SET(MENGINE_INSTALL_PATH "" CACHE STRING "Mengine install path")
SET(MENGINE_SECURE_VALUE "0123456789A" CACHE STRING "Mengine secure value")