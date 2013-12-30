#	include "Landscape2D.h" 
#	include "Layer2D.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "ResourceImage.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/clamp.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Landscape2D::Landscape2D()
		: m_materialGroup(nullptr)
		, m_material(nullptr)
		, m_texturesNum(0)
        , m_invalidateMaterial(true)
		, m_invalidateVerticesWM(true)
	{ 

	}

	//////////////////////////////////////////////////////////////////////////
	Landscape2D::~Landscape2D()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	bool Landscape2D::_compile()
	{
		if( this->compileResources_() == false )
		{
			return false;
		}
        
        this->invalidateMaterial();
		
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool Landscape2D::compileResources_()
	{
		int m_resourceImage_size = m_resourceImage.size();
		if( m_resourceImage_size == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "Landscape2D::compileResources_ '%s' image list size = 0"
				, m_name.c_str() 
				);

			return false;
		}

		for(int i=0;i<m_resourceImage_size;i++)
		{
			if( m_resourceImage[i]->compile() == false )
			{
				LOGGER_ERROR(m_serviceProvider)( "Sprite::compileResources_ '%s' image resource %s not compile"
					, m_name.c_str() 
					, m_resourceImage[i]->getName().c_str()
					);

				return false;
			}

			m_textures.push_back(m_resourceImage[i]->getTexture());
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::_release()
	{
		int m_resourceImage_size = m_resourceImage.size();
		for(int i=0;i<m_resourceImage_size;i++)
		{
			m_resourceImage[i]->release();
			m_textures[i]=nullptr;
		}

		m_verticesWM.clear();

		m_materialGroup = nullptr;
		m_material = nullptr;
	}

    //////////////////////////////////////////////////////////////////////////
    void Landscape2D::invalidateMaterial()
    {
        m_invalidateMaterial = true;
    }

	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::updateMaterial()
	{
        m_invalidateMaterial = false;

		m_texturesNum = 1;

		m_materialGroup = RENDERMATERIAL_SERVICE(m_serviceProvider)
				->getMaterialGroup( CONST_STRING(m_serviceProvider, SolidSprite) );

		ETextureAddressMode textureU = TAM_CLAMP;
		ETextureAddressMode textureV = TAM_CLAMP;

		m_material = m_materialGroup->getMaterial( textureU, textureV );

		if( m_material == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Landscape2D::updateMaterial_ %s m_material is NULL"
				, this->getName().c_str()
				);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::_render( RenderCameraInterface * _camera )
	{
		Node::_render( _camera );
		
        const RenderMaterial * material = this->getMaterial();
		const RenderVertex2D * vertices = this->getVerticesWM();

		for(int i=0, index=0;i<m_textures.size();i++,index+=4)
		{
			RENDER_SERVICE(m_serviceProvider)
				->addRenderQuad( _camera, material, &m_textures[i], m_texturesNum, &vertices[index], 4 );		
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::setBackParts( const TVResourceImage  &_resourceImage, int _countX, int _countY)
	{
		m_countX = _countX;
		m_countY = _countY;
	    m_resourceImage = _resourceImage;
	}

	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::invalidateVerticesWM()
	{
		m_invalidateVerticesWM = true;
	}

	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::updateVerticesWM()
	{
		size_t length = m_resourceImage.size()*4;
		m_verticesWM.resize(length);

		const mt::mat4f & wm = this->getWorldMatrix();

		m_invalidateVerticesWM = false;

		for(int y=0; y<m_countY; y++)
		{
			for(int x=0; x<m_countX; x++)
			{	
				int ri = y * m_countX + x; 
				const mt::vec4f & uv = m_resourceImage[ri]->getUVImage();
				mt::vec2f sz = m_resourceImage[ri]->getMaxSize();

				// left-upper
				RenderVertex2D vertex1;
				vertex1.color=0xffffffff;
				vertex1.uv.x =uv.x;
				vertex1.uv.y =uv.y;
				vertex1.uv2.x=0.0f;
				vertex1.uv2.y=0.0f;

				mt::vec3f p1;
				p1.x = x * sz.x;
				p1.y = y * sz.y;
				p1.z = 0.0f;
				mt::mul_v3_m4( vertex1.pos, p1, wm );

				m_verticesWM[ri*4]=vertex1;

				// right-upper		
				RenderVertex2D vertex2;
				vertex2.color=0xffffffff;
				vertex2.uv.x =uv.z;
				vertex2.uv.y =uv.y;
				vertex2.uv2.x=0.0f;
				vertex2.uv2.y=0.0f;

				mt::vec3f p2;
				p2.x = (x+1) * sz.x;
				p2.y = y * sz.y;
				p2.z = 0.0f;
				mt::mul_v3_m4( vertex2.pos, p2, wm );

				m_verticesWM[(ri*4)+1]=vertex2;

				// right-down
				RenderVertex2D vertex3;
				vertex3.color=0xffffffff;
				vertex3.uv.x =uv.z;
				vertex3.uv.y =uv.w;
				vertex3.uv2.x=0.0f;
				vertex3.uv2.y=0.0f;

				mt::vec3f p3;
				p3.x = (x+1) * sz.x;
				p3.y = (y+1) * sz.y;
				p3.z = 0.0f;
				mt::mul_v3_m4( vertex3.pos, p3, wm );

				m_verticesWM[(ri*4)+2]=vertex3;

				// left-down
				RenderVertex2D vertex4;
				vertex4.color=0xffffffff;
				vertex4.uv.x =uv.x;
				vertex4.uv.y =uv.w;
				vertex4.uv2.x=0.0f;
				vertex4.uv2.y=0.0f;

				mt::vec3f p4;
				p4.x = x* sz.x;
				p4.y = (y+1) * sz.y;
				p4.z = 0.0f;
				mt::mul_v3_m4( vertex4.pos, p4, wm );

				m_verticesWM[(ri*4)+3]=vertex4;
			}
		}	
	}

	//////////////////////////////////////////////////////////////////////////
	void Landscape2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateVerticesWM();
	}
}
