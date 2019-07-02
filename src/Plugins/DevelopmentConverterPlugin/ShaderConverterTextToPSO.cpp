#include "ShaderConverterTextToPSO.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/FilePath.h"

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
        m_convertExt = ".pso";

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ShaderConverterTextToPSO::convert()
    {
        FilePath fxcPath = CONFIG_VALUE( "Engine", "FxcPath", STRINGIZE_FILEPATH_LOCAL( "REGISTER" ) );

        if( fxcPath == STRINGIZE_FILEPATH_LOCAL( "REGISTER" ) )
        {
            Char WindowsKitsInstallationFolder[256];
            PLATFORM_SERVICE()
                ->getLocalMachineRegValue( "SOFTWARE\\WOW6432Node\\Microsoft\\Windows Kits\\Installed Roots", "KitsRoot10", WindowsKitsInstallationFolder, 256 );

            PLATFORM_SERVICE()
                ->findFiles( "", WindowsKitsInstallationFolder, "x64\\fxc.exe", [&fxcPath]( const FilePath& _fp )
            { 
                fxcPath = _fp;

                return false;
            } );
        }

        const ConstString & pakPath = m_options.fileGroup->getFolderPath();

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();
        
        String buffer = " /nologo /T ps_2_0 /O3 /Fo \"" + full_output + "\" \"" + full_input + "\"";

        LOGGER_MESSAGE( "converting file '%s' to '%s'\n%s"
            , full_input.c_str()
            , full_output.c_str()
            , buffer.c_str()
        );

        if( PLATFORM_SERVICE()
            ->createProcess( fxcPath.c_str(), buffer.c_str() ) == false )
        {
            LOGGER_ERROR( "invalid convert:\n%s"
                , buffer.c_str()
            );

            return false;
        }

        return true;
    }
}