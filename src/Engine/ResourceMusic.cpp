#include "ResourceMusic.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceMusic::ResourceMusic()
        : m_volume( 1.f )
        , m_external( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMusic::~ResourceMusic()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMusic::setVolume( float _volume )
    {
        m_volume = _volume;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceMusic::getVolume() const
    {
        return m_volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMusic::setExternal( bool _external )
    {
        m_external = _external;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMusic::isExternal() const
    {
        return m_external;
    }    
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMusic::_convert()
    {
        const ConstString & converterType = this->getConverterType();
        const FilePath & filePath = this->getFilePath();

        FilePath newFilePath;
        ConstString newCodecType;
        if( this->convertDefault_( converterType, filePath, newFilePath, newCodecType ) == false )
        {
            return false;
        }

        this->setFilePath( newFilePath );
        this->setCodecType( newCodecType );

        return true;
    }
}
