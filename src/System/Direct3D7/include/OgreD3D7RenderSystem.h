/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://ogre.sourceforge.net/

Copyright (c) 2000-2005 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/
#ifndef __D3DRENDERSYSTEM_H__
#define __D3DRENDERSYSTEM_H__

// Precompiler options
#include "OgreD3D7Prerequisites.h"
#include "OgreString.h"



#include "OgreRenderSystem.h"
#include "OgreD3D7HardwareBufferManager.h"

namespace Ogre {

    class DDDriverList;
    class DDDriver;

    /**
      Implementation of DirectX as a rendering system.
     */
    class D3DRenderSystem : public RenderSystem
    {
    private:
        // Direct3D rendering device
        // Only created after top-level window created
        LPDIRECT3DDEVICE7 mlpD3DDevice;
        D3DDEVICEDESC7 mD3DDeviceDesc;

        // List of DD drivers installed (video cards)
        // Enumerates itself
        DDDriverList* mDriverList;
        // Currently active driver
        DDDriver* mActiveDDDriver;


        HINSTANCE mhInstance;
		bool mDeviceLost;



		// Stored options
		ConfigOptionMap mOptions;

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
 		eD3DTexType _ogreTexTypeToD3DTexType(TextureType ogreTexType)
 		{
 			eD3DTexType ret;
 			switch (ogreTexType)
 			{
 			case TEX_TYPE_1D :
 			case TEX_TYPE_2D :
 				ret = D3D_TEX_TYPE_NORMAL;
 				break;
 			case TEX_TYPE_CUBE_MAP :
 				ret = D3D_TEX_TYPE_CUBE;
 				break;
 			default :
 				OGRE_EXCEPT( Exception::ERR_INVALIDPARAMS, "Invalid tex.type", "D3D9RenderSystem::_ogreTexTypeToD3DTexType" );
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
 			size_t coordIndex;
 			/// type of auto tex. calc. used
 			TexCoordCalcMethod autoTexCoordType;
            /// Frustum, used with the above in projective texturing
            const Frustum* frustum;
 			/// texture, if it's 0/NULL the tex layer is disabled
 			LPDIRECTDRAWSURFACE7 pTex;
 		} mTexStageDesc[OGRE_MAX_TEXTURE_LAYERS];


        // Matrix conversion
        D3DMATRIX makeD3DMatrix(const Matrix4& mat);
        Matrix4 convertD3DMatrix(const D3DMATRIX& mat);

        void initConfigOptions(void);
        void initInputDevices(void);
        void processInputDevices(void);
        void setD3DLight(size_t index, Light* light);

        D3DCMPFUNC convertCompareFunction(CompareFunction func);
        D3DSTENCILOP convertStencilOp(StencilOperation op);

		// state management methods, very primitive !!!
		HRESULT __SetRenderState(D3DRENDERSTATETYPE state, DWORD value);
		HRESULT __SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value);


        D3DTEXTURESTAGESTATETYPE _getFilterCode(FilterType ft) const;
        DWORD _getFilter(FilterType ft, FilterOptions fo) const;
		DWORD _getCurrentAnisotropy(size_t unit);

        HardwareBufferManager* mHardwareBufferManager;
        GpuProgramManager* mGpuProgramManager;


        unsigned short mCurrentLights;
        Matrix4 mViewMatrix;
        // saved scene blending factors
        SceneBlendFactor mSavedSrcFactor, mSavedDestFactor;


	public:
        // Default constructor / destructor
        D3DRenderSystem(HINSTANCE hInstance);
        ~D3DRenderSystem();



        // ----------------------------------
        // Overridden RenderSystem functions
        // ----------------------------------
        /** See
          RenderSystem
         */
        const String& getName(void) const;
        /** See
          RenderSystem
         */
        ConfigOptionMap& getConfigOptions(void);
        /** See
          RenderSystem
         */
        void setConfigOption(const String &name, const String &value);
        /** See
          RenderSystem
         */
        String validateConfigOptions(void);
        /** See
          RenderSystem
         */
        RenderWindow* initialise(bool autoCreateWindow, const String& windowTitle = "OGRE Render Window");
        /** See
          RenderSystem
         */
        void reinitialise(void); // Used if settings changed mid-rendering
        /** See
          RenderSystem
         */
        void shutdown(void);

        /** See
          RenderSystem
         */
        void setAmbientLight(float r, float g, float b);
        /** See
          RenderSystem
         */
        void setShadingType(ShadeOptions so);
        /** See
          RenderSystem
         */
        void setLightingEnabled(bool enabled);
        
		/// @copydoc RenderSystem::createRenderWindow
		RenderWindow* createRenderWindow(const String &name, unsigned int width, unsigned int height, 
			bool fullScreen, const NameValuePairList *miscParams = 0);

		/// @copydoc RenderSystem::createRenderTexture
		RenderTexture * createRenderTexture( const String & name, unsigned int width, unsigned int height,
		 	TextureType texType = TEX_TYPE_2D, PixelFormat internalFormat = PF_X8R8G8B8, 
			const NameValuePairList *miscParams = 0 ); 

		/** See
          RenderSystem
         */
        void destroyRenderWindow(RenderWindow* pWin);

        /** See
          RenderSystem
         */
        String getErrorDescription(long errorNumber) const;

        /** See
          RenderSystem
         */
        void convertColourValue(const ColourValue& colour, uint32* pDest);

		/// Notify of a device lost
		void _notifyDeviceLost(void);
		/// Attempt to restore a device
		void _restoreLostDevice(void);
		/// Restore all surfaces
		void _restoreSurfaces(void);
		/// Recreate the primary context
		void _recreateContext(void);

        // -----------------------------
        // Low-level overridden members
        // -----------------------------
        /** See
          RenderSystem
         */
        void _useLights(const LightList& lights, unsigned short limit);
        /** See
          RenderSystem
         */
        void _setWorldMatrix(const Matrix4 &m);
        /** See
          RenderSystem
         */
        void _setViewMatrix(const Matrix4 &m);
        /** See
          RenderSystem
         */
        void _setProjectionMatrix(const Matrix4 &m);
        /** See
          RenderSystem
         */
        void _setSurfaceParams(const ColourValue &ambient,
            const ColourValue &diffuse, const ColourValue &specular,
            const ColourValue &emissive, Real shininess, TrackVertexColourType tracking);
        /** See
          RenderSystem
         */
        void _setTexture(size_t unit, bool enabled, const String &texname);
        /** See
          RenderSystem
         */
        void _setTextureBlendMode(size_t unit, const LayerBlendModeEx& bm);
        /** See
          RenderSystem
         */
        void _setTextureAddressingMode(size_t unit, TextureUnitState::TextureAddressingMode tam);
        /** See
          RenderSystem
         */
        void _setTextureMatrix(size_t unit, const Matrix4& xform);
        /** See
          RenderSystem
         */
        void _setTextureCoordSet( size_t unit, size_t index );
        /** See
          RenderSystem
         */
        void _setTextureCoordCalculation(size_t unit, TexCoordCalcMethod m, 
            const Frustum* frustum = 0);
        /** See
          RenderSystem
         */
        void _setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor);
        /** See
          RenderSystem
         */
        void _setAlphaRejectSettings(CompareFunction func, unsigned char value);
        /** See
          RenderSystem
         */
        void _setViewport(Viewport *vp);
        /** See
          RenderSystem
         */
        void _beginFrame(void);
        /** See
          RenderSystem
         */
        void _render(const RenderOperation& op);
        /** See
          RenderSystem
         */
        void _endFrame(void);
        /** See
          RenderSystem
         */
        void _setCullingMode(CullingMode mode);
        /** See
          RenderSystem
         */
        void _setDepthBufferParams(bool depthTest = true, bool depthWrite = true, CompareFunction depthFunction = CMPF_LESS_EQUAL);
        /** See
          RenderSystem
         */
        void _setDepthBufferCheckEnabled(bool enabled = true);
        /** See
          RenderSystem
         */
        void _setDepthBufferWriteEnabled(bool enabled = true);
        /** See
          RenderSystem
         */
        void _setDepthBufferFunction(CompareFunction func = CMPF_LESS_EQUAL);
        /** See
          RenderSystem
         */
        void _setDepthBias(ushort bias);
        /** See RenderSystem
		@remarks
			Direct3D7 DOES NOT SUPPORT COLOUR WRITE MASKING! This feature is 
            emulated by using a null scene blend which will only work as long
            as setSceneBlend is called before it. 
         */
		void _setColourBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);
        
		/** See
          RenderSystem
         */
        void _setFog(FogMode mode, const ColourValue& colour, Real density, Real start, Real end);
        /** See
          RenderSystem
         */
        void _makeProjectionMatrix(const Radian& fovy, Real aspect, Real nearPlane, 
            Real farPlane, Matrix4& dest, bool forGpuProgram = false);
        /** See
        RenderSystem
        */
        void _makeProjectionMatrix(Real left, Real right, Real bottom, Real top,
            Real nearPlane, Real farPlane, Matrix4& dest, bool forGpuProgram = false);
        /** See
          RenderSystem
         */
        void _makeOrthoMatrix(const Radian& fovy, Real aspect, Real nearPlane, Real farPlane, 
            Matrix4& dest, bool forGpuProgram = false);
        /** See
        RenderSystem
        */
        void _applyObliqueDepthProjection(Matrix4& matrix, const Plane& plane, 
            bool forGpuProgram);
        /** See
        RenderSystem
        */
		void _setPolygonMode(PolygonMode level);
        /** See
          RenderSystem
         */
        void setStencilCheckEnabled(bool enabled);
        /** See
          RenderSystem
         */
        void setStencilBufferParams(CompareFunction func = CMPF_ALWAYS_PASS, 
            uint32 refValue = 0, uint32 mask = 0xFFFFFFFF, 
            StencilOperation stencilFailOp = SOP_KEEP, 
            StencilOperation depthFailOp = SOP_KEEP,
            StencilOperation passOp = SOP_KEEP, 
            bool twoSidedOperation = false);
        void _setTextureUnitFiltering(size_t unit, FilterType ftype, FilterOptions filter);
        /** See
          RenderSystem
         */
		void _setTextureLayerAnisotropy(size_t unit, unsigned int maxAnisotropy);
        /** See
          RenderSystem
         */
		void setVertexDeclaration(VertexDeclaration* decl);
        /** See
          RenderSystem
         */
		void setVertexBufferBinding(VertexBufferBinding* binding);
        /** See
          RenderSystem
         */
        void setNormaliseNormals(bool normalise);
        /** See
          RenderSystem
         */
        void bindGpuProgram(GpuProgram* prg) { /* do nothing */}
        /** See
          RenderSystem
         */
        void unbindGpuProgram(GpuProgramType gptype){ /* do nothing */}
        /** See
          RenderSystem
         */
        void bindGpuProgramParameters(GpuProgramType gptype, GpuProgramParametersSharedPtr params) { /* do nothing */}    
        /** See
          RenderSystem
         */
        void setClipPlanes(const PlaneList& clipPlanes);
        /** See
          RenderSystem
         */
        void setScissorTest(bool enabled, size_t left = 0, size_t top = 0, size_t right = 800, size_t bottom = 600)
        { /* do nothing, d3d7 does not support scissor rect */ }
        /** See
        RenderSystem
        */
        void clearFrameBuffer(unsigned int buffers, 
            const ColourValue& colour = ColourValue::Black, 
            Real depth = 1.0f, unsigned short stencil = 0);

        void setClipPlane (ushort index, Real A, Real B, Real C, Real D);
        void enableClipPlane (ushort index, bool enable);
        /** See
        RenderSystem
        */
        HardwareOcclusionQuery* createHardwareOcclusionQuery(void);
        Real getHorizontalTexelOffset(void);
        Real getVerticalTexelOffset(void);
        Real getMinimumDepthInputValue(void);
        Real getMaximumDepthInputValue(void);

		//
		MultiRenderTarget * createMultiRenderTarget(const String & name); 
		void _setPointSpritesEnabled(bool enabled);
		void _setPointParameters(Real size, bool attenuationEnabled, 
			Real constant, Real linear, Real quadratic, Real minSize, Real maxSize);
		void _setTexture(size_t unit, bool enabled, 
			const TexturePtr &texPtr);
		void _setTextureAddressingMode(size_t unit, const TextureUnitState::UVWAddressingMode& uvw);
		void _setTextureBorderColour(size_t unit, const ColourValue& colour);
		void _setTextureMipmapBias(size_t unit, float bias);
		void _setDepthBias(float constantBias, float slopeScaleBias = 0.0f);
		VertexElementType getColourVertexElementType(void) const;
		void _convertProjectionMatrix(const Matrix4& matrix,
			Matrix4& dest, bool forGpuProgram = false);
		void bindGpuProgramPassIterationParameters(GpuProgramType gptype);
		void preExtraThreadsStarted();
		void postExtraThreadsStarted();
		void registerThread();
		void unregisterThread();

        // ----------------------------------
        // End Overridden members
        // ----------------------------------
    };
}
#endif

