#include "BaseHierarchy.h"

#include "Kernel/Node.h"
#include "Kernel/Assertion.h"
#include "Kernel/IntrusivePtrScope.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const ConstString & getHierarchyReceiverName( const HierarchyInterface * _hierarchy )
        {
            HierarchyReceiverInterface * hierarchyReceiver = _hierarchy->getHierarchyReceiver();

            Identity * identity = dynamic_cast<Identity *>(hierarchyReceiver);

            if( identity == nullptr )
            {
                return ConstString::none();
            }

            const ConstString & name = identity->getName();

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        static const ConstString & getHierarchyReceiverName( const HierarchyInterfacePtr & _hierarchy )
        {
            HierarchyReceiverInterface * hierarchyReceiver = _hierarchy->getHierarchyReceiver();

            Identity * identity = dynamic_cast<Identity *>(hierarchyReceiver);

            if( identity == nullptr )
            {
                return ConstString::none();
            }

            const ConstString & name = identity->getName();

            return name;
        }
    }
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
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        MENGINE_ASSERTION( _hierarchy != this, "node '%s' invalid self child node"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        this->addChild_( m_children.end(), _hierarchy );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::addChildFront( const HierarchyInterfacePtr & _hierarchy )
    {
        MENGINE_ASSERTION( _hierarchy != nullptr, "node '%s' invalid add front child NULL node"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        MENGINE_ASSERTION( _hierarchy != this, "node '%s' invalid self child node"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        this->addChild_( m_children.begin(), _hierarchy );
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseHierarchy::addChildAfter( const HierarchyInterfacePtr & _hierarchy, const HierarchyInterfacePtr & _after )
    {
        MENGINE_ASSERTION( _hierarchy != nullptr, "node '%s' invalid add child NULL node (node)"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        MENGINE_ASSERTION( _after != nullptr, "node '%s' invalid add after NULL node (node)"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        MENGINE_ASSERTION( _hierarchy != _after, "node '%s' invalid add child '%s' is equal after '%s' (node)"
            , Detail::getHierarchyReceiverName( this ).c_str()
            , Detail::getHierarchyReceiverName( _hierarchy ).c_str()
            , Detail::getHierarchyReceiverName( _after ).c_str()
        );

        MENGINE_ASSERTION( stdex::helper::intrusive_find( m_children.begin(), m_children.end(), BaseHierarchyPtr::from( _after ) ) != m_children.end(), "node '%s' after is not child"
            , Detail::getHierarchyReceiverName( this ).c_str()
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
        }

        HierarchyReceiverInterface * receiver = this->getHierarchyReceiver();

        receiver->onHierarchyAddChild( _child );
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
            , Detail::getHierarchyReceiverName( this ).c_str()
            , Detail::getHierarchyReceiverName( _hierarchy ).c_str()
        );

        MENGINE_ASSERTION_RETURN( _hierarchy != this, false, "node '%s' invalid self child node"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        HierarchyReceiverInterface * receiver = _hierarchy->getHierarchyReceiver();

        receiver->onHierarchyDeactivate();

        //check if deactivate remove from parent!
        HierarchyInterface * parent = _hierarchy->getParent();

        if( parent != this )
        {
            return false;
        }

        this->removeChild_( _hierarchy );

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
    void BaseHierarchy::removeParent_()
    {
        HierarchyReceiverInterface * receiver = this->getHierarchyReceiver();

        receiver->onHierarchyRemoveParent( m_parent );

        BaseHierarchy * oldparent = m_parent;
        m_parent = nullptr;

        receiver->onHierarchyChangeParent( oldparent, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseHierarchy::setParent_( BaseHierarchy * _parent )
    {
        HierarchyReceiverInterface * receiver = this->getHierarchyReceiver();

        receiver->onHierarchySetParent( _parent );

        BaseHierarchy * oldparent = m_parent;
        m_parent = _parent;

        receiver->onHierarchyChangeParent( oldparent, _parent );
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
    bool BaseHierarchy::hasChild( const LambdaFindChild & _lambda, bool _recursive ) const
    {
        for( const BaseHierarchyPtr & hierarchy : m_children )
        {
            if( _lambda( hierarchy ) == true )
            {
                return true;
            }
        }

        if( _recursive == true )
        {
            for( const BaseHierarchyPtr & children : m_children )
            {
                if( children->hasChild( _lambda, true ) == false )
                {
                    continue;
                }

                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    HierarchyInterfacePtr BaseHierarchy::findChild( const LambdaFindChild & _lambda, bool _recursive ) const
    {
        for( const BaseHierarchyPtr & hierarchy : m_children )
        {
            if( _lambda( hierarchy ) == true )
            {
                return hierarchy;
            }
        }

        if( _recursive == true )
        {
            for( const BaseHierarchyPtr & children : m_children )
            {
                const HierarchyInterfacePtr & child = children->findChild( _lambda, true );

                if( child == nullptr )
                {
                    continue;
                }

                return child;
            }
        }

        return HierarchyInterfacePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    HierarchyInterfacePtr BaseHierarchy::getSiblingPrev() const
    {
        BaseHierarchy * parent = this->getParent();

        if( parent == nullptr )
        {
            return HierarchyInterfacePtr::none();
        }

        IntrusiveSlugListHierarchyChild & parent_children = parent->getChildren_();

        IntrusiveSlugListHierarchyChild::iterator it_found =
            stdex::helper::intrusive_find( parent_children.begin(), parent_children.end(), BaseHierarchyPtr::from( this ) );

        if( it_found == parent_children.begin() )
        {
            return nullptr;
        }

        --it_found;

        const HierarchyInterfacePtr & prev_hierarchy = *it_found;

        return prev_hierarchy;
    }
    //////////////////////////////////////////////////////////////////////////
    HierarchyInterfacePtr BaseHierarchy::getSiblingNext() const
    {
        BaseHierarchy * parent = this->getParent();

        if( parent == nullptr )
        {
            return HierarchyInterfacePtr::none();
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
    bool BaseHierarchy::emptyChildren() const
    {
        bool result = m_children.empty();

        return result;
    }
}