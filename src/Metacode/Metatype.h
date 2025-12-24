#pragma once

#include "Interface/RenderEnumInterface.h"

#include "metabuf/Metadata.hpp"

#include "Kernel/ConstString.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"
#include "Kernel/Color.h"
#include "Kernel/Polygon.h"
#include "Kernel/Viewport.h"
#include "Kernel/String.h"
#include "Kernel/WString.h"
#include "Kernel/Floats.h"

#include "Config/Typedef.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"

#include "math/box2.h"
#include "math/uv4.h"

namespace Metabuf
{
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, bool & _value, void * _userData );
    void archive_read( Reader & ar, uint8_t & _value, void * _userData );
    void archive_read( Reader & ar, uint16_t & _value, void * _userData );
    void archive_read( Reader & ar, uint32_t & _value, void * _userData );
    void archive_read( Reader & ar, float & _value, void * _userData );
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::String & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::ConstString & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::FilePath & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::Tags & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::WChar & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::WString & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::Color & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::Polygon & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::Viewport & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::Floats & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::Int8s & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::Int16s & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::Int32s & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::UInt8s & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::UInt16s & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::UInt32s & _value, void * _userData );
    void archive_read( Reader & ar, mt::vec2f & _value, void * _userData );
    void archive_read( Reader & ar, mt::vec3f & _value, void * _userData );
    void archive_read( Reader & ar, mt::vec4f & _value, void * _userData );
    void archive_read( Reader & ar, mt::mat4f & _value, void * _userData );
    void archive_read( Reader & ar, mt::box2f & _value, void * _userData );
    void archive_read( Reader & ar, mt::uv4f & _value, void * _userData );
    //////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Mengine::EBlendFactor & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::EBlendOp & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::ETextureOp & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::ETextureArgument & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::ETextureAddressMode & _value, void * _userData );
    void archive_read( Reader & ar, Mengine::EVertexAttributeType & _value, void * _userData );
}