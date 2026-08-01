#include "MCPSceneWaitCondition.h"

#include "Interface/SceneServiceInterface.h"

#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool MCPSceneWaitCondition::evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response )
    {
        const ScenePtr & scene = SCENE_SERVICE()->getCurrentScene();
        bool exists = scene != nullptr;
        const Char * name = _request.condition.get( "name", "" );
        const Char * nodeType = _request.condition.get( "nodeType", "" );
        ConstString expectedName = Helper::stringizeString( name );
        ConstString expectedType = Helper::stringizeString( nodeType );

        if( exists == true && name[0] != '\0' )
        {
            const ConstString & sceneName = scene->getName();
            exists = expectedName == sceneName;
        }

        if( exists == true && nodeType[0] != '\0' )
        {
            const ConstString & sceneType = scene->getType();
            exists = expectedType == sceneType;
        }

        _response->satisfied = exists;
        bool sceneExists = scene != nullptr;
        _response->details.set( "exists", sceneExists );

        if( scene != nullptr )
        {
            const ConstString & sceneName = scene->getName();
            const ConstString & sceneType = scene->getType();
            _response->details.set( "name", sceneName );
            _response->details.set( "nodeType", sceneType );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
