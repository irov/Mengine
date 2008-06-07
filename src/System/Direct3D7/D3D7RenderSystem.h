#	pragma once
// Precompiler options

#	include "Interface/LogSystemInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/TextureInterface.h"

#	include "Menge/Color.h"

#include "D3D7Prerequisites.h"

#include "D3D7HardwareBufferManager.h"

#	include <list>

namespace Menge
{

    class DDDriverList;
    class DDDriver;

	class TextureManager;
    // Implementation of DirectX as a rendering system.
    class D3DRenderSystem : public RenderSystemInterface
    {
	public:
        // Default constructor / destructor
        D3DRenderSystem();
        ~D3DRenderSystem();

        const String& getName() const override;

//        TConfigOptionMap& getConfigOptions() override;

        void setConfigOption( const String& _name, const String& _value ) override;

//        String validateConfigOptions() override;

		void initialise( LogSystemInterface* _logInterface ) override;

//		void reinitialise() override; // Used if settings changed mid-rendering

		void shutdown() override;

//		void setAmbientLight( float _r, float _g, float _b ) override;

//		void setShadingType( ShadeOptions _so ) override;

//		void setLightingEnabled( bool _enabled ) override;
        
		bool createRenderWindow( const String& _name, unsigned int _width, unsigned int _height, 
			bool _fullScreen, const NameValuePairList* _miscParams = 0 ) override;

//		RenderTexture* createRenderTexture( const String& _name, unsigned int _width, unsigned int _height,
	//		/*TextureType _texType = TEX_TYPE_2D, */PixelFormat _internalFormat = PF_X8R8G8B8, 
	//		const NameValuePairList* _miscParams = 0 ) override; 


//		void destroyRenderWindow( const String& _name ) override;

//		void destroyRenderTexture( const String& _name ) override;

//		void destroyRenderTarget( const String& _name ) override;

//		void attachRenderTarget( RenderTarget& _target ) override;

//		RenderTarget * getRenderTarget( const String& _name ) override;

//		RenderTarget * detachRenderTarget( const String& _name ) override;

        void destroyRenderWindow(RenderWindow* pWin);

//        String getErrorDescription( long _errorNumber ) const override;

		void setWaitForVerticalBlank( bool _enabled ) /*override*/;

		bool getWaitForVerticalBlank() const /*override*/;

        void convertColourValue( const float* _colour, uint32* pDest );

		/// Notify of a device lost
		void _notifyDeviceLost();
		/// Attempt to restore a device
		void _restoreLostDevice();
		/// Restore all surfaces
		void _restoreSurfaces();
		/// Recreate the primary context
		void _recreateContext();

//        void _useLights( const TLightList& _lights, unsigned short _limit ) override;

//		void _setWorldMatrix( const float* _m ) override;

//		void _setViewMatrix( const float* _m ) override;

//		void _setProjectionMatrix( const float* _m ) override;

//		void _setSurfaceParams( const float* _ambient, const float* _diffuse, const float* _specular,
 //           const float* _emissive, float _shininess, int _tracking ) override;

		void _setTexture( std::size_t _unit, bool _enabled, const String& _texname );

//		void _setTextureUnitSettings( std::size_t _texUnit, TextureUnitState& _tl ) override;

		void _setTextureBlendMode( std::size_t _unit, const LayerBlendModeEx& _bm );

		void _setTextureAddressingMode( std::size_t _unit, TextureUnitState::UVWAddressingMode& _uvw );

		void _setTextureMatrix( std::size_t _unit, const float* _xform );

		void _setTextureCoordSet( std::size_t _unit, std::size_t _index );

		void _setTextureCoordCalculation( std::size_t _unit, TexCoordCalcMethod _m, 
            const Frustum* _frustum = 0 );

		void _setSceneBlending( EBlendFactor _sourceFactor, EBlendFactor _destFactor );

		void _setAlphaRejectSettings( CompareFunction _func, unsigned char _value );

		void _setViewport( Viewport* _vp );

//		Viewport* _getViewport() override;

//		void _disableTextureUnitsFrom( std::size_t _texUnit ) override;

//		void _disableTextureUnit( std::size_t _texUnit ) override;

//		void _setVertexTexture( std::size_t _unit, const Texture* _tex ) override;

//		void _setTextureUnitFiltering( std::size_t _unit, FilterOptions _minFilter,
//			FilterOptions _magFilter, FilterOptions _mipFilter ) override;

//		CullingMode _getCullingMode() const override;

//		const RenderSystemCapabilities* getCapabilities() const override;

//		void setClipPlane( unsigned short _index, const float* _plane ) override;

//		void setCurrentPassIterationCount( const std::size_t _count ) override;

//		void addListener( Listener* _l ) override;

//		void removeListener( Listener* _l ) override;

//		const TStringVector& getRenderSystemEvents() const override;

		void _beginFrame();

		void _render( const RenderOperation& _op );

		void _endFrame();

		void _setCullingMode( CullingMode _mode );

		void _setDepthBufferParams( bool _depthTest = true, bool _depthWrite = true, CompareFunction _depthFunction = CMPF_LESS_EQUAL );

		void _setDepthBufferCheckEnabled( bool _enabled = true );

		void _setDepthBufferWriteEnabled( bool _enabled = true );

		void _setDepthBufferFunction( CompareFunction _func = CMPF_LESS_EQUAL );

		void _setDepthBias( unsigned short _bias );

		void _setColourBufferWriteEnabled( bool _red, bool _green, bool _blue, bool _alpha );

//		void _setFog( FogMode _mode, const float* _colour, float _density, float _start, float _end ) override;

		void _makeProjectionMatrix( const float _fovy, float _aspect, float _nearPlane, 
            float _farPlane, float* _dest, bool _forGpuProgram = false );

		void _makeProjectionMatrix( float _left, float _right, float _bottom, float _top,
            float _nearPlane, float _farPlane, float* _dest, bool _forGpuProgram = false );

		void _makeOrthoMatrix( const float _fovy, float _aspect, float _nearPlane, float _farPlane, 
            float* _dest, bool _forGpuProgram = false );

		void _applyObliqueDepthProjection( float* _matrix, const float* _plane, bool _forGpuProgram );

		void _setPolygonMode( PolygonMode _level );

		void setStencilCheckEnabled( bool _enabled );

		void setStencilBufferParams( CompareFunction _func = CMPF_ALWAYS_PASS, 
            uint32 _refValue = 0, uint32 _mask = 0xFFFFFFFF, 
            StencilOperation _stencilFailOp = SOP_KEEP, 
            StencilOperation _depthFailOp = SOP_KEEP,
            StencilOperation _passOp = SOP_KEEP, 
            bool _twoSidedOperation = false );
		void _setTextureUnitFiltering( std::size_t _unit, FilterType _ftype, FilterOptions _filter );

		void _setTextureLayerAnisotropy( std::size_t _unit, unsigned int _maxAnisotropy );

		void setVertexDeclaration( VertexDeclaration* _decl );

		void setVertexBufferBinding( VertexBufferBinding* _binding );

		void setNormaliseNormals( bool _normalise );

		void bindGpuProgram( GpuProgram* _prg ) { /* do nothing */}

		void unbindGpuProgram( GpuProgramType _gptype ){ /* do nothing */}

		//void bindGpuProgramParameters( GpuProgramType gptype, GpuProgramParametersSharedPtr params) { /* do nothing */}    

		//void setClipPlanes( const PlaneList& clipPlanes );

		void setScissorTest( bool _enabled, std::size_t _left = 0, std::size_t _top = 0, std::size_t _right = 800, std::size_t _bottom = 600 )
        { /* do nothing, d3d7 does not support scissor rect */ }

//		void clearFrameBuffer( unsigned int _buffers, const float* _colour, 
//								float _depth = 1.0f, unsigned short _stencil = 0 ) override;

        void setClipPlane( unsigned short _index, float _A, float _B, float _C, float _D );
        void enableClipPlane( unsigned short _index, bool _enable );

		//HardwareOcclusionQuery* createHardwareOcclusionQuery() override;
		//void destroyHardwareOcclusionQuery( HardwareOcclusionQuery* _hq ) override;
//        float getHorizontalTexelOffset() override;
//        float getVerticalTexelOffset() override;
        float getMinimumDepthInputValue();
        float getMaximumDepthInputValue();

		//
		//MultiRenderTarget * createMultiRenderTarget( const String& _name ) override; 
//		void _setPointSpritesEnabled( bool _enabled ) override;
//		void _setPointParameters( float _size, bool _attenuationEnabled, 
//			float _constant, float _linear, float _quadratic, float _minSize, float _maxSize ) override;
//		void _setTexture( std::size_t _unit, bool _enabled, const Texture* _texPtr ) override;
		void _setTextureAddressingMode(size_t unit, const TextureUnitState::UVWAddressingMode& uvw);
//		void _setTextureBorderColour( std::size_t _unit, const float* _colour ) override;
		void _setTextureMipmapBias( std::size_t _unit, float _bias );
		void _setDepthBias(float constantBias, float slopeScaleBias = 0.0f);
		VertexElementType getColourVertexElementType(void) const;
		void _convertProjectionMatrix( const float* _matrix, float* _dest, bool _forGpuProgram = false );
		void bindGpuProgramPassIterationParameters(GpuProgramType gptype);
		void preExtraThreadsStarted();
		void postExtraThreadsStarted();
		void registerThread();
		void unregisterThread();

	    // ----------------------------------
        // End Overridden members
        // ----------------------------------

	protected:
		// The render targets.
		TRenderTargetMap m_renderTargets;
		// The render targets, ordered by priority.
		TRenderTargetPriorityMap m_prioritisedRenderTargets;
		// The Active render target.
		RenderTarget * m_activeRenderTarget;
		// The Active GPU programs and gpu program parameters
		//GpuProgramParametersSharedPtr mActiveVertexGpuProgramParameters;
		//GpuProgramParametersSharedPtr mActiveFragmentGpuProgramParameters;

		// Texture manager
		// A concrete class of this will be created and
		// made available under the TextureManager singleton,
		// managed by the RenderSystem
		TextureManager* m_textureManager;

		/// Used to store the capabilities of the graphics card
		RenderSystemCapabilities* m_capabilities;

		// Active viewport (dest for future rendering operations)
		Viewport* mActiveViewport;

		CullingMode mCullingMode;

		bool m_VSync;
		bool mWBuffer;

		size_t mBatchCount;
		size_t mFaceCount;
		size_t mVertexCount;

		/// Saved manual colour blends
		Color mManualBlendColours[MENGE_MAX_TEXTURE_LAYERS][2];

		bool mInvertVertexWinding;

		/// Texture units from this upwards are disabled
		size_t mDisabledTexUnitsFrom;

		/// number of times to render the current state
		size_t mCurrentPassIterationCount;

		//updates pass iteration rendering state including bound gpu program parameter
		//pass iteration auto constant entry
		//@returns True if more iterations are required
		bool updatePassIterationRenderState();

		/// List of names of events this rendersystem may raise
		TStringVector m_eventNames;

		/// Internal method for firing a rendersystem event
		//virtual void fireEvent( const String& _name, const NameValuePairList* _params = 0);

		typedef std::list<Listener*> TListenerList;
		TListenerList m_eventListeners;

		bool m_vertexProgramBound;
		bool m_fragmentProgramBound;

	private:
		// Direct3D rendering device
		// Only created after top-level window created
		LPDIRECT3DDEVICE7 mlpD3DDevice;
		D3DDEVICEDESC7 m_D3DDeviceDesc;

		// List of DD drivers installed (video cards)
		// Enumerates itself
		DDDriverList* m_driverList;
		// Currently active driver
		DDDriver* m_activeDDDriver;

		HINSTANCE m_hInstance;
		bool m_deviceLost;

		// Stored options
		TConfigOptionMap m_options;

		// Private utilities
		DDDriverList* getDirectDrawDrivers(void);
		void refreshDDSettings(void);

		/// enum identifying D3D9 tex. types
		enum eD3DTexType
		{
			/// standard texture
			D3D_TEX_TYPE_NORMAL,
			/// cube texture
			D3D_TEX_TYPE_CUBE,
			/// volume texture
			D3D_TEX_TYPE_VOLUME
		};

		/// return the D3DtexType equivalent of a Ogre tex. type
		eD3DTexType _ogreTexTypeToD3DTexType( ETextureType _texType )
		{
			eD3DTexType ret;
			switch (_texType)
			{
			case TEX_TYPE_1D :
			case TEX_TYPE_2D :
				ret = D3D_TEX_TYPE_NORMAL;
				break;
			case TEX_TYPE_CUBE_MAP :
				ret = D3D_TEX_TYPE_CUBE;
				break;
			default :
				assert( 0 && "D3D9RenderSystem::_ogreTexTypeToD3DTexType -> Invalid tex.type" );
				break;
			}
			return ret;
		}

		/// structure holding texture unit settings for every stage
		struct sD3DTextureStageDesc
		{
			/// the type of the texture
			eD3DTexType texType;
			/// wich texCoordIndex to use
			std::size_t coordIndex;
			/// type of auto tex. calc. used
			TexCoordCalcMethod autoTexCoordType;
			/// Frustum, used with the above in projective texturing
			const Frustum* frustum;
			/// texture, if it's 0/NULL the tex layer is disabled
			LPDIRECTDRAWSURFACE7 pTex;
		} m_texStageDesc[MENGE_MAX_TEXTURE_LAYERS];


		// Matrix conversion
		//D3DMATRIX makeD3DMatrix( const Matrix4& _mat );
		//Matrix4 convertD3DMatrix( const D3DMATRIX& _mat );

		void initConfigOptions();
		void initInputDevices();
		void processInputDevices();
		void setD3DLight( std::size_t _index, Light* _light );

		D3DCMPFUNC convertCompareFunction( CompareFunction _func );
		D3DSTENCILOP convertStencilOp( StencilOperation _op );

		// state management methods, very primitive !!!
		HRESULT __SetRenderState( D3DRENDERSTATETYPE _state, DWORD _value );
		HRESULT __SetTextureStageState( DWORD _stage, D3DTEXTURESTAGESTATETYPE _type, DWORD _value );

		D3DTEXTURESTAGESTATETYPE _getFilterCode( FilterType _ft ) const;
		DWORD _getFilter( FilterType _ft, FilterOptions _fo ) const;
		DWORD _getCurrentAnisotropy( std::size_t _unit );

		//HardwareBufferManager* m_hardwareBufferManager;
		//GpuProgramManager* m_gpuProgramManager;

		unsigned short m_currentLights;
		//Matrix4 m_viewMatrix;
		// saved scene blending factors
		EBlendFactor m_savedSrcFactor, m_savedDestFactor;

		// LogSystemInterface
		LogSystemInterface* m_logInterface;
		bool m_initialized;
    };
}

