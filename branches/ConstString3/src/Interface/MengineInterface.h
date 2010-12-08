#	pragma once

namespace Menge
{
	class ListenerInterface
	{
	};

	class CallbackInterface
	{

	};

	class IdentityInterface
	{
	public:
		virtual void setName(const String & _name) = 0;
		virtual const String & getName() const = 0;
		virtual const String & getType() const = 0;
	};

	class Allocator2DInterface
	{
	public:
		virtual const mt::vec2f & getWorldPosition() = 0;
		virtual const mt::vec2f & getWorldDirection() = 0;

		virtual const mt::vec2f & getLocalPosition() const = 0;
		virtual const mt::vec2f & getLocalDirection() const = 0;

		virtual const mt::vec2f& getOrigin() const = 0;
		virtual const mt::vec2f& getScale() const = 0;
		virtual float getAngle() const = 0;

		virtual void setLocalPosition( const mt::vec2f & _position ) = 0;
		virtual void setLocalDirection( const mt::vec2f & _direction ) = 0;
		virtual void setOrigin( const mt::vec2f& _origin ) = 0;
		virtual void setScale( const mt::vec2f& _scale ) = 0;
		virtual void setAngle( float _alpha ) = 0;
		virtual void translate( const mt::vec2f & _delta ) = 0;
	};

	class ResourceInterface
	{
	public:
		virtual bool compile() = 0;
		virtual void release() = 0;
		virtual bool isCompile() const = 0;
	};

	class RenderableInterface
	{
	public:
		virtual void hide( bool _value ) = 0;
		virtual bool isHide() const = 0;
	};

	class NodeInterface
		: virtual public IdentityInterface
		, virtual public Allocator2DInterface
		, virtual public ResourceInterface
		, virtual public RenderableInterface
	{
	public:
		virtual bool activate() = 0;
		virtual void deactivate() = 0;
		virtual bool isActivate() const = 0;

		virtual void enable() = 0;
		virtual void disable() = 0;

		virtual bool isEnable() const = 0;

		virtual void setUpdatable( bool _updatable ) = 0;
		virtual bool isUpdatable() const = 0;

		virtual void addChildren( NodeInterface * _node ) = 0;
		virtual void addChildrenFront( NodeInterface * _node ) = 0;
		virtual void removeChildren( NodeInterface * _node ) = 0;

		virtual NodeInterface * gerParent() const = 0;

		virtual NodeInterface * getChildren( const std::string & _name, bool _recursive ) const = 0;
		virtual bool isChildren( NodeInterface * _node, bool _recursive ) const = 0;

		virtual bool updatable() const = 0; 

		virtual void setListener( NodeListenerInterface * _listener ) = 0;
		virtual NodeListenerInterface * getListener() const = 0;

		virtual mt::vec2f getScreenPosition() const = 0;
	
		virtual void setLocalColor( const ColourValue& _color ) = 0;
		virtual void setLocalAlpha( float _alpha ) = 0;

		virtual const ColourValue & getWorldColor() const = 0;
		virtual const ColourValue & getLocalColor() const = 0;

		
		virtual void moveTo( float _time, const mt::vec2f& _point, NodeCallbackInterface* _cb ) = 0;
		virtual void moveToStop() = 0;

		virtual void angleToCb( float _time, float _angle, NodeCallbackInterface* _cb ) = 0;
		virtual void angleToStop() = 0;

		virtual void scaleToCb( float _time, const mt::vec2f& _scale, NodeCallbackInterface* _cb ) = 0;
		virtual void scaleToStop() = 0;

		virtual void accMoveToCb( float _time, const mt::vec2f& _point, NodeCallbackInterface* _cb ) = 0;
		virtual void accAngleToCb( float _time, float _angle, NodeCallbackInterface* _cb ) = 0;
		
		virtual void localColorToCb( float _time, const ColourValue& _color, NodeCallbackInterface * _cb ) = 0;
		virtual void localAlphaToCb( float _time, float _alpha, NodeCallbackInterface * _cb ) = 0;
		virtual void localColorToStop() = 0;
	};

	class ParticleEmitterInterface
		: virtual public NodeInterface
	{
	public:
		virtual void play() = 0;
		virtual void playFromPosition( float _pos ) = 0;

		virtual void stop() = 0;
		virtual void pause() = 0;
		
		virtual void restart() = 0;

		virtual void setLooped( bool _loop ) = 0;
		virtual bool getLooped() const = 0;

		virtual void setAutoPlay( bool _autoPlay ) = 0;
		virtual bool getAutoPlay() const = 0;

		virtual void setLeftBorder( float _leftBorder ) = 0;
		virtual void setResource( const String& _resourceName ) = 0;
		virtual void setEmitter( const String& _emitterName ) = 0;
		virtual void setEmitterRelative( bool _relative ) = 0;
	};

	class SoundEmitterInterface
		: virtual public ParticleEmitterInterface
	{
	public:
		virtual void play() = 0;
		virtual void pause() = 0;
		virtual void stop() = 0;

		virtual bool isPlaying() = 0;

		virtual void setVolume( float _volume ) = 0;
		virtual float getVolume() = 0;

		virtual void setLooped( bool _loop ) = 0;
		virtual bool isLooping() = 0;

		virtual void setSoundResource( const String& _name ) = 0;
	};

	class TextFieldInterface
		: virtual public NodeInterface
	{
	public:
		virtual void setMaxLen( float _len ) = 0;
		virtual void setText( const String& _text ) = 0;
		virtual void setHeight( float _height ) = 0;
		virtual float getHeight() const = 0;
		virtual const String& getText() const = 0;
		virtual void setOutlineColor( const ColourValue& _color ) = 0;
		virtual const ColourValue& getOutlineColor() const = 0;

		virtual const mt::vec2f& getLength() = 0;

		virtual void setLineOffset( float _offset ) = 0;
		virtual float getLineOffset() const = 0;

		virtual void setResource( const String& _resName ) = 0;
		virtual const std::string & getResource() const = 0;

		virtual void setOutlineResource( const String& _outlineName ) = 0;

		virtual bool getCenterAlign() const = 0;
		virtual void setCenterAlign( bool _centerAlign ) = 0;

		virtual bool getRightAlign() const = 0;
		virtual void setRightAlign( bool _rightAlign ) = 0;

		virtual bool getLeftAlign() const = 0;
		virtual void setLeftAlign( bool _leftAlign ) = 0;

		virtual float getCharOffset() const = 0;
		virtual void setCharOffset( float _offset ) = 0;

		virtual void setTextByKey( const String& _key ) = 0;
		virtual const std::string & getTextKey() const = 0;
	};

	class ArrowInterface
		: virtual public NodeInterface
	{
	public:
		virtual void setOffsetClick( const mt::vec2f & _offsetClick ) = 0;
		virtual const mt::vec2f & getOffsetClick() const = 0;

		virtual void setPolygon( const mt::polygon & _polygon ) = 0;
		virtual const mt::polygon & getPolygon() const = 0;
	};

	class HotSpotInterface
		: virtual public NodeInterface
	{
		virtual void addPoint( const mt::vec2f & _p ) = 0;
		virtual bool testPoint( const mt::vec2f & _p ) = 0;
		virtual void clearPoints() = 0;
	};

	class HotSpotImageInterface
		: virtual public HotSpotInterface
	{
	public:
		virtual void setAlphaTest( float _value ) = 0;
		virtual float getAlphaTest() const = 0;

		virtual void setResourceName( const String& _resourceName ) = 0;
		virtual void setFrame( std::size_t _frame ) = 0;
	};

	class SpriteInterface
		: virtual public NodeInterface
	{
	public:
		virtual void setImageIndex( std::size_t _index );
		virtual std::size_t getImageIndex() const;

		virtual void setImageResource( const String& _name );
		virtual void setImageAlpha( const String& _name );

		virtual std::size_t getImageCount();

		virtual mt::vec2f getImageSize();

		virtual void setCenterAlign( bool _centerAlign );
		virtual bool getCenterAlign() const;

		virtual const String& getImageResource() const;

		virtual void flip( bool _x );

		virtual void setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY );
		virtual void setPercentVisibilityToCb( float _time, const mt::vec2f& _percentX, const mt::vec2f& _percentY, PyObject* _cb );
		virtual void setPercentVisibilityToStop();
	};

	class AnimationInterface
		: virtual public SpriteInterface
	{
	public:
		virtual void play() = 0;
		virtual void stop() = 0;
		virtual void pause() = 0;
		virtual void resume() = 0;

		virtual void setLooped( bool _loop ) = 0;
		virtual bool getLooped() const = 0;

		virtual void setAnimationResource( const String& _resource ) = 0;
		virtual void setAnimationFactor( float _factor ) = 0;
		virtual float getAnimationFactor() const = 0;

		virtual std::size_t getCurrentFrame() const = 0;
		virtual std::size_t getFrameCount() const = 0;
		virtual void setCurrentFrame( std::size_t _frame ) = 0;
	};

	class RigidBody2DInterface
		: virtual public NodeInterface
	{
	public:
		virtual void applyForce( float _forceX, float _forceY, float _pointX, float _pointY ) = 0;
		virtual void applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY ) = 0;
		virtual void applyConstantForce( float _forceX, float _forceY, float _pointX, float _pointY ) = 0;
		virtual void removeConstantForce() = 0;
		virtual void setDirectionForce( bool _relative ) = 0;
		virtual float getMass() const = 0;
		virtual mt::vec2f getLinearVelocity() const = 0;
		virtual void setLinearVelocity( float _x, float _y, bool _countGravity ) = 0;
		virtual void unsetLinearVelocity() = 0;
		virtual void wakeUp() = 0;
		virtual void freeze( bool _freeze ) = 0;
		virtual void setCollisionMask( int _mask ) = 0;
		virtual void enableStabilization( bool _enable, float _stabilityAngle, float _stabilityForce ) = 0;
	};

	class VideoInterface
		: virtual public NodeInterface
	{
	public:
		virtual void play() = 0;
		virtual void stop() = 0;
		virtual void pause() = 0;

		virtual void setLooped( bool _loop ) = 0;
		virtual bool getLooped() const = 0;

		virtual void setVideoResource( const String & _resource ) = 0;
		virtual const String & getVideoResource() const = 0;

		virtual void setSoundResource( const String& _resource ) = 0;
		virtual const String & getSoundResource() const = 0;
	};

	class WindowInterface
		: virtual public NodeInterface
	{
	public:
		virtual void setClientSize( const mt::vec2f& _clientSize ) = 0;
		virtual void setClientSizeClip( const mt::vec2f& _clientSize ) = 0;
		virtual void setClientSizeInTiles( const mt::vec2f& _tiles ) = 0;
		virtual const mt::vec2f getClientSize() const = 0;
		virtual mt::vec2f getWindowSize() const = 0;
		virtual const mt::vec2f& getTileSize( int _tile ) const = 0;
	};

	class PointInterface
		: virtual public NodeInterface
	{
	public:
		virtual bool testHotSpot( HotSpotInterface * _hotspot ) = 0;
	};

	class LayerInterface
		: virtual public NodeInterface
	{
	public:
		virtual const mt::vec2f & getSize() const = 0;
	};

	class Layer2DInterface
		: virtual public LayerInterface
	{
	public:
		virtual void setParallaxFactor( const mt::vec2f & _factor ) = 0;
		virtual const mt::vec2f & getParallaxFactor() const = 0;

		virtual void setRenderViewport( const Viewport & _viewport ) = 0;
		virtual const Viewport & getRenderViewport() const = 0;

		virtual mt::vec2f screenToLocal( const mt::vec2f& _point ) = 0;
	};

	class SceneInterface
		: virtual public NodeInterface
	{
	public:
		virtual void layerAppend( const String & _layer, NodeInterface * _node ) = 0;
		virtual void layerRemove( NodeInterface * _node ) = 0;
		virtual void layerHide( const String & _layer, bool _value ) = 0; // depricated

		virtual NodeInterface * getNode( const String & _name ) = 0;
		virtual const mt::vec2f & getLayerSize( const String& _name ) = 0; // depricated

		virtual void setRenderTarget( const String& _cameraName, const mt::vec2f& _size ) = 0;
		virtual const String& getRenderTarget() const = 0;

		virtual void renderSelf() = 0; //depricated

		virtual void blockInput( bool _block ) = 0;
		virtual bool getBlockInput() const = 0;

		virtual void setCameraPosition( float _x, float _y ) = 0;
		virtual void enableCameraFollowing( bool _enable, float _force ) = 0;

		virtual void setCameraTarget( NodeInterface * _target ) = 0;
		virtual void setCameraBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower ) = 0;
	};

	class MengineInterface
	{
	public:
		std::size_t schedule( float _timing, CallbackInterface * _cb );
		void scheduleRemove( std::size_t _id );
		void scheduleRemoveAll();
		void scheduleStopAll();
		void scheduleResumeAll();
		void scheduleFreeze( std::size_t _id, bool _freeze );
		float getMouseX();
		float getMouseY();
		bool isMouseDown( int _button );
		bool isKeyDown( int _key );


		void setCurrentScene( const String& _name, bool _destroyOld = false );
		void setCurrentSceneCb( const String& _name, CallbackInterface * _cb );
		SceneInterface * getCurrentScene();
		void setArrow( const std::string & _name );
		ArrowInterface * getArrow();
		const mt::polygon & getHotSpotPolygon( HotSpotInterface * _hotspot );
		mt::vec2f getHotSpotImageSize( HotSpotImageInterface * _hotspotImage );

		const mt::vec2f & getCamera2DPosition();
		void setCamera2DDirection( float x, float y );
		void destroyNode( NodeInterface * _node );
		void destroyScene( SceneInterface * _scene );
		void destroySceneByName( const std::string & _nameScene );
		
		NodeInterface * createNode( const std::string & _type );
		NodeInterface * createNodeFromXml( const std::string & _xml );
		
		bool directResourceCompile( const String& _nameResource );
		void directResourceRelease( const String& _nameResource );
		void directResourceUnload( const String& _nameResource );
		void directResourceFileCompile( const String& _resourceFile );

		typedef std::list<std::string> TListFiles;
		void deferredResourceFileCompile( const TListFiles & _resourceFiles, CallbackInterface * _progressCallback );

		void directResourceFileRelease( const String& _resourceFile );
		void directResourceFileUnload( const String& _resourceFile );
		SpriteInterface * createShot( const String& _name, mt::vec2f _min,  mt::vec2f _max );

		void setFullscreenMode( bool _fullscreen );
		bool getFullscreenMode();

		void minimizeWindow();

		void addResourceListener( CallbackInterface * _listener );
		void removeResourceListener( CallbackInterface * _listener );

		void renderOneFrame();
		void writeImageToFile( const String& _resource, int _frame, const String& _filename );

		void setSoundEnabled( bool _enabled );
		void setParticlesEnabled( bool _enabled );

		void createResourceFromXml( const String& _xml );
		void createImageResource( const String& _resourceName, const String& _pakName, const String& _filename );

		mt::vec2f screenToLocal( const String& _layerName, const mt::vec2f& _point );

		void setMouseBounded( bool _bounded );

		bool setBlow( bool _active );
		float getBlow();

		void setEnoughBlow( float _enoughBlow );
		void setBlowCallback( CallbackInterface * _sulkcallback );

		void setCursorPosition( float _x, float _y );
		bool isInViewport( const mt::vec2f & _pos );

		size_t getResourceCount( const String& _resourceFile );
		void enableTextureFiltering( bool _enable );
		bool isTextureFilteringEnabled();
		bool existText( const String & _key );

		//PyObject * s_pickHotspot();

		void quitApplication();
	};
}