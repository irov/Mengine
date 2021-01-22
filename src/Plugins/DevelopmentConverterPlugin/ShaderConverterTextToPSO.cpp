#include "ShaderConverterTextToPSO.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/Win32PlatformExtensionInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/FilePath.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ShaderConverterTextToPSO::ShaderConverterTextToPSO()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ShaderConverterTextToPSO::~ShaderConverterTextToPSO()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ShaderConverterTextToPSO::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".pso" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShaderConverterTextToPSO::_finalize()
    {
        //Empty
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ShaderConverterTextToPSO::convert()
    {
        Win32PlatformExtensionInterface * win32Platform = PLATFORM_SERVICE()
            ->getPlatformExtention();

        FilePath fxcPath = CONFIG_VALUE( "Engine", "FxcPath", STRINGIZE_FILEPATH_LOCAL( "REGISTER" ) );

        if( fxcPath == STRINGIZE_FILEPATH_LOCAL( "REGISTER" ) )
        {
            Char WindowsKitsInstallationFolder[256] = {'\0'};
            if( win32Platform->getLocalMachineRegValue( "SOFTWARE\\WOW6432Node\\Microsoft\\Windows Kits\\Installed Roots", "KitsRoot10", WindowsKitsInstallationFolder, 255 ) == false )
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

        const ConstString & folderPath = m_options.fileGroup->getFolderPath();

        String full_input = folderPath.c_str();
        full_input += m_options.inputFilePath.c_str();

        String full_output = folderPath.c_str();
        full_output += m_options.outputFilePath.c_str();

        String buffer = "/nologo /T ps_2_0 /O3 /Fo \"" + full_output + "\" \"" + full_input + "\"";

        LOGGER_MESSAGE( "converting file '%s' to '%s'\n%s"
            , full_input.c_str()
            , full_output.c_str()
            , buffer.c_str()
        );

        uint32_t exitCode;
        if( win32Platform->createProcess( fxcPath.c_str(), buffer.c_str(), true, &exitCode ) == false )
        {
            LOGGER_ERROR( "invalid convert:\n%s"
                , buffer.c_str()
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