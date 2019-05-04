#include "DevelopmentConverter.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevelopmentConverter::DevelopmentConverter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevelopmentConverter::~DevelopmentConverter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevelopmentConverter::initialize()
    {
        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );

        if( fileGroup == nullptr )
        {
            return false;
        }

        m_fileGroup = fileGroup;

        bool successful = this->_initialize();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & DevelopmentConverter::getConvertExt() const
    {
        return m_convertExt;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentConverter::setOptions( ConverterOptions * _options )
    {
        m_options = *(_options);
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevelopmentConverter::validateVersion( const InputStreamInterfacePtr & _stream ) const
    {
        MENGINE_UNUSED( _stream );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
