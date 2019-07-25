#include "TexturepackerPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"

#include "ResourceTexturepacker.h"

#ifdef MENGINE_USE_PYTHON_FRAMEWORK
#include "TexturepackerScriptEmbedding.h"
#endif

#include "LoaderResourceTexturepacker.h"

#include "Kernel/ResourcePrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Texturepacker, Mengine::TexturepackerPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TexturepackerPlugin::TexturepackerPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TexturepackerPlugin::~TexturepackerPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TexturepackerPlugin::_initializePlugin()
    {
#ifdef MENGINE_USE_PYTHON_FRAMEWORK
        SERVICE_WAIT( ScriptServiceInterface, []()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "TexturepackerScriptEmbedding" ), TexturepackerScriptEmbedding );

            return true;
        } );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceTexturepacker, 64>>() ) == false )
        {
            return false;
        }

        SERVICE_WAIT( LoaderServiceInterface, []()
        {
            VOCABULARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ), Helper::makeFactorableUnique<LoaderResourceTexturepacker>() );

            return true;
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TexturepackerPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_PYTHON_FRAMEWORK
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "TexturepackerScriptEmbedding" ) );
        }
#endif

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ) );
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceTexturepacker" ) );
    }
}