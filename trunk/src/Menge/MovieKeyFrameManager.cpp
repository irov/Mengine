#	include "MovieKeyFrameManager.h"
#	include "MovieKeyConverter.h"

#	include "Interface/LoaderInterface.h"
#	include "Interface/ArchiveInterface.h"

#	include "Consts.h"

#	include "Archive/ArchiveRead.hpp"
#	include "Config/Blobject.h"

#	include "Logger/Logger.h"

#   include "Codec/ConverterFactory.h"

#	include "Metacode.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MovieKeyFrameService, Menge::MovieKeyFrameServiceInterface, Menge::MovieKeyFrameManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieKeyFrameManager::MovieKeyFrameManager()
        : m_serviceProvider(nullptr)
		, m_movieKeyConverter(nullptr)
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieKeyFrameManager::~MovieKeyFrameManager()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MovieKeyFrameManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MovieKeyFrameManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MovieKeyFrameManager::initialize()
	{
		m_movieKeyConverter = new ConverterFactory<MovieKeyConverter>(m_serviceProvider, CONST_STRING(m_serviceProvider, binToAekMovie));

		CONVERTER_SERVICE(m_serviceProvider)
			->registerConverter( CONST_STRING(m_serviceProvider, binToAekMovie), m_movieKeyConverter );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieKeyFrameManager::finalize()
	{
		CONVERTER_SERVICE(m_serviceProvider)
			->unregisterConverter( CONST_STRING(m_serviceProvider, binToAekMovie) );

		delete m_movieKeyConverter;
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieFramePackInterface * MovieKeyFrameManager::getMovieFramePak( const ConstString & _pak, const FilePath & _path )
	{
		MovieFramePackInterface * framePak = this->createMovieFramePak_( _pak, _path );
		 
		if( framePak == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "MovieKeyFrameManager: can`t load file category '%s' path '%s'"
				, _pak.c_str()
				, _path.c_str()
				);

			return nullptr;
		}

		return framePak;
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieFramePackInterface * MovieKeyFrameManager::createMovieFramePak_( const ConstString & _pak, const FilePath & _path )
	{
		InputStreamInterfacePtr input_stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _pak, _path );

		if( input_stream == nullptr )
		{
			return false;
		}

		size_t binary_aek_header;
		input_stream->read( &binary_aek_header, sizeof(binary_aek_header) );

		size_t binary_aek_size;
		input_stream->read( &binary_aek_size, sizeof(binary_aek_size) );

		size_t compress_size;
		input_stream->read( &compress_size, sizeof(compress_size) );

		static TBlobject compress;
		compress.resize(compress_size);

		input_stream->read( &compress[0], compress_size );

		static TBlobject binary_aek;
		binary_aek.resize(binary_aek_size);

		size_t uncompressSize = 0;
		if( ARCHIVE_SERVICE(m_serviceProvider)
			->uncompress( &binary_aek[0], binary_aek_size, uncompressSize, &compress[0], compress_size ) == false )
		{
			return false;
		}

		ArchiveRead ar(binary_aek.begin(), binary_aek.end());

		ar.begin();

		size_t layers_framepack_size;
		ar >> layers_framepack_size;

		MovieFramePack * pack = m_poolMovieFramePack.createObjectT();

		pack->initialize( layers_framepack_size );
		
		for( size_t it_layer = 0; it_layer != layers_framepack_size; ++it_layer )
		{
			bool immutable;
			ar >> immutable;

			if( immutable == true )
			{
				MovieFrameSource frame;
				ar >> frame;

				pack->initializeLayer( it_layer + 1, 0, true );
				pack->setLayerImmutableFrame( it_layer + 1, frame );
			}
			else
			{
				size_t frames_size;
				ar >> frames_size;

				pack->initializeLayer( it_layer + 1, frames_size, false );

				if( frames_size > 0 )
				{
					TVectorMovieFrameSource & frames = pack->mutableLayerFrames( it_layer + 1 );
					frames.resize(frames_size);

					ar.readPODs( &frames[0], frames_size );
				}
			}
		}

		size_t layers_timeremap_size;
		ar >> layers_timeremap_size;

		for( size_t it_layer = 0; it_layer != layers_timeremap_size; ++it_layer )
		{
			size_t times_size;
			ar >> times_size;

			if( times_size > 0 )
			{
				MovieLayerTimeRemap & timeremap = pack->mutableLayerTimeRemap( it_layer + 1 );
				timeremap.times.resize( times_size );

				ar.readPODs( &timeremap.times[0], times_size );
			}
		}

		//bool exist = false;
		//
		//Metacode::Meta_KeyFramesPack keyFramesPack;

  //      if( LOADER_SERVICE(m_serviceProvider)->load( _pak, _path, &keyFramesPack, exist ) == false )
  //      {
  //          if( exist == false )
  //          {
  //              LOGGER_ERROR(m_serviceProvider)( "MovieKeyFrameManager: KeyFramesFile '%s' not found"
  //                  , _path.c_str()
  //                  );
  //          }
  //          else
  //          {
  //              LOGGER_ERROR(m_serviceProvider)( "MovieKeyFrameManager: KeyFramesFile invalid parse '%s' "
  //                  , _path.c_str()
  //                  );
  //          }

  //          return nullptr;
  //      }

  //      size_t maxIndex = keyFramesPack.get_MaxIndex();

  //      MovieFramePack * pack = m_poolMovieFramePack.createObjectT();

  //      pack->initialize( maxIndex );

  //      const Metacode::Meta_KeyFramesPack::TVectorMeta_TimeRemap & includes_timeremaps = keyFramesPack.get_IncludesTimeRemap();

  //      for( Metacode::Meta_KeyFramesPack::TVectorMeta_TimeRemap::const_iterator 
  //          it = includes_timeremaps.begin(),
  //          it_end = includes_timeremaps.end();
  //      it != it_end;
  //      ++it )
  //      {
  //          const Metacode::Meta_KeyFramesPack::Meta_TimeRemap & meta_timeremap = *it;

  //          size_t layerIndex = meta_timeremap.get_LayerIndex();

  //          const Floats & floats = meta_timeremap.get_Time();

  //          MovieLayerTimeRemap timeremap;

  //          timeremap.times = floats;

  //          pack->addLayerTimeRemap( layerIndex, timeremap );
  //      }

  //      
  //      const Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames2D & includes_frames2d = keyFramesPack.get_IncludesKeyFrames2D(); 

  //      for( Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames2D::const_iterator
  //          it = includes_frames2d.begin(),
  //          it_end = includes_frames2d.end();
  //      it != it_end;
  //      ++it )
  //      {
  //          const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D & meta_frames2d = *it;

  //          size_t layerIndex = meta_frames2d.get_LayerIndex();
  //          
  //          size_t count = 0;
  //          meta_frames2d.get_Count( count );
  //          
  //          bool immutable = false;
  //          meta_frames2d.get_Immutable( immutable );

  //          pack->initializeLayer( layerIndex, count, immutable );

		//	if( immutable == true )
		//	{
		//		const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D & includes_frame2d = meta_frames2d.get_IncludesKeyFrame2D();

		//		for( Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D::const_iterator
		//			it = includes_frame2d.begin(),
		//			it_end = includes_frame2d.end();
		//		it != it_end;
		//		++it )
		//		{
		//			const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & meta_frame2d = *it;

		//			mt::vec2f anchorPoint2d;
		//			mt::vec2f position2d;
		//			mt::vec2f scale2d;
		//			float angle;
		//			float opacity = 1.f;

		//			size_t count = 1;

		//			float volume = 1.f;

		//			meta_frame2d.get_AnchorPoint( anchorPoint2d );
		//			meta_frame2d.get_Position( position2d );
		//			meta_frame2d.get_Scale( scale2d );
		//			meta_frame2d.get_Rotation( angle );
		//			meta_frame2d.get_Opacity( opacity );
		//			meta_frame2d.get_Count( count );
		//			meta_frame2d.get_Volume( volume );

		//			MovieFrameSource frame;

		//			frame.anchorPoint = mt::vec3f(anchorPoint2d, 0.f);
		//			frame.position = mt::vec3f(position2d, 0.f);
		//			frame.scale = mt::vec3f(scale2d, 1.f);
		//			frame.opacity = opacity;
		//			frame.volume = volume;

		//			frame.rotation.x = -angle;
		//			frame.rotation.y = 0.f;
		//			frame.rotation.z = 0.f;

		//			pack->setLayerImmutableFrame( layerIndex, frame );
		//		}
		//	}
		//	else
		//	{
		//		MovieFrameSource frame;
		//		bool frameLast = false;
		//
		//		const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D & includes_frame2d = meta_frames2d.get_IncludesKeyFrame2D();

		//		for( Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D::const_iterator
		//			it = includes_frame2d.begin(),
		//			it_end = includes_frame2d.end();
		//		it != it_end;
		//		++it )
		//		{
		//			const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & meta_frame2d = *it;

		//			size_t count = 1;
		//			
		//			mt::vec2f anchorPoint2d;
		//			mt::vec2f position2d;
		//			mt::vec2f scale2d;

		//			float angle;

		//			float volume = 1.f;

		//			if( frameLast == true )
		//			{
		//				angle = -frame.rotation.x;

		//				anchorPoint2d = frame.anchorPoint.to_vec2f();
		//				position2d = frame.position.to_vec2f();
		//				scale2d = frame.scale.to_vec2f();
		//				volume = frame.volume;
		//			}
		//			else
		//			{
		//				frameLast = true;
		//			}					

		//			meta_frame2d.get_AnchorPoint( anchorPoint2d );
		//			meta_frame2d.get_Position( position2d );
		//			meta_frame2d.get_Scale( scale2d );
		//			meta_frame2d.get_Rotation( angle );
		//			meta_frame2d.get_Opacity( frame.opacity );
		//			meta_frame2d.get_Count( count );
		//			meta_frame2d.get_Volume( volume );

		//			frame.anchorPoint = mt::vec3f(anchorPoint2d, 0.f);
		//			frame.position = mt::vec3f(position2d, 0.f);
		//			frame.scale = mt::vec3f(scale2d, 1.f);

		//			frame.volume = volume;

		//			frame.rotation.x = -angle;
		//			frame.rotation.y = 0.f;
		//			frame.rotation.z = 0.f;

		//			for( size_t i = 0; i != count; ++i )
		//			{
		//				pack->addLayerFrame( layerIndex, frame );
		//			}
		//		}
		//	}
		//}

    //    const Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames3D & includes_frames3d = keyFramesPack.get_IncludesKeyFrames3D(); 

    //    for( Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames3D::const_iterator
    //        it = includes_frames3d.begin(),
    //        it_end = includes_frames3d.end();
    //    it != it_end;
    //    ++it )
    //    {
    //        const Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D & meta_frames3d = *it;

    //        size_t layerIndex = meta_frames3d.get_LayerIndex();

    //        size_t count = 0;
    //        meta_frames3d.get_Count( count );

    //        bool immutable = false;
    //        meta_frames3d.get_Immutable( immutable );

    //        pack->initializeLayer( layerIndex, count, immutable );

    //        const Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::TVectorMeta_KeyFrame3D & includes_frame3d = meta_frames3d.get_IncludesKeyFrame3D();

    //        for( Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::TVectorMeta_KeyFrame3D::const_iterator
    //            it = includes_frame3d.begin(),
    //            it_end = includes_frame3d.end();
    //        it != it_end;
    //        ++it )
    //        {
    //            const Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D & meta_frame3d = *it;

    //            size_t count = 1;

    //            mt::vec3f anchorPoint;
    //            mt::vec3f rotation;
    //            mt::vec3f position;
    //            mt::vec3f scale;

    //            MovieFrameSource frame;

    //            float volume = 1.f;

    //            if( pack->getLayerFrameLast( layerIndex, frame ) == true )
    //            {
    //                anchorPoint = frame.anchorPoint;
    //                position = frame.position;
    //                rotation = frame.rotation;
    //                scale = frame.scale;
    //                volume = frame.volume;
    //            }

    //            meta_frame3d.get_AnchorPoint( frame.anchorPoint );
    //            meta_frame3d.get_Position( frame.position );
    //            meta_frame3d.get_Rotation( frame.rotation );
    //            meta_frame3d.get_Scale( frame.scale );                
    //            meta_frame3d.get_Opacity( frame.opacity );
    //            meta_frame3d.get_Count( count );

    //            //if( meta_frame3d.has_Mask() == true )
    //            //{
    //            //    Polygon polygon;
    //            //    meta_frame3d.get_Mask( polygon );

    //            //    boost::geometry::correct( polygon );

    //            //    size_t index = pack->addPolygon( polygon );

    //            //    frame.mask = index;
    //            //}
    //            //else
    //            //{
    //            //    frame.mask = mask;
    //            //}
				//
				//frame.volume = 1.f;
				//meta_frame3d.get_Volume( frame.volume );

    //            if( pack->isLayerImmutable( layerIndex ) == false )
    //            {
    //                for( size_t i = 0; i != count; ++i )
    //                {
    //                    pack->addLayerFrame( layerIndex, frame );
    //                }
    //            }
    //            else
    //            {
    //                pack->setLayerImmutableFrame( layerIndex, frame );
    //            }
    //        }
    //    }

		return pack;
	}
	////////////////////////////////////////////////////////////////////////////////
}
