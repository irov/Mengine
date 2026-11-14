#include "MCPSceneContext.h"

#include "Interface/PlayerServiceInterface.h"
#include "Interface/RenderInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/TransformationInterface.h"

#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/NodeScreenPosition.h"
#include "Kernel/RenderCameraHelper.h"
#include "Kernel/RenderContext.h"
#include "Kernel/Stringstream.h"

#include "Config/StdString.h"

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
        static NodePtr findNodeByIdentity( const NodePtr & _node, UniqueId _identity )
        {
            UniqueId identity = _node->getUniqueIdentity();

            if( identity == _identity )
            {
                return _node;
            }

            NodePtr found;
            _node->foreachChildrenUnslugBreak( [&found, _identity]( const NodePtr & _child )
            {
                NodePtr node = Detail::findNodeByIdentity( _child, _identity );

                if( node == nullptr )
                {
                    return true;
                }

                found = node;

                return false;
            } );

            return found;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isNodeMatched( const NodePtr & _node, const String & _path, const ConstString & _name, const ConstString & _type, const String & _queryPath )
        {
            if( _name.empty() == false )
            {
                const ConstString & nodeName = _node->getName();

                if( _name != nodeName )
                {
                    return false;
                }
            }

            if( _type.empty() == false )
            {
                const ConstString & nodeType = Helper::getFactorableType( _node );

                if( _type != nodeType )
                {
                    return false;
                }
            }

            if( _queryPath.empty() == false )
            {
                if( _queryPath != _path )
                {
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void makeNodeRenderContext( const Node * _node, RenderContext * const _context )
        {
            const RenderContext * playerContext = PLAYER_SERVICE()
                ->getRenderContext();

            *_context = *playerContext;

            Helper::fillNodeRenderContextInheritance( _node, _context );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool getNodeScreenOrigin( const Node * _node, mt::vec2f * const _position )
        {
            const TransformationInterface * transformation = _node->getTransformation();

            if( transformation == nullptr )
            {
                return false;
            }

            Helper::getNodeScreenPosition( _node, _position );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool getNodeScreenBox( const Node * _node, mt::box2f * const _box )
        {
            const RenderInterface * render = _node->getRender();

            if( render == nullptr )
            {
                return false;
            }

            const mt::box2f * boundingBox = render->getBoundingBox();

            if( boundingBox == nullptr )
            {
                return false;
            }

            RenderContext context;
            Detail::makeNodeRenderContext( _node, &context );

            Helper::worldToScreenBox( &context, *boundingBox, _box );

            return true;
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
        m_sceneIdentity = INVALID_UNIQUE_ID;
        m_sceneGeneration = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPSceneContext::refreshGeneration()
    {
        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

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
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MCPSceneContext::getGeneration() const
    {
        return m_sceneGeneration;
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

        if( stream.fail() == true )
        {
            *_error = "invalid scene handle";

            return nullptr;
        }

        if( separator != ':' )
        {
            *_error = "invalid scene handle";

            return nullptr;
        }

        if( generation != m_sceneGeneration )
        {
            *_error = "scene handle is stale: the scene changed";

            return nullptr;
        }

        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        if( scene == nullptr )
        {
            *_error = "there is no current scene";

            return nullptr;
        }

        NodePtr node = Detail::findNodeByIdentity( scene, identity );

        if( node == nullptr )
        {
            *_error = "scene handle is stale: the node left the scene";

            return nullptr;
        }

        return node;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPSceneContext::findNodes( const jpp::object & _query, uint32_t _limit, VectorMCPNodes * const _nodes, String * const _error ) const
    {
        for( auto && [key, value] : _query )
        {
            if( value.is_type_string() == false )
            {
                *_error = "scene query values must be strings";

                return false;
            }

            if( StdString::strcmp( key, "name" ) == 0 )
            {
                continue;
            }

            if( StdString::strcmp( key, "type" ) == 0 )
            {
                continue;
            }

            if( StdString::strcmp( key, "path" ) == 0 )
            {
                continue;
            }

            *_error = "unsupported scene query key '";
            _error->append( key );
            _error->append( "', expected name, type or path" );

            return false;
        }

        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        if( scene == nullptr )
        {
            *_error = "there is no current scene";

            return false;
        }

        const Char * name = _query.get( "name", "" );
        const Char * type = _query.get( "type", "" );
        const Char * path = _query.get( "path", "" );

        ConstString queryName = Helper::stringizeString( name );
        ConstString queryType = Helper::stringizeString( type );
        String queryPath = path;

        String rootPath;
        this->findNodes_( scene, rootPath, queryName, queryType, queryPath, _limit, _nodes );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPSceneContext::findNodes_( const NodePtr & _node, const String & _path, const ConstString & _name, const ConstString & _type, const String & _queryPath, uint32_t _limit, VectorMCPNodes * const _nodes ) const
    {
        if( _nodes->size() >= _limit )
        {
            return;
        }

        if( Detail::isNodeMatched( _node, _path, _name, _type, _queryPath ) == true )
        {
            _nodes->emplace_back( _node );
        }

        _node->foreachChildren( [this, &_path, &_name, &_type, &_queryPath, _limit, _nodes]( const NodePtr & _child )
        {
            const ConstString & childName = _child->getName();

            String childPath = _path;

            if( childPath.empty() == false )
            {
                childPath.append( "/" );
            }

            const Char * childNameValue = childName.c_str();
            childPath.append( childNameValue );

            this->findNodes_( _child, childPath, _name, _type, _queryPath, _limit, _nodes );
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPSceneContext::makeNodePath_( const NodePtr & _node, String * const _path ) const
    {
        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        const Node * sceneNode = scene.get();

        for( const Node * node = _node.get(); node != nullptr && node != sceneNode; node = node->getParent() )
        {
            const ConstString & name = node->getName();

            if( _path->empty() == false )
            {
                _path->insert( 0, "/" );
            }

            const Char * nameValue = name.c_str();
            _path->insert( 0, nameValue );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPSceneContext::getNodeScreenPoint( const NodePtr & _node, mt::vec2f * const _point ) const
    {
        const Node * node = _node.get();

        mt::box2f screenBox;
        if( Detail::getNodeScreenBox( node, &screenBox ) == true )
        {
            *_point = (screenBox.minimum + screenBox.maximum) * 0.5f;

            return true;
        }

        if( Detail::getNodeScreenOrigin( node, _point ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object MCPSceneContext::makeNodePropertiesJSON( const NodePtr & _node ) const
    {
        String handle = this->makeNodeHandle( _node );
        UniqueId identity = _node->getUniqueIdentity();
        ConstString type = Helper::getFactorableType( _node );
        ConstString name = _node->getName();
        bool enabled = _node->isEnable();
        bool active = _node->isActivate();
        bool frozen = _node->isFreeze();

        String path;
        this->makeNodePath_( _node, &path );

        jpp::object result = jpp::make_object();
        result.set( "handle", handle );
        result.set( "id", identity );
        result.set( "type", type );
        result.set( "name", name );
        result.set( "path", path );
        result.set( "enabled", enabled );
        result.set( "active", active );
        result.set( "frozen", frozen );

        const Node * node = _node.get();

        const TransformationInterface * transformation = _node->getTransformation();

        if( transformation != nullptr )
        {
            const mt::vec3f & localPosition = transformation->getLocalPosition();
            const mt::vec3f & localScale = transformation->getLocalScale();
            const mt::vec3f & localOrientation = transformation->getLocalOrientation();
            mt::vec3f worldPosition = transformation->getWorldPosition();
            jpp::array position = Detail::makeMCPVec3JSON( localPosition );
            jpp::array scale = Detail::makeMCPVec3JSON( localScale );
            jpp::array orientation = Detail::makeMCPVec3JSON( localOrientation );
            jpp::array world = Detail::makeMCPVec3JSON( worldPosition );
            jpp::object transform = jpp::make_object();
            transform.set( "position", position );
            transform.set( "scale", scale );
            transform.set( "orientation", orientation );
            transform.set( "worldPosition", world );
            result.set( "transform", transform );

            mt::vec2f screenPosition;
            if( Detail::getNodeScreenOrigin( node, &screenPosition ) == true )
            {
                jpp::array screenPositionValue = jpp::make_array();
                screenPositionValue.push_back( screenPosition.x );
                screenPositionValue.push_back( screenPosition.y );
                result.set( "screenPosition", screenPositionValue );
            }
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

            mt::box2f screenBox;
            if( Detail::getNodeScreenBox( node, &screenBox ) == true )
            {
                jpp::array screenBoxValue = jpp::make_array();
                screenBoxValue.push_back( screenBox.minimum.x );
                screenBoxValue.push_back( screenBox.minimum.y );
                screenBoxValue.push_back( screenBox.maximum.x );
                screenBoxValue.push_back( screenBox.maximum.y );
                result.set( "screenBox", screenBoxValue );
            }
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object MCPSceneContext::makeNodeJSON( const NodePtr & _node, uint32_t _depth, uint32_t _maxDepth, bool _includeDisabled, uint32_t * const _nodeCount ) const
    {
        ++(*_nodeCount);

        jpp::object result = this->makeNodePropertiesJSON( _node );
        jpp::array children = jpp::make_array();

        if( _depth < _maxDepth )
        {
            _node->foreachChildren( [this, &children, _depth, _maxDepth, _includeDisabled, _nodeCount]( const NodePtr & _child )
            {
                bool enabled = _child->isEnable();

                if( _includeDisabled == false && enabled == false )
                {
                    return;
                }

                uint32_t childDepth = _depth + 1;
                jpp::object child = this->makeNodeJSON( _child, childDepth, _maxDepth, _includeDisabled, _nodeCount );
                children.push_back( child );
            } );
        }

        result.set( "children", children );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
}
