#	include "AnimationGroup.h"

#	include "Animation.h"

#	include "ObjectImplement.h"

#	include "ResourceManager.h"
#	include "LogEngine.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(AnimationGroup)
	//////////////////////////////////////////////////////////////////////////
	AnimationGroup::AnimationGroup()
	: m_currentAnimation( 0 )
	{}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::loader( XmlElement * _xml )
	{
		SceneNode2D::loader(_xml);

		//XML_FOR_EACH_TREE( _xml )
		//{
		//	XML_CHECK_NODE( "Animations" )
		//	{
		//		SceneNode2D::loader(XML_CURRENT_NODE);
		//	}		
		//}
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
	void AnimationGroup::stop()
	{
		if( m_currentAnimation )
		{
			m_currentAnimation->stop();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::pause()
	{
		if( m_currentAnimation )
		{
			m_currentAnimation->pause();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::setLooped( bool _loop )
	{
		if( m_currentAnimation )
		{
			m_currentAnimation->setLooped( _loop );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool AnimationGroup::getLooped() const
	{
		if( m_currentAnimation )
		{
			return m_currentAnimation->getLooped();
		}
		// ???
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::setAnimationListener( PyObject * _listener )
	{
		if( m_currentAnimation )
		{
			m_currentAnimation->setAnimationListener( _listener );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AnimationGroup::setAnimationResource( const std::string & _resource )
	{
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

		if( m_currentAnimation == NULL )
		{
			MENGE_LOG("AnimationGroup: no found animation with name %s \n", _name.c_str() );
			return;
		}
	}
}