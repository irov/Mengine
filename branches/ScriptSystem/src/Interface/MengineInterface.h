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

		
		virtual void moveToCb( float _time, const mt::vec2f& _point, NodeCallbackInterface* _cb ) = 0;
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
}