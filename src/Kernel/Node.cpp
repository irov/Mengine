#include "Kernel/Node.h"

#include "Interface/TimelineServiceInterface.h"

#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/TransformationInterface.h"
#include "Interface/UpdationInterface.h"
#include "Interface/EventationInterface.h"

#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/IntrusivePtrScope.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/NodePickerHierarchy.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Node::Node()
        : m_state( EN_STATE_ENABLE )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Node::~Node()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::dispose()
    {
        m_state |= EN_STATE_DISPOSE;

        this->removeFromParent();
        this->disable();
        this->release();

        this->_dispose();

        this->removeChildren( []( const NodePtr & )
        {} );

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        this->unwrap();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::disposeAll()
    {
        this->foreachChildrenSlug( []( const NodePtr & _node )
        {
            _node->disposeAll();
        } );

        this->dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_dispose()
    {
        EventationInterface * eventation = this->getEventation();

        if( eventation != nullptr )
        {
            eventation->removeEvents();
        }

        this->clearAffectorHub();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_destroy()
    {
        MENGINE_ASSERTION_FATAL( this->isActivate() == false, "node '%s' type '%s' destroy in activate state"
            , this->getName().c_str()
            , this->getType().c_str()
        );

        this->release();

        this->clearAffectorHub();

        this->removeChildren( []( const NodePtr & )
        {} );

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        this->unwrap();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::activate()
    {
        if( this->isEnable() == false )
        {
            return true;
        }

        if( this->compile() == false )
        {
            return false;
        }

        if( this->isActivate() == true )
        {
            return true;
        }

        if( this->_activate() == false )
        {
            this->release();

            return false;
        }

        IntrusivePtrScope ankh( this );

        m_state |= EN_STATE_ACTIVE;

        if( this->foreachChildrenSlugBreak( []( const NodePtr & _child )
        {
            bool result = _child->activate();

            return result;
        } ) == false )
        {
            m_state &= ~EN_STATE_ACTIVE;

            this->release();

            return false;
        }

        if( this->isActivate() == false )
        {
            this->release();

            return false;
        }

        m_state |= EN_STATE_AFTER_ACTIVE;

        if( this->_afterActivate() == false )
        {
            m_state &= ~(EN_STATE_AFTER_ACTIVE | EN_STATE_ACTIVE);

            this->release();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::deactivate()
    {
        if( this->isActivate() == false )
        {
            return;
        }

        if( this->isDeactivating() == true )
        {
            return;
        }

        IntrusivePtrScope ankh( this );

        m_state |= EN_STATE_DEACTIVATING;

        if( this->isAfterActive() == false )
        {
            LOGGER_WARNING( "node '%s' invalid deactivate in 'activate state'"
                , this->getName().c_str()
            );
        }

        m_state &= ~EN_STATE_AFTER_ACTIVE;

        this->_deactivate();

        this->foreachChildrenSlug( []( const NodePtr & _child )
        {
            _child->deactivate();
        } );

        m_state &= ~(EN_STATE_ACTIVE | EN_STATE_DEACTIVATING);

        this->_afterDeactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::enable()
    {
        if( this->isActivate() == true )
        {
            return true;
        }

        m_state |= EN_STATE_ENABLE;

        Node * parent = this->getParent();

        if( parent != nullptr )
        {
            if( parent->isActivate() == false )
            {
                return true;
            }
        }
        else
        {
            return true;
        }

        IntrusivePtrScope ankh( this );

        if( this->activate() == false )
        {
            m_state &= ~EN_STATE_ENABLE;

            return false;
        }

        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            picker->updatePickers();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::enableForce()
    {
        if( this->isActivate() == true )
        {
            return true;
        }

        m_state |= EN_STATE_ENABLE;

        Node * parent = this->getParent();

        if( parent != nullptr )
        {
            if( parent->isActivate() == false )
            {
                return true;
            }
        }

        IntrusivePtrScope ankh( this );

        if( this->activate() == false )
        {
            m_state &= ~EN_STATE_ENABLE;

            return false;
        }

        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            picker->updatePickers();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::disable()
    {
        IntrusivePtrScope ankh( this );

        this->deactivate();

        m_state &= ~EN_STATE_ENABLE;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::addTransformationRelation_( Node * _parent, EHierarchyInsert _hint )
    {
        MENGINE_UNUSED( _hint );

        TransformationInterface * transformation = this->getTransformation();

        if( transformation != nullptr )
        {
            TransformationInterface * newParentTransformation = _parent->getTransformation();

            transformation->setRelationTransformation( newParentTransformation );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::refreshTransformationRelation_( EHierarchyInsert _hint )
    {
        MENGINE_UNUSED( _hint );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removeTransformationRelation_()
    {
        TransformationInterface * transformation = this->getTransformation();

        if( transformation != nullptr )
        {
            transformation->removeRelationTransformation();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::addRenderRelation_( Node * _parent, EHierarchyInsert _hint )
    {
        if( _hint != Mengine::EHierarchyInsert::EHI_UNKNOWN )
        {
            RenderInterface * renderSelf = this->getRender();
            RenderInterface * renderParent = _parent->getRender();

            if( renderSelf != nullptr && renderParent != nullptr )
            {
                switch( _hint )
                {
                case Mengine::EHierarchyInsert::EHI_BACK:
                    renderSelf->setRelationRender( renderParent );
                    break;
                case Mengine::EHierarchyInsert::EHI_FRONT:
                    renderSelf->setRelationRenderFront( renderParent );
                    break;
                default:
                    break;
                }

                return;
            }
        }

        Node * nodeRenderRelative;
        RenderInterface * renderRelative = Helper::getNodeRenderInheritance( _parent, &nodeRenderRelative );

        if( renderRelative != nullptr )
        {
            renderRelative->clearRenderChildren();

            nodeRenderRelative->foreachRenderCloseChildren_( [renderRelative]( RenderInterface * _render )
            {
                _render->setRelationRender( renderRelative );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::refreshRenderRelation_( EHierarchyInsert _hint )
    {
        MENGINE_UNUSED( _hint );

        Node * nodeRenderRelative;
        RenderInterface * renderRelative = Helper::getNodeRenderInheritance( this, &nodeRenderRelative );

        if( renderRelative != nullptr )
        {
            renderRelative->clearRenderChildren();

            nodeRenderRelative->foreachRenderCloseChildren_( [renderRelative]( RenderInterface * _render )
            {
                _render->setRelationRender( renderRelative );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removeRenderRelation_()
    {
        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            render->removeRelationRender();
        }
        else
        {
            Node * parent = this->getParent();

            if( Helper::hasNodeRenderInheritance( parent ) == true )
            {
                this->foreachRenderCloseChildren_( []( RenderInterface * _render )
                {
                    _render->removeRelationRender();
                } );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::addPickerRelation_( Node * _parent, EHierarchyInsert _hint )
    {
        if( _hint != Mengine::EHierarchyInsert::EHI_UNKNOWN )
        {
            PickerInterface * pickerSelf = this->getPicker();
            PickerInterface * pickerParent = _parent->getPicker();

            if( pickerSelf != nullptr && pickerParent != nullptr )
            {
                switch( _hint )
                {
                case Mengine::EHierarchyInsert::EHI_BACK:
                    pickerSelf->setRelationPicker( pickerParent );
                    break;
                case Mengine::EHierarchyInsert::EHI_FRONT:
                    pickerSelf->setRelationPickerFront( pickerParent );
                    break;
                default:
                    break;
                }

                return;
            }
        }

        Node * nodePickerParent;
        PickerInterface * pickerParent = Helper::getNodePickerInheritance( _parent, &nodePickerParent );

        if( pickerParent != nullptr )
        {
            pickerParent->clearPickerChildren();

            nodePickerParent->foreachPickerCloseChildren_( [pickerParent]( PickerInterface * _picker )
            {
                _picker->setRelationPicker( pickerParent );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::refreshPickerRelation_( EHierarchyInsert _hint )
    {
        MENGINE_UNUSED( _hint );

        Node * nodePickerParent;
        PickerInterface * pickerParent = Helper::getNodePickerInheritance( this, &nodePickerParent );

        if( pickerParent != nullptr )
        {
            pickerParent->clearPickerChildren();

            nodePickerParent->foreachPickerCloseChildren_( [pickerParent]( PickerInterface * _picker )
            {
                _picker->setRelationPicker( pickerParent );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removePickerRelation_()
    {
        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            picker->removeRelationPicker();
        }
        else
        {
            Node * parent = this->getParent();

            if( Helper::hasNodePickerInheritance( parent ) == true )
            {
                this->foreachPickerCloseChildren_( []( PickerInterface * _childPicker )
                {
                    _childPicker->removeRelationPicker();
                } );
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    HashType Node::getHierarchyHash() const
    {
        const ConstString & name = this->getName();

        HashType hash = name.hash();

        for( Node * parent = this->getParent(); parent != nullptr; parent = parent->getParent() )
        {
            const ConstString & parent_name = parent->getName();

            HashType parent_hash = parent_name.hash();

            hash = (hash << 5) | (hash >> (sizeof( HashType ) * 8 - 5));
            hash ^= parent_hash;
        }

        return hash;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Node::findUniqueChild( UniqueId _uniqueIdentity ) const
    {
        NodePtr found;
        this->foreachChildrenUnslugBreak( [&found, _uniqueIdentity]( const NodePtr & _child )
        {
            UniqueId childUID = _child->getUniqueIdentity();

            if( childUID != _uniqueIdentity )
            {
                return true;
            }

            found = _child;

            return false;
        } );

        return found;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachRenderCloseChildren_( const LambdaRenderCloseChildren & _lambda )
    {
        this->foreachChildren( [&_lambda]( const NodePtr & _child )
        {
            RenderInterface * render = _child->getRender();

            if( render != nullptr )
            {
                _lambda( render );
            }
            else
            {
                _child->foreachRenderCloseChildren_( _lambda );
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachRenderReverseCloseChildren_( const LambdaRenderCloseChildren & _lambda )
    {
        this->foreachChildrenReverse( [&_lambda]( const NodePtr & _child )
        {
            RenderInterface * render = _child->getRender();

            if( render != nullptr )
            {
                _lambda( render );
            }
            else
            {
                _child->foreachRenderReverseCloseChildren_( _lambda );
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachPickerCloseChildren_( const LambdaPickerCloseChildren & _lambda )
    {
        this->foreachChildren( [&_lambda]( const NodePtr & _child )
        {
            PickerInterface * picker = _child->getPicker();

            if( picker != nullptr )
            {
                _lambda( picker );
            }
            else
            {
                _child->foreachPickerCloseChildren_( _lambda );
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::foreachPickerReverseCloseChildren_( const LambdaPickerCloseChildren & _lambda )
    {
        this->foreachChildrenReverse( [&_lambda]( const NodePtr & _child )
        {
            PickerInterface * picker = _child->getPicker();

            if( picker != nullptr )
            {
                _lambda( picker );
            }
            else
            {
                _child->foreachPickerReverseCloseChildren_( _lambda );
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::visitChildren( const VisitorPtr & _visitor )
    {
        IntrusivePtrScope ankh( this );

        this->foreachChildrenSlug( [_visitor]( const NodePtr & _child )
        {
            _child->visit( _visitor );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::visitThree( const VisitorPtr & _visitor )
    {
        IntrusivePtrScope ankh( this );

        this->foreachChildrenSlug( [_visitor]( const NodePtr & _child )
        {
            _child->visit( _visitor );

            _child->visitChildren( _visitor );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::freeze( bool _value )
    {
        if( this->isFreeze() == _value )
        {
            return;
        }
        
        if( _value == true )
        {
            m_state |= EN_STATE_FREEZE;
        }
        else
        {
            m_state &= ~EN_STATE_FREEZE;
        }

        this->_freeze( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_freeze( bool _value )
    {
        IntrusivePtrScope ankh( this );

        this->foreachChildrenSlug( [_value]( const NodePtr & _child )
        {
            _child->freeze( _value );
        } );

        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            picker->setPickerFreeze( _value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::_activate()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::_afterActivate()
    {
        UpdationInterface * updation = this->getUpdation();

        if( updation != nullptr )
        {
            uint32_t deep = this->getLeafDeep();

            updation->activate( EUM_NODE_BASE, deep );
        }

        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            render->setRenderEnable( true );
        }

        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            picker->setPickerEnable( true );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_deactivate()
    {
        UpdationInterface * updation = this->getUpdation();

        if( updation != nullptr )
        {
            updation->deactivate();
        }

        RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            render->setRenderEnable( false );
        }

        PickerInterface * picker = this->getPicker();

        if( picker != nullptr )
        {
            picker->setPickerEnable( false );
        }

        if( this->availableAffectorHub() == true )
        {
            const AffectorHubInterfacePtr & affectorHub = this->getAffectorHub();

            affectorHub->stopAllAffectors();
            this->clearAffectorHub();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_afterDeactivate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::compile()
    {
        if( Compilable::compile() == false )
        {
            LOGGER_ERROR( "node '%s' type '%s' not compiled"
                , this->getName().c_str()
                , this->getType().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::release()
    {
        if( this->isActivate() == true )
        {
            this->deactivate();
        }

        this->foreachChildrenSlug( []( const NodePtr & _child )
        {
            _child->release();
        } );

        Compilable::release();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_recompile()
    {
        if( this->isEnable() == false )
        {
            return;
        }

        if( this->isActivate() == true )
        {
            return;
        }

        this->activate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_uncompile()
    {
        if( this->isEnable() == false )
        {
            return;
        }

        if( this->isActivate() == false )
        {
            return;
        }

        this->deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchySetParent( Node * _newParent, EHierarchyInsert _hint )
    {
        this->addTransformationRelation_( _newParent, _hint );
        this->addRenderRelation_( _newParent, _hint );
        this->addPickerRelation_( _newParent, _hint );

        UpdationInterface * updation = this->getUpdation();

        if( _newParent != nullptr && updation != nullptr )
        {
            uint32_t deep = this->getLeafDeep();

            updation->replace( deep );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchyRemoveParent( Node * _oldParent )
    {
        MENGINE_UNUSED( _oldParent );

        this->removeTransformationRelation_();
        this->removeRenderRelation_();
        this->removePickerRelation_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchyRefreshChild( const NodePtr & _node, EHierarchyInsert _hint )
    {
        MENGINE_UNUSED( _node );

        this->refreshTransformationRelation_( _hint );
        this->refreshRenderRelation_( _hint );
        this->refreshPickerRelation_( _hint );
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchyChangeParent( Node * _oldParent, Node * _newParent )
    {
        MENGINE_UNUSED( _oldParent );
        MENGINE_UNUSED( _newParent );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchyAddChild( const NodePtr & _node )
    {
        TransformationInterface * transformation = _node->getTransformation();

        if( transformation != nullptr )
        {
            transformation->invalidateWorldMatrix();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchyRemoveChild( const NodePtr & _node )
    {
        MENGINE_UNUSED( _node );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    AffectorHubProviderInterface * Node::getAffectorHubProvider()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    EUpdateMode Node::getAffectorableUpdatableMode() const
    {
        return EUM_NODE_AFFECTOR;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Node::getAffectorableUpdatableLeafDeep() const
    {
        uint32_t deep = this->getLeafDeep();

        return deep;
    }
    //////////////////////////////////////////////////////////////////////////
}
