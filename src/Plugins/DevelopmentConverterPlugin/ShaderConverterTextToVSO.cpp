#include "ShaderConverterTextToVSO.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/Logger.h"

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
        m_convertExt = ".pso";

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ShaderConverterTextToVSO::convert()
    {
        const ConstString & pakPath = m_options.fileGroup->getFolderPath();

        String full_input = pakPath.c_str();
        full_input += m_options.inputFileName.c_str();

        String full_output = pakPath.c_str();
        full_output += m_options.outputFileName.c_str();

        String buffer = "fxc.exe /T vs_1_1 /O3 /Fo \"" + full_output + "\" \"" + full_input + "\"";

        LOGGER_MESSAGE( "converting file '%s' to '%s'\n%s"
            , full_input.c_str()
            , full_output.c_str()
            , buffer.c_str()
        );

        if( PLATFORM_SERVICE()
            ->cmd( buffer.c_str() ) == false )
        {
            LOGGER_ERROR( "invalid convert:\n%s"
                , buffer.c_str()
            );

            return false;
        }

        return true;
    }
}