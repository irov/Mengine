#include "BaseHierarchy.h"

#include "Kernel/Node.h"
#include "Kernel/Assertion.h"
#include "Kernel/IntrusivePtrScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseHierarchy::BaseHierarchy()
        : m_parent( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseHierarchy::~BaseHierarchy()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::addChild( const HierarchyInterfacePtr & _hierarchy )
    {
        MENGINE_ASSERTION( _hierarchy != nullptr, "node '%s' invalid add child NULL node"
            , this->getHierarchyReceiver()->getHierarchyName().c_str()
        );

        MENGINE_ASSERTION( _hierarchy != this, "node '%s' invalid self child node"
            , this->getHierarchyReceiver()->getHierarchyName().c_str()
        );

        this->addChild_( m_children.end(), _hierarchy );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::addChildFront( const HierarchyInterfacePtr & _hierarchy )
    {
        MENGINE_ASSERTION( _hierarchy != nullptr, "node '%s' invalid add front child NULL node"
            , this->getHierarchyReceiver()->getHierarchyName().c_str()
        );

        MENGINE_ASSERTION( _hierarchy != this, "node '%s' invalid self child node"
            , this->getHierarchyReceiver()->getHierarchyName().c_str()
        );

        this->addChild_( m_children.begin(), _hierarchy );
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHierarchy::addChildAfter( const HierarchyInterfacePtr & _hierarchy, const HierarchyInterfacePtr & _after )
    {
        MENGINE_ASSERTION( _hierarchy != nullptr, "node '%s' invalid add child NULL node (node)"
            , this->getHierarchyReceiver()->getHierarchyName().c_str()
        );

        MENGINE_ASSERTION( _after != nullptr, "node '%s' invalid add after NULL node (node)"
            , this->getHierarchyReceiver()->getHierarchyName().c_str()
        );

        MENGINE_ASSERTION( _hierarchy != _after, "node '%s' invalid add child '%s' is equal after '%s' (node)"
            , this->getHierarchyReceiver()->getHierarchyName().c_str()
            , _hierarchy->getHierarchyReceiver()->getHierarchyName().c_str()
            , _after->getHierarchyReceiver()->getHierarchyName().c_str()
        );

        MENGINE_ASSERTION( stdex::helper::intrusive_find( m_children.begin(), m_children.end(), BaseHierarchyPtr::from( _after ) ) != m_children.end(), "node '%s' after is not child"
            , this->getHierarchyReceiver()->getHierarchyName().c_str()
        );

        IntrusiveSlugListHierarchyChild::iterator it_after( BaseHierarchyPtr::from( _after ) );

        this->addChild_( it_after, _hierarchy );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::addChild_( const IntrusiveSlugListHierarchyChild::iterator & _insert, const BaseHierarchyPtr & _child )
    {
        IntrusivePtrScope ankh( this );

        BaseHierarchy * child_parent = _child->getParent();

        if( child_parent == this )
        {
            const BaseHierarchyPtr & insert_hierarchy = *_insert;

            if( insert_hierarchy == _child )
            {
                return;
            }

            child_parent->eraseChild_( _child );

            this->insertChild_( _insert, _child );

            HierarchyReceiverInterface * receiver = this->getHierarchyReceiver();

            receiver->onHierarchyRefreshChild( _child );
        }
        else
        {
            if( child_parent != nullptr )
            {
                child_parent->eraseChild_( _child );
            }

            this->insertChild_( _insert, _child );

            _child->setParent_( this );

            HierarchyReceiverInterface * receiver = this->getHierarchyReceiver();

            receiver->onHierarchyAddChild( _child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::insertChild_( const IntrusiveSlugListHierarchyChild::iterator & _insert, const BaseHierarchyPtr & _node )
    {
        m_children.insert( _insert, _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::eraseChild_( const BaseHierarchyPtr & _node )
    {
        m_children.remove( _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::foreachChildren( const LambdaNode & _lambda ) const
    {
        for( const HierarchyInterfacePtr & child : m_children )
        {
            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::foreachChildrenUnslug( const LambdaNode & _lambda ) const
    {
        for( IntrusiveSlugListHierarchyChild::unslug_const_iterator
            it = m_children.ubegin(),
            it_end = m_children.uend();
            it != it_end;
            ++it )
        {
            HierarchyInterfacePtr child( *it );

            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::foreachChildrenReverse( const LambdaNode & _lambda ) const
    {
        for( IntrusiveSlugListHierarchyChild::const_reverse_iterator
            it = m_children.rbegin(),
            it_end = m_children.rend();
            it != it_end;
            ++it )
        {
            HierarchyInterfacePtr child( *it );

            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::foreachChildrenUnslugBreak( const LambdaNodeBreak & _lambda ) const
    {
        for( IntrusiveSlugListHierarchyChild::unslug_const_iterator
            it = m_children.ubegin(),
            it_end = m_children.uend();
            it != it_end;
            ++it )
        {
            HierarchyInterfacePtr child( *it );

            if( _lambda( child ) == false )
            {
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHierarchy::removeChild( const HierarchyInterfacePtr & _hierarchy )
    {
        MENGINE_ASSERTION_RETURN( stdex::helper::intrusive_has( m_children.begin(), m_children.end(), BaseHierarchyPtr::from( _hierarchy ) ) == true, false, "node '%s' not found children '%s'"
            , this->getHierarchyReceiver()->getHierarchyName().c_str()
            , _hierarchy->getHierarchyReceiver()->getHierarchyName().c_str()
        );

        MENGINE_ASSERTION_RETURN( _hierarchy != this, false, "node '%s' invalid self child node"
            , this->getHierarchyReceiver()->getHierarchyName().c_str()
        );

        HierarchyReceiverInterface * receiver = _hierarchy->getHierarchyReceiver();

        receiver->onHierarchyDeactivate();

        //check if deactivate remove from parent!
        HierarchyInterface * parent = _hierarchy->getParent();

        if( parent != this )
        {
            return false;
        }

        receiver->onHierarchyRemoveChild( _hierarchy );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::removeChild_( const BaseHierarchyPtr & _hierarchy )
    {
        HierarchyReceiverInterface * receiver = this->getHierarchyReceiver();

        receiver->onHierarchyRemoveChild( _hierarchy );

        _hierarchy->removeParent_();

        this->eraseChild_( _hierarchy );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t BaseHierarchy::getChildrenRecursiveCount() const
    {
        IntrusiveSlugListHierarchyChild::size_type size = m_children.size();

        this->foreachChildren( [&size]( const HierarchyInterfacePtr & _child )
        {
            size += _child->getChildrenRecursiveCount();
        } );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static IntrusiveSlugListHierarchyChild::const_iterator s_hierarchy_find_child( const IntrusiveSlugListHierarchyChild & _child, const ConstString & _name )
        {
            typename IntrusiveSlugListHierarchyChild::const_iterator it_found =
                stdex::helper::intrusive_find_if( _child.begin(), _child.end(), [_name]( const BaseHierarchyPtr & _hierarchy )
            {
                BaseHierarchy * hierarchy_ptr = _hierarchy.get();

                HierarchyReceiverInterface * receiver = hierarchy_ptr->getHierarchyReceiver();

                const ConstString & receiver_name = receiver->getHierarchyName();

                return receiver_name == _name;
            } );

            return it_found;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    HierarchyInterfacePtr BaseHierarchy::findChild( const ConstString & _name, bool _recursion ) const
    {
        IntrusiveSlugListHierarchyChild::const_iterator it_found =
            Detail::s_hierarchy_find_child( m_children, _name );

        if( it_found != m_children.end() )
        {
            BaseHierarchyPtr children = *it_found;

            return children;
        }

        if( _recursion == true )
        {
            for( const BaseHierarchyPtr & children : m_children )
            {
                BaseHierarchyPtr node = children->findChild( _name, true );

                if( node == nullptr )
                {
                    continue;
                }

                return node;
            }
        }
        else
        {
            HierarchyReceiverInterface * receiver = this->getHierarchyReceiver();

            HierarchyInterfacePtr node = receiver->onHierarchyFindChild( _name, _recursion );

            if( node != nullptr )
            {
                return node;
            }
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    HierarchyInterfacePtr BaseHierarchy::getSiblingPrev() const
    {
        BaseHierarchy * parent = this->getParent();

        if( parent == nullptr )
        {
            return nullptr;
        }

        IntrusiveSlugListHierarchyChild & parent_children = parent->getChildren_();

        IntrusiveSlugListHierarchyChild::iterator it_found =
            stdex::helper::intrusive_find( parent_children.begin(), parent_children.end(), BaseHierarchyPtr::from( this ) );

        if( it_found == parent_children.begin() )
        {
            return nullptr;
        }

        --it_found;

        HierarchyInterfacePtr prev_hierarchy = *it_found;

        return prev_hierarchy;
    }
    //////////////////////////////////////////////////////////////////////////
    HierarchyInterfacePtr BaseHierarchy::getSiblingNext() const
    {
        BaseHierarchy * parent = this->getParent();

        if( parent == nullptr )
        {
            return nullptr;
        }

        IntrusiveSlugListHierarchyChild & parent_children = parent->getChildren_();

        IntrusiveSlugListHierarchyChild::iterator it_found =
            stdex::helper::intrusive_find( parent_children.begin(), parent_children.end(), BaseHierarchyPtr::from( this ) );

        ++it_found;

        if( it_found == parent_children.end() )
        {
            return nullptr;
        }

        const HierarchyInterfacePtr & next_node = *it_found;

        return next_node;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHierarchy::hasChild( const ConstString & _name, bool _recursive ) const
    {
        IntrusiveSlugListHierarchyChild::const_iterator it_found =
            Detail::s_hierarchy_find_child( m_children, _name );

        if( it_found != m_children.end() )
        {
            return true;
        }

        if( _recursive == true )
        {
            for( const HierarchyInterfacePtr & children : m_children )
            {
                if( children->hasChild( _name, true ) == true )
                {
                    return true;
                }
            }
        }

        HierarchyReceiverInterface * receiver = this->getHierarchyReceiver();

        if( receiver->onHierarchyHasChild( _name, _recursive ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHierarchy::emptyChildren() const
    {
        return m_children.empty();
    }
}