#   include "MovieKeyConverterXMLToAEK.h"

#	include "Interface/LoaderInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Metacode/Metacode.h"

#	include "Archive/ArchiveWrite.hpp"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MovieKeyConverterXMLToAEK::MovieKeyConverterXMLToAEK( ServiceProviderInterface * _serviceProvider )
		: DevelopmentConverter(_serviceProvider)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MovieKeyConverterXMLToAEK::~MovieKeyConverterXMLToAEK()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool MovieKeyConverterXMLToAEK::initialize()
	{
		m_convertExt = ".aek";

		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool MovieKeyConverterXMLToAEK::convert()
	{
		MovieFramePack movieFramePack;
		if( this->loadFramePak_( movieFramePack ) == false )
		{
			return false;
		}		
		
		if( this->writeFramePak_( movieFramePack ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieKeyConverterXMLToAEK::loadFramePak_( MovieFramePack & _movieFramePack )
	{
		bool exist = false;

		Metacode::Meta_KeyFramesPack keyFramesPack;

		String binPath( m_options.inputFileName.c_str(), m_options.inputFileName.size() );

		String::size_type size = binPath.size();
		binPath[size-3] = L'b';
		binPath[size-2] = L'i';
		binPath[size-1] = L'n';

		FilePath path_bin = Helper::stringizeString( m_serviceProvider, binPath );

		if( LOADER_SERVICE(m_serviceProvider)->load( m_options.pakName, path_bin, &keyFramesPack, exist ) == false )
		{
			if( exist == false )
			{
				LOGGER_ERROR(m_serviceProvider)( "MovieKeyConverter::convert: KeyFramesFile '%s' not found"
					, m_options.inputFileName.c_str()
					);
			}
			else
			{
				LOGGER_ERROR(m_serviceProvider)( "MovieKeyConverter::convert: KeyFramesFile invalid parse '%s' "
					, m_options.inputFileName.c_str()
					);
			}

			return false;
		}

		size_t maxIndex = keyFramesPack.get_MaxIndex();

		_movieFramePack.initialize( maxIndex );

		const Metacode::Meta_KeyFramesPack::TVectorMeta_TimeRemap & includes_timeremaps = keyFramesPack.get_IncludesTimeRemap();

		for( Metacode::Meta_KeyFramesPack::TVectorMeta_TimeRemap::const_iterator 
			it = includes_timeremaps.begin(),
			it_end = includes_timeremaps.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_KeyFramesPack::Meta_TimeRemap & meta_timeremap = *it;

			size_t layerIndex = meta_timeremap.get_LayerIndex();

			const Floats & floats = meta_timeremap.get_Time();

			MovieLayerTimeRemap timeremap;

			timeremap.times = floats;

			_movieFramePack.addLayerTimeRemap( layerIndex, timeremap );
		}


		const Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames2D & includes_frames2d = keyFramesPack.get_IncludesKeyFrames2D(); 

		for( Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames2D::const_iterator
			it = includes_frames2d.begin(),
			it_end = includes_frames2d.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D & meta_frames2d = *it;

			size_t layerIndex = meta_frames2d.get_LayerIndex();

			size_t count = 0;
			meta_frames2d.get_Count( count );

			bool immutable = false;
			meta_frames2d.get_Immutable( immutable );

			_movieFramePack.initializeLayer( layerIndex, count, immutable );

			if( immutable == true )
			{
				const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D & includes_frame2d = meta_frames2d.get_IncludesKeyFrame2D();

				for( Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D::const_iterator
					it = includes_frame2d.begin(),
					it_end = includes_frame2d.end();
				it != it_end;
				++it )
				{
					const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & meta_frame2d = *it;

					mt::vec2f anchorPoint2d;
					mt::vec2f position2d;
					mt::vec2f scale2d;
					float angle;
					float opacity = 1.f;

					size_t count = 1;

					float volume = 1.f;

					meta_frame2d.get_AnchorPoint( anchorPoint2d );
					meta_frame2d.get_Position( position2d );
					meta_frame2d.get_Scale( scale2d );
					meta_frame2d.get_Rotation( angle );
					meta_frame2d.get_Opacity( opacity );
					meta_frame2d.get_Count( count );
					meta_frame2d.get_Volume( volume );

					MovieFrameSource frame;

					frame.anchorPoint = mt::vec3f(anchorPoint2d, 0.f);
					frame.position = mt::vec3f(position2d, 0.f);
					frame.scale = mt::vec3f(scale2d, 1.f);
					frame.opacity = opacity;
					frame.volume = volume;

					frame.rotation.x = -angle;
					frame.rotation.y = 0.f;
					frame.rotation.z = 0.f;

					_movieFramePack.setLayerImmutableFrame( layerIndex, frame );
				}
			}
			else
			{
				MovieFrameSource frame;
				bool frameLast = false;

				const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D & includes_frame2d = meta_frames2d.get_IncludesKeyFrame2D();

				for( Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D::const_iterator
					it = includes_frame2d.begin(),
					it_end = includes_frame2d.end();
				it != it_end;
				++it )
				{
					const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & meta_frame2d = *it;

					size_t count = 1;

					mt::vec2f anchorPoint2d;
					mt::vec2f position2d;
					mt::vec2f scale2d;

					float angle;

					float volume = 1.f;

					if( frameLast == true )
					{
						angle = -frame.rotation.x;

						anchorPoint2d = frame.anchorPoint.to_vec2f();
						position2d = frame.position.to_vec2f();
						scale2d = frame.scale.to_vec2f();
						volume = frame.volume;
					}
					else
					{
						frameLast = true;
					}					

					meta_frame2d.get_AnchorPoint( anchorPoint2d );
					meta_frame2d.get_Position( position2d );
					meta_frame2d.get_Scale( scale2d );
					meta_frame2d.get_Rotation( angle );
					meta_frame2d.get_Opacity( frame.opacity );
					meta_frame2d.get_Count( count );
					meta_frame2d.get_Volume( volume );

					frame.anchorPoint = mt::vec3f(anchorPoint2d, 0.f);
					frame.position = mt::vec3f(position2d, 0.f);
					frame.scale = mt::vec3f(scale2d, 1.f);

					frame.volume = volume;

					frame.rotation.x = -angle;
					frame.rotation.y = 0.f;
					frame.rotation.z = 0.f;

					for( size_t i = 0; i != count; ++i )
					{
						_movieFramePack.addLayerFrame( layerIndex, frame );
					}
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

			size_t count = 0;
			meta_frames3d.get_Count( count );

			bool immutable = false;
			meta_frames3d.get_Immutable( immutable );

			_movieFramePack.initializeLayer( layerIndex, count, immutable );

			const Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::TVectorMeta_KeyFrame3D & includes_frame3d = meta_frames3d.get_IncludesKeyFrame3D();

			for( Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::TVectorMeta_KeyFrame3D::const_iterator
				it = includes_frame3d.begin(),
				it_end = includes_frame3d.end();
			it != it_end;
			++it )
			{
				const Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D & meta_frame3d = *it;

				size_t count = 1;
				
				mt::vec3f anchorPoint;
				mt::vec3f rotation;
				mt::vec3f position;
				mt::vec3f scale;

				MovieFrameSource frame;

				float volume = 1.f;

				if( _movieFramePack.getLayerFrameLast( layerIndex, frame ) == true )
				{
					anchorPoint = frame.anchorPoint;
					position = frame.position;
					rotation = frame.rotation;
					scale = frame.scale;
					volume = frame.volume;
				}

				meta_frame3d.get_AnchorPoint( frame.anchorPoint );
				meta_frame3d.get_Position( frame.position );
				meta_frame3d.get_Rotation( frame.rotation );
				meta_frame3d.get_Scale( frame.scale );                
				meta_frame3d.get_Opacity( frame.opacity );
				meta_frame3d.get_Count( count );

				//if( meta_frame3d.has_Mask() == true )
				//{
				//    Polygon polygon;
				//    meta_frame3d.get_Mask( polygon );

				//    boost::geometry::correct( polygon );

				//    size_t index = pack->addPolygon( polygon );

				//    frame.mask = index;
				//}
				//else
				//{
				//    frame.mask = mask;
				//}

				frame.volume = 1.f;
				meta_frame3d.get_Volume( frame.volume );

				if( _movieFramePack.isLayerImmutable( layerIndex ) == false )
				{
					for( size_t i = 0; i != count; ++i )
					{
						_movieFramePack.addLayerFrame( layerIndex, frame );
					}
				}
				else
				{
					_movieFramePack.setLayerImmutableFrame( layerIndex, frame );
				}
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieKeyConverterXMLToAEK::writeFramePak_( MovieFramePack & _movieFramePack )
	{
		TBlobject binary_aek;

		ArchiveWrite aw(binary_aek);

		const TVectorMovieFrameLayer & layers_framepack = _movieFramePack.getLayers();
				
		size_t layers_framepack_size = layers_framepack.size();
		aw << layers_framepack_size;

		for( TVectorMovieFrameLayer::const_iterator
			it = layers_framepack.begin(),
			it_end = layers_framepack.end();
		it != it_end;
		++it )
		{
			const MovieLayerFrame & frame = (*it);
			
			aw << frame.immutable;

			if( frame.immutable == true )
			{
				aw << frame.source;
			}
			else
			{
				const TVectorMovieFrameSource & frames = frame.frames;

				size_t frames_size = frames.size();
				aw << frames_size;

				if( frames_size > 0 )
				{
					aw.writePODs( &frames[0], frames_size );
				}
			}
		}

		const TVectorMovieLayerTimeRemap & layers_timeremap = _movieFramePack.getTimeremap();

		size_t layers_timeremap_size = layers_timeremap.size();
		aw << layers_timeremap_size;

		for( TVectorMovieLayerTimeRemap::const_iterator
			it = layers_timeremap.begin(),
			it_end = layers_timeremap.end();
		it != it_end;
		++it )
		{
			const MovieLayerTimeRemap & timeremap = *it;

			const Floats & times = timeremap.times;

			size_t times_size = times.size();
			aw << times_size;

			if( times_size > 0 )
			{
				aw.writePODs( &times[0], times_size );
			}
		}

		size_t binary_aek_size = binary_aek.size();
		
		size_t compress_bound = ARCHIVE_SERVICE(m_serviceProvider)
			->compressBound( binary_aek_size );

		TBlobject compress;
		compress.resize( compress_bound );

		size_t compress_size = 0;
		if( ARCHIVE_SERVICE(m_serviceProvider)
			->compress( &compress[0], compress_bound, compress_size, &binary_aek[0], binary_aek_size ) == false )
		{
			return false;
		}
		
		OutputStreamInterfacePtr output_stream = FILE_SERVICE(m_serviceProvider)
			->openOutputFile( m_options.pakName, m_options.outputFileName );

		if( output_stream == nullptr )
		{
			return false;
		}

		size_t binary_aek_header = 0xAEAEBABE;
		output_stream->write( &binary_aek_header, sizeof(binary_aek_header) );

		output_stream->write( &binary_aek_size, sizeof(binary_aek_size) );
		output_stream->write( &compress_size, sizeof(compress_size) );

		output_stream->write( &compress[0], compress_size );

		return true;
	}
}