#	include "MovieKeyFrameManager.h"

#	include "LogEngine.h"
#	include "Metacode.h"

#	include "LoaderEngine.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieKeyFrameManager::MovieKeyFrameManager()
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieKeyFrameManager::~MovieKeyFrameManager()
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieFramePack * MovieKeyFrameManager::getMovieFramePak( const ConstString & _pak, const WString & _path )
	{
		MovieFramePack * framePak = this->createMovieFramePak_( _pak, _path );
		 
		if( framePak == NULL )
		{
			MENGE_LOG_ERROR( "MovieKeyFrameManager: can`t load file category '%s' path '%S'"
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
		
		Metacode::Meta_KeyFramesPack keyFramesPack;

        if( LoaderEngine::get()
            ->load( _pak, _path, &keyFramesPack, exist ) == false )
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

            return NULL;
        }

        size_t maxIndex = keyFramesPack.get_MaxIndex();

        MovieFramePack * pack = new MovieFramePack(maxIndex);
        
        const Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames2D & includes_frames2d = keyFramesPack.get_IncludesKeyFrames2D(); 

        for( Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames2D::const_iterator
            it = includes_frames2d.begin(),
            it_end = includes_frames2d.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D & meta_frames2d = *it;

            size_t layerIndex = meta_frames2d.get_LayerIndex();
            
            size_t count = -1;
            meta_frames2d.get_Count( count );
            
            bool immutable = false;
            meta_frames2d.get_Immutable( immutable );

            pack->initializeLayer( layerIndex, count, immutable );

            const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D & includes_frame2d = meta_frames2d.get_IncludesKeyFrame2D();

            for( Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D::const_iterator
                it = includes_frame2d.begin(),
                it_end = includes_frame2d.end();
            it != it_end;
            ++it )
            {
                const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & meta_frame2d = *it;

                size_t count = 1;

                float angle;

                mt::vec2f anchorPoint2d;
                mt::vec2f position2d;
                mt::vec2f scale2d;

                MovieFrameSource frame;
				
				float volume = 1.f;

                if( pack->getLayerFrameLast( layerIndex, frame ) == true )
                {
                    angle = -frame.rotation.x;

                    anchorPoint2d = frame.anchorPoint.to_vec2f();
                    position2d = frame.position.to_vec2f();
                    scale2d = frame.scale.to_vec2f();
					volume = frame.volume;
                }

                meta_frame2d.get_AnchorPoint( anchorPoint2d );
                meta_frame2d.get_Position( position2d );
                meta_frame2d.get_Scale( scale2d );
                meta_frame2d.get_Rotation( angle );
                meta_frame2d.get_Opacity( frame.opacity );
                meta_frame2d.get_Count( count );
				meta_frame2d.get_Volume( volume )

                frame.anchorPoint = mt::vec3f(anchorPoint2d, 0.f);
                frame.position = mt::vec3f(position2d, 0.f);
                frame.scale = mt::vec3f(scale2d, 1.f);

				frame.volume = volume;

                frame.rotation.x = -angle;
                frame.rotation.y = 0.f;
                frame.rotation.z = 0.f;

                if( pack->isLayerImmutable( layerIndex ) == false )
                {
                    for( size_t i = 0; i != count; ++i )
                    {
                        pack->addLayerFrame( layerIndex, frame );
                    }
                }
                else
                {
                    pack->setLayerImmutableFrame( layerIndex, frame );
                }
            }
        }

        const Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames3D & includes_frames3d = keyFramesPack.get_IncludesKeyFrames3D(); 

        for( Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames3D::const_iterator
            it = includes_frames3d.begin(),
            it_end = includes_frames3d.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D & meta_frames3d = *it;

            size_t layerIndex = meta_frames3d.get_LayerIndex();

            pack->initializeLayer( layerIndex, -1, false );

            const Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::TVectorMeta_KeyFrame3D & includes_frame3d = meta_frames3d.get_IncludesKeyFrame3D();

            for( Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::TVectorMeta_KeyFrame3D::const_iterator
                it = includes_frame3d.begin(),
                it_end = includes_frame3d.end();
            it != it_end;
            ++it )
            {
                const Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D & meta_frame3d = *it;

                MovieFrameSource frame;

                meta_frame3d.get_AnchorPoint( frame.anchorPoint );
                meta_frame3d.get_Position( frame.position );
                meta_frame3d.get_Rotation( frame.rotation );
                meta_frame3d.get_Scale( frame.scale );
                meta_frame3d.get_Opacity( frame.opacity );
				
				frame.volume = 1.f;
				meta_frame3d.get_Volume( frame.volume );

                pack->addLayerFrame( layerIndex, frame );
            }
        }

		return pack;
	}
	////////////////////////////////////////////////////////////////////////////////
	void MovieKeyFrameManager::releaseMovieFramePak( MovieFramePack * _framePak )
	{
		delete _framePak;
	}
	////////////////////////////////////////////////////////////////////////////////
}
