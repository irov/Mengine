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
        virtual void visitMovieLayer( class Movie2 * _movie, const ConstString & _name, Node * _node ) = 0;
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

    public:
        void addSprite( const ConstString & _name, ShapeQuadFixed * _sprite );
        ShapeQuadFixed * getSprite( const ConstString & _name ) const;
        bool hasSprite( const ConstString & _name ) const;

    public:
        void addParticle( const ConstString & _name, ParticleEmitter2 * _particleEmitter );
        ParticleEmitter2 * getParticle( const ConstString & _name ) const;
        bool hasParticle( const ConstString & _name ) const;

    public:
        void addSlot( const ConstString & _name, Movie2Slot * _slot );
        Movie2Slot * getSlot( const ConstString & _name ) const;
        bool hasSlot( const ConstString & _name ) const;

    public:
        void addSocket( const ConstString & _name, HotSpotPolygon * _hotspot );
        HotSpotPolygon * getSocket( const ConstString & _name ) const;
        bool hasSocket( const ConstString & _name ) const;
        void visitSockets( VisitorMovie2Layer * _visitor );

    public:
        void addText( const ConstString & _name, TextField * _text );
        TextField * getText( const ConstString & _name ) const;
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

        typedef stdex::map<ConstString, Movie2Slot *> TMapSlots;
        TMapSlots m_slots;

        typedef stdex::map<ConstString, HotSpotPolygon *> TMapSockets;
        TMapSockets m_sockets;

        typedef stdex::map<ConstString, TextField *> TMapTexts;
        TMapTexts m_texts;

        typedef stdex::map<ConstString, ShapeQuadFixed *> TMapSprites;
        TMapSprites m_sprites;

        typedef stdex::map<ConstString, ParticleEmitter2 *> TMapParticleEmitter2s;
        TMapParticleEmitter2s m_particleEmitters;

        typedef stdex::vector<MatrixProxy *> TVectorMatrixProxies;
        TVectorMatrixProxies m_matrixProxies;
	};
}