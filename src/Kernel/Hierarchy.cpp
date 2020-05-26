#include "Hierarchy.h"

#include "Kernel/Node.h"
#include "Kernel/Assertion.h"
#include "Kernel/IntrusivePtrScope.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const ConstString & getHierarchyReceiverName( const Hierarchy * _hierarchy )
        {
            const Identity * identity = dynamic_cast<const Identity *>(_hierarchy);

            if( identity == nullptr )
            {
                return ConstString::none();
            }

            const ConstString & name = identity->getName();

            return name;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Hierarchy::Hierarchy()
        : m_parent( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Hierarchy::~Hierarchy()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::addChild( const NodePtr & _node )
    {
        MENGINE_ASSERTION_FATAL( _node != nullptr, "node '%s' invalid add child NULL node"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        MENGINE_ASSERTION_FATAL( _node != this, "node '%s' invalid self child node"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        this->addChild_( m_children.end(), _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::addChildFront( const NodePtr & _node )
    {
        MENGINE_ASSERTION_FATAL( _node != nullptr, "node '%s' invalid add front child NULL node"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        MENGINE_ASSERTION_FATAL( _node != this, "node '%s' invalid self child node"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        this->addChild_( m_children.begin(), _node );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Hierarchy::addChildAfter( const NodePtr & _node, const NodePtr & _after )
    {
        MENGINE_ASSERTION_FATAL( _node != nullptr, "node '%s' invalid add child NULL node (node)"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        MENGINE_ASSERTION_FATAL( _after != nullptr, "node '%s' invalid add after NULL node (node)"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        MENGINE_ASSERTION_FATAL( _node != _after, "node '%s' invalid add child '%s' is equal after '%s' (node)"
            , Detail::getHierarchyReceiverName( this ).c_str()
            , _node->getName().c_str()
            , _after->getName().c_str()
        );

        MENGINE_ASSERTION_FATAL( stdex::helper::intrusive_find( m_children.begin(), m_children.end(), NodePtr::from( _after ) ) != m_children.end(), "node '%s' after is not child"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        IntrusiveSlugListHierarchyChild::iterator it_after( NodePtr::from( _after ) );

        this->addChild_( it_after, _node );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::addChild_( const IntrusiveSlugListHierarchyChild::iterator & _insert, const NodePtr & _child )
    {
        IntrusivePtrScope ankh( this );

        Node * child_parent = _child->getParent();

        if( child_parent == this )
        {
            const NodePtr & insert_hierarchy = *_insert;

            if( insert_hierarchy == _child )
            {
                return;
            }

            child_parent->eraseChild_( _child );

            this->insertChild_( _insert, _child );

            this->onHierarchyRefreshChild( _child );
        }
        else
        {
            if( child_parent != nullptr )
            {
                child_parent->eraseChild_( _child );
            }

            this->insertChild_( _insert, _child );

            Node * parentNode = static_cast<Node *>(this);

            _child->setParent_( parentNode );
        }

        this->onHierarchyAddChild( _child );
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::insertChild_( const IntrusiveSlugListHierarchyChild::iterator & _insert, const NodePtr & _node )
    {
        m_children.insert( _insert, _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::eraseChild_( const NodePtr & _node )
    {
        m_children.remove( _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::foreachChildren( const LambdaHierarchy & _lambda ) const
    {
        for( const NodePtr & child : m_children )
        {
            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::foreachChildrenSlug( const LambdaHierarchy & _lambda ) const
    {
        if( m_children.empty() == false )
        {
            NodePtr single = m_children.single();

            if( single != nullptr )
            {
                _lambda( single );
            }
            else
            {
                for( IntrusiveSlugHierarchyChild it( m_children ); it.eof() == false; )
                {
                    NodePtr child( *it );

                    it.next_shuffle();

                    _lambda( child );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Hierarchy::foreachChildrenSlugBreak( const LambdaHierarchyBreak & _lambda ) const
    {
        if( m_children.empty() == true )
        {
            return true;
        }

        NodePtr single = m_children.single();

        if( single != nullptr )
        {
            if( _lambda( single ) == false )
            {
                return false;
            }
        }
        else
        {
            for( IntrusiveSlugHierarchyChild it( m_children ); it.eof() == false; )
            {
                NodePtr child( *it );

                it.next_shuffle();

                if( _lambda( child ) == false )
                {
                    return false;
                }
            }
        }
    
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::foreachChildrenUnslug( const LambdaHierarchy & _lambda ) const
    {
        for( IntrusiveSlugListHierarchyChild::unslug_const_iterator
            it = m_children.ubegin(),
            it_end = m_children.uend();
            it != it_end;
            ++it )
        {
            NodePtr child( *it );

            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::foreachChildrenReverse( const LambdaHierarchy & _lambda ) const
    {
        for( IntrusiveSlugListHierarchyChild::const_reverse_iterator
            it = m_children.rbegin(),
            it_end = m_children.rend();
            it != it_end;
            ++it )
        {
            NodePtr child( *it );

            _lambda( child );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Hierarchy::foreachChildrenUnslugBreak( const LambdaHierarchyBreak & _lambda ) const
    {
        for( IntrusiveSlugListHierarchyChild::unslug_const_iterator
            it = m_children.ubegin(),
            it_end = m_children.uend();
            it != it_end;
            ++it )
        {
            NodePtr child( *it );

            if( _lambda( child ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Hierarchy::removeChild( const NodePtr & _node )
    {
        MENGINE_ASSERTION_FATAL( stdex::helper::intrusive_has( m_children.begin(), m_children.end(), NodePtr::from( _node ) ) == true, "node '%s' not found children '%s'"
            , Detail::getHierarchyReceiverName( this ).c_str()
            , _node->getName().c_str()
        );

        MENGINE_ASSERTION_FATAL( _node != this, "node '%s' invalid self child node"
            , Detail::getHierarchyReceiverName( this ).c_str()
        );

        _node->deactivate();

        //check if deactivate remove from parent!
        Node * parent = _node->getParent();

        if( parent != this )
        {
            return false;
        }

        this->removeChild_( _node );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::removeChild_( const NodePtr & _node )
    {
        this->onHierarchyRemoveChild( _node );

        _node->removeParent_();

        this->eraseChild_( _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::destroyChildren( const LambdaDestroyChildren & _lambda )
    {
        if( m_children.empty() == true )
        {
            return;
        }

        for( IntrusiveSlugHierarchyChild it( m_children ); it.eof() == false; )
        {
            NodePtr hierarchy( *it );

            IntrusiveSlugListHierarchyChild::iterator it_node( hierarchy );

            it.next_shuffle();

            hierarchy->deactivate();

            Hierarchy * parent = hierarchy->getParent();

            if( parent != this )
            {
                continue;
            }

            hierarchy->release();

            hierarchy->removeParent_();

            m_children.erase( it_node );

            _lambda( hierarchy );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::removeChildren( const LambdaRemoveChildren & _lambda )
    {
        if( m_children.empty() == true )
        {
            return;
        }

        for( IntrusiveSlugHierarchyChild it( m_children ); it.eof() == false; )
        {
            NodePtr hierarchy( *it );

            IntrusiveSlugListHierarchyChild::iterator it_node( hierarchy );

            it.next_shuffle();

            hierarchy->deactivate();

            Hierarchy * parent = hierarchy->getParent();

            if( parent != this )
            {
                continue;
            }

            hierarchy->release();

            hierarchy->removeParent_();

            m_children.erase( it_node );

            _lambda( hierarchy );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Hierarchy::removeFromParent()
    {
        if( m_parent == nullptr )
        {
            return true;
        }

        bool result = m_parent->removeChild( NodePtr::from( this ) );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::removeParent_()
    {
        this->onHierarchyRemoveParent( m_parent );

        Node * oldparent = m_parent;
        m_parent = nullptr;

        this->onHierarchyChangeParent( oldparent, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void Hierarchy::setParent_( Node * _parent )
    {
        this->onHierarchySetParent( _parent );

        Node * oldparent = m_parent;
        m_parent = _parent;

        this->onHierarchyChangeParent( oldparent, _parent );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Hierarchy::getChildrenCount() const
    {
        IntrusiveSlugListHierarchyChild::size_type size = m_children.size();

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Hierarchy::getChildrenRecursiveCount() const
    {
        IntrusiveSlugListHierarchyChild::size_type size = m_children.size();

        this->foreachChildren( [&size]( const NodePtr & _child )
        {
            size += _child->getChildrenRecursiveCount();
        } );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Hierarchy::hasChild( const LambdaFindChild & _lambda, bool _recursive ) const
    {
        for( const NodePtr & hierarchy : m_children )
        {
            if( _lambda( hierarchy ) == true )
            {
                return true;
            }
        }

        if( _recursive == true )
        {
            for( const NodePtr & children : m_children )
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
    NodePtr Hierarchy::findChild( const LambdaFindChild & _lambda, bool _recursive ) const
    {
        for( const NodePtr & hierarchy : m_children )
        {
            if( _lambda( hierarchy ) == true )
            {
                return hierarchy;
            }
        }

        if( _recursive == true )
        {
            for( const NodePtr & children : m_children )
            {
                const NodePtr & child = children->findChild( _lambda, true );

                if( child == nullptr )
                {
                    continue;
                }

                return child;
            }
        }

        return NodePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Hierarchy::getLeafDeep() const
    {
        uint32_t deep = 0;

        Node * parent = m_parent;

        while( parent != nullptr )
        {
            ++deep;

            parent = parent->getParent();
        }

        return deep;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Hierarchy::getSiblingPrev() const
    {
        Node * parent = this->getParent();

        if( parent == nullptr )
        {
            return NodePtr::none();
        }

        IntrusiveSlugListHierarchyChild & parent_children = parent->getChildren_();

        IntrusiveSlugListHierarchyChild::iterator it_found =
            stdex::helper::intrusive_find( parent_children.begin(), parent_children.end(), NodePtr::from( this ) );

        if( it_found == parent_children.begin() )
        {
            return nullptr;
        }

        --it_found;

        const NodePtr & prev_hierarchy = *it_found;

        return prev_hierarchy;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Hierarchy::getSiblingNext() const
    {
        Node * parent = this->getParent();

        if( parent == nullptr )
        {
            return NodePtr::none();
        }

        IntrusiveSlugListHierarchyChild & parent_children = parent->getChildren_();

        IntrusiveSlugListHierarchyChild::iterator it_found =
            stdex::helper::intrusive_find( parent_children.begin(), parent_children.end(), NodePtr::from( this ) );

        ++it_found;

        if( it_found == parent_children.end() )
        {
            return nullptr;
        }

        const NodePtr & next_node = *it_found;

        return next_node;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Hierarchy::emptyChildren() const
    {
        bool result = m_children.empty();

        return result;
    }
}