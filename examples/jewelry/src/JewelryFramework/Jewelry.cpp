#include "Jewelry.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"
#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

#include "Engine/Engine.h"
#include "Engine/HotSpotCircle.h"

#include "Kernel/Document.h"
#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/ShapeCircle.h"
#include "Kernel/ShapeQuadFixed.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const Color jewelry_colors[9] = {
        Color( 1.f, 0.f, 0.f, 1.f ), Color( 0.f, 1.f, 0.f, 1.f ), Color( 0.f, 0.f, 1.f, 1.f ),
        Color( 1.f, 1.f, 0.f, 1.f ), Color( 0.f, 1.f, 1.f, 1.f ), Color( 1.f, 0.f, 1.f, 1.f ),
        Color( 1.f, 0.5f, 0.5f, 1.f ), Color( 0.5f, 1.f, 1.f, 1.f ), Color( 1.f, 0.5f, 1.f, 1.f )
    };
    //////////////////////////////////////////////////////////////////////////
    Jewelry::Jewelry()
        : m_size( 50.f )
        , m_stride( 10.f )
        , m_row( 0 )
        , m_column( 0 )
        , m_type( 0 )
        , m_super( EJSUPER_NORMAL )
        , m_state( EJS_NONE )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Jewelry::~Jewelry()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Jewelry::getType() const
    {
        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
    EJewelrySuper Jewelry::getSuper() const
    {
        return m_super;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::setColumn( uint32_t _column )
    {
        m_column = _column;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Jewelry::getColumn() const
    {
        return m_column;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::setRow( uint32_t _row )
    {
        m_row = _row;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Jewelry::getRow() const
    {
        return m_row;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::bomb( const GOAP::SourceInterfacePtr & _source )
    {
        m_super = EJSUPER_BOMB;

        auto && [source_disappear, source_appear] = Cook::addParallel<2>( _source );
        Cook::addTransformationScaleTime( source_disappear, m_nodeActive, m_nodeActive, nullptr, mt::vec3f( 0.5f, 0.5f, 0.5f ), 200.f, ETASK_FLAG_NONE, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addNodeEnable( source_appear, m_nodeBomb );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Jewelry::isBomb() const
    {
        return m_nodeBomb->isEnable();
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::makeNodeActive_()
    {
        SurfaceSolidColorPtr surface = Helper::generateSurfaceSolidColor( MENGINE_DOCUMENT_FACTORABLE );

        Color jewelry_color = jewelry_colors[m_type];

        surface->setSolidColor( jewelry_color );

        float width = m_size * 0.5f;
        surface->setSolidSize( {width, width} );

        ShapeCirclePtr shape = Helper::generateShapeCircle( MENGINE_DOCUMENT_FACTORABLE );

        shape->setSurface( surface );

        m_node->addChild( shape );

        m_nodeActive = shape;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::makeNodeBlock_()
    {
        SurfaceSolidColorPtr surface = Helper::generateSurfaceSolidColor( MENGINE_DOCUMENT_FACTORABLE );

        Color jewelry_color = jewelry_colors[m_type] * 0.5f;

        surface->setSolidColor( jewelry_color );

        float full_size = m_size + 10.f;
        surface->setSolidSize( {full_size, full_size} );

        ShapeQuadFixedPtr shape = Helper::generateShapeQuadFixed( MENGINE_DOCUMENT_FACTORABLE );

        shape->setSurface( surface );

        TransformationInterface * shapeTransformation = shape->getTransformation();
        shapeTransformation->setLocalOrigin( {full_size * 0.5f, full_size * 0.5f, 0.f} );

        shape->disable();

        m_node->addChild( shape );

        m_nodeBlock = shape;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::makeNodeBomb_()
    {
        SurfaceSolidColorPtr surface = Helper::generateSurfaceSolidColor( MENGINE_DOCUMENT_FACTORABLE );

        Color jewelry_color = jewelry_colors[m_type] * 0.5f;

        surface->setSolidColor( jewelry_color );

        float full_size = m_size * 0.5f + 10.f;
        surface->setSolidSize( {full_size, full_size} );

        ShapeCirclePtr shape = Helper::generateShapeCircle( MENGINE_DOCUMENT_FACTORABLE );

        shape->setSurface( surface );

        shape->disable();

        m_node->addChild( shape );

        m_nodeBomb = shape;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::makePickerable_()
    {
        HotSpotCirclePtr hotspot = Helper::generateHotSpotCircle( MENGINE_DOCUMENT_FACTORABLE );

        float radius = m_size * 0.5f;
        hotspot->setRadius( radius );

        m_node->addChild( hotspot );

        m_pickerable = hotspot;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::block( const GOAP::SourceInterfacePtr & _source )
    {
        if( this->isDead() == true )
        {
            return;
        }

        if( this->isBlock() == true )
        {
            return;
        }

        m_state |= EJS_BLOCK;

        Cook::addTransformationScaleTime( _source, m_nodeActive, m_nodeActive, nullptr, mt::vec3f( 1.0f, 1.0f, 1.0f ), 100.f, ETASK_FLAG_NONE, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addNodeEnable( _source, m_nodeBlock );
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::dead( const GOAP::SourceInterfacePtr & _source )
    {
        if( this->isDead() == true )
        {
            return;
        }

        m_state |= EJS_DEAD;

        m_matrix->removeJewelry( JewelryPtr::from( this ) );

        Cook::addTransformationScaleTime( _source, m_nodeActive, m_nodeActive, nullptr, mt::vec3f( 0.0f, 0.0f, 0.0f ), 200.f, ETASK_FLAG_NONE, MENGINE_DOCUMENT_FACTORABLE );

        Cook::addFunction( _source, this, &Jewelry::finalize );
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::explosive( const GOAP::SourceInterfacePtr & _source )
    {
        if( this->isDead() == true )
        {
            return;
        }

        m_state |= EJS_DEAD;

        m_matrix->removeJewelry( JewelryPtr::from( this ) );

        Cook::addTransformationScaleTime( _source, m_nodeBomb, m_nodeBomb, nullptr, mt::vec3f( 1.2f, 1.2f, 1.2f ), 200.f, ETASK_FLAG_NONE, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addTransformationScaleTime( _source, m_nodeActive, m_nodeActive, nullptr, mt::vec3f( 0.0f, 0.0f, 0.0f ), 200.f, ETASK_FLAG_NONE, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( _source, this, &Jewelry::finalize );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Jewelry::isBlock() const
    {
        return (m_state & EJS_BLOCK) == EJS_BLOCK;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Jewelry::isMove() const
    {
        return (m_state & EJS_MOVE) == EJS_MOVE;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Jewelry::isDead() const
    {
        return (m_state & EJS_DEAD) == EJS_DEAD;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Jewelry::initialize( EJewelrySuper _super, uint32_t _type, const JewelryMatrixPtr & _matrix, uint32_t _column, uint32_t _row, float _size, float _stride )
    {
        m_matrix = _matrix;

        m_column = _column;
        m_row = _row;
        m_type = _type;
        m_super = _super;
        m_size = _size;
        m_stride = _stride;

        NodePtr node = Helper::generateNode( MENGINE_DOCUMENT_FACTORABLE );

        m_node = node;

        this->makeNodeActive_();
        this->makeNodeBlock_();
        this->makeNodeBomb_();
        this->makePickerable_();

        TransformationInterface * transformation = m_node->getTransformation();

        transformation->setLocalPosition( {float( _column ) * (m_size + m_stride), float( _row ) * (m_size + m_stride), 0.f} );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::finalize()
    {
        m_state |= EJS_DEAD;

        m_node->removeFromParent();
        m_node = nullptr;

        m_nodeActive->removeFromParent();
        m_nodeActive = nullptr;

        m_nodeBlock->removeFromParent();
        m_nodeBlock = nullptr;

        m_nodeBomb->removeFromParent();
        m_nodeBomb = nullptr;

        m_pickerable = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Jewelry::getNode() const
    {
        return m_node;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Jewelry::getNodeActive() const
    {
        return m_nodeActive;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Jewelry::getNodeBomb() const
    {
        return m_nodeBomb;
    }
    //////////////////////////////////////////////////////////////////////////
    const PickerablePtr & Jewelry::getPickerable() const
    {
        return m_pickerable;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::move( const GOAP::SourceInterfacePtr & _source, uint32_t _row, float _time )
    {
        if( this->isDead() == true )
        {
            return;
        }

        m_matrix->moveJewelry( m_column, _row, JewelryPtr::from( this ) );

        mt::vec3f new_position;
        new_position.x = float( m_column ) * 60.f;
        new_position.y = float( _row ) * 60.f;
        new_position.z = 0.f;

        m_state |= EJS_MOVE;

        Cook::addTransformationTranslateTime( _source, m_node, m_node, nullptr, new_position, _time, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::stop()
    {
        if( this->isDead() == true )
        {
            return;
        }

        m_state &= ~EJS_MOVE;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::pickHand( const GOAP::SourceInterfacePtr & _source )
    {
        if( this->isDead() == true )
        {
            return;
        }

        Cook::addTransformationScaleTime( _source, m_nodeActive, m_nodeActive, nullptr, mt::vec3f( 1.2f, 1.2f, 1.2f ), 200.f, ETASK_FLAG_NONE, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::unpickHand( const GOAP::SourceInterfacePtr & _source )
    {
        if( this->isDead() == true )
        {
            return;
        }

        Cook::addTransformationScaleTime( _source, m_nodeActive, m_nodeActive, nullptr, mt::vec3f( 1.0f, 1.0f, 1.0f ), 200.f, ETASK_FLAG_NONE, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
}