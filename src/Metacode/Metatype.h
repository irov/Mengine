#   pragma once

#   include "Interface/RenderEnumInterface.h"

#	include "metabuf/Metadata.hpp"

#   include "stdex/memory_reader.h"

#	include "Core/ConstString.h"
#	include "Core/Tags.h"
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
	void archive_read( Reader & ar, bool & _value, void * _userData );
	void archive_read( Reader & ar, uint8_t & _value, void * _userData );
	void archive_read( Reader & ar, uint16_t & _value, void * _userData );
	void archive_read( Reader & ar, uint32_t & _value, void * _userData );
	void archive_read( Reader & ar, float & _value, void * _userData );
	//////////////////////////////////////////////////////////////////////////
    void archive_read( Reader & ar, Menge::String & _value, void * _userData );
    void archive_read( Reader & ar, Menge::ConstString & _value, void * _userData );
	void archive_read( Reader & ar, Menge::FilePath & _value, void * _userData );
	void archive_read( Reader & ar, Menge::Tags & _value, void * _userData );
    void archive_read( Reader & ar, Menge::WChar & _value, void * _userData );
    void archive_read( Reader & ar, Menge::WString & _value, void * _userData );
    void archive_read( Reader & ar, Menge::ColourValue & _value, void * _userData );    
    void archive_read( Reader & ar, Menge::Polygon & _value, void * _userData );
	void archive_read( Reader & ar, Menge::Viewport & _value, void * _userData );
    void archive_read( Reader & ar, Menge::Floats & _value, void * _userData );
	void archive_read( Reader & ar, Menge::Int8s & _value, void * _userData );
	void archive_read( Reader & ar, Menge::Int16s & _value, void * _userData );
	void archive_read( Reader & ar, Menge::Int32s & _value, void * _userData );
	void archive_read( Reader & ar, Menge::UInt8s & _value, void * _userData );
	void archive_read( Reader & ar, Menge::UInt16s & _value, void * _userData );
	void archive_read( Reader & ar, Menge::UInt32s & _value, void * _userData );
	void archive_read( Reader & ar, mt::vec2f & _value, void * _userData );
    void archive_read( Reader & ar, mt::vec3f & _value, void * _userData );
    void archive_read( Reader & ar, mt::vec4f & _value, void * _userData );
	void archive_read( Reader & ar, mt::box2f & _value, void * _userData );
	void archive_read( Reader & ar, mt::uv4f & _value, void * _userData );
	//////////////////////////////////////////////////////////////////////////
	void archive_read( Reader & ar, Menge::EBlendFactor & _value, void * _userData );
	void archive_read( Reader & ar, Menge::EBlendOp & _value, void * _userData );
	void archive_read( Reader & ar, Menge::ETextureOp & _value, void * _userData );
	void archive_read( Reader & ar, Menge::ETextureArgument & _value, void * _userData );
	void archive_read( Reader & ar, Menge::ETextureAddressMode & _value, void * _userData );
}