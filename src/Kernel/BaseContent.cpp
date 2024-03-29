#include "BaseContent.h"

#include "Kernel/AssertionFilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseContent::BaseContent()
        : m_validNoExist( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseContent::~BaseContent()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseContent::setFileGroup( const FileGroupInterfacePtr & _fileGroup )
    {
        m_fileGroup = _fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & BaseContent::getFileGroup() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseContent::setFilePath( const FilePath & _filePath )
    {
        MENGINE_ASSERTION_VALIDATE_FILEPATH( _filePath );

        m_filePath = _filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & BaseContent::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseContent::setCodecType( const ConstString & _codecType )
    {
        m_codecType = _codecType;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & BaseContent::getCodecType() const
    {
        return m_codecType;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseContent::setConverterType( const ConstString & _converterType )
    {
        m_converterType = _converterType;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & BaseContent::getConverterType() const
    {
        return m_converterType;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseContent::setDataflow( const DataflowInterfacePtr & _dataflow )
    {
        m_dataflow = _dataflow;
    }
    //////////////////////////////////////////////////////////////////////////
    const DataflowInterfacePtr & BaseContent::getDataflow() const
    {
        return m_dataflow;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseContent::setValidNoExist( bool _validNoExist )
    {
        m_validNoExist = _validNoExist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseContent::isValidNoExist() const
    {
        return m_validNoExist;
    }
    //////////////////////////////////////////////////////////////////////////
}
