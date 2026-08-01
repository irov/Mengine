#include "MCPSceneContext.h"

#include "Interface/SceneServiceInterface.h"
#include "Interface/TransformationInterface.h"
#include "Interface/RenderInterface.h"

#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static jpp::array makeMCPVec3JSON( const mt::vec3f & _value )
        {
            jpp::array value = jpp::make_array();
            value.push_back( _value.x );
            value.push_back( _value.y );
            value.push_back( _value.z );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MCPSceneContext::MCPSceneContext()
        : m_sceneIdentity( INVALID_UNIQUE_ID )
        , m_sceneGeneration( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPSceneContext::finalize()
    {
        m_sceneNodes.clear();
        m_sceneIdentity = INVALID_UNIQUE_ID;
        m_sceneGeneration = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPSceneContext::refreshGeneration()
    {
        const ScenePtr & scene = SCENE_SERVICE()->getCurrentScene();
        UniqueId sceneIdentity = scene != nullptr ? scene->getUniqueIdentity() : INVALID_UNIQUE_ID;

        if( m_sceneIdentity == sceneIdentity )
        {
            return;
        }

        m_sceneIdentity = sceneIdentity;
        ++m_sceneGeneration;

        if( m_sceneGeneration == 0 )
        {
            ++m_sceneGeneration;
        }

        m_sceneNodes.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPSceneContext::clearNodes()
    {
        m_sceneNodes.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MCPSceneContext::getGeneration() const
    {
        uint32_t generation = m_sceneGeneration;

        return generation;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MCPSceneContext::getNodeCount() const
    {
        size_t count = m_sceneNodes.size();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    const Map<UniqueId, NodePtr> & MCPSceneContext::getNodes() const
    {
        const Map<UniqueId, NodePtr> & nodes = m_sceneNodes;

        return nodes;
    }
    //////////////////////////////////////////////////////////////////////////
    String MCPSceneContext::makeNodeHandle( const NodePtr & _node ) const
    {
        UniqueId identity = _node->getUniqueIdentity();
        Stringstream stream;
        stream << m_sceneGeneration << ':' << identity;

        String handle = stream.str();

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr MCPSceneContext::resolveNode( const Char * _handle, String * const _error ) const
    {
        uint32_t generation = 0;
        UniqueId identity = INVALID_UNIQUE_ID;
        Char separator = '\0';

        IStringstream stream( _handle );
        stream >> generation >> separator >> identity;

        if( stream.fail() == true || separator != ':' || generation != m_sceneGeneration )
        {
            *_error = "scene handle is stale";

            return nullptr;
        }

        Map<UniqueId, NodePtr>::const_iterator it = m_sceneNodes.find( identity );

        if( it == m_sceneNodes.end() )
        {
            *_error = "scene handle is stale";

            return nullptr;
        }

        NodePtr node = it->second;

        return node;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object MCPSceneContext::makeNodePropertiesJSON( const NodePtr & _node ) const
    {
        String handle = this->makeNodeHandle( _node );
        UniqueId identity = _node->getUniqueIdentity();
        ConstString type = _node->getType();
        ConstString name = _node->getName();
        bool enabled = _node->isEnable();
        bool active = _node->isActivate();
        bool frozen = _node->isFreeze();

        jpp::object result = jpp::make_object();
        result.set( "handle", handle );
        result.set( "id", identity );
        result.set( "type", type );
        result.set( "name", name );
        result.set( "enabled", enabled );
        result.set( "active", active );
        result.set( "frozen", frozen );

        const TransformationInterface * transformation = _node->getTransformation();

        if( transformation != nullptr )
        {
            const mt::vec3f & localPosition = transformation->getLocalPosition();
            const mt::vec3f & localScale = transformation->getLocalScale();
            const mt::vec3f & localOrientation = transformation->getLocalOrientation();
            jpp::array position = Detail::makeMCPVec3JSON( localPosition );
            jpp::array scale = Detail::makeMCPVec3JSON( localScale );
            jpp::array orientation = Detail::makeMCPVec3JSON( localOrientation );
            jpp::object transform = jpp::make_object();
            transform.set( "position", position );
            transform.set( "scale", scale );
            transform.set( "orientation", orientation );
            result.set( "transform", transform );
        }

        const RenderInterface * render = _node->getRender();

        if( render != nullptr )
        {
            bool renderEnabled = render->isRenderEnable();
            bool hidden = render->isHide();
            bool localHidden = render->isLocalHide();
            bool visible = renderEnabled == true && hidden == false && localHidden == false;
            ZGroupType zGroup = render->getZGroup();
            ZIndexType zIndex = render->getZIndex();

            result.set( "visible", visible );
            result.set( "renderEnabled", renderEnabled );
            result.set( "hidden", hidden );
            result.set( "zGroup", zGroup );
            result.set( "zIndex", zIndex );

            const Color & color = render->getLocalColor();
            float red = color.getR();
            float green = color.getG();
            float blue = color.getB();
            float alpha = color.getA();
            jpp::array colorValue = jpp::make_array();
            colorValue.push_back( red );
            colorValue.push_back( green );
            colorValue.push_back( blue );
            colorValue.push_back( alpha );
            result.set( "color", colorValue );
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object MCPSceneContext::makeNodeJSON( const NodePtr & _node, uint32_t _depth, uint32_t _maxDepth, bool _includeDisabled )
    {
        UniqueId identity = _node->getUniqueIdentity();
        m_sceneNodes[identity] = _node;

        jpp::object result = this->makeNodePropertiesJSON( _node );
        jpp::array children = jpp::make_array();

        if( _depth < _maxDepth )
        {
            _node->foreachChildren( [this, &children, _depth, _maxDepth, _includeDisabled]( const NodePtr & _child )
            {
                bool enabled = _child->isEnable();

                if( _includeDisabled == false && enabled == false )
                {
                    return;
                }

                jpp::object child = this->makeNodeJSON( _child, _depth + 1, _maxDepth, _includeDisabled );
                children.push_back( child );
            } );
        }

        result.set( "children", children );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
}
