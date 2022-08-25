#include "DevToDebugWidgetText.h"

#include "DevToDebugProperty.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetText::DevToDebugWidgetText()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugWidgetText::~DevToDebugWidgetText()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * DevToDebugWidgetText::getTypeJson() const
    {
        return "text";
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugWidgetText::process( const jpp::object & _data, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands )
    {
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _mutex );
        MENGINE_UNUSED( _commands );

        //Emty
    }
    //////////////////////////////////////////////////////////////////////////
}