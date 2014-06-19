#   pragma once

#   include "stdex/memory_reader.h"

#	include "Utils/Core/ConstString.h"
#	include "Utils/Core/ColourValue.h"
#	include "Utils/Core/GlyphChar.h"
#	include "Utils/Core/Polygon.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"
#   include "Config/Floats.h"

#   include "Math/vec2.h"
#   include "Math/vec3.h"
#   include "Math/vec4.h"

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
    void archive_read( stdex::memory_reader & ar, Menge::GlyphCode & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, Menge::Polygon & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, Menge::Floats & _value, void * _userData );	
    void archive_read( stdex::memory_reader & ar, mt::vec2f & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, mt::vec3f & _value, void * _userData );
    void archive_read( stdex::memory_reader & ar, mt::vec4f & _value, void * _userData );    
}