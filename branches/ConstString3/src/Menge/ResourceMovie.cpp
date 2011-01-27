#	include "ResourceMovie.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"

#	include "BinParser.h"

#	include "Utils/Core/Stream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceMovie );
	//////////////////////////////////////////////////////////////////////////
	ResourceMovie::ResourceMovie()
		: m_duration(0.f)
		, m_width(0.f)
		, m_height(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceMovie::~ResourceMovie()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceMovie::getLayerSize() const
	{
		return m_layers.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const MovieLayer & ResourceMovie::getLayer( std::size_t _index ) const
	{
		return m_layers[_index];
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::loader( BinParser * _parser )
	{
		ResourceReference::loader(_parser);

		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_NODE( Protocol::MovieLayer )
			{
				m_layers.push_back( MovieLayer() );
				MovieLayer & ml = m_layers.back();

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer_Index, ml.index );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer_In, ml.in );
					BIN_CASE_ATTRIBUTE( Protocol::MovieLayer_Out, ml.out );
				}

				BIN_PARSE_METHOD_ARG1( this, &ResourceMovie::loaderMovieLayer_, ml );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::loaderMovieLayer_( BinParser * _parser, MovieLayer & _ml )
	{
		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_NODE( Protocol::KeyFrame )
			{
				MovieLayer::Frame frame;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame_AnchorPoint, frame.anchorPoint );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame_Position, frame.position );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame_Scale, frame.scale );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame_Rotation, frame.angle );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame_Opacity, frame.opacity );
				}

				_ml.frames.push_back( frame );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie::_compile()
	{
		if( ResourceReference::_compile() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie::_release()
	{
		ResourceReference::_release();

		m_layers.clear();
	}
}