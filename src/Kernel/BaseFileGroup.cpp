#include "BaseFileGroup.h"

#include "Kernel/Assertion.h"
#include "Kernel/PathHelper.h"

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
        MENGINE_ASSERTION_FATAL( Helper::isCorrectFolderPathA( _relationPath.c_str() ) == true, "file group '%s' relation path '%s' invalid correct"
            , this->getName().c_str()
            , _relationPath.c_str()
        );

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
        MENGINE_ASSERTION_FATAL( this->isPacked() == true || Helper::isCorrectFolderPathA( _folderPath.c_str() ) == true, "file group '%s' folder path '%s' invalid correct"
            , this->getName().c_str()
            , _folderPath.c_str()
        );

        m_folderPath = _folderPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & BaseFileGroup::getFolderPath() const
    {
        return m_folderPath;
    }
    //////////////////////////////////////////////////////////////////////////
}
