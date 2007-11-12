#	include "Emitter.h" 

#	include "LogEngine.h"

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "ParticleEngine.h"

#	include "Viewport.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ResourceEmitterContainer.h"

#	include "ResourceManager.h"

#	include "math/box2.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Emitter);
	//////////////////////////////////////////////////////////////////////////
	Emitter::Emitter()
		: m_image( 0 )
		, m_interface( 0 )
		, m_resourceEmitter( 0 )
	{}
	//////////////////////////////////////////////////////////////////////////
	Emitter::~Emitter()
	{}	
	///////////////////////////////////////////////////////////////////////////
	bool Emitter::isVisible( const Viewport & _viewPort )
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::_activate()
	{
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_deactivate()
	{
		SceneNode2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::_compile()
	{
		if( SceneNode2D::_compile() == false )
		{
			return false;
		}

		m_resourceEmitter = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceEmitterContainer>( m_resourceName );

		if( m_resourceEmitter == NULL )
		{
			MENGE_LOG( "Error: Emitter can't open resource file '%s'\n", m_resourceName.c_str() );
			return false;
		}

		const EmitterContainerInterface * m_container = m_resourceEmitter->getContainer();

		if( m_container == NULL )
		{
			MENGE_LOG( "Error: Emitter can't open container file '%s'\n", m_resourceName.c_str() );
			return false;
		}

		m_interface = Holder<ParticleEngine>::hostage()->createEmitterFromContainer( m_emitterName.c_str(), m_container );

		if( m_interface == 0 )
		{
			MENGE_LOG( "Error: Emitter can't create emitter source '%s'\n", m_resourceName.c_str() );
			return false;
		}

		m_image = m_resourceEmitter->getImage();
	
		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_release()
	{
		SceneNode2D::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "Resource", "Name", m_resourceName );
			XML_CHECK_VALUE_NODE( "Emitter", "Name", m_emitterName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_render()
	{
		if( isPlaying() == false )
		{
			return;
		}
		int n = m_interface->getParticlesTypeCount();

		const mt::mat3f & rwm = getWorldMatrix();

		for ( int i = n - 1; i >= 0; i-- )
		{
			bool nextParticleType = false;

			Holder<ParticleEngine>::hostage()->lockEmitter( m_interface, i );

			///	int texture_frame=Magic_GetTextureFrame();
			//	MAGIC_TEXTURE* magic_texture=&(m_texture[texture_frame]);					

			while ( nextParticleType == false )
			{
				RenderParticle * p = Holder<ParticleEngine>::hostage()->nextParticle();
				if( p == NULL )
				{
					nextParticleType = true;
				}
				else
				{
					mt::vec4f uv(p->u0, p->v0, p->u1, p->v1);

					Holder<RenderEngine>::hostage()->renderImage(
						rwm, 
						mt::vec2f(p->x1, p->y1),
						mt::vec2f(p->x2, p->y2),
						mt::vec2f(p->x3, p->y3),
						mt::vec2f(p->x4, p->y4),
						uv,
						p->color,
						m_image
						);
					}
			}

			Holder<ParticleEngine>::hostage()->unlockEmitter( m_interface );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::play( bool _leftVisible )
	{
		m_interface->play( _leftVisible );
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::stop()
	{
		m_interface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::isPlaying() const
	{
		return m_interface->isPlaying();
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_update( float _timing )
	{
		m_interface->update();
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::_debugRender()
	{
	}
	//////////////////////////////////////////////////////////////////////////
}