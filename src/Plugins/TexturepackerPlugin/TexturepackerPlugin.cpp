#include "TexturepackerPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "ResourceTexturepacker.h"
#include "TexturepackerScriptEmbedding.h"

#include "Kernel/ResourcePrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY(Texturepacker, Mengine::TexturepackerPlugin);
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
		SERVICE_WAIT(ScriptServiceInterface, []()
		{
			ADD_SCRIPT_EMBEDDING(TexturepackerScriptEmbedding);

			return true;
		});

		if (PROTOTYPE_SERVICE()
			->addPrototype("Resource"_c, "ResourceImageTexturepacker"_c, Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceTexturepacker, 64>>()) == false)
		{
			return false;
		}

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TexturepackerPlugin::_finalizePlugin()
    {
        if (SERVICE_EXIST(ScriptServiceInterface) == true)
        {
            REMOVE_SCRIPT_EMBEDDING(TexturepackerScriptEmbedding);
        }

        PROTOTYPE_SERVICE()
            ->removePrototype("Resource"_c, "ResourceImageTexturepacker"_c);

    }
}