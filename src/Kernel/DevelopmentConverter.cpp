#include "DevelopmentConverter.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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
    bool DevelopmentConverter::initialize( const FileGroupInterfacePtr & _fileGroupDev )
    {
        m_fileGroupDev = _fileGroupDev;

        bool successful = this->_initialize();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentConverter::finalize()
    {
        this->_finalize();

        m_fileGroupDev = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & DevelopmentConverter::getConvertExt() const
    {
        return m_convertExt;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevelopmentConverter::setOptions( const ConverterOptions & _options )
    {
        m_options = _options;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConverterOptions & DevelopmentConverter::getOptions() const
    {
        return m_options;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevelopmentConverter::validateVersion( const InputStreamInterfacePtr & _stream ) const
    {
        MENGINE_UNUSED( _stream );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & DevelopmentConverter::getDevFileGroup() const
    {
        return m_fileGroupDev;
    }
    //////////////////////////////////////////////////////////////////////////
}
