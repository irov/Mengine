OPTION(MENGINE_BUILD_MENGINE_MASTER_RELEASE "Mengine build master release" ON)
OPTION(MENGINE_BUILD_MENGINE_BUILD_PUBLISH "Mengine build publish" OFF)
OPTION(MENGINE_BUILD_MENGINE_LOGGER_LEVEL_FORCE_VERBOSE "Mengine set logger level verbose" OFF)
OPTION(MENGINE_BUILD_MENGINE_DEVELOPMENT "Mengine build development" OFF)
OPTION(MENGINE_USE_PRECOMPILED_HEADER "Mengine use precompiled header" OFF)
OPTION(MENGINE_USE_SCRIPT_SERVICE "Mengine use script" ON)
OPTION(MENGINE_EXTERNAL_PDB "Mengine external pdb" OFF)
OPTION(MENGINE_EXTERNAL_BOOTSTRAPPER_NAME "Mengine external bootstrapper" OFF)
OPTION(MENGINE_USE_SUBFOLDER "Mengine use subfolder" OFF)

SET(MENGINE_EXTERNAL_PDB_PATH "" CACHE STRING "Mengine external pdb path")
SET(MENGINE_EXTERNAL_SOURCE_PATH "" CACHE STRING "Mengine external source path")
SET(MENGINE_EXTERNAL_TEMP_PATH "" CACHE STRING "Mengine external temp path")
SET(MENGINE_SUBFOLDER_NAME "Mengine" CACHE STRING "Mengine subfolder name")
SET(MENGINE_INSTALL_PATH "" CACHE STRING "Mengine install path")
SET(MENGINE_SECURE_VALUE "0123456789A" CACHE STRING "Mengine secure value")