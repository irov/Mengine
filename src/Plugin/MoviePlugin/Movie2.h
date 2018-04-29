#pragma once

#include "Kernel/Node.h"
#include "Kernel/Animatable.h"
#include "Kernel/Surface.h"

#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/MatrixProxy.h"

#include "Engine/ShapeQuadFixed.h"
#include "Engine/HotSpotPolygon.h"
#include "Engine/ParticleEmitter2.h"
#include "Engine/TextField.h"

#include "ResourceMovie2.h"
#include "Movie2Slot.h"

#include "stdex/stl_map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2EventReceiver
        : public AnimatableEventReceiver
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
    class VisitorMovie2Layer
    {
    public:
        virtual void visitMovieLayer( class Movie2 * _movie, uint32_t _index, Node * _node ) = 0;
    };
	//////////////////////////////////////////////////////////////////////////
	class Movie2
		: public Node
        , public Eventable
		, public Animatable
	{
        EVENT_RECEIVER( Movie2EventReceiver );

	public:
		Movie2();
		~Movie2() override;

    public:
		void setResourceMovie2( const ResourceMovie2Ptr & _resourceMovie );
		const ResourceMovie2Ptr & getResourceMovie2() const;

	public:
		bool setCompositionName( const ConstString & _compositionName );
		const ConstString & getCompositionName() const;

	public:
		float getDuration() const;
		bool setWorkAreaFromEvent( const ConstString & _eventName );
		void removeWorkArea();

    public:
        bool hasSubComposition( const ConstString & _name ) const;
		void playSubComposition( const ConstString & _name );
		void stopSubComposition( const ConstString & _name );
        void interruptSubComposition( const ConstString & _name, bool _skip );

    public:
        void setLoopSubComposition( const ConstString & _name, bool _loop );

    public:
        void setEnableMovieLayers( const ConstString & _name, bool _enable );

	protected:
		bool _play( uint32_t _enumerator, float _time ) override;
		bool _restart( uint32_t _enumerator, float _time ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( uint32_t _enumerator, float _time ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	protected:
		bool _compile() override;
		void _release() override;
		
	protected:
		bool _activate() override;
		void _deactivate() override;

		void _afterActivate() override;

	protected:
		void _setLoop( bool _value ) override;
		void _setTiming( float _timing ) override;
		float _getTiming() const override;
        void _setFirstFrame() override;
        void _setLastFrame() override;

	protected:
		void _update( float _current, float _timing ) override;

	protected:
		void _render( Mengine::RenderServiceInterface * _renderService, const RenderState * _state ) override;

	protected:
		void _changeParent( Node * _oldParent, Node * _newParent ) override;
		void _addChild( Node * _node ) override;
		void _removeChild( Node * _node ) override;

	public:
		void addSurface( const SurfacePtr & _surface );
        void removeSurface( const SurfacePtr & _surface );

    protected:
        void addSprite_( uint32_t _index, ShapeQuadFixed * _sprite );
        ShapeQuadFixed * getSprite_( uint32_t _index ) const;

    public:
        ShapeQuadFixed * findSprite( const ConstString & _name ) const;
        bool hasSprite( const ConstString & _name ) const;

    protected:
        void addParticle_( uint32_t _index, ParticleEmitter2 * _particleEmitter );
        ParticleEmitter2 * getParticle_( uint32_t _index ) const;

    public:
        ParticleEmitter2 * findParticle( const ConstString & _name ) const;
        bool hasParticle( const ConstString & _name ) const;

    protected:
        void addSlot_( uint32_t _index, Movie2Slot * _slot );
        Movie2Slot * getSlot_( uint32_t _index ) const;

    public:
        Movie2Slot * findSlot( const ConstString & _name ) const;
        bool hasSlot( const ConstString & _name ) const;

    protected:
        void addSocket_( uint32_t _index, HotSpotPolygon * _hotspot );
        HotSpotPolygon * getSocket_( uint32_t _index ) const;

    public:
        HotSpotPolygon * findSocket( const ConstString & _name ) const;
        bool hasSocket( const ConstString & _name ) const;
        void visitSockets( VisitorMovie2Layer * _visitor );

    protected:
        void addText_( uint32_t _index, TextField * _text );
        TextField * getText_( uint32_t _index ) const;

    public:
        TextField * findText( const ConstString & _name ) const;
        bool hasText( const ConstString & _name ) const;
        
    public:
        void addMatrixProxy( MatrixProxy * _matrixProxy );
        
	public:
		struct Camera
		{
			RenderCameraProjection * projection;
			RenderViewport * viewport;
		};

		Camera * addCamera( const ConstString & _name, RenderCameraProjection * _projection, RenderViewport * _viewport );
        bool removeCamera( const ConstString & _name );
		bool hasCamera( const ConstString & _name ) const;

		bool getCamera( const ConstString & _name, Camera ** _camera );

    protected:
        bool createCompositionLayers_();
        void destroyCompositionLayers_();

    protected:
        void _destroy() override;

	public:
		ResourceHolder<ResourceMovie2> m_resourceMovie2;

		ConstString m_compositionName;

		aeMovieComposition * m_composition;

		struct Mesh
		{
			TVectorRenderVertex2D vertices;
			TVectorRenderIndices indices;

			RenderMaterialInterfacePtr material;
		};

		typedef stdex::vector<Mesh> TVectorMesh;
		TVectorMesh m_meshes;

		typedef stdex::map<ConstString, Camera> TMapCamera;
		TMapCamera m_cameras;

		typedef stdex::vector<SurfacePtr> TVectorSurfaces;
		TVectorSurfaces m_surfaces;

        typedef stdex::map<uint32_t, Movie2Slot *> TMapSlots;
        TMapSlots m_slots;

        typedef stdex::map<uint32_t, HotSpotPolygon *> TMapSockets;
        TMapSockets m_sockets;

        typedef stdex::map<uint32_t, TextField *> TMapTexts;
        TMapTexts m_texts;

        typedef stdex::map<uint32_t, ShapeQuadFixed *> TMapSprites;
        TMapSprites m_sprites;

        typedef stdex::map<uint32_t, ParticleEmitter2 *> TMapParticleEmitter2s;
        TMapParticleEmitter2s m_particleEmitters;

        typedef stdex::vector<MatrixProxy *> TVectorMatrixProxies;
        TVectorMatrixProxies m_matrixProxies;

    protected:
        static ae_bool_t __movie_composition_node_provider( const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptrptr_t _nd, ae_voidptr_t _ud );
	};
}