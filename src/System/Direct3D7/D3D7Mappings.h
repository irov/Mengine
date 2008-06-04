#	pragma once

#include "D3D7Prerequisites.h"
//#include "OgreCommon.h"
//#include "OgreLight.h"
//#include "OgreMaterial.h"
//#include "OgreRenderSystem.h"
//#include "OgreHardwareBuffer.h"
//#include "OgreHardwareIndexBuffer.h"

//#include "OgreNoMemoryMacros.h"
#include <d3d.h>
#include <d3dx.h>
//#include <d3dxerr.h>
//#include "OgreMemoryMacros.h"


namespace Ogre 
{
	class D3D7Mappings
	{
	public:
		/// enum identifying D3D7 tex. types
		enum eD3DTexType
		{
			/// standard texture
			D3D_TEX_TYPE_NORMAL,
			/// cube texture
			D3D_TEX_TYPE_CUBE,
			/// volume texture
			D3D_TEX_TYPE_VOLUME,
			/// just to have it...
			D3D_TEX_TYPE_NONE
		};

		/// enum identifying D3D7 filter usage type
		enum eD3DFilterUsage
		{
			/// min filter
			D3D_FUSAGE_MIN,
			/// mag filter
			D3D_FUSAGE_MAG,
			/// mip filter
			D3D_FUSAGE_MIP
		};

		/// return a D3D7 equivalent for a Ogre ShadeOptions value
		static DWORD get(ShadeOptions so);
		/// return a D3D7 equivalent for a Ogre LightTypes value
		static D3DLIGHTTYPE get(Ogre::Light::LightTypes lightType);
		/// return a D3D7 equivalent for a Ogre TexCoordCalsMethod value
//		static DWORD get(TexCoordCalcMethod m, const D3DCAPS7& caps);
		/// return a D3D7 equivalent for a Ogre TextureAddressingMode value
//		static D3DTEXTUREADDRESS get(TextureUnitState::TextureAddressingMode tam, const D3DCAPS7& devCaps);
		/// return a D3D7 equivalent for a Ogre LayerBlendType value
		static D3DTEXTURESTAGESTATETYPE get(LayerBlendType lbt);
		/// return a D3D7 equivalent for a Ogre LayerBlendOperationEx value
//		static DWORD get(LayerBlendOperationEx lbo, const D3DCAPS9& devCaps);
		/// return a D3D7 equivalent for a Ogre LayerBlendSource value
		static DWORD get(LayerBlendSource lbs, bool perStageConstants);
		/// return a D3D7 equivalent for a Ogre SceneBlendFactor value
		static D3DBLEND get(SceneBlendFactor sbf);
		/// return a D3D7 equivalent for a Ogre CompareFunction value
		static DWORD get(CompareFunction cf);
		/// return a D3D7 equivalent for a Ogre CillingMode value
		static DWORD get(CullingMode cm, bool flip);
		/// return a D3D7 equivalent for a Ogre FogMode value
		static D3DFOGMODE get(FogMode fm);
		/// return a D3D7 equivalent for a Ogre PolygonMode value
		static D3DFILLMODE get(PolygonMode level);
		/// return a D3D7 equivalent for a Ogre StencilOperation value
		static DWORD get(StencilOperation op, bool invert = false);
		/// return a D3D7 state type for Ogre FilterType value
//		static D3DSAMPLERSTATETYPE get(FilterType ft);
		/// return a D3D7 filter option for Ogre FilterType & FilterOption value
//		static DWORD get(FilterType ft, FilterOptions fo, const D3DCAPS7& devCaps, eD3DTexType texType);
		/// return the D3DtexType equivalent of a Ogre tex. type
		static eD3DTexType get(TextureType ogreTexType);
        /// return the combination of D3DUSAGE values for Ogre buffer usage
        static DWORD get(HardwareBuffer::Usage usage);
        /// Get lock options
        static DWORD get(HardwareBuffer::LockOptions options, HardwareBuffer::Usage usage);
        /// Get index type
        static DDPIXELFORMAT get(HardwareIndexBuffer::IndexType itype);
		/// Get vertex data type
		static D3DDECLTYPE get(VertexElementType vType);
		/// Get vertex semantic
		static D3DDECLUSAGE get(VertexElementSemantic sem);
        // Convert matrix to D3D style
        static 	D3DXMATRIX makeD3DXMatrix( const Matrix4& mat );
        // Convert matrix from D3D style
        static Matrix4 convertD3DXMatrix( const D3DXMATRIX& mat );

		/// utility method, convert D3D7 pixel format to Ogre pixel format
		static PixelFormat _getPF(D3DFORMAT d3dPF);
		/// utility method, convert Ogre pixel format to D3D7 pixel format
		static D3DFORMAT _getPF(PixelFormat ogrePF);
		/// utility method, find closest Ogre pixel format that D3D7 can support
		static PixelFormat _getClosestSupportedPF(PixelFormat ogrePF);
	};
}
#endif
