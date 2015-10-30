#   pragma once

#	include "Interface/RenderSystemInterface.h"

#   include "stdex/memory_reader.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"
#	include "Core/ColourValue.h"
#	include "Core/Polygon.h"
#	include "Core/Viewport.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"
#   include "Config/Floats.h"

#   include "Math/vec2.h"
#   include "Math/vec3.h"
#   include "Math/vec4.h"

#   include "Math/box2.h"
#   include "Math/uv4.h"

namespace Metabuf
{
    //////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, bool & _value, void * _userData );
	void archive_read( stdex::memory_reader & ar, uint8_t & _value, void * _userData );
	void archive_read( stdex::memory_reader & ar, uint16_t & _value, void * _userData );
	void archive_read( stdex::memory_reader & ar, uint32_t & _value, void * _userData );
	void archive_read( stdex::memory_reader & ar, float & _value, void * _userData );
	//////////////////////////////////////////////////////////////////////////
    void archive_read( stdex::memory_reader & ar, Menge::String & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, Menge::ConstString & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, Menge::WChar & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, Menge::WString & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, Menge::ColourValue & _value, void * _userData );    
    //void archive_read( stdex::memory_reader & ar, Menge::Polygon & _value, void * _userData );
	void archive_read( stdex::memory_reader & ar, Menge::Viewport & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, Menge::Floats & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, mt::vec2f & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, mt::vec3f & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, mt::vec4f & _value, void * _userData );
	void archive_read( stdex::memory_reader & ar, mt::box2f & _value, void * _userData );
	void archive_read( stdex::memory_reader & ar, mt::uv4f & _value, void * _userData );
	//////////////////////////////////////////////////////////////////////////
	void archive_read( stdex::memory_reader & ar, Menge::EBlendFactor & _value, void * _userData );
	void archive_read( stdex::memory_reader & ar, Menge::ETextureOp & _value, void * _userData );
	void archive_read( stdex::memory_reader & ar, Menge::ETextureArgument & _value, void * _userData );
	void archive_read( stdex::memory_reader & ar, Menge::ETextureAddressMode & _value, void * _userData );
}