#	include "LoaderMovieKeyFrame.h"
#	include "MovieFramePack.h"

#	include "BinParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	LoaderMovieKeyFrame::LoaderMovieKeyFrame()
		: m_pack(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MovieFramePack * LoaderMovieKeyFrame::getFramePack() const
	{
		return m_pack;
	}
	//////////////////////////////////////////////////////////////////////////
	void LoaderMovieKeyFrame::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::KeyFramesPack )
			{
				size_t maxIndex;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::KeyFramesPack_MaxIndex, maxIndex );
				}

				m_pack = new MovieFramePack(maxIndex);

				BIN_PARSE_METHOD( this, &LoaderMovieKeyFrame::loaderKeyFramesPack_ );
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	void LoaderMovieKeyFrame::loaderKeyFramesPack_( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::KeyFrames2D )
			{			
				size_t layerIndex = 0;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrames2D_LayerIndex, layerIndex );
				}

				if( layerIndex == 0 )
				{
					MENGE_LOG_ERROR( "MovieKeyFrameManager: loaderMovieFramePak KeyFrames2D layer not determined  " );
					BIN_SKIP();
				}

				BIN_PARSE_METHOD_CARG1( this, &LoaderMovieKeyFrame::loaderKeyFrames2D_, layerIndex );
			}

			BIN_CASE_NODE( Protocol::KeyFrames3D )
			{
				size_t layerIndex = 0;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrames3D_LayerIndex, layerIndex );
				}

				if( layerIndex == 0 )
				{
					MENGE_LOG_ERROR( "MovieKeyFrameManager: loaderMovieFramePak KeyFrames2D layer not determined  " );
					BIN_SKIP();
				}

				BIN_PARSE_METHOD_CARG1( this, &LoaderMovieKeyFrame::loaderKeyFrames3D_, layerIndex );
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	void LoaderMovieKeyFrame::loaderKeyFrames2D_( BinParser * _parser, size_t _layerIndex )
	{
		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_NODE( Protocol::KeyFrame2D )
			{
				size_t count = 1;

				float angle;

				mt::vec2f anchorPoint2d;
				mt::vec2f position2d;
				mt::vec2f scale2d;

				MovieFrameSource frame;
				if( m_pack->getLayerFrameLast( _layerIndex, frame ) == true )
				{
					angle = -frame.rotation.x;

					anchorPoint2d = frame.anchorPoint.to_vec2f();
					position2d = frame.position.to_vec2f();
					scale2d = frame.scale.to_vec2f();
				}				

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_AnchorPoint, anchorPoint2d );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Position, position2d );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Scale, scale2d );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Rotation, angle );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Opacity, frame.opacity );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Count, count );
				}

				frame.anchorPoint = mt::vec3f(anchorPoint2d, 0.f);
				frame.position = mt::vec3f(position2d, 0.f);
				frame.scale = mt::vec3f(scale2d, 1.f);

				frame.rotation.x = -angle;
				frame.rotation.y = 0.f;
				frame.rotation.z = 0.f;

				for( size_t i = 0; i != count; ++i )
				{
					m_pack->addLayerFrame( _layerIndex, frame );
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	void LoaderMovieKeyFrame::loaderKeyFrames3D_( BinParser * _parser, size_t _layerIndex )
	{
		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_NODE( Protocol::KeyFrame3D )
			{
				MovieFrameSource frame;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_AnchorPoint, frame.anchorPoint );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Position, frame.position );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Rotation, frame.rotation );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Scale, frame.scale );					
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Opacity, frame.opacity );
				}

				m_pack->addLayerFrame( _layerIndex, frame );
			}
		}
	}
}