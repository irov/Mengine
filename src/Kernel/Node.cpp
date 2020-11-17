#include "Kernel/Node.h"

#include "Interface/TimelineServiceInterface.h"

#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/UpdationInterface.h"

#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/IntrusivePtrScope.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/NodePickerHierarchy.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Node::Node()
        : m_uniqueIdentity( 0 )
        , m_active( false )
        , m_deactivating( false )
        , m_afterActive( false )
        , m_enable( true )
        , m_freeze( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Node::~Node()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_destroy()
    {
        this->release();

        this->destroyChildren( []( const NodePtr & )
        {} );

        this->unwrap();
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

        if( m_active == true )
        {
            return true;
        }

        if( this->_activate() == false )
        {
            this->release();

            return false;
        }

        IntrusivePtrScope ankh( this );

        m_active = true;

        if( this->foreachChildrenSlugBreak( []( const NodePtr & _hierarchy )
        {
            bool result = _hierarchy->activate();

            return result;
        } ) == false )
        {
            m_active = false;

            this->release();

            return false;
        }

        if( m_active == false )
        {
            this->release();

            return false;
        }

        m_afterActive = true;

        if( this->_afterActivate() == false )
        {
            m_afterActive = false;
            m_active = false;

            this->release();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::deactivate()
    {
        if( m_active == false )
        {
            return;
        }

        if( m_deactivating == true )
        {
            return;
        }

        m_deactivating = true;

        if( m_afterActive == false )
        {
            LOGGER_ERROR( "node '%s' invalid deactivate in 'activate state'"
                , this->getName().c_str()
            );
        }

        m_afterActive = false;

        this->_deactivate();

        this->foreachChildrenSlug( []( const NodePtr & _hierarchy )
        {
            NodePtr child = _hierarchy;

            child->deactivate();
        } );

        m_active = false;
        m_deactivating = false;

        this->_afterDeactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Node::enable()
    {
        if( this->isActivate() == true )
        {
            return true;
        }

        m_enable = true;

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

        if( this->activate() == false )
        {
            m_enable = false;

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

        m_enable = true;

        Node * parent = this->getParent();

        if( parent != nullptr )
        {
            if( parent->isActivate() == false )
            {
                return true;
            }
        }

        if( this->activate() == false )
        {
            m_enable = false;

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

        m_enable = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::removeRelationRender_()
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
    void Node::refreshRenderRelation_( Node * _parent )
    {
        Node * nodeRenderParent;
        RenderInterface * renderParent = Helper::getNodeRenderInheritance( _parent, &nodeRenderParent );

        if( renderParent != nullptr )
        {
            renderParent->clearRenderChildren();

            nodeRenderParent->foreachRenderCloseChildren_( [renderParent]( RenderInterface * _render )
            {
                _render->setRelationRender( renderParent );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::refreshPickerRelation_( Node * _parent )
    {
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
    void Node::setUniqueIdentity( UniqueId _uniqueIdentity )
    {
        m_uniqueIdentity = _uniqueIdentity;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId Node::getUniqueIdentity() const
    {
        return m_uniqueIdentity;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Node::findUniqueChild( uint32_t _uniqueIdentity ) const
    {
        NodePtr found;

        this->foreachChildrenUnslugBreak( [&found, _uniqueIdentity]( const NodePtr & _child )
        {
            uint32_t childUID = _child->getUniqueIdentity();

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
    void Node::removeRelationPicker_()
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
        this->visit( _visitor );

        IntrusivePtrScope ankh( this );

        this->foreachChildrenSlug( [_visitor]( const NodePtr & _child )
        {
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

        m_freeze = _value;

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
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::setSpeedFactor( float _speedFactor )
    {
        MENGINE_UNUSED( _speedFactor );

        //TODO: REMOVE
    }
    //////////////////////////////////////////////////////////////////////////
    float Node::getSpeedFactor() const
    {
        return 0.f;
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
        bool result = Compilable::compile();

        return result;
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
        if( m_enable == false )
        {
            return;
        }

        if( m_active == true )
        {
            return;
        }

        this->activate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_uncompile()
    {
        if( m_enable == false )
        {
            return;
        }

        if( m_active == false )
        {
            return;
        }

        this->deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_invalidateWorldMatrix() const
    {
        const RenderInterface * render = this->getRender();

        if( render != nullptr )
        {
            render->invalidateBoundingBox();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchySetParent( Node * _newParent )
    {
        this->setRelationTransformation( _newParent );

        this->refreshRenderRelation_( _newParent );
        this->refreshPickerRelation_( _newParent );

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

        this->removeRelationTransformation();
        this->removeRelationRender_();
        this->removeRelationPicker_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Node::_hierarchyRefreshChild( const NodePtr & _node )
    {
        MENGINE_UNUSED( _node );

        this->refreshRenderRelation_( this );
        this->refreshPickerRelation_( this );
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
        MENGINE_UNUSED( _node );

        //Empty
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
}
