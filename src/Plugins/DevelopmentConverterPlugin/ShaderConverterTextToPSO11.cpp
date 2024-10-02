#include "ShaderConverterTextToPSO11.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Environment/Windows/Win32Register.h"
#include "Environment/Windows/Win32CreateProcess.h"

#include "Kernel/Logger.h"
#include "Kernel/FilePath.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/PathString.h"

#include "Config/StdIO.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ShaderConverterTextToPSO11::ShaderConverterTextToPSO11()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ShaderConverterTextToPSO11::~ShaderConverterTextToPSO11()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ShaderConverterTextToPSO11::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".pso" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShaderConverterTextToPSO11::_finalize()
    {
        //Empty
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ShaderConverterTextToPSO11::convert()
    {
        FilePath fxcPath = CONFIG_VALUE( "Engine", "FxcPath", STRINGIZE_FILEPATH_LOCAL( "REGISTER" ) );

        if( fxcPath == STRINGIZE_FILEPATH_LOCAL( "REGISTER" ) )
        {
            Char WindowsKitsInstallationFolder[MENGINE_MAX_PATH] = {'\0'};
            if( Helper::Win32GetLocalMachineRegValue( L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows Kits\\Installed Roots", L"KitsRoot10", WindowsKitsInstallationFolder, MENGINE_MAX_PATH ) == false )
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

        PathString full_input;
        full_input += inputFolderPath;
        full_input += inputFilePath;

        PathString full_output;
        full_output += outputFolderPath;
        full_output += outputFilePath;

        WChar command[MENGINE_MAX_COMMAND_LENGTH] = {'\0'};
        MENGINE_SWPRINTF( command, MENGINE_MAX_COMMAND_LENGTH, L"/nologo /T ps_4_0_level_9_1 /O3 /Gec /Fo \"%S\" \"%S\""
            , full_output.c_str()
            , full_input.c_str()
        );

        LOGGER_MESSAGE_RELEASE( "converting file '%s' to '%s'\n%ls"
            , full_input.c_str()
            , full_output.c_str()
            , command
        );

        uint32_t exitCode;
        if( Helper::Win32CreateProcess( fxcPath.c_str(), command, true, &exitCode ) == false )
        {
            LOGGER_ERROR( "invalid convert file '%s' to '%s':\n%ls"
                , full_input.c_str()
                , full_output.c_str()
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