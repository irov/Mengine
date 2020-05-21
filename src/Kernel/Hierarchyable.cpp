#include "Hierarchyable.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Hierarchyable::Hierarchyable()
        : m_hierarchy( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Hierarchyable::~Hierarchyable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Hierarchyable::availableHierarchyable() const
    {
        return m_hierarchy != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchyable::clearHierarchyable()
    {
        m_hierarchy = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const HierarchyInterfacePtr & Hierarchyable::getHierarchyable() const
    {
        if( m_hierarchy == nullptr )
        {
            HierarchyInterfacePtr hierarchy = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "BaseHierarchy" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            HierarchyReceiverInterface * receiver = const_cast<Hierarchyable *>(this)->getHierarchyableReceiver();

            hierarchy->setHierarchyReceiver( receiver );

            m_hierarchy = hierarchy;
        }

        return m_hierarchy;
    }
}
