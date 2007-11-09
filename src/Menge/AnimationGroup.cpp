#	include "AnimationGroup.h"

#	include "Animation.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(AnimationGroup)
	//////////////////////////////////////////////////////////////////////////
	AnimationGroup::AnimationGroup()
	: m_currentAnimation( NULL )
	{}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE( "Animations" )
			{
				SceneNode2D::loader(XML_CURRENT_NODE);
			}		
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool AnimationGroup::_compile()
	{
		if( SceneNode2D::_compile() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::_update( float _timing )
	{
		SceneNode2D::_update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AnimationGroup::_activate()
	{
		if( SceneNode2D::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::_deactivate()
	{
		SceneNode2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::play( const std::string& _name )
	{
		if( isActivate() == false )
		{
			return;
		}

		setCurrentAnimation( _name );
	
		if( m_currentAnimation )
		{
			m_currentAnimation->play();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::stop( const std::string& _name )
	{
		setCurrentAnimation( _name );

		if( m_currentAnimation )
		{
			m_currentAnimation->stop();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::pause( const std::string & _name )
	{
		setCurrentAnimation( _name );

		if( m_currentAnimation )
		{
			m_currentAnimation->pause();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::setLooped( const std::string & _name, bool _looped )
	{
		setCurrentAnimation( _name );

		if( m_currentAnimation )
		{
			m_currentAnimation->setLooped( _looped );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool AnimationGroup::getLooped( const std::string & _name )
	{
		setCurrentAnimation( _name );

		if( m_currentAnimation )
		{
			return m_currentAnimation->getLooped();
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::setAnimationListener( const std::string & _name, PyObject * _listener )
	{
		setCurrentAnimation( _name );

		if( m_currentAnimation )
		{
			m_currentAnimation->setAnimationListener( _listener );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::setAnimationResource( const std::string & _name, const std::string & _resource )
	{
		setCurrentAnimation( _name );

		if( m_currentAnimation )
		{
			m_currentAnimation->setAnimationResource( _resource );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::setCurrentAnimation( const std::string & _name )
	{
		if( m_currentAnimation )
		{
			m_currentAnimation->disable();
		}

		m_currentAnimation = this->getChildrenT<Animation>( _name );
	}
}