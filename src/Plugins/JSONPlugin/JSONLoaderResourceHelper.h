#pragma once

#include "Interface/ContentInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/JSON.h"

#include "math/vec2.h"

namespace Mengine
{
    namespace Detail
    {
        bool setJSONContentFile( const ContentInterfacePtr & _content, const jpp::object & _json, bool _codecFromPath );
        bool getJSONMemberVec2Required( const jpp::object & _json, const Char * _node, const Char * _key, mt::vec2f * const _value );
        bool getJSONResourceImage( Resource * _resource, const ConstString & _groupName, const ConstString & _resourceImageName, ResourceImagePtr * const _resourceImage );
    }
}
