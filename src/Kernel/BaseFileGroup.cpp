#include "BaseFileGroup.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseFileGroup::BaseFileGroup()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseFileGroup::~BaseFileGroup()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseFileGroup::initialize()
    {
        bool result = this->_initialize();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseFileGroup::finalize()
    {
        m_baseFileGroup = nullptr;
        m_parentFileGroup = nullptr;

        this->_finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseFileGroup::setName( const ConstString & _name )
    {
        m_name = _name;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & BaseFileGroup::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseFileGroup::setBaseFileGroup( const FileGroupInterfacePtr & _baseFileGroup )
    {
        m_baseFileGroup = _baseFileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & BaseFileGroup::getBaseFileGroup() const
    {
        return m_baseFileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseFileGroup::setParentFileGroup( const FileGroupInterfacePtr & _parentFileGroup )
    {
        m_parentFileGroup = _parentFileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & BaseFileGroup::getParentFileGroup() const
    {
        return m_parentFileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseFileGroup::setRelationPath( const FilePath & _relationPath )
    {
        m_relationPath = _relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & BaseFileGroup::getRelationPath() const
    {
        return m_relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseFileGroup::setFolderPath( const FilePath & _folderPath )
    {
        m_folderPath = _folderPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & BaseFileGroup::getFolderPath() const
    {
        return m_folderPath;
    }
}
