#	include "Config/Typedef.h"
#   include "Config/Stringstream.h"

#	include "ScriptWrapper.h"

#	include "Kernel/Node.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/rand.h"

#	include "Core/Polygon.h"
#	include "Core/ColourValue.h"
#	include "Core/MemoryHelper.h"

#	include "Account.h"
#	include "Game.h"
#	include "Application.h"

#	include "Movie.h"
#	include "HotSpotShape.h"

#	include "Logger/Logger.h"

#	include "Interface/ThreadSystemInterface.h"
#   include "Interface/UnicodeInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/TextInterface.h"
#	include "Interface/TimerInterface.h"
#	include "Interface/UserdataInterface.h"

#	include "ResourceAnimation.h"
//#	include "ResourceSequence.h"

#	include "Player.h"

#	include "Watchdog.h"

#	include <ctime>
#	include <sstream>
#	include <iomanip>

#	include "Kernel/ResourceImage.h"
#	include "ResourceCursorICO.h"
#	include "AccountManager.h"

#	include "Kernel/Affector.h"

#	include "Core/String.h"
#   include "Core/CRC32.h"

#	include "Math/angle.h"

#	include "Interface/ScriptSystemInterface.h"

#   include "pybind/pybind.hpp"
#   include "pybind/pickle.hpp"

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class HelperScriptMethod
	{
    public:
        HelperScriptMethod( ServiceProviderInterface * _serviceProvider )
            : m_serviceProvider(_serviceProvider)
        {
        }

    public:
        bool s_is_class( PyObject * _obj )
        {
			return pybind::detail::is_class( _obj );
        }
		
        const ConstString & s_getLanguagePack()
        {
            const ConstString & locale = APPLICATION_SERVICE(m_serviceProvider)
				->getLocale();

			return locale;
        }

        WString s_utf8ToUnicode( const String & _utf8 )
        {
            WString unicode;
            Helper::utf8ToUnicode( m_serviceProvider, _utf8, unicode );

            return unicode;
        }

        String s_unicodeToUtf8( const WString & _unicode )
        {
            String utf8;
            Helper::unicodeToUtf8( m_serviceProvider, _unicode, utf8 );

            return utf8;
        }

		void s_debug()
		{
			printf("debug!\n");

#	if defined(_MSC_VER) && !defined(I3D_ARCH_ARM)
			_asm int 3;
#	endif			
		}

		size_t s_globalmemoryuse()
		{
			size_t mu = stdex_allocator_globalmemoryuse();

			return mu;
		}

		size_t s_memoryuse()
		{
			size_t gmu = stdex_allocator_memoryuse();

			return gmu;
		}

		bool s_hasGameData( const ConstString & _name )
		{
			bool result = USERDATA_SERVICE(m_serviceProvider)
				->hasUserdata( _name );

			return result;
		}

		bool s_writeGameData( const ConstString & _name, PyObject * _data, PyObject * _pickleTypes )
		{
			size_t size;
			pybind::pickle( _data, _pickleTypes, nullptr, 0, size );

			MemoryCacheBufferInterfacePtr buffer = Helper::createMemoryCacheBuffer( m_serviceProvider, size, "s_writeAccountBinaryFile" );

			if( buffer == nullptr )
			{
				return false;
			}

			void * memory_buffer = buffer->getMemory();
			size_t memory_size = buffer->getSize();

			if( pybind::pickle( _data, _pickleTypes, memory_buffer, memory_size, size ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("s_writeGameData: data %s invalid pickle"
					, _name.c_str()
					);

				return false;
			}

			if( USERDATA_SERVICE( m_serviceProvider )->writeUserdata( _name, memory_buffer, memory_size ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("s_writeGameData: data %s invalid write"
					, _name.c_str()
					);

				return false;
			}

			return true;
		}
		
		PyObject * s_loadGameData( const ConstString & _name, PyObject * _pickleTypes )
		{
			MemoryCacheBufferInterfacePtr binaryBuffer = USERDATA_SERVICE( m_serviceProvider )
				->loadUserdata( _name );

			if( binaryBuffer == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_readGameData: data %ls invalid load"
					, _name.c_str()
					);

				return pybind::ret_none();
			}

			void * binaryBuffer_memory = binaryBuffer->getMemory();
			size_t binaryBuffer_size = binaryBuffer->getSize();

			PyObject * py_data = pybind::unpickle( binaryBuffer_memory, binaryBuffer_size, _pickleTypes );

			if( py_data == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_readGameData: data %s invalid unpickle"
					, _name.c_str()
					);

				return pybind::ret_none();
			}

			return py_data;
		}

		bool s_isAltDown() const
		{
			return INPUT_SERVICE(m_serviceProvider)
				->isAltDown();
		}

		bool s_isShiftDown() const
		{
			return INPUT_SERVICE(m_serviceProvider)
				->isShiftDown();
		}

		bool s_isCtrlDown() const
		{
			return INPUT_SERVICE(m_serviceProvider)
				->isCtrlDown();
		}

		bool s_isKeyDown( uint32_t _keyCode ) const
		{
			return INPUT_SERVICE(m_serviceProvider)
				->isKeyDown( _keyCode );
		}

		bool s_isExclusiveKeyDown( uint32_t _keyCode ) const
		{
			return INPUT_SERVICE(m_serviceProvider)
				->isExclusiveKeyDown( _keyCode );
		}

		bool s_isAnyKeyDown() const
		{
			return INPUT_SERVICE(m_serviceProvider)
				->isAnyKeyDown();
		}

		bool s_isAnyMouseButtonDown() const
		{
			return INPUT_SERVICE(m_serviceProvider)
				->isAnyMouseButtonDown();
		}

		bool s_isMouseButtonDown( uint32_t _button ) const
		{
			return INPUT_SERVICE(m_serviceProvider)
				->isMouseButtonDown( _button );
		}

		void s_printChildren2( Node * _node, uint32_t _tab )
		{
			TListNodeChild & children = _node->getChildren();

			for( TSlugChild it(children); it.eof() == false; )
			{
				Node * child = *it;

				it.next_shuffle();

				ColourValue color;
				child->calcTotalColor( color );

				printf( "%.*s%s%s [%s] (%.0f, %.0f) %.2f\n"
					, _tab
					, "                                         "
					, child->isActivate() ? child->isEnable() ? "+" : "-" : "#"
					, child->getName().c_str()
					, child->getType().c_str()
					, child->getWorldPosition().x
					, child->getWorldPosition().y
					, color.getA()
					);

				s_printChildren2( child, _tab + 1 );
			}
		}

		void s_printChildren( Node * _node )
		{
			s_printChildren2( _node, 0 );
		}

		size_t s_getGroupResourcesMemoryUse( const ConstString & _category, const ConstString & _group )
		{
			size_t memoryUse = RESOURCE_SERVICE( m_serviceProvider )
				->getGroupResourcesMemoryUse( _category, _group );

			return memoryUse;
		}

        void s_setCursorPosition( const mt::vec2f & _pos )
        {
            const Resolution & contentResolution = APPLICATION_SERVICE(m_serviceProvider)
                ->getContentResolution();

            const Resolution & currentResolution = APPLICATION_SERVICE(m_serviceProvider)
                ->getCurrentResolution();

            mt::vec2f adapt_pos;
            adapt_pos.x = _pos.x / contentResolution.getWidth() * currentResolution.getWidth();
            adapt_pos.y = _pos.y / contentResolution.getHeight() * currentResolution.getHeight();

            PLATFORM_SERVICE(m_serviceProvider)
                ->setCursorPosition( adapt_pos );
        }

        bool s_isValidWindowMode()
        {
            //bool check = APPLICATION_SERVICE(m_serviceProvider)
                //->isValidWindowMode();

            //return check;

            return true;
        }

        void s_addGlobalModule( const String & _name, PyObject * _module )
        {
            SCRIPT_SERVICE(m_serviceProvider)
                ->addGlobalModule( _name, _module );
        }

	public:
		int mt_rand( int a )
		{	
			return mt::rand( a );
		}

		float mt_randf( float a )
		{	
			return mt::randf( a );
		}

		int mt_range_rand( int a, int b )
		{	
			return mt::range_rand( a, b );
		}
		
		float mt_range_randf( float a, float b )
		{	
			return mt::range_randf( a, b );
		}

		float mt_sqrtf( float a )
		{
			return sqrtf(a);
		}

		float mt_absf( float a )
		{
			return fabsf(a);
		}

		float mt_cosf( float a )
		{
			return mt::cosf_fast(a);
		}

		float mt_sinf( float a )
		{
			return mt::sinf_fast(a);
		}

		float mt_tanf( float a )
		{
			return tanf( a );
		}

		float mt_acosf( float a )
		{
			return acosf( a );
		}

		float mt_atanf( float _x )
		{
			return ::atanf( _x );
		}

		mt::vec2f mt_direction( const mt::vec2f & _from, const mt::vec2f & _to )
		{
			mt::vec2f direction;
			sub_v2_v2(direction, _to, _from);

			mt::vec2f direction_n;
			norm_v2_v2(direction_n, direction);

			return direction_n;
		}

		float my_angle_from_v3_v3( const mt::vec2f & _from, const mt::vec2f & _to )
		{
			mt::vec2f direction;
			sub_v2_v2(direction, _to, _from);

			mt::vec2f direction_n;
			norm_v2_v2(direction_n, direction);

			float angle = mt::signed_angle( direction_n );

			return angle;
		}

		mt::vec2f mt_norm_v2( const mt::vec2f & _vec )
		{
			mt::vec2f v_n;
			mt::norm_v2_v2( v_n, _vec );

			return v_n;
		}

		bool s_intersectsEllipseVsPoint( const mt::vec2f & _pos, float _radius, float _ellipse, const mt::vec2f & _point )
		{
			mt::vec2f v = _point - _pos;
			
			v.y /= _ellipse;

			float v_sqrdist = v.sqrlength();

			if( v_sqrdist > _radius * _radius )
			{
				return false;
			}

			return true;
		}

		bool s_intersectsBoxes( const mt::vec2f & _minFirst, const mt::vec2f & _maxFirst, const mt::vec2f & _minSecond, const mt::vec2f & _maxSecond )
		{
			mt::box2f box_first(_minFirst, _maxFirst);
			mt::box2f box_second( _minSecond, _maxSecond );

			bool result = mt::is_intersect( box_first, box_second );

			return result;
		}

		mt::vec2f projectionPointToLine( const mt::vec2f & _point, const mt::vec2f & _v0, const mt::vec2f & _v1 )
		{
			mt::vec2f dir = _v1 - _v0;
			
			if( mt::dot_v2_v2(dir, dir) < 0.0001f )
			{
				return _v0;
			}

			mt::vec2f dir_norm;
			float dir_length = norm_v2_f(dir_norm, dir);

			mt::vec2f dir_point = _point - _v0;

			float dist = dot_v2_v2(dir_point, dir_norm);

			if( dist < 0.f )
			{
				dist = 0.f;
			}
			else if( dist > dir_length )
			{
				dist = dir_length;
			}

			mt::vec2f line_point = _v0 + dir_norm * dist;

			return line_point;
		}

		pybind::tuple s_angle_correct_interpolate_from_to( float _from, float _to )
		{
			float correct_angle_from;
			float correct_angle_to;

			mt::angle_correct_interpolate_from_to( _from, _to, correct_angle_from, correct_angle_to );

			pybind::tuple py_result = pybind::make_tuple_t( correct_angle_from, correct_angle_to );

			return py_result;
		}

        float s_angle_between_two_vectors( const mt::vec2f & _v1, const mt::vec2f & _v2 )
        {
            mt::vec2f v1_norm;
            mt::norm_v2_v2( v1_norm, _v1 );

            mt::vec2f v2_norm;
            mt::norm_v2_v2( v2_norm, _v2 );

            float cos_v = mt::dot_v2_v2( v1_norm, v2_norm );

            float angle = acosf( cos_v );

            return angle;
        }

		pybind::list s_getPolygonPoints( const Polygon & _polygon )
		{
			pybind::list py_list;

			const mt::vec2f * ring = _polygon.outer_points();
			uint32_t ring_count = _polygon.outer_count();

			py_list.append( ring, ring + ring_count );

			return py_list;
		}

		Polygon s_intersectionPolygons( Polygon _p1, Polygon _p2 )
		{
			_p1.correct();
			_p2.correct();

			TVectorPolygon output;
			_p1.difference( _p2, output );

			if( output.empty() == true )
			{
				return _p1;
			}

			Polygon inter = output[0];

			inter.correct();

			return inter;
		}

		bool s_intersectsPolygons( Polygon _p1, Polygon _p2 )
		{
			_p1.correct();
			_p2.correct();

			bool intersect = _p1.intersects( _p2 );

			return intersect;
		}

		bool s_intersectsPolygonsWM( const mt::mat4f & _wm1, Polygon _p1, const mt::mat4f & _wm2, Polygon _p2 )
		{
			_p1.correct();
			_p2.correct();

			Polygon p1wm;
			_p1.mul_wm( p1wm, _wm1 );

			Polygon p2wm;
			_p2.mul_wm( p2wm, _wm2 );
						
			bool intersect = p1wm.intersects( p2wm );

			return intersect;
		}

		bool s_intersectsPolygonsWMP( const mt::vec3f & _wm1, Polygon _p1, const mt::vec3f & _wm2, Polygon _p2 )
		{
			_p1.correct();
			_p2.correct();

			Polygon p1wm;
			_p1.transpose( p1wm, _wm1.to_vec2f() );

			Polygon p2wm;
			_p2.transpose( p2wm, _wm2.to_vec2f() );
						
			bool intersect = p1wm.intersects( p2wm );

			return intersect;
		}

		bool s_intersectsMoviesHotspot( Movie * _movie1, const ConstString & _socket1, Movie * _movie2, const ConstString & _socket2 )
		{
			if( _movie1 == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectsMoviesHotspot movie1 is NULL"
					);

				return false;
			}

			if( _movie2 == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectsMoviesHotspot movie2 is NULL"
					);

				return false;
			}

			if( _movie1->compile() == false )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectsMoviesHotspot movie1 invalid compile"
					);

				return false;
			}

			if( _movie2->compile() == false )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectsMoviesHotspot movie2 invalid compile"
					);

				return false;
			}

			Node * node1;
			Movie * submovie1;

			if( _movie1->hasMovieNode( _socket1, CONST_STRING( m_serviceProvider, MovieSocketShape ), &node1, &submovie1 ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectsMoviesHotspot movie1 %s not found socket shape %s"
					, _movie1->getName().c_str()
					, _socket1.c_str()
					);

				return false;
			}

			Node * node2;
			Movie * submovie2;

			if( _movie2->hasMovieNode( _socket2, CONST_STRING( m_serviceProvider, MovieSocketShape ), &node2, &submovie2 ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectsMoviesHotspot movie2 %s not found socket shape %s"
					, _movie2->getName().c_str()
					, _socket2.c_str()
					);

				return false;
			}

			HotSpotShape * shape1 = static_cast<HotSpotShape *>(node1);

			if( shape1->compile() == false )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectsMoviesHotspot movie1 %s socket shape %s invalid compile"
					, _movie1->getName().c_str()
					, _socket1.c_str()
					);

				return false;
			}

			HotSpotShape * shape2 = static_cast<HotSpotShape *>(node2);

			if( shape2->compile() == false )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectsMoviesHotspot movie2 %s socket shape %s invalid compile"
					, _movie2->getName().c_str()
					, _socket2.c_str()
					);

				return false;
			}

			const Polygon & p1 = shape1->getPolygonWM();
			const Polygon & p2 = shape2->getPolygonWM();

			bool result = s_intersectsPolygons( p1, p2 );

			return result;
		}

		bool s_intersectMoviesHotspotVsPolygon( Movie * _movie, const ConstString & _socket, Polygon _polygon )
		{
			_polygon.correct();

			if( _movie == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectMoviesHotspotVsPolygon movie is NULL"
					);

				return false;
			}

			if( _movie->compile() == false )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectMoviesHotspotVsPolygon movie invalid compile"
					);

				return false;
			}

			Node * node;
			Movie * submovie;

			if( _movie->hasMovieNode( _socket, CONST_STRING( m_serviceProvider, MovieSocketShape ), &node, &submovie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectMoviesHotspotVsPolygon movie %s not found socket shape %s"
					, _movie->getName().c_str()
					, _socket.c_str()
					);

				return false;
			}

			HotSpotShape * shape = static_cast<HotSpotShape *>(node);

			if( shape->compile() == false )
			{
				LOGGER_ERROR( m_serviceProvider )("s_intersectMoviesHotspotVsPolygon movie %s socket shape %s invalid compile"
					, _movie->getName().c_str()
					, _socket.c_str()
					);

				return false;
			}

			const Polygon & p1 = shape->getPolygonWM();

			bool result = s_intersectsPolygons( p1, _polygon );

			return result;
		}

#	ifdef PYBIND_VISIT_OBJECTS
		class MyObjectVisits
			: public pybind::pybind_visit_objects
		{
		public:
			MyObjectVisits( const pybind::list & _py_list )
				: m_py_list(_py_list)
			{
			}

		protected:
			void visit( PyObject * _obj ) override
			{
				m_py_list.append( _obj );
			}

		protected:
			pybind::list m_py_list;
		};
#	endif

		pybind::list s_objects()
		{
			pybind::list py_list;

#	ifdef PYBIND_VISIT_OBJECTS
			MyObjectVisits mov(py_list);
			pybind::visit_objects(&mov);
#	endif

			return py_list;
		}

        class MyVisitorRenderTexture
            : public VisitorRenderTextureInterface
        {
        public:
			MyVisitorRenderTexture( const pybind::list & _list )
                : m_list(_list)
            {
            }

        protected:
            void visitRenderTexture( const RenderTextureInterface * _texture ) override
            {
                const FilePath & filePath = _texture->getFileName();

                m_list.append( filePath );
            }

        protected:
			pybind::list m_list;
        };

		pybind::list s_textures()
        {
			pybind::list py_list;

            MyVisitorRenderTexture mvrt(py_list);
            RENDERTEXTURE_SERVICE(m_serviceProvider)
                ->visitTexture( &mvrt );

            return py_list;
        }

		float s_watchdog( const String & _tag )
		{
			float watch = WATCHDOG( m_serviceProvider, _tag );

			return watch;
		}

		bool s_isPointInsidePolygon( const mt::vec2f & _point, const Polygon & _polygon )
		{
			Polygon point_polygon;
			point_polygon.append(_point );
			point_polygon.correct();

			Polygon correct_polygon(_polygon);
			correct_polygon.correct();
			
			bool result = point_polygon.intersects( correct_polygon );

			return result;
		}

		uint64_t s_getTimeMs()
		{
			uint64_t ms = TIMER_SERVICE( m_serviceProvider )
				->getMilliseconds();

			return ms;
		}

        uint64_t s_getTime()
        {
            uint64_t ms = s_getTimeMs();

            uint64_t s = ms / 1000;

            return s;
        }

        uint32_t s_getDate()
        {
            std::time_t ctTime; 
            std::time(&ctTime);

            return static_cast<unsigned int>(ctTime);
        }

		String s_getTimeString()
		{
			std::time_t ctTime;
			std::time(&ctTime);

			std::tm* sTime = std::localtime( &ctTime );

			Stringstream str;
			str << std::setw(2) << std::setfill('0') << sTime->tm_hour
				<< ":" << std::setw(2) << std::setfill('0') << sTime->tm_min
				<< ":" << std::setw(2) << std::setfill('0') << sTime->tm_sec;

			return str.str();
		}

		class MyAccountVisitorInterface
			: public AccountVisitorInterface
		{
		public:
			MyAccountVisitorInterface( TVectorWString & _accounts )
				: m_accounts(_accounts)
			{
			}

		protected:
			void onAccount( const AccountInterfacePtr & _account ) override
			{
				const WString & name = _account->getName();

				m_accounts.push_back( name );
			}

		protected:
			TVectorWString & m_accounts;

		private:
			void operator = ( const MyAccountVisitorInterface & )
			{
				//Empty
			}
		};

        TVectorWString s_getAccounts()
		{
			TVectorWString v_accounts;
			MyAccountVisitorInterface mav(v_accounts);

			ACCOUNT_SERVICE(m_serviceProvider)
				->visitAccounts( &mav );

			return v_accounts;
		}

		bool s_addSetting( const ConstString & _setting, const WString & _defaultValue, const pybind::object & _applyFunc )
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Error addSetting: currentAccount is none [%s]"
                    , _setting.c_str()
					);

				return false;
			}
			
			bool result = currentAccount->addSetting( _setting, _defaultValue, _applyFunc );

			return result;
		}

		bool s_changeSetting( const ConstString & _setting, const WString & _value )
		{
            AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
                ->getCurrentAccount();

            if( currentAccount == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("changeSetting: currentAccount is none [%s]"
                    , _setting.c_str()
                    );

                return false;
            }

            bool result = currentAccount->changeSetting( _setting, _value );

            return result;
		}

		bool s_changeSettingBool( const ConstString & _setting, bool _value )
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE( m_serviceProvider )
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("changeSetting: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return false;
			}

			WString setting_value;
			if( _value == true )
			{
				setting_value = L"True";
			}
			else
			{
				setting_value = L"False";
			}

			bool result = currentAccount->changeSetting( _setting, setting_value );

			return result;
		}

        bool s_hasSetting( const ConstString & _setting )
        {
            AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
                ->getCurrentAccount();

            if( currentAccount == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("getSetting: currentAccount is none [%s]"
                    , _setting.c_str()
                    );

                return false;
            }

            bool value = currentAccount->hasSetting( _setting );
            
            return value;
        }

		PyObject * s_getSetting( const ConstString & _setting )
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("getSetting: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const WString & value = currentAccount->getSetting( _setting );

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		PyObject * s_getSettingBool( const ConstString & _setting )
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("getSettingBool: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const WString & value = currentAccount->getSetting( _setting );

			if( value == L"True" ||
				value == L"true" )
			{
				return pybind::ret_true();
			}

			if( value == L"False" ||
				value == L"false" )
			{
				return pybind::ret_false();
			}

			const WString & accountID = currentAccount->getName();

			LOGGER_ERROR( m_serviceProvider )("getAccountSettingUInt account '%ls' setting '%s' value '%ls' is not bool [True|False]"
				, accountID.c_str()
				, _setting.c_str()
				, value.c_str()
				);

			return pybind::ret_none();
		}

		PyObject * s_getSettingInt( const ConstString & _setting )
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("getSettingInt: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = currentAccount->getSetting( _setting );

			int value;
			if( Utils::wstringToInt( setting, value ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("getSettingInt: can't scanf from [%s]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		PyObject * s_getSettingUInt( const ConstString & _setting )
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("getSettingUInt: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = currentAccount->getSetting( _setting );

			uint32_t value;
			if( Utils::wstringToUnsigned( setting, value ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("getSettingUInt: can't scanf from [%s]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		PyObject * s_getSettingFloat( const ConstString & _setting )
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("getSettingFloat: currentAccount is none [%s]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = currentAccount->getSetting( _setting );

			float value;
			if( Utils::wstringToFloat( setting, value ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("getSettingFloat: can't scanf from [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		float s_getSettingFloatDefault( const ConstString & _setting, float _default )
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				return _default;
			}

			const WString & setting = currentAccount->getSetting( _setting );

			float value;
			if( Utils::wstringToFloat( setting, value ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("getSettingFloat: can't scanf from [%S]"
					, _setting.c_str()
					);

				return _default;
			}

			return value;
		}

		PyObject * s_getAccountSetting( const WString & _accountID, const ConstString & _setting )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE(m_serviceProvider)
				->getAccount( _accountID );
						
			if( account == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("getAccountSetting account '%ls' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & value = account->getSetting( _setting );

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		PyObject * s_getAccountSettingBool( const WString& _accountID, const ConstString & _setting )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE(m_serviceProvider)
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("getAccountSettingUInt account '%ls' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & value = account->getSetting( _setting );

			if( value == L"True" ||
				value == L"true" )
			{
				return pybind::ret_true();
			}

			if( value == L"False" ||
				value == L"false" )
			{
				return pybind::ret_false();
			}

			LOGGER_ERROR( m_serviceProvider )("getAccountSettingUInt account '%ls' setting '%s' value '%ls' is not bool [True|False]"
				, _accountID.c_str()
				, _setting.c_str()
				, value.c_str()
				);

			return pybind::ret_none();
		}

		PyObject * s_getAccountSettingUInt( const WString& _accountID, const ConstString & _setting )
		{
			AccountInterfacePtr account = ACCOUNT_SERVICE(m_serviceProvider)
				->getAccount( _accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("getAccountSettingUInt account '%ls' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = account->getSetting( _setting );

			uint32_t value;
			if( Utils::wstringToUnsigned( setting, value ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("getAccountSettingUInt account '%ls'can't scanf from '%s'"
                    , _accountID.c_str()
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		PyObject * s_createAccount()
		{
            AccountInterfacePtr account = ACCOUNT_SERVICE(m_serviceProvider)
                ->createAccount();

            if( account == nullptr )
            {
                return pybind::ret_none();
            }
            
			const WString & accountId = account->getName();

            PyObject * py_value = pybind::ptr( accountId );

            return py_value;
		}

		void s_selectAccount( const WString& _accountID )
		{
			ACCOUNT_SERVICE(m_serviceProvider)
				->selectAccount( _accountID );
		}
		
		bool s_hasCurrentAccount()
		{
			return ACCOUNT_SERVICE(m_serviceProvider)
				->hasCurrentAccount();
		}

		void s_setDefaultAccount( const WString & _accountID )
		{
			return ACCOUNT_SERVICE(m_serviceProvider)
				->setDefaultAccount( _accountID );
		}

		const WString & s_getDefaultAccount()
		{
			return ACCOUNT_SERVICE(m_serviceProvider)
				->getDefaultAccount();
		}

		bool s_hasDefaultAccount()
		{
			return ACCOUNT_SERVICE(m_serviceProvider)
				->hasDefaultAccount();
		}

		bool s_isCurrentDefaultAccount()
		{
			return ACCOUNT_SERVICE(m_serviceProvider)
				->isCurrentDefaultAccount();
		}

		bool s_selectDefaultAccount()
		{
			return ACCOUNT_SERVICE(m_serviceProvider)
				->selectDefaultAccount();
		}
		
		void s_saveAccount()
		{
			//AccountInterface * currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
			//	->getCurrentAccount();

			//if( currentAccount == NULL )
			//{
			//	LOGGER_ERROR(m_serviceProvider)("Error saveCurrentAccount: currentAccount is none"
			//		);

			//	return;
			//}

			//currentAccount->save();
		}
	
		void s_saveAccounts()
		{
            ACCOUNT_SERVICE(m_serviceProvider)
                ->saveAccounts();
		}

		void s_saveAccountsInfo()
		{
			//ACCOUNT_SERVICE(m_serviceProvider)
			//	->saveAccountsInfo();
		}

		void s_deleteAccount( const WString& _accountName )
		{
			ACCOUNT_SERVICE(m_serviceProvider)
				->deleteAccount( _accountName );
		}
				
		const WString & s_getCurrentAccountName()
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
  				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Error getCurrentAccountName: currentAccount is none"
					);

				return Utils::emptyWString();
			}

			const WString & name = currentAccount->getName();

			return name;
		}

		bool s_writeAccountPickleFile( const WString & _fileName, PyObject * _data, PyObject * _pickleTypes )
		{
            String utf8_fileName;
            if( Helper::unicodeToUtf8( m_serviceProvider, _fileName, utf8_fileName ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("s_writeAccountBinaryFile: invalid file %ls convert to utf8"                    
                    , _fileName.c_str()
                    );

                return false;                     
            }

            AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{                
				LOGGER_ERROR(m_serviceProvider)("writeAccountPickleFile: invalid write file %ls (currentAccount is none)"
                    , _fileName.c_str()
					);

				return false;
			}

            ConstString filepath = Helper::stringizeString( m_serviceProvider, utf8_fileName );

			size_t size;
			if( pybind::pickle( _data, _pickleTypes, nullptr, 0, size ) == false )
			{
				const WString & accountName = currentAccount->getName();

				LOGGER_ERROR(m_serviceProvider)("writeAccountPickleFile: account %ls invalid get pickle size"
					, accountName.c_str()
					);

				return false;
			}

			MemoryCacheBufferInterfacePtr buffer = Helper::createMemoryCacheBuffer( m_serviceProvider, size, "s_writeAccountBinaryFile" );

			if( buffer == nullptr )
			{
				const WString & accountName = currentAccount->getName();

				LOGGER_ERROR( m_serviceProvider )("writeAccountPickleFile: account %ls invalid get memory for %d size"
					, accountName.c_str()
					, size
					);

				return false;
			}

			void * memory_buffer = buffer->getMemory();
			size_t memory_size = buffer->getSize();
			
			if( pybind::pickle( _data, _pickleTypes, memory_buffer, memory_size, size ) == false )
			{
				const WString & accountName = currentAccount->getName();

				LOGGER_ERROR(m_serviceProvider)("writeAccountPickleFile: account %ls invalid pickle"
					, accountName.c_str()
					);

				return false;
			}

            if( currentAccount->writeBinaryFile( filepath, memory_buffer, memory_size ) == false )
            {
                const WString & accountName = currentAccount->getName();

                LOGGER_ERROR(m_serviceProvider)("writeAccountPickleFile: account %ls invalid write file %ls"
                    , accountName.c_str()
                    , _fileName.c_str()
                    );
                
                return false;
            }
      
			return true;
		}

		PyObject * s_loadAccountPickleFile( const WString & _fileName, PyObject * _pickleTypes )
		{
            String utf8_fileName;
            if( Helper::unicodeToUtf8( m_serviceProvider, _fileName, utf8_fileName ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid convert filename %ls to utf8"
                    , _fileName.c_str()
                    );

                return pybind::ret_none();
            }

            AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid load file %ls (currentAccount is none)"
                    , _fileName.c_str()
					);

				return pybind::ret_none();
			}

            ConstString filename = Helper::stringizeString( m_serviceProvider, utf8_fileName );

			MemoryCacheBufferInterfacePtr binaryBuffer = currentAccount->loadBinaryFile( filename );

			if( binaryBuffer == nullptr )
            {
                const WString & accountName = currentAccount->getName();

                LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: account %ls invalid load file %ls"
                    , accountName.c_str()
                    , _fileName.c_str()
                    );

                return pybind::ret_none();
            }

			void * binaryBuffer_memory = binaryBuffer->getMemory();
			size_t binaryBuffer_size = binaryBuffer->getSize();
	
			PyObject * py_data = pybind::unpickle( binaryBuffer_memory, binaryBuffer_size, _pickleTypes );

			if( py_data == nullptr )
			{
				const WString & accountName = currentAccount->getName();

				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: account %ls invalid unpickle file %ls"
					, accountName.c_str()
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}

			return py_data;
		}

		bool s_hasAccountPickleFile( const WString & _fileName )
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid load file %ls (currentAccount is none)"
					, _fileName.c_str()
					);

				return false;
			}

			String utf8_fileName;
			if( Helper::unicodeToUtf8( m_serviceProvider, _fileName, utf8_fileName ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid convert filename %ls to utf8"
					, _fileName.c_str()
					);

				return false;
			}

			ConstString filename = Helper::stringizeString( m_serviceProvider, utf8_fileName );

			bool exist = currentAccount->hasBinaryFile( filename );

			return exist;
		}

		bool s_writeAccountBinaryFile_deprecated( const WString & _fileName, const Blobject & _data )
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{                
				LOGGER_ERROR(m_serviceProvider)("s_writeAccountBinaryFile: invalid get current account"
					);

				return false;
			}

			String utf8_fileName;
			if( Helper::unicodeToUtf8( m_serviceProvider, _fileName, utf8_fileName ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("s_writeAccountBinaryFile: invalid file %ls convert to utf8"                    
					, _fileName.c_str()
					);

				return false;                     
			}

			ConstString filename = Helper::stringizeString( m_serviceProvider, utf8_fileName );

			OutputStreamInterfacePtr stream = currentAccount->openWriteBinaryFile( filename );
			
			if( stream == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_writeAccountBinaryFile: invalid open file %ls"
					, _fileName.c_str()
					);

				return false;
			}

			ArchivatorInterfacePtr archivator = ARCHIVE_SERVICE(m_serviceProvider)
				->getArchivator( Helper::stringizeString(m_serviceProvider, "zip") );

			if( archivator == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_writeAccountBinaryFile: invalid get archivator 'zip'"
					);

				return false;
			}

			uint32_t data_size = _data.size();

			MemoryInputInterfacePtr compress_memory = ARCHIVE_SERVICE( m_serviceProvider )
				->compressBuffer( archivator, &_data[0], data_size );

			if( compress_memory == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_writeAccountBinaryFile: invalid compress file %ls"
					, _fileName.c_str()
					);

				return false;
			}
						
			const void * compressBuffer = compress_memory->getMemory();
			size_t compressSize = compress_memory->getSize();

			uint32_t value_crc32 = Helper::make_crc32( (const unsigned char *)compressBuffer, compressSize );

			if( stream->write( &value_crc32, sizeof(value_crc32) ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("s_writeAccountBinaryFile: invalid write 'crc32' %ls"
					, _fileName.c_str()
					);

				return false;
			}

			if( stream->write( &data_size, sizeof(data_size) ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("s_writeAccountBinaryFile: invalid write 'data size' %ls"
					, _fileName.c_str()
					);

				return false;
			}

			if( stream->write( compressBuffer, compressSize ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("s_writeAccountBinaryFile: invalid write 'data' %ls"
					, _fileName.c_str()
					);

				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * s_loadAccountBinaryFile_deprecated( const WString & _fileName )
		{
			AccountInterfacePtr currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid load file %ls (currentAccount is none)"
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}

			String utf8_fileName;
			if( Helper::unicodeToUtf8( m_serviceProvider, _fileName, utf8_fileName ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid convert filename %ls to utf8"
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}
			
			ConstString filename = Helper::stringizeString( m_serviceProvider, utf8_fileName );

			InputStreamInterfacePtr stream = currentAccount->openReadBinaryFile( filename );

			if( stream == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid open file '%ls'"
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}

			size_t file_size = stream->size();

			uint32_t load_crc32 = 0;
			if( stream->read( &load_crc32, sizeof(load_crc32) ) != sizeof(load_crc32) )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid load file %ls (load crc32)"
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}

			uint32_t load_data_size = 0;
			if( stream->read( &load_data_size, sizeof(load_data_size) ) != sizeof(load_data_size) )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid load file %ls (load data size)"
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}

			size_t load_compress_size = file_size - sizeof(load_crc32) - sizeof(load_data_size);

			Blobject archive_blob;
			archive_blob.resize( load_compress_size );

			Blobject::value_type * archive_blob_buffer = &archive_blob[0];
			
			if( stream->read( archive_blob_buffer, load_compress_size ) != load_compress_size )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid load file %ls (load data)"
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}

			stream = nullptr;

			size_t check_crc32 = Helper::make_crc32( &archive_blob[0], load_compress_size );

			if( load_crc32 != check_crc32 )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid load file %ls (crc32 incorect)"
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}

			ArchivatorInterfacePtr archivator = ARCHIVE_SERVICE(m_serviceProvider)
				->getArchivator( Helper::stringizeString(m_serviceProvider, "zip") );

			if( archivator == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: invalid get archivator %s"
					, "zip"
					);

				return pybind::ret_none();
			}

			Blobject data_blob;
			data_blob.resize( load_data_size );

			size_t uncompress_size;
			if( archivator->decompress( &data_blob[0], load_data_size, &archive_blob[0], load_compress_size, uncompress_size ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: file '%ls' invalid decompress"
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}

			if( load_data_size != uncompress_size )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: file '%ls' invalid decompress size"
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_data = pybind::string_from_char_size( (const char *)&data_blob[0], load_data_size );

			if( py_data == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_loadAccountPickleFile: %ls invalid make python string"
					, _fileName.c_str()
					);

				return pybind::ret_none();
			}

			return py_data;
		}

		void s_setParticlesEnabled( bool _enable )
		{
			APPLICATION_SERVICE(m_serviceProvider)
				->setParticleEnable( _enable );
		}

		bool s_hasTextByKey( const ConstString& _key )
		{
			bool value = TEXT_SERVICE(m_serviceProvider)
				->existText( _key, nullptr );

			return value;
		}

		WString s_getTextByKey( const ConstString& _key )
		{
			const TextEntryInterface * entry;				
			if( TEXT_SERVICE(m_serviceProvider)
				->existText( _key, &entry ) == false )
			{
				pybind::throw_exception("Menge.getTextByKey invalid get key %s"
					, _key.c_str()
					);
			}

			const ConstString & text = entry->getValue();

            WString unicode;
            if( Helper::utf8ToUnicode(m_serviceProvider, text, unicode ) == false )
            {
				pybind::throw_exception("Menge.getTextByKey invalid text key %s convert %s to unicode"
					, _key.c_str()
					, text.c_str()
					);
            }

			return unicode;
		}

		uint32_t s_getTextCharCountByKey( const ConstString& _key )
		{
			const TextEntryInterface * entry;
			if( TEXT_SERVICE(m_serviceProvider)
				->existText( _key, &entry ) == false )
			{				
				pybind::throw_exception("Menge.getTextCharCountByKey invalid get key %s"
					, _key.c_str()
					);
			}

			const ConstString & text = entry->getValue();

			uint32_t count = text.size();

			return count;
		}

		void s_setVSync( bool _vSync )
		{
			APPLICATION_SERVICE(m_serviceProvider)
				->setVSync( _vSync );
		}

		bool s_getVSync()
		{
			bool vsync = APPLICATION_SERVICE(m_serviceProvider)
				->getVSync();

            return vsync;
		}

		void s_setCursorMode( bool _mode )
		{
			APPLICATION_SERVICE(m_serviceProvider)
				->setCursorMode( _mode );
		}

		bool s_getCursorMode()
		{
			bool cursorMode = APPLICATION_SERVICE(m_serviceProvider)
				->getCursorMode();

            return cursorMode;
		}

		bool s_setCursorIcon(const ConstString & _resourceName)
		{
			APPLICATION_SERVICE(m_serviceProvider)
				->setCursorIcon( _resourceName );

			return true;
		}

    protected:
        ServiceProviderInterface * m_serviceProvider;
    };

	//////////////////////////////////////////////////////////////////////////
	//REGISTER_SCRIPT_CLASS( Menge, ScriptHelper, Base )
	void ScriptWrapper::helperWrap( ServiceProviderInterface * _serviceProvider )
	{
		//srand( (unsigned)std::time( NULL ) );
        HelperScriptMethod * helperScriptMethod = new HelperScriptMethod(_serviceProvider);

  //      pybind::def_functor( "addInterpolatorLinearVector", helperScriptMethod, &HelperScriptMethod::addInterpolatorLinearVector );
		//pybind::def_functor( "addInterpolatorLinearFloat", helperScriptMethod, &HelperScriptMethod::addInterpolatorLinearFloat);

		//pybind::def_functor( "addGlobalInterpolatorLinearVector", helperScriptMethod, &HelperScriptMethod::addGlobalInterpolatorLinearVector );
		//pybind::def_functor( "addGlobalInterpolatorLinearFloat", helperScriptMethod, &HelperScriptMethod::addGlobalInterpolatorLinearFloat);		

        pybind::def_functor( "rand", helperScriptMethod, &HelperScriptMethod::mt_rand );
		pybind::def_functor( "randf", helperScriptMethod, &HelperScriptMethod::mt_randf );
		pybind::def_functor( "range_rand", helperScriptMethod, &HelperScriptMethod::mt_range_rand );
		pybind::def_functor( "range_randf", helperScriptMethod, &HelperScriptMethod::mt_range_randf );
			
		pybind::def_functor( "sqrtf", helperScriptMethod, &HelperScriptMethod::mt_sqrtf );
		pybind::def_functor( "absf", helperScriptMethod, &HelperScriptMethod::mt_absf );
		pybind::def_functor( "cosf", helperScriptMethod, &HelperScriptMethod::mt_cosf );
		pybind::def_functor( "sinf", helperScriptMethod, &HelperScriptMethod::mt_sinf );
		pybind::def_functor( "tanf", helperScriptMethod, &HelperScriptMethod::mt_tanf );
		pybind::def_functor( "acosf", helperScriptMethod, &HelperScriptMethod::mt_acosf );
		pybind::def_functor( "atanf", helperScriptMethod, &HelperScriptMethod::mt_atanf );
		pybind::def_functor( "direction", helperScriptMethod, &HelperScriptMethod::mt_direction );
		pybind::def_functor( "angle_from_v3_v3", helperScriptMethod, &HelperScriptMethod::my_angle_from_v3_v3 );
        pybind::def_functor( "norm_v2", helperScriptMethod, &HelperScriptMethod::mt_norm_v2 );

        pybind::def_functor( "angle_correct_interpolate_from_to", helperScriptMethod, &HelperScriptMethod::s_angle_correct_interpolate_from_to );
        pybind::def_functor( "angle_between_two_vectors", helperScriptMethod, &HelperScriptMethod::s_angle_between_two_vectors );

		pybind::def_functor( "projectionPointToLine", helperScriptMethod, &HelperScriptMethod::projectionPointToLine );

		pybind::def_functor( "isPointInsidePolygon", helperScriptMethod, &HelperScriptMethod::s_isPointInsidePolygon );

		pybind::def_functor( "getTime", helperScriptMethod, &HelperScriptMethod::s_getTime );
        pybind::def_functor( "getTimeMs", helperScriptMethod, &HelperScriptMethod::s_getTimeMs );

        pybind::def_functor( "getDate", helperScriptMethod, &HelperScriptMethod::s_getDate );
        
        
		pybind::def_functor( "getTimeString", helperScriptMethod, &HelperScriptMethod::s_getTimeString );

		pybind::def_functor( "getAccounts", helperScriptMethod, &HelperScriptMethod::s_getAccounts );
		pybind::def_functor( "addSetting", helperScriptMethod, &HelperScriptMethod::s_addSetting );
		pybind::def_functor( "getSetting", helperScriptMethod, &HelperScriptMethod::s_getSetting );
        pybind::def_functor( "hasSetting", helperScriptMethod, &HelperScriptMethod::s_hasSetting );

		pybind::def_functor( "getSettingBool", helperScriptMethod, &HelperScriptMethod::s_getSettingBool );
		pybind::def_functor( "getSettingInt", helperScriptMethod, &HelperScriptMethod::s_getSettingInt );
		pybind::def_functor( "getSettingUInt", helperScriptMethod, &HelperScriptMethod::s_getSettingUInt );
		pybind::def_functor( "getSettingFloat", helperScriptMethod, &HelperScriptMethod::s_getSettingFloat );
		pybind::def_functor( "getSettingFloatDefault", helperScriptMethod, &HelperScriptMethod::s_getSettingFloatDefault );		
		

		pybind::def_functor( "changeSetting", helperScriptMethod, &HelperScriptMethod::s_changeSetting );
		pybind::def_functor( "changeSettingBool", helperScriptMethod, &HelperScriptMethod::s_changeSettingBool );
		
		

		pybind::def_functor( "getAccountSetting", helperScriptMethod, &HelperScriptMethod::s_getAccountSetting );
		pybind::def_functor( "getAccountSettingBool", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingBool );
		pybind::def_functor( "getAccountSettingUInt", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingUInt );
		
				

		pybind::def_functor( "createAccount", helperScriptMethod, &HelperScriptMethod::s_createAccount );
		pybind::def_functor( "selectAccount", helperScriptMethod, &HelperScriptMethod::s_selectAccount );
		pybind::def_functor( "deleteAccount", helperScriptMethod, &HelperScriptMethod::s_deleteAccount );
				
		pybind::def_functor( "saveAccount", helperScriptMethod, &HelperScriptMethod::s_saveAccount );
		pybind::def_functor( "saveAccounts", helperScriptMethod, &HelperScriptMethod::s_saveAccounts );
		pybind::def_functor( "saveAccountsInfo", helperScriptMethod, &HelperScriptMethod::s_saveAccountsInfo );
		//pybind::def_function( "getDataPath", &ScriptHelper::s_getDataPath );
		pybind::def_functor( "hasCurrentAccount", helperScriptMethod, &HelperScriptMethod::s_hasCurrentAccount );
		pybind::def_functor( "getCurrentAccountName", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountName );

		pybind::def_functor( "setDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_setDefaultAccount );
		pybind::def_functor( "getDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_getDefaultAccount );
		pybind::def_functor( "hasDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_hasDefaultAccount );
		pybind::def_functor( "isCurrentDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_isCurrentDefaultAccount );
		pybind::def_functor( "selectDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_selectDefaultAccount );

        pybind::def_functor( "writeAccountPickleFile", helperScriptMethod, &HelperScriptMethod::s_writeAccountPickleFile );
        pybind::def_functor( "loadAccountPickleFile", helperScriptMethod, &HelperScriptMethod::s_loadAccountPickleFile );
		pybind::def_functor( "hasAccountPickleFile", helperScriptMethod, &HelperScriptMethod::s_hasAccountPickleFile );

		pybind::def_functor( "writeAccountBinaryFile_deprecated", helperScriptMethod, &HelperScriptMethod::s_writeAccountBinaryFile_deprecated );
		pybind::def_functor( "loadAccountBinaryFile_deprecated", helperScriptMethod, &HelperScriptMethod::s_loadAccountBinaryFile_deprecated );
		
		pybind::def_functor( "setParticlesEnabled", helperScriptMethod, &HelperScriptMethod::s_setParticlesEnabled );

		//pybind::def_function( "unicode", &ScriptHelper::s_unicode );
		//pybind::def_function( "ansi", &ScriptHelper::s_ansi );

		
		pybind::def_functor( "hasTextByKey", helperScriptMethod, &HelperScriptMethod::s_hasTextByKey );
		pybind::def_functor( "getTextByKey", helperScriptMethod, &HelperScriptMethod::s_getTextByKey );
		pybind::def_functor( "getTextCharCountByKey", helperScriptMethod, &HelperScriptMethod::s_getTextCharCountByKey );
		

		//pybind::def_function( "loadPak", &ScriptHelper::s_loadPak );
		//pybind::def_function( "getImageCount", &ScriptHelper::s_getImageCount );
		pybind::def_functor( "setVSync", helperScriptMethod, &HelperScriptMethod::s_setVSync );
		pybind::def_functor( "getVSync", helperScriptMethod, &HelperScriptMethod::s_getVSync );
		pybind::def_functor( "setCursorMode", helperScriptMethod, &HelperScriptMethod::s_setCursorMode );
		pybind::def_functor( "getCursorMode", helperScriptMethod, &HelperScriptMethod::s_getCursorMode );
		pybind::def_functor( "setCursorIcon", helperScriptMethod, &HelperScriptMethod::s_setCursorIcon );
		//pybind::def_function( "setAsScreensaver", &ScriptHelper::s_setAsScreensaver );
        
		//pybind::def_function( "createAnimationSequence", &ScriptHelper::s_createAnimationSequence );

		pybind::def_functor( "intersectsEllipseVsPoint", helperScriptMethod, &HelperScriptMethod::s_intersectsEllipseVsPoint );
		pybind::def_functor( "intersectsBoxes", helperScriptMethod, &HelperScriptMethod::s_intersectsBoxes );

		pybind::def_functor( "getPolygonPoints", helperScriptMethod, &HelperScriptMethod::s_getPolygonPoints );
		pybind::def_functor( "intersectionPolygons", helperScriptMethod, &HelperScriptMethod::s_intersectionPolygons );
		pybind::def_functor( "intersectsPolygons", helperScriptMethod, &HelperScriptMethod::s_intersectsPolygons );
		pybind::def_functor( "intersectsPolygonsWM", helperScriptMethod, &HelperScriptMethod::s_intersectsPolygonsWM );
		pybind::def_functor( "intersectsPolygonsWMP", helperScriptMethod, &HelperScriptMethod::s_intersectsPolygonsWMP );
		pybind::def_functor( "intersectsMoviesHotspot", helperScriptMethod, &HelperScriptMethod::s_intersectsMoviesHotspot );
		pybind::def_functor( "intersectMoviesHotspotVsPolygon", helperScriptMethod, &HelperScriptMethod::s_intersectMoviesHotspotVsPolygon );
		

		pybind::def_functor( "objects", helperScriptMethod, &HelperScriptMethod::s_objects );
        pybind::def_functor( "textures", helperScriptMethod, &HelperScriptMethod::s_textures );
		
		pybind::def_functor( "watchdog", helperScriptMethod, &HelperScriptMethod::s_watchdog );

        pybind::def_functor( "is_class", helperScriptMethod, &HelperScriptMethod::s_is_class );
        pybind::def_functor( "addGlobalModule", helperScriptMethod, &HelperScriptMethod::s_addGlobalModule );

        pybind::def_function( "angle_norm", &mt::angle_norm );
        pybind::def_function( "angle_delta_deg", &mt::angle_delta_deg );

        pybind::def_function( "length_v2_v2", &mt::length_v2_v2 );
        pybind::def_function( "sqrlength_v2_v2", &mt::sqrlength_v2_v2 );
		
		pybind::def_function( "length_v3_v3", &mt::length_v3_v3 );
		pybind::def_function( "sqrlength_v3_v3", &mt::sqrlength_v3_v3 );
        
        pybind::def_function( "signed_angle", &mt::signed_angle );
        pybind::def_function( "angle_length", &mt::angle_length );
        pybind::def_function( "perp", &mt::perp );
        pybind::def_function( "perp_left", &mt::perp_left );

        pybind::def_function( "getrefcount", &pybind::refcount );
		pybind::def_function( "is_class", &pybind::is_class );
		pybind::def_function( "is_type_class", &pybind::is_type_class );
        pybind::def_function( "is_wrap", &pybind::is_wrap );

        pybind::def_functor( "getLanguagePack", helperScriptMethod, &HelperScriptMethod::s_getLanguagePack );

        pybind::def_functor( "isValidWindowMode", helperScriptMethod, &HelperScriptMethod::s_isValidWindowMode );

        pybind::def_functor( "utf8ToUnicode", helperScriptMethod, &HelperScriptMethod::s_utf8ToUnicode );
        pybind::def_functor( "unicodeToUtf8", helperScriptMethod, &HelperScriptMethod::s_unicodeToUtf8 );
        pybind::def_functor( "setCursorPosition", helperScriptMethod, &HelperScriptMethod::s_setCursorPosition );

		pybind::def_functor( "debug", helperScriptMethod, &HelperScriptMethod::s_debug );
		pybind::def_functor( "globalmemoryuse", helperScriptMethod, &HelperScriptMethod::s_globalmemoryuse );
		pybind::def_functor( "memoryuse", helperScriptMethod, &HelperScriptMethod::s_memoryuse );

		pybind::def_functor( "hasGameData", helperScriptMethod, &HelperScriptMethod::s_hasGameData );
		pybind::def_functor( "writeGameData", helperScriptMethod, &HelperScriptMethod::s_writeGameData );
		pybind::def_functor( "loadGameData", helperScriptMethod, &HelperScriptMethod::s_loadGameData );


		pybind::def_functor( "isAltDown", helperScriptMethod, &HelperScriptMethod::s_isAltDown );
		pybind::def_functor( "isShiftDown", helperScriptMethod, &HelperScriptMethod::s_isShiftDown );
		pybind::def_functor( "isCtrlDown", helperScriptMethod, &HelperScriptMethod::s_isCtrlDown );

		pybind::def_functor( "isKeyDown", helperScriptMethod, &HelperScriptMethod::s_isKeyDown );
		pybind::def_functor( "isExclusiveKeyDown", helperScriptMethod, &HelperScriptMethod::s_isExclusiveKeyDown );
		pybind::def_functor( "isAnyKeyDown", helperScriptMethod, &HelperScriptMethod::s_isAnyKeyDown );
		
		pybind::def_functor( "isAnyMouseButtonDown", helperScriptMethod, &HelperScriptMethod::s_isAnyMouseButtonDown );
		pybind::def_functor( "isMouseButtonDown", helperScriptMethod, &HelperScriptMethod::s_isMouseButtonDown );

		pybind::def_functor( "printChildren", helperScriptMethod, &HelperScriptMethod::s_printChildren );

		pybind::def_functor( "getGroupResourcesMemoryUse", helperScriptMethod, &HelperScriptMethod::s_getGroupResourcesMemoryUse );
	}
}
