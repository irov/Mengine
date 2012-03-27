#	include "ResourceAnimation.h"
#	include "ResourceImplement.h"

#	include "ResourceImage.h"
#	include "ResourceAnimation.h"

#	include "ResourceManager.h"

#	include "BinParser.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceAnimation );
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimation::ResourceAnimation()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Sequence )
			{
				AnimationSequence sq;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Sequence_ResourceImageName, sq.resourceName );
					BIN_CASE_ATTRIBUTE( Protocol::Sequence_Delay, sq.delay );
				}

				m_sequence.push_back( sq );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceAnimation::_compile()
	{
		if( m_sequence.empty() )
		{
			MENGE_LOG_ERROR( "Animation: sequence count is empty '%s'"
				, getName().c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimation::getSequenceCount() const
	{
		return m_sequence.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceAnimation::getSequenceDelay( size_t _sequence ) const
	{
		return m_sequence[_sequence].delay;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceAnimation::getSequenceResourceName( size_t _sequence ) const
	{
		return m_sequence[_sequence].resourceName;
	}	
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceAnimation::getLastFrameIndex() const
	{
		return m_sequence.size() - 1;	
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::setSequences( const TVectorAnimationSequence & _sequence )
	{
		m_sequence = _sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorAnimationSequence & ResourceAnimation::getSequences() const
	{
		return m_sequence;
	}
	//////////////////////////////////////////////////////////////////////////
}
