#include "ShaderConverterTextToVSO.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Environment/Windows/Win32PlatformServiceExtensionInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/FilePath.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConfigHelper.h"

#include "Config/StdIO.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ShaderConverterTextToVSO::ShaderConverterTextToVSO()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ShaderConverterTextToVSO::~ShaderConverterTextToVSO()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ShaderConverterTextToVSO::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".vso" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShaderConverterTextToVSO::_finalize()
    {
        //Empty
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ShaderConverterTextToVSO::convert()
    {
        Win32PlatformServiceExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getDynamicUnknown();

        FilePath fxcPath = CONFIG_VALUE( "Engine", "FxcPath", STRINGIZE_FILEPATH_LOCAL( "REGISTER" ) );

        if( fxcPath == STRINGIZE_FILEPATH_LOCAL( "REGISTER" ) )
        {
            Char WindowsKitsInstallationFolder[MENGINE_MAX_PATH] = {'\0'};
            if( win32Platform->getLocalMachineRegValue( L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows Kits\\Installed Roots", L"KitsRoot10", WindowsKitsInstallationFolder, MENGINE_MAX_PATH ) == false )
            {
                LOGGER_ERROR( "not found REGISTER Windows Kits installed roots" );

                return false;
            }

            bool successful = false;
            PLATFORM_SERVICE()
                ->findFiles( "", WindowsKitsInstallationFolder, "x64\\fxc.exe", [&fxcPath, &successful]( const FilePath & _fp )
            {
                fxcPath = _fp;
                successful = true;

                return false;
            } );

            if( successful == false )
            {
                return false;
            }
        }

        const FileGroupInterfacePtr & inputFileGroup = m_options.inputContent->getFileGroup();
        const FilePath & inputFilePath = m_options.inputContent->getFilePath();

        const FileGroupInterfacePtr & outputFileGroup = m_options.outputContent->getFileGroup();
        const FilePath & outputFilePath = m_options.outputContent->getFilePath();

        const FilePath & inputFolderPath = inputFileGroup->getFolderPath();
        const FilePath & outputFolderPath = outputFileGroup->getFolderPath();

        String full_input = inputFolderPath.c_str();
        full_input += inputFilePath.c_str();

        String full_output = outputFolderPath.c_str();
        full_output += outputFilePath.c_str();

        WChar command[MENGINE_MAX_COMMAND_LENGTH] = {'\0'};
        MENGINE_WNSPRINTF( command, MENGINE_MAX_COMMAND_LENGTH, L"/nologo /T vs_1_1 /O3 /Fo \"%S\" \"%S\""
            , full_output.c_str()
            , full_input.c_str()
        );

        LOGGER_MESSAGE_RELEASE( "converting file '%s' to '%s'"
            , full_input.c_str()
            , full_output.c_str()
        );

        uint32_t exitCode;
        if( win32Platform->createProcess( fxcPath.c_str(), command, true, &exitCode ) == false )
        {
            LOGGER_ERROR( "invalid convert:\n%s"
                , command
            );

            return false;
        }

        if( exitCode != 0 )
        {
            return false;
        }

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
}