#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"
#	include "Kernel/Surface.h"

#	include "Kernel/RenderCameraProjection.h"
#	include "Kernel/RenderViewport.h"
#   include "Kernel/MatrixProxy.h"

#   include "Menge/HotSpotPolygon.h"
#   include "Menge/ParticleEmitter2.h"
#   include "Menge/TextField.h"

#   include "ResourceMovie2.h"
#   include "Movie2Slot.h"

#   include "stdex/stl_map.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2EventReceiver
        : public AnimatableEventReceiver
    {
    public:
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
		~Movie2();

    public:
		void setResourceMovie2( const ResourceMovie2Ptr & _resourceMovie );
		const ResourceMovie2Ptr & getResourceMovie2() const;

	public:
		void setCompositionName( const ConstString & _name );
		const ConstString & getCompositionName() const;

	public:
		float getDuration() const;
		void setWorkAreaFromEvent( const ConstString & _eventName );
		void removeWorkArea();

		void playSubComposition( const ConstString & _name );
		void stopSubComposition( const ConstString & _name );

    public:
        void setEnableMovieLayers( const ConstString & _name, bool _enable );

	protected:
		bool _play( float _time ) override;
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
		void _render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state ) override;

	protected:
		void _changeParent( Node * _oldParent, Node * _newParent ) override;
		void _addChild( Node * _node ) override;
		void _removeChild( Node * _node ) override;

	public:
		void addSurface( const SurfacePtr & _surface );
        void removeSurface( const SurfacePtr & _surface );

    public:
        void addParticle( ParticleEmitter2 * _particleEmitter );
        void removeParticle( ParticleEmitter2 * _particleEmitter );

    public:
        void addSlot( const ConstString & _name, Movie2Slot * _slot );
        Movie2Slot * getSlot( const ConstString & _name ) const;
        bool hasSlot( const ConstString & _name ) const;

    public:
        void addSocketShape( const ConstString & _name, HotSpotPolygon * _hotspot );
        HotSpot * getSocket( const ConstString & _name ) const;
        bool hasSocket( const ConstString & _name ) const;

    public:
        void addText( const ConstString & _name, TextField * _text );

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

        typedef stdex::map<ConstString, HotSpot *> TMapSockets;
        TMapSockets m_sockets;

        typedef stdex::map<ConstString, TextField *> TMapTexts;
        TMapTexts m_texts;

        typedef stdex::vector<ParticleEmitter2 *> TVectorParticleEmitter2s;
        TVectorParticleEmitter2s m_particleEmitters;

        typedef stdex::vector<MatrixProxy *> TVectorMatrixProxies;
        TVectorMatrixProxies m_matrixProxies;
	};
}