#   include "MovieKeyConverterXMLToAEK.h"

#	include "Interface/LoaderInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/MovieKeyFrameInterface.h"

#	include "Metacode/Metacode.h"

#	include "Archive/ArchiveWrite.hpp"

#	include "Core/Stream.h"
#	include "Core/MemoryAllocator.h"

#	include "Metacode/Metacode.h"

#	include "Logger/Logger.h"

#	include "Math/quat.h"

#	include "poly2tri/poly2tri.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<MovieFrameSource> TVectorMovieFrameSource;
	//////////////////////////////////////////////////////////////////////////
	struct ConverterMovieLayerFrame
	{
		TVectorMovieFrameSource frames;
		uint32_t count;

		MovieFrameSource source;
		uint8_t immutable;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<ConverterMovieLayerFrame> TVectorConverterMovieFrameLayer;
	//////////////////////////////////////////////////////////////////////////
	MovieKeyConverterXMLToAEK::MovieKeyConverterXMLToAEK()
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

		m_archivator = ARCHIVE_SERVICE( m_serviceProvider )
			->getArchivator( STRINGIZE_STRING_LOCAL( m_serviceProvider, "lz4" ) );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool MovieKeyConverterXMLToAEK::convert()
	{
		Blobject buffer;
		if( this->loadFramePak_( buffer ) == false )
		{
			return false;
		}

		if( this->writeFramePak_( buffer ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieKeyConverterXMLToAEK::validateVersion( const InputStreamInterfacePtr & _stream ) const
	{
		MovieFramePackInterfacePtr framePack = DATA_SERVICE( m_serviceProvider )
			->dataflowT<MovieFramePackInterfacePtr>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "aekMovie" ), _stream );

		if( framePack == nullptr )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieKeyConverterXMLToAEK::loadFramePak_( Blobject & _buffer )
	{
		bool exist = false;

		Metacode::Meta_KeyFramesPack keyFramesPack;

		PathString binPath;

		binPath += m_options.inputFileName;
		binPath.replace_last( "bin" );

		FilePath path_bin = Helper::stringizeString( m_serviceProvider, binPath );

		if( LOADER_SERVICE( m_serviceProvider )
			->load( m_options.pakName, path_bin, &keyFramesPack, exist ) == false )
		{
			if( exist == false )
			{
				LOGGER_ERROR( m_serviceProvider )("MovieKeyConverter::convert: KeyFramesFile '%s' not found"
					, m_options.inputFileName.c_str()
					);
			}
			else
			{
				LOGGER_ERROR( m_serviceProvider )("MovieKeyConverter::convert: KeyFramesFile invalid parse '%s' "
					, m_options.inputFileName.c_str()
					);
			}

			return false;
		}



		ArchiveWrite aw( _buffer );

		uint32_t true_metacode_version = Metacode::get_metacode_version();

		aw << true_metacode_version;

		uint32_t keyframes_version = keyFramesPack.get_Version();

		aw << keyframes_version;

		uint32_t maxIndex = keyFramesPack.get_MaxIndex();

		aw << maxIndex;

		TVectorConverterMovieFrameLayer frameLayers;
		frameLayers.resize( maxIndex );

		for( TVectorConverterMovieFrameLayer::iterator
			it = frameLayers.begin(),
			it_end = frameLayers.end();
		it != it_end;
		++it )
		{
			ConverterMovieLayerFrame & layer = *it;

			layer.count = 0;
			layer.immutable = 0;
		}

		const Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames2D & includes_frames2d = keyFramesPack.get_IncludesKeyFrames2D();

		for( Metacode::Meta_KeyFramesPack::TVectorMeta_KeyFrames2D::const_iterator
			it = includes_frames2d.begin(),
			it_end = includes_frames2d.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D & meta_frames2d = *it;

			uint32_t layerIndex = meta_frames2d.get_LayerIndex();

			ConverterMovieLayerFrame & frameLayer = frameLayers[layerIndex - 1];

			bool immutable = false;
			meta_frames2d.get_Immutable( immutable );

			frameLayer.immutable = immutable ? 1 : 0;

			if( frameLayer.immutable == 1 )
			{
				const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D & includes_frame2d = meta_frames2d.get_IncludesKeyFrame2D();

				for( Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D::const_iterator
					it_frame = includes_frame2d.begin(),
					it_frame_end = includes_frame2d.end();
				it_frame != it_frame_end;
				++it_frame )
				{
					const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & meta_frame2d = *it_frame;

					mt::vec2f anchorPoint2d( 0.f, 0.f );
					mt::vec2f position2d( 0.f, 0.f );
					mt::vec2f scale2d( 0.f, 0.f );
					float angle = 0.f;
					float opacity = 1.f;

					uint32_t count = 1;

					float volume = 1.f;

					meta_frame2d.get_AnchorPoint( anchorPoint2d );
					meta_frame2d.get_Position( position2d );
					meta_frame2d.get_Scale( scale2d );
					meta_frame2d.get_Rotation( angle );
					meta_frame2d.get_Opacity( opacity );
					meta_frame2d.get_Count( count );
					meta_frame2d.get_Volume( volume );

					MovieFrameSource frame;
					frame.anchorPoint = mt::vec3f( anchorPoint2d, 0.f );
					frame.position = mt::vec3f( position2d, 0.f );
					frame.scale = mt::vec3f( scale2d, 1.f );
					frame.opacity = opacity;
					frame.volume = volume;

					frame.rotation.x = -angle;
					frame.rotation.y = 0.f;
					frame.rotation.z = 0.f;

					frameLayer.source = frame;
					frameLayer.count = count;
				}
			}
			else
			{
				uint32_t count = 0;
				meta_frames2d.get_Count( count );

				frameLayer.count = count;

				MovieFrameSource frame;
				frame.anchorPoint = mt::vec3f( 0.f, 0.f, 0.f );
				frame.position = mt::vec3f( 0.f, 0.f, 0.f );
				frame.rotation = mt::vec3f( 0.f, 0.f, 0.f );
				frame.scale = mt::vec3f( 1.f, 1.f, 1.f );
				frame.opacity = 0.f;
				frame.volume = 0.f;

				bool frameLast = false;

				const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D & includes_frame2d = meta_frames2d.get_IncludesKeyFrame2D();

				for( Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::TVectorMeta_KeyFrame2D::const_iterator
					it_frame = includes_frame2d.begin(),
					it_frame_end = includes_frame2d.end();
				it_frame != it_frame_end;
				++it_frame )
				{
					const Metacode::Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & meta_frame2d = *it_frame;

					uint32_t count_frame = 1;

					mt::vec2f anchorPoint2d( 0.f, 0.f );
					mt::vec2f position2d( 0.f, 0.f );
					mt::vec2f scale2d( 1.f, 1.f );

					float angle = 0.f;

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
					meta_frame2d.get_Count( count_frame );
					meta_frame2d.get_Volume( volume );

					frame.anchorPoint = mt::vec3f( anchorPoint2d, 0.f );
					frame.position = mt::vec3f( position2d, 0.f );
					frame.scale = mt::vec3f( scale2d, 1.f );

					frame.volume = volume;

					frame.rotation.x = -angle;
					frame.rotation.y = 0.f;
					frame.rotation.z = 0.f;

					for( uint32_t i = 0; i != count_frame; ++i )
					{
						frameLayer.frames.push_back( frame );
					}
				}

				if( frameLayer.frames.size() != frameLayer.count )
				{
					return false;
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

			uint32_t layerIndex = meta_frames3d.get_LayerIndex();

			ConverterMovieLayerFrame & frameLayer = frameLayers[layerIndex - 1];

			uint32_t count = 0;
			meta_frames3d.get_Count( count );

			frameLayer.count = count;

			bool immutable = false;
			meta_frames3d.get_Immutable( immutable );

			frameLayer.immutable = immutable ? 1 : 0;

			MovieFrameSource frame;
			frame.anchorPoint = mt::vec3f( 0.f, 0.f, 0.f );
			frame.position = mt::vec3f( 0.f, 0.f, 0.f );
			frame.rotation = mt::vec3f( 0.f, 0.f, 0.f );
			frame.scale = mt::vec3f( 1.f, 1.f, 1.f );
			frame.opacity = 0.f;
			frame.volume = 0.f;

			mt::vec3f rotation( 0.f, 0.f, 0.f );
			mt::vec3f orientation( 0.f, 0.f, 0.f );

			const Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::TVectorMeta_KeyFrame3D & includes_frame3d = meta_frames3d.get_IncludesKeyFrame3D();

			for( Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::TVectorMeta_KeyFrame3D::const_iterator
				it_frame = includes_frame3d.begin(),
				it_frame_end = includes_frame3d.end();
			it_frame != it_frame_end;
			++it_frame )
			{
				const Metacode::Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D & meta_frame3d = *it_frame;

				uint32_t count_frame = 1;

				meta_frame3d.get_AnchorPoint( frame.anchorPoint );
				meta_frame3d.get_Position( frame.position );
				meta_frame3d.get_Rotation( rotation );
				meta_frame3d.get_Orientation( orientation );
				meta_frame3d.get_Scale( frame.scale );
				meta_frame3d.get_Opacity( frame.opacity );
				meta_frame3d.get_Count( count_frame );

				frame.volume = 1.f;
				meta_frame3d.get_Volume( frame.volume );

				mt::quatf qo;
				mt::make_quat_from_euler( qo, orientation );
				
				mt::quatf qr;
				mt::make_quat_from_euler( qr, rotation );

				mt::quatf qor;
				mt::mul_q_q( qor, qo, qr );

				mt::quat_to_euler( qor, frame.rotation );

				if( frameLayer.immutable == 0 )
				{
					for( uint32_t i = 0; i != count_frame; ++i )
					{
						frameLayer.frames.push_back( frame );
					}
				}
				else
				{
					frameLayer.source = frame;
					frameLayer.count = count_frame;
				}
			}

			if( frameLayer.immutable == 0 )
			{
				if( frameLayer.frames.size() != frameLayer.count )
				{
					return false;
				}
			}
		}

		for( TVectorConverterMovieFrameLayer::const_iterator
			it = frameLayers.begin(),
			it_end = frameLayers.end();
		it != it_end;
		++it )
		{
			const ConverterMovieLayerFrame & frame = (*it);

			aw << frame.count;
			aw << frame.immutable;

			if( frame.count == 0 )
			{
				continue;
			}

			if( frame.immutable == 1 )
			{
				aw << frame.source.anchorPoint;
				aw << frame.source.position;
				aw << frame.source.rotation;
				aw << frame.source.scale;
				aw << frame.source.opacity;
				aw << frame.source.volume;
			}
			else
			{
				const TVectorMovieFrameSource & frames = frame.frames;

#	define WRITE_FRAME_SOURCE( Type, Member )\
												{ \
					Type value = frames[0].Member; \
					\
					uint8_t value_immutable = 1; \
					for( uint32_t i = 1; i != frame.count; ++i ) \
															{ \
						const MovieFrameSource & source = frames[i]; \
						\
						if( source.Member == value ) \
																		{ \
							continue; \
																		} \
						\
						value_immutable = 0; \
						break; \
															} \
					\
					aw << value_immutable; \
					\
					if( value_immutable == 1 ) \
															{ \
						aw << value; \
															} \
											else \
													{ \
						for( uint32_t i = 0; i != frame.count; ++i ) \
																		{ \
							const MovieFrameSource & source = frames[i]; \
							\
							aw << source.Member; \
																		} \
													} \
												}

				WRITE_FRAME_SOURCE( mt::vec3f, anchorPoint );
				WRITE_FRAME_SOURCE( mt::vec3f, position );
				WRITE_FRAME_SOURCE( float, rotation.x );
				WRITE_FRAME_SOURCE( float, rotation.y );
				WRITE_FRAME_SOURCE( float, rotation.z );
				WRITE_FRAME_SOURCE( mt::vec3f, scale );
				WRITE_FRAME_SOURCE( float, opacity );
				WRITE_FRAME_SOURCE( float, volume );

#	undef WRITE_FRAME_SOURCE
			}
		}

		const Metacode::Meta_KeyFramesPack::TVectorMeta_TimeRemap & includes_timeremaps = keyFramesPack.get_IncludesTimeRemap();

		uint32_t remapsSize = (uint32_t)includes_timeremaps.size();
		aw << remapsSize;

		for( Metacode::Meta_KeyFramesPack::TVectorMeta_TimeRemap::const_iterator
			it = includes_timeremaps.begin(),
			it_end = includes_timeremaps.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_KeyFramesPack::Meta_TimeRemap & meta_timeremap = *it;

			uint32_t layerIndex = meta_timeremap.get_LayerIndex();

			aw << layerIndex;

			const Floats & floats = meta_timeremap.get_Time();

			uint32_t floats_size = (uint32_t)floats.size();
			aw << floats_size;

			if( floats_size > 0 )
			{
				aw.writePODs( &floats[0], floats_size );
			}
		}

		const Metacode::Meta_KeyFramesPack::TVectorMeta_ImageShape & includes_imageshapes = keyFramesPack.get_IncludesImageShape();

		uint32_t shapesSize = (uint32_t)includes_imageshapes.size();
		aw << shapesSize;

		for( Metacode::Meta_KeyFramesPack::TVectorMeta_ImageShape::const_iterator
			it = includes_imageshapes.begin(),
			it_end = includes_imageshapes.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_KeyFramesPack::Meta_ImageShape & meta_imageshape = *it;

			uint32_t layerIndex = meta_imageshape.get_LayerIndex();

			aw << layerIndex;

			const mt::vec2f & imageMaxSize = meta_imageshape.get_ImageMaxSize();

			mt::vec2f imageSize;
			if( meta_imageshape.get_ImageSize( imageSize ) == false )
			{
				imageSize = imageMaxSize;
			}

			mt::vec2f imageOffset( 0.f, 0.f );
			meta_imageshape.get_ImageOffset( imageOffset );

			bool subtract = false;
			meta_imageshape.get_Subtract( subtract );

			const Metacode::Meta_KeyFramesPack::Meta_ImageShape::TVectorMeta_Shape & includes_shapes = meta_imageshape.get_IncludesShape();

			uint32_t includes_shapes_size = (uint32_t)includes_shapes.size();

			aw << includes_shapes_size;

			for( Metacode::Meta_KeyFramesPack::Meta_ImageShape::TVectorMeta_Shape::const_iterator
				it_shape = includes_shapes.begin(),
				it_shape_end = includes_shapes.end();
			it_shape != it_shape_end;
			++it_shape )
			{
				const Metacode::Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape & meta_shape = *it_shape;

				Menge::Polygon polygon = meta_shape.get_Polygon();
				polygon.correct();

				mt::vec2f v0( 0.f, 0.f );
				mt::vec2f v1( imageMaxSize.x, 0.f );
				mt::vec2f v2( imageMaxSize.x, imageMaxSize.y );
				mt::vec2f v3( 0.f, imageMaxSize.y );

				Polygon imagePolygon;
				imagePolygon.append( v0 );
				imagePolygon.append( v1 );
				imagePolygon.append( v2 );
				imagePolygon.append( v3 );
				imagePolygon.correct();

				TVectorPolygon output;
				if( subtract == false )
				{
					if( polygon.intersection( imagePolygon, output ) == false )
					{
						LOGGER_ERROR( m_serviceProvider )("MovieKeyConverterXMLToAEK::loadFramePak_ layer %d shapes invalid"
							, layerIndex
							);

						return false;
					}
				}
				else
				{
					if( imagePolygon.difference( polygon, output ) == false )
					{
						LOGGER_ERROR( m_serviceProvider )("MovieKeyConverterXMLToAEK::loadFramePak_ layer %d shapes invalid"
							, layerIndex
							);

						return false;
					}
				}

				MovieFrameShape shape;
				if( output.empty() == true )
				{
					shape.pos = nullptr;
					shape.uv = nullptr;
					shape.indices = nullptr;

					shape.vertexCount = 0;
					shape.indexCount = 0;

					aw << shape.vertexCount;
				}
				else
				{
					std::vector<p2t::Point> p2t_points;

					size_t max_points = 0;

					for( TVectorPolygon::const_iterator
						it_polygon = output.begin(),
						it_polygon_end = output.end();
					it_polygon != it_polygon_end;
					++it_polygon)
					{
						const Menge::Polygon & shape_vertex = *it_polygon;

						size_t outer_count = shape_vertex.outer_count();

						max_points += outer_count - 1;

						size_t inners_count = shape_vertex.inners_count();

						for( size_t index = 0; index != inners_count; ++index )
						{
							size_t inner_count = shape_vertex.inner_count( index );

							max_points += inner_count - 1;
						}
					}

					if( max_points >= MENGINE_MOVIE_SHAPE_MAX_VERTEX )
					{
						LOGGER_ERROR( m_serviceProvider )("MovieKeyConverterXMLToAEK::loadFramePak_ layer %d vertex overflow %d (max %d)"
							, layerIndex
							, max_points
							, MENGINE_MOVIE_SHAPE_MAX_VERTEX
							);

						return false;
					}

					p2t_points.reserve( max_points );

					Menge::TVectorIndices shape_indices;

					for( TVectorPolygon::const_iterator
						it_polygon = output.begin(),
						it_polygon_end = output.end();
					it_polygon != it_polygon_end;
					++it_polygon)
					{
						const Menge::Polygon & shape_vertex = *it_polygon;

						std::vector<p2t::Point*> p2t_polygon;

						size_t outer_count = shape_vertex.outer_count();

						for( size_t index = 0; index != outer_count - 1; ++index )
						{
							const mt::vec2f & v = shape_vertex.outer_point( index );

							p2t_points.push_back( p2t::Point( v.x, v.y ) );
							p2t::Point * p = &p2t_points.back();

							p2t_polygon.push_back( p );
						}

						p2t::CDT * cdt = new p2t::CDT( p2t_polygon );

						size_t inners_count = shape_vertex.inners_count();

						for( size_t index_inners = 0; index_inners != inners_count; ++index_inners )
						{
							std::vector<p2t::Point*> p2t_hole;

							size_t inner_count = shape_vertex.inner_count( index_inners );

							for( size_t index_inner = 0; index_inner != inner_count - 1; ++index_inner )
							{
								const mt::vec2f & v = shape_vertex.inner_point( index_inners, index_inner );

								p2t_points.push_back( p2t::Point( v.x, v.y ) );
								p2t::Point * p = &p2t_points.back();

								p2t_hole.push_back( p );
							}

							cdt->AddHole( p2t_hole );
						}

						cdt->Triangulate();

						std::vector<p2t::Triangle*> triangles = cdt->GetTriangles();

						for( std::vector<p2t::Triangle*>::iterator
							it_triangle = triangles.begin(),
							it_triangle_end = triangles.end();
						it_triangle != it_triangle_end;
						++it_triangle)
						{
							p2t::Triangle* tr = *it_triangle;

							p2t::Point * p0 = tr->GetPoint( 0 );
							p2t::Point * p1 = tr->GetPoint( 1 );
							p2t::Point * p2 = tr->GetPoint( 2 );

							p2t::Point * pb = &p2t_points[0];

							uint32_t i0 = (uint32_t)std::distance( pb, p0 );
							uint32_t i1 = (uint32_t)std::distance( pb, p1 );
							uint32_t i2 = (uint32_t)std::distance( pb, p2 );

							shape_indices.push_back( i0 );
							shape_indices.push_back( i1 );
							shape_indices.push_back( i2 );
						}

						delete cdt;
					}

					size_t shapeVertexCount = p2t_points.size();
					size_t shapeIndicesCount = shape_indices.size();

					if( shapeIndicesCount >= MENGINE_MOVIE_SHAPE_MAX_INDICES )
					{
						LOGGER_ERROR( m_serviceProvider )("MovieKeyConverterXMLToAEK::loadFramePak_ layer %d index overflow %d (max $d)"
							, layerIndex
							, shapeIndicesCount
							, MENGINE_MOVIE_SHAPE_MAX_INDICES
							);

						return false;
					}

					shape.vertexCount = (uint16_t)shapeVertexCount;
					shape.indexCount = (uint16_t)shapeIndicesCount;

					shape.pos = Helper::allocateMemory<mt::vec2f>( shapeVertexCount );
					shape.uv = Helper::allocateMemory<mt::vec2f>( shapeVertexCount );

					shape.indices = Helper::allocateMemory<RenderIndices>( shapeIndicesCount );

					for( size_t i = 0; i != shapeVertexCount; ++i )
					{
						const p2t::Point & shape_pos = p2t_points[i];

						shape.pos[i].x = (float)shape_pos.x;
						shape.pos[i].y = (float)shape_pos.y;

						shape.uv[i].x = ((float)shape_pos.x - imageOffset.x) / imageSize.x;
						shape.uv[i].y = ((float)shape_pos.y - imageOffset.y) / imageSize.y;
					}

					for( size_t i = 0; i != shapeIndicesCount; ++i )
					{
						shape.indices[i] = (RenderIndices)shape_indices[i];
					}

					aw << shape.vertexCount;

					if( shape.vertexCount > 0 )
					{
						aw.writePODs( shape.pos, shape.vertexCount );
						aw.writePODs( shape.uv, shape.vertexCount );

						aw << shape.indexCount;

						aw.writePODs( shape.indices, shape.indexCount );
					}

					Helper::freeMemory( shape.pos );
					Helper::freeMemory( shape.uv );
					Helper::freeMemory( shape.indices );
				}
			}
		}

		const Metacode::Meta_KeyFramesPack::TVectorMeta_Polygon & includes_polygons = keyFramesPack.get_IncludesPolygon();

		uint32_t polygonsSize = (uint32_t)includes_polygons.size();
		aw << polygonsSize;

		for( Metacode::Meta_KeyFramesPack::TVectorMeta_Polygon::const_iterator
			it = includes_polygons.begin(),
			it_end = includes_polygons.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_KeyFramesPack::Meta_Polygon & meta_polygon = *it;

			uint32_t layerIndex = meta_polygon.get_LayerIndex();

			aw << layerIndex;

			const Menge::Polygon & polygon = meta_polygon.get_Value();

			size_t polygon_size = polygon.num_points();

			if( polygon_size >= MENGINE_MOVIE_POLYGON_MAX_VERTEX )
			{
				LOGGER_ERROR( m_serviceProvider )("MovieKeyConverterXMLToAEK::loadFramePak_ layer %d polygon vertex overflow %d (max $d)"
					, layerIndex
					, polygon_size
					, MENGINE_MOVIE_POLYGON_MAX_VERTEX
					);

				return false;
			}

			aw << (uint8_t)polygon_size;

			if( polygon_size > 0 )
			{
				const mt::vec2f * countour = polygon.outer_points();

				for( uint32_t i = 0; i != polygon_size; ++i )
				{
					const mt::vec2f & p = countour[i];
					aw << p.x;
					aw << p.y;
				}
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieKeyConverterXMLToAEK::writeFramePak_( const Blobject & _buffer )
	{
		OutputStreamInterfacePtr output_stream = FILE_SERVICE( m_serviceProvider )
			->openOutputFile( m_options.pakName, m_options.outputFileName );

		if( output_stream == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("MovieKeyConverterXMLToAEK::writeFramePak_ invalid open file %s:%s"
				, m_options.pakName.c_str()
				, m_options.outputFileName.c_str()
				);

			return false;
		}

		const void * buffer_memory = &_buffer[0];
		size_t buffer_size = _buffer.size();

		if( Helper::writeStreamArchiveData( m_serviceProvider, output_stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_AEK ), GET_MAGIC_VERSION( MAGIC_AEK ), false, buffer_memory, buffer_size, EAC_BEST ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MovieKeyConverterXMLToAEK::writeFramePak_ invalid write stream %s:%s"
				, m_options.pakName.c_str()
				, m_options.outputFileName.c_str()
				);

			return false;
		}

		return true;
	}
}