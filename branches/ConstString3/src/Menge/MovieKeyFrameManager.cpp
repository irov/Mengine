#	include "MovieKeyFrameManager.h"
#	include "LogEngine.h"
#	include "BinParser.h"
#	include "Loadable.h"
#	include "LoaderEngine.h"


namespace Menge
{
	namespace 
	{
		class LoadableMovieKeyFrameManager
			: public Loadable
		{
		public:
			LoadableMovieKeyFrameManager( MovieKeyFrameManager * _manager, MovieFramePack * _pak )
				: m_manager(_manager)
				, m_pack(_pak)
			{
			}

		protected:
			void loader( BinParser * _parser ) override
			{
				m_manager->loaderMovieFramePak( _parser, m_pack );
			}

		protected:
			MovieKeyFrameManager * m_manager;
			MovieFramePack * m_pack;
		};
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieKeyFrameManager::MovieKeyFrameManager()
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieKeyFrameManager::~MovieKeyFrameManager()
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	void MovieKeyFrameManager::loaderMovieFramePak( BinParser * _parser, MovieFramePack * _framePak )
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

				_framePak->initialise( maxIndex );

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

					BIN_PARSE_METHOD_CARG2( this, &MovieKeyFrameManager::loaderKeyFrames2D_, layerIndex, _framePak );
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

					BIN_PARSE_METHOD_CARG2( this, &MovieKeyFrameManager::loaderKeyFrames3D_, layerIndex, _framePak );
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	void MovieKeyFrameManager::loaderKeyFrames2D_( BinParser * _parser, size_t _layerIndex, MovieFramePack * _framePak )
	{
		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_NODE( Protocol::KeyFrame2D )
			{
				MovieFrameSource frame;				

				float angle;

				if( _framePak->isLayerEmpty( _layerIndex ) == false)
				{
					const MovieFrameSource & old_frame = _framePak->getLastLayerFrame( _layerIndex );

					frame.scale = old_frame.scale;
					frame.position = old_frame.position;
					frame.rotation = old_frame.rotation;
					frame.opacity = old_frame.opacity;
					frame.anchorPoint = old_frame.anchorPoint;
					angle = frame.rotation.z;
				}

				size_t count = 1;
				
				mt::vec2f anchorPoint2d = frame.anchorPoint.to_vec2f();
				mt::vec2f position2d = frame.position.to_vec2f();
				mt::vec2f scale2d = frame.scale.to_vec2f();

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_AnchorPoint, anchorPoint2d );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Position, position2d );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Scale, scale2d );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Rotation, angle );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Opacity, frame.opacity );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame2D_Index, count );
				}

				frame.anchorPoint = mt::vec3f(anchorPoint2d, 0.f);
				frame.position = mt::vec3f(position2d, 0.f);
				frame.scale = mt::vec3f(scale2d, 1.f);
				
				frame.rotation.x = 0;
				frame.rotation.y = 0;
				frame.rotation.z = angle;
				
				for( size_t i = 0; i != count; ++i )
				{
					_framePak->setLayerFrame( _layerIndex, frame );
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	void MovieKeyFrameManager::loaderKeyFrames3D_( BinParser * _parser, size_t _layerIndex, MovieFramePack * _framePak )
	{
		BIN_SWITCH_ID(_parser)
		{
			BIN_CASE_NODE( Protocol::KeyFrame3D )
			{
				MovieFrameSource frame;
				mt::vec2f scale;
				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_AnchorPoint, frame.anchorPoint );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Position, frame.position );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Rotation, frame.rotation );
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Scale, scale );					
					BIN_CASE_ATTRIBUTE( Protocol::KeyFrame3D_Opacity, frame.opacity );
				}

				frame.scale = mt::vec3f(scale,1);
				_framePak->setLayerFrame( _layerIndex, frame );
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieFramePack * MovieKeyFrameManager::getMovieFramePak( const ConstString & _pak, const WString & _path )
	{
		MovieFramePack * framePak = this->createMovieFramePak_( _pak, _path );
		 
		if( framePak == NULL )
		{
			MENGE_LOG_ERROR( "MovieKeyFrameManager: can`t load file category %s path %S"
				, _pak.c_str()
				, _path.c_str()
				);

			return NULL;
		}

		return framePak;
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieFramePack * MovieKeyFrameManager::createMovieFramePak_( const ConstString & _pak, const WString & _path )
	{
		bool exist = false;
		
		MovieFramePack * framePack = new MovieFramePack();

		LoadableMovieKeyFrameManager loadable( this, framePack );

		if( LoaderEngine::get()
			->load( _pak, _path, &loadable, exist ) == false )
		{
			if( exist == false )
			{
				MENGE_LOG_ERROR( "MovieKeyFrameManager: KeyFramesFile '%S' not found"
					, _path.c_str()
					);
			}
			else
			{
				MENGE_LOG_ERROR( "MovieKeyFrameManager: KeyFramesFile invalid parse '%S' "
					, _path.c_str()
					);
			}

			delete framePack;
			return NULL;
		}

		return framePack;
	}
	////////////////////////////////////////////////////////////////////////////////
	void MovieKeyFrameManager::releaseMovieFramePak( MovieFramePack * _framePak )
	{
		//TVectorMovieFramePack::iterator it = m_framePacks.find(_framePak);
		//if( it == m_framePacks.end() )
		//{
		//	MENGE_LOG_ERROR( "MovieKeyFrameManager: releaseMovieFramePak  not exist framePack"
		//		); 
		//	return;
		//}

		//m_framePacks.erase(_framePak);
		delete _framePak;
	}

}
