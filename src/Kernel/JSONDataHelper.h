#pragma once

#include "Kernel/JSON.h"
#include "Kernel/Color.h"
#include "Kernel/Floats.h"
#include "Kernel/Polygon.h"

#include "Interface/RenderEnumInterface.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/uv4.h"
#include "math/box2.h"
#include "math/mat4.h"

namespace Mengine
{
    namespace Helper
    {
        bool hasJSONField( const jpp::object & _json, const Char * _key );
        bool getJSONField( const jpp::object & _json, const Char * _key, jpp::object * const _value );
        bool getJSONMemberField( const jpp::object & _json, const Char * _node, const Char * _key, jpp::object * const _value );

        bool getJSONConstString( const jpp::object & _json, const Char * _key, ConstString * const _value );
        bool getJSONFilePath( const jpp::object & _json, const Char * _key, FilePath * const _value );
        bool getJSONBool( const jpp::object & _json, const Char * _key, bool * const _value );
        bool getJSONFloat( const jpp::object & _json, const Char * _key, float * const _value );
        bool getJSONUInt32( const jpp::object & _json, const Char * _key, uint32_t * const _value );

        bool getJSONMemberConstString( const jpp::object & _json, const Char * _node, const Char * _key, ConstString * const _value );
        bool getJSONMemberFilePath( const jpp::object & _json, const Char * _node, const Char * _key, FilePath * const _value );
        bool getJSONMemberBool( const jpp::object & _json, const Char * _node, const Char * _key, bool * const _value );
        bool getJSONMemberFloat( const jpp::object & _json, const Char * _node, const Char * _key, float * const _value );
        bool getJSONMemberUInt32( const jpp::object & _json, const Char * _node, const Char * _key, uint32_t * const _value );

        bool getJSONVec2f( const jpp::object & _json, const Char * _key, mt::vec2f * const _value );
        bool getJSONVec3f( const jpp::object & _json, const Char * _key, mt::vec3f * const _value );
        bool getJSONVec4f( const jpp::object & _json, const Char * _key, mt::vec4f * const _value );
        bool getJSONUV4f( const jpp::object & _json, const Char * _key, mt::uv4f * const _value );
        bool getJSONBox2f( const jpp::object & _json, const Char * _key, mt::box2f * const _value );
        bool getJSONMat4f( const jpp::object & _json, const Char * _key, mt::mat4f * const _value );
        bool getJSONColor( const jpp::object & _json, const Char * _key, Color * const _value );
        bool getJSONPolygon( const jpp::object & _json, const Char * _key, Polygon * const _value );
        bool getJSONUInt32s( const jpp::object & _json, const Char * _key, UInt32s * const _value );

        bool getJSONMemberVec2f( const jpp::object & _json, const Char * _node, const Char * _key, mt::vec2f * const _value );
        bool getJSONMemberUV4f( const jpp::object & _json, const Char * _node, const Char * _key, mt::uv4f * const _value );
        bool getJSONMemberPolygon( const jpp::object & _json, const Char * _node, const Char * _key, Polygon * const _value );
        bool getJSONMemberColor( const jpp::object & _json, const Char * _node, const Char * _key, Color * const _value );

        bool getJSONVertexAttributeType( const jpp::object & _json, const Char * _key, EVertexAttributeType * const _value );
        bool getJSONBlendFactor( const jpp::object & _json, const Char * _key, EBlendFactor * const _value );
        bool getJSONBlendOp( const jpp::object & _json, const Char * _key, EBlendOp * const _value );
        bool getJSONTextureAddressMode( const jpp::object & _json, const Char * _key, ETextureAddressMode * const _value );

        bool getJSONMemberBlendFactor( const jpp::object & _json, const Char * _node, const Char * _key, EBlendFactor * const _value );
        bool getJSONMemberBlendOp( const jpp::object & _json, const Char * _node, const Char * _key, EBlendOp * const _value );
        bool getJSONMemberTextureAddressMode( const jpp::object & _json, const Char * _node, const Char * _key, ETextureAddressMode * const _value );
    }
}
