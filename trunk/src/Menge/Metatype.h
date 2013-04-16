#   pragma once

#   include "metabuf/ArchiveReader.hpp"

#	include "Utils/Core/ConstString.h"
#	include "Utils/Core/ColourValue.h"

#	include "Config/Typedef.h"

#   include "Math/vec2.h"
#   include "Math/vec3.h"
#   include "Math/vec4.h"

namespace Metabuf
{
    //////////////////////////////////////////////////////////////////////////
    void archive_read( ArchiveReader & ar, Menge::String & _value, void * _userData );
    void archive_read( ArchiveReader & ar, Menge::ConstString & _value, void * _userData );
    void archive_read( ArchiveReader & ar, Menge::WChar & _value, void * _userData );
    void archive_read( ArchiveReader & ar, Menge::WString & _value, void * _userData );
    void archive_read( ArchiveReader & ar, Menge::ColourValue & _value, void * _userData );
    void archive_read( ArchiveReader & ar, mt::vec2f & _value, void * _userData );
    void archive_read( ArchiveReader & ar, mt::vec3f & _value, void * _userData );
    void archive_read( ArchiveReader & ar, mt::vec4f & _value, void * _userData );
}