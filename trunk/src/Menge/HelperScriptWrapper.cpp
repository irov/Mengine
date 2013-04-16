#	include "Config/Typedef.h"

#	include "ScriptWrapper.h"

#	include "Kernel/Node.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/rand.h"

#	include "Core/Polygon.h"
#	include "Core/ColourValue.h"

#	include "Account.h"
#	include "Game.h"
#	include "Application.h"

#	include "Logger/Logger.h"

#	include "Interface/ThreadSystemInterface.h"
#	include "Interface/EntityInterface.h"
#   include "Interface/UnicodeInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "ResourceAnimation.h"
//#	include "ResourceSequence.h"

#	include "Player.h"
#	include "TimingManager.h"

#	include "Watchdog.h"

#	include <ctime>
#	include <sstream>
#	include <iomanip>

#	include "TextManager.h"

#	include "ThreadTaskLoadPak.h"

#	include "ResourceImage.h"
#	include "ResourceCursorICO.h"
#	include "AccountManager.h"

#	include "Kernel/Affector.h"

#	include "Utils/Core/File.h"
#	include "Utils/Core/String.h"
#   include "Utils/Core/CRC32.h"

#	include "Math/angle.h"

#	include "Interface/ScriptSystemInterface.h"

#	include "EventManager.h"
#	include "TimingManager.h"

#pragma warning(push, 0) 
#	include "boost/geometry/geometries/box.hpp"
#pragma warning(pop) 


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

        void s_addText( const ConstString & _id, const WString & _text, const ConstString & _font, float _charOffset, float _lineOffset )
        {
            TextEntry te;

            te.text = _text;
            te.font = _font;
            te.charOffset = _charOffset;
            te.lineOffset = _lineOffset;

            TEXT_SERVICE(m_serviceProvider)
                ->addTextEntry( _id, te );
        }
        
        const ConstString & s_getLanguagePack()
        {
            const ConstString & languagePack = GAME_SERVICE(m_serviceProvider)
                ->getLanguagePack();

            return languagePack;
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

		int mt_randf( float a )
		{	
			return (int)mt::randf( a );
		}

		int mt_range_rand( int a, int b )
		{	
			return mt::range_rand( a, b );
		}

		int mt_range_randf( float a, float b )
		{	
			return (int)mt::range_randf( a, b );
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
			return cosf(a);
		}

		float mt_sinf( float a )
		{
			return sinf(a);
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
			norm_v2(direction_n, direction);

			return direction_n;
		}

		mt::vec2f mt_norm_v2( const mt::vec2f & _vec )
		{
			mt::vec2f v_n;
			mt::norm_v2( v_n, _vec );

			return v_n;
		}

		bool s_intersectsBoxes( const mt::vec2f & _minFirst, const mt::vec2f & _maxFirst, const mt::vec2f & _minSecond, const mt::vec2f & _maxSecond )
		{
			typedef boost::geometry::model::box<mt::vec2f> Box;
			Box box_first(_minFirst, _maxFirst);
			Box box_second(_minSecond, _maxSecond);

			bool result = boost::geometry::intersects(box_first, box_second);

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

		PyObject * s_angle_correct_interpolate_from_to( float _from, float _to )
		{
			float correct_angle_from;
			float correct_angle_to;

			mt::angle_correct_interpolate_from_to( _from, _to, correct_angle_from, correct_angle_to );

			PyObject * py_result = pybind::build_value("(ff)", correct_angle_from, correct_angle_to);

			return py_result;
		}

		PyObject * s_getPolygonPoints( const Polygon & _polygon )
		{
			PyObject * py_list = pybind::list_new(0);

			const Polygon::ring_type & ring = _polygon.outer();

			for( Polygon::ring_type::const_iterator 
				it = ring.begin(),
				it_end = ring.end();
			it != it_end;
			++it )
			{
				PyObject * py_point = pybind::ptr(*it);
				pybind::list_appenditem( py_list, py_point );
				pybind::decref( py_point );
			}

			return py_list;
		}

		class MyObjectVisits
			: public pybind::pybind_visit_objects
		{
		public:
			MyObjectVisits( PyObject * _py_list )
				: m_py_list(_py_list)
			{
			}

		protected:
			void visit( PyObject * _obj ) override
			{
				pybind::list_appenditem(m_py_list, _obj);
			}

		protected:
			PyObject * m_py_list;
		};

		PyObject * s_objects()
		{
			PyObject * py_list = pybind::list_new(0);

#	ifdef PYBIND_VISIT_OBJECTS
			MyObjectVisits mov(py_list);
			pybind::visit_objects(&mov);
#	endif

			return py_list;
		}

		float s_watchdog( const String & _tag )
		{
            WatchdogInterface * watchdog = APPLICATION_SERVICE(m_serviceProvider)
                ->getWatchdog();

			float watch = watchdog->watch( _tag );

			return watch;
		}

		bool s_isPointInsidePolygon( const mt::vec2f & _point, const Polygon & _polygon )
		{
			Polygon point_polygon;
			boost::geometry::append(point_polygon, _point);
			boost::geometry::correct(point_polygon);

			Polygon correct_polygon(_polygon);
			boost::geometry::correct(correct_polygon);
			
			bool result = boost::geometry::intersects(point_polygon, correct_polygon);

			return result;
		}

		unsigned long s_getTimeMs()
		{
            TimerInterface * timer = PLATFORM_SERVICE(m_serviceProvider)
                ->getTimer();

            unsigned long ms = timer->getMilliseconds();

			return ms;
		}

        unsigned long s_getTime()
        {
            unsigned long ms = s_getTimeMs();

            unsigned long s = ms / 1000;

            return s;
        }

        unsigned int s_getDate()
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

        TVectorWString s_getAccounts()
		{
			const TMapAccounts & accounts = ACCOUNT_SERVICE(m_serviceProvider)
				->getAccounts();

			TVectorWString v_accounts;

			for( TMapAccounts::const_iterator
				it = accounts.begin(),
				it_end = accounts.end();
			it != it_end;
			++it )
			{
				v_accounts.push_back( it->first );
			}

			return v_accounts;
		}

		void s_addSetting( const WString& _setting, const WString& _defaultValue, PyObject* _applyFunc )
		{
			AccountInterface * currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				LOGGER_ERROR(m_serviceProvider)( "Error addSetting: currentAccount is none"
					);

				return;
			}
			
			currentAccount->addSetting( _setting, _defaultValue, _applyFunc );
		}

		void s_changeSetting( const WString& _setting, const WString& _value )
		{
			ACCOUNT_SERVICE(m_serviceProvider)
				->changeSetting( _setting, _value );
		}

        bool s_hasSetting( const WString& _setting )
        {
            AccountInterface * currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
                ->getCurrentAccount();

            if( currentAccount == NULL )
            {
                LOGGER_ERROR(m_serviceProvider)( "getSetting: currentAccount is none [%S]"
                    , _setting.c_str()
                    );

                return false;
            }

            bool value = currentAccount->hasSetting( _setting );
            
            return value;
        }

		PyObject * s_getSetting( const WString& _setting )
		{
			AccountInterface * currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				LOGGER_ERROR(m_serviceProvider)( "getSetting: currentAccount is none [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const WString & value = currentAccount->getSetting( _setting );

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		PyObject * s_getSettingUInt( const WString& _setting )
		{
			AccountInterface * currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				LOGGER_ERROR(m_serviceProvider)( "getSettingUInt: currentAccount is none [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = currentAccount->getSetting( _setting );

			size_t value;
			if( Utils::wstringToUnsigned( setting, value ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)( "getSettingUInt: can't scanf from [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		PyObject * s_getSettingFloat( const WString& _setting )
		{
			AccountInterface * currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				LOGGER_ERROR(m_serviceProvider)( "getSettingFloat: currentAccount is none [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = currentAccount->getSetting( _setting );

			float value;
			if( Utils::wstringToFloat( setting, value ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)( "getSettingFloat: can't scanf from [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		PyObject* s_getAccountSetting( const WString& _accountID, const WString& _setting )
		{
			AccountInterface * account = ACCOUNT_SERVICE(m_serviceProvider)
				->getAccount( _accountID );
						
			if( account == NULL )
			{
				LOGGER_ERROR(m_serviceProvider)( "Error getAccountSetting: Account '%S' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & value = account->getSetting( _setting );

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		PyObject* s_getAccountSettingUInt( const WString& _accountID, const WString& _setting )
		{
			AccountInterface * account = ACCOUNT_SERVICE(m_serviceProvider)
				->getAccount( _accountID );

			if( account == NULL )
			{
				LOGGER_ERROR(m_serviceProvider)( "Error getAccountSetting: Account '%S' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = account->getSetting( _setting );

			size_t value;
			if( Utils::wstringToUnsigned( setting, value ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)( "getSettingUInt: can't scanf from [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		PyObject * s_createAccount()
		{
            AccountInterface * account = ACCOUNT_SERVICE(m_serviceProvider)
                ->createAccount();

            if( account == NULL )
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

		bool s_selectDefaultAccount()
		{
			return ACCOUNT_SERVICE(m_serviceProvider)
				->selectDefaultAccount();
		}
		
		void s_saveAccount()
		{
			AccountInterface * currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				LOGGER_ERROR(m_serviceProvider)( "Error saveCurrentAccount: currentAccount is none"
					);

				return;
			}

			currentAccount->save();
		}
	
		void s_saveAccounts()
		{
			ACCOUNT_SERVICE(m_serviceProvider)
				->saveAccounts();
		}

		void s_saveAccountsInfo()
		{
			ACCOUNT_SERVICE(m_serviceProvider)
				->saveAccountsInfo();
		}

		void s_deleteAccount( const WString& _accountName )
		{
			ACCOUNT_SERVICE(m_serviceProvider)
				->deleteAccount( _accountName );
		}
				
		const WString & s_getCurrentAccountName()
		{
			AccountInterface * currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				LOGGER_ERROR(m_serviceProvider)( "Error getCurrentAccountName: currentAccount is none"
					);

				return Utils::emptyWString();
			}

			const WString & name = currentAccount->getName();

			return name;
		}

		bool s_writeAccountBinaryFile( const WString & _filename, const TBlobject & _data )
		{
            String utf8_filename;
            if( Helper::unicodeToUtf8( m_serviceProvider, _filename, utf8_filename ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)( "s_writeAccountBinaryFile: invalid file %ls convert to utf8"                    
                    , _filename.c_str()
                    );

                return false;                     
            }

            AccountInterface * currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == NULL )
			{                
				LOGGER_ERROR(m_serviceProvider)( "s_writeAccountBinaryFile: invalid write file %ls (currentAccount is none)"
                    , _filename.c_str()
					);

				return false;
			}

            FilePath filepath = Helper::stringizeString( m_serviceProvider, utf8_filename );
            
            if( currentAccount->writeBinaryFile( filepath, _data ) == false )
            {
                const WString & accountName = currentAccount->getName();

                LOGGER_ERROR(m_serviceProvider)( "s_writeAccountBinaryFile: account %ls invalid write file %ls"
                    , accountName.c_str()
                    , _filename.c_str()
                    );
                
                return false;
            }
      
			return true;
		}

		PyObject * s_loadAccountBinaryFile( const WString & _filename )
		{
            String utf8_filename;
            if( Helper::unicodeToUtf8( m_serviceProvider, _filename, utf8_filename ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)( "s_loadAccountBinaryFile: invalid convert filename %ls to utf8"
                    , _filename.c_str()
                    );

                return pybind::ret_none();
            }

            AccountInterface * currentAccount = ACCOUNT_SERVICE(m_serviceProvider)
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				LOGGER_ERROR(m_serviceProvider)( "s_loadAccountBinaryFile: invalid load file %ls (currentAccount is none)"
                    , _filename.c_str()
					);

				return pybind::ret_none();
			}

            FilePath filename = Helper::stringizeString( m_serviceProvider, utf8_filename );

            TBlobject blob;
            if( currentAccount->loadBinaryFile( filename, blob ) == false )
            {
                const WString & accountName = currentAccount->getName();

                LOGGER_ERROR(m_serviceProvider)( "s_loadAccountBinaryFile: account %ls invalid load file %ls"
                    , accountName.c_str()
                    , _filename.c_str()
                    );

                return pybind::ret_none();
            }

	
            PyObject * py_data = pybind::string_from_char_size( reinterpret_cast<char *>(&blob[0]), blob.size() );

			return py_data;
		}

		void s_setParticlesEnabled( bool _enable )
		{
			APPLICATION_SERVICE(m_serviceProvider)
				->setParticlesEnabled( _enable );
		}

		WString s_getTextByKey( const ConstString& _key )
		{
			const TextEntry & entry = TEXT_SERVICE(m_serviceProvider)
				->getTextEntry( _key );

			return entry.text;
		}

		size_t s_getTextCharCountByKey( const ConstString& _key )
		{
			const TextEntry & entry = TEXT_SERVICE(m_serviceProvider)
				->getTextEntry( _key );

			//size_t count;

			//Application::get()
				//->utf8Count( entry.text, count );

			return entry.text.size();
		}

		void s_setVSync( bool _vSync )
		{
			APPLICATION_SERVICE(m_serviceProvider)
				->setVSync( _vSync );
		}

		bool s_getVSync()
		{
			return APPLICATION_SERVICE(m_serviceProvider)
				->getVSync();
		}

		void s_setCursorMode( bool _mode )
		{
			APPLICATION_SERVICE(m_serviceProvider)
				->setCursorMode( _mode );
		}

		bool s_getCursorMode()
		{
			return APPLICATION_SERVICE(m_serviceProvider)
				->getCursorMode();
		}

		bool s_setCursorIcon(const ConstString & _resourceName)
		{
			APPLICATION_SERVICE(m_serviceProvider)
				->setCursorIcon( _resourceName );

			return true;
		}

	    class TimingInterpolatorLinearVector
		    : public TimingListener
		    //, public Eventable
	    {
	    public:
		    TimingInterpolatorLinearVector( ServiceProviderInterface * _serviceProvider, float _time, const mt::vec2f & _from, const mt::vec2f & _to, PyObject * _cb )
			    : m_serviceProvider(_serviceProvider)
                , m_cb(_cb)
		    {
			    pybind::incref(m_cb);

			    m_interpolator.start( _from, _to, _time, &mt::length_v2 );
		    }

		    ~TimingInterpolatorLinearVector()
		    {
			    pybind::decref(m_cb);
		    }

	    protected:
		    bool updateTiming( size_t _id, float _timing ) override
		    {
			    mt::vec2f out;
			    bool done = m_interpolator.update( _timing, &out );
			
			    PyObject * py_out = pybind::ptr(out);

			    EVENT_SERVICE(m_serviceProvider)
				    ->addEventFormat( EVENT_TIMING, m_cb, "(iOO)", _id, py_out, pybind::get_bool(done) );

			    pybind::decref( py_out );

			    return done;
		    }

		    void removeTiming( size_t _id ) override
		    {			
			    EVENT_SERVICE(m_serviceProvider)
				    ->addEventFormat( EVENT_TIMING, m_cb, "(iOO)", _id, pybind::get_none(), pybind::get_bool(false) );
		    }

            void deleteTimingListener() const override
            {
                delete this;
            }

	    protected:
            ServiceProviderInterface * m_serviceProvider;

		    PyObject * m_cb;

		    ValueInterpolatorLinear<mt::vec2f> m_interpolator;		
	    };

	    size_t addInterpolatorLinearVector( float _time, const mt::vec2f & _from, const mt::vec2f & _to, PyObject * _cb )
	    {
		    TimingManagerInterface * timingManager = PLAYER_SERVICE(m_serviceProvider)
			    ->getTimingManager();

		    TimingListener * timing =
			    new TimingInterpolatorLinearVector( m_serviceProvider, _time, _from, _to, _cb );

		    size_t id = timingManager->timing( false, false, 0.f, timing );

		    return id;
	    }

	    size_t addGlobalInterpolatorLinearVector( float _time, const mt::vec2f & _from, const mt::vec2f & _to, PyObject * _cb )
	    {
		    TimingManagerInterface * timingManager = PLAYER_SERVICE(m_serviceProvider)
			    ->getTimingManager();

		    TimingListener * timing =
			    new TimingInterpolatorLinearVector( m_serviceProvider, _time, _from, _to, _cb );

		    size_t id = timingManager->timing( false, true, 0.f, timing );

		    return id;
	    }

	    class TimingInterpolatorLinearFloat
		    : public TimingListener
	    {
	    public:
		    TimingInterpolatorLinearFloat( ServiceProviderInterface * _serviceProvider, float _time, float _from, float _to, PyObject * _cb )
			    : m_serviceProvider(_serviceProvider)
                , m_cb(_cb)
		    {
			    pybind::incref(m_cb);
			    m_interpolator.start( _from, _to, _time, &fabsf );
		    }

		    ~TimingInterpolatorLinearFloat()
		    {
			    pybind::decref(m_cb);
		    }

	    protected:
		    bool updateTiming( size_t _id, float _timing ) override
		    {
			    float out;
			    bool done = m_interpolator.update( _timing, &out );

			    EVENT_SERVICE(m_serviceProvider)
				    ->addEventFormat( EVENT_TIMING, m_cb, "(ifO)", _id, out, pybind::get_bool(done), pybind::get_bool(false) );

			    return done;
		    }

		    void removeTiming( size_t _id ) override
		    {			
			    EVENT_SERVICE(m_serviceProvider)
				    ->addEventFormat( EVENT_TIMING, m_cb, "(iOO)", _id, pybind::get_none(), pybind::get_bool(false) );
		    }

            void deleteTimingListener() const override
            {
                delete this;
            }

	    protected:
            ServiceProviderInterface * m_serviceProvider;

		    PyObject * m_cb;

		    ValueInterpolatorLinear<float> m_interpolator;
	    };

	    size_t addInterpolatorLinearFloat( float _time, float _from, float _to, PyObject * _cb )
	    {
		    TimingManagerInterface * timingManager = PLAYER_SERVICE(m_serviceProvider)
			    ->getTimingManager();

		    TimingListener * timing =
			    new TimingInterpolatorLinearFloat( m_serviceProvider, _time, _from, _to, _cb );

		    size_t id = timingManager->timing( false, false, 0.f, timing );

		    return id;
	    }

	    size_t addGlobalInterpolatorLinearFloat( float _time, float _from, float _to, PyObject * _cb )
	    {
		    TimingManagerInterface * timingManager = PLAYER_SERVICE(m_serviceProvider)
			    ->getTimingManager();

		    TimingListener * timing =
			    new TimingInterpolatorLinearFloat( m_serviceProvider, _time, _from, _to, _cb );

		    size_t id = timingManager->timing( false, true, 0.f, timing );

		    return id;
	    }

	    void removeTiming( size_t id )
	    {
		    TimingManagerInterface * timingManager = PLAYER_SERVICE(m_serviceProvider)
			    ->getTimingManager();

		    timingManager->remove( id );
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

        pybind::def_functor( "addInterpolatorLinearVector", helperScriptMethod, &HelperScriptMethod::addInterpolatorLinearVector );
		pybind::def_functor( "addInterpolatorLinearFloat", helperScriptMethod, &HelperScriptMethod::addInterpolatorLinearFloat);

		pybind::def_functor( "addGlobalInterpolatorLinearVector", helperScriptMethod, &HelperScriptMethod::addGlobalInterpolatorLinearVector );
		pybind::def_functor( "addGlobalInterpolatorLinearFloat", helperScriptMethod, &HelperScriptMethod::addGlobalInterpolatorLinearFloat);		

		pybind::def_functor( "removeTiming", helperScriptMethod, &HelperScriptMethod::removeTiming );

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
        pybind::def_functor( "norm_v2", helperScriptMethod, &HelperScriptMethod::mt_norm_v2 );
		
		
		pybind::def_functor( "angle_correct_interpolate_from_to", helperScriptMethod, &HelperScriptMethod::s_angle_correct_interpolate_from_to );

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

		pybind::def_functor( "getSettingUInt", helperScriptMethod, &HelperScriptMethod::s_getSettingUInt );
		pybind::def_functor( "getSettingFloat", helperScriptMethod, &HelperScriptMethod::s_getSettingFloat );
		

		pybind::def_functor( "changeSetting", helperScriptMethod, &HelperScriptMethod::s_changeSetting );
		

		pybind::def_functor( "getAccountSetting", helperScriptMethod, &HelperScriptMethod::s_getAccountSetting );
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
		pybind::def_functor( "selectDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_selectDefaultAccount );		

        pybind::def_functor( "writeUserBinaryFile", helperScriptMethod, &HelperScriptMethod::s_writeAccountBinaryFile );
        pybind::def_functor( "loadUserBinaryFile", helperScriptMethod, &HelperScriptMethod::s_loadAccountBinaryFile );


		pybind::def_functor( "setParticlesEnabled", helperScriptMethod, &HelperScriptMethod::s_setParticlesEnabled );

		//pybind::def_function( "unicode", &ScriptHelper::s_unicode );
		//pybind::def_function( "ansi", &ScriptHelper::s_ansi );

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

		pybind::def_functor( "intersectsBoxes", helperScriptMethod, &HelperScriptMethod::s_intersectsBoxes );

		pybind::def_functor( "getPolygonPoints", helperScriptMethod, &HelperScriptMethod::s_getPolygonPoints );


		pybind::def_functor( "objects", helperScriptMethod, &HelperScriptMethod::s_objects );
		
		pybind::def_functor( "watchdog", helperScriptMethod, &HelperScriptMethod::s_watchdog );

        pybind::def_functor( "is_class", helperScriptMethod, &HelperScriptMethod::s_is_class );
        pybind::def_functor( "addGlobalModule", helperScriptMethod, &HelperScriptMethod::s_addGlobalModule );

        pybind::def_function( "angle_norm", &mt::angle_norm );
        pybind::def_function( "angle_delta_deg", &mt::angle_delta_deg );

        pybind::def_function( "length_v2_v2", &mt::length_v2_v2 );
        pybind::def_function( "sqrlength_v2_v2", &mt::sqrlength_v2_v2 );


        pybind::def_function( "signed_angle", &mt::signed_angle );
        pybind::def_function( "angle_length", &mt::angle_length );
        pybind::def_function( "perp", &mt::perp );
        pybind::def_function( "perp_left", &mt::perp_left );

        pybind::def_function( "getrefcount", &pybind::refcount );
        pybind::def_function( "decref", &pybind::decref );
        pybind::def_function( "is_wrap", &pybind::is_wrap );

        pybind::def_functor( "addText", helperScriptMethod, &HelperScriptMethod::s_addText );
        pybind::def_functor( "getLanguagePack", helperScriptMethod, &HelperScriptMethod::s_getLanguagePack );

        pybind::def_functor( "isValidWindowMode", helperScriptMethod, &HelperScriptMethod::s_isValidWindowMode );

        pybind::def_functor( "utf8ToUnicode", helperScriptMethod, &HelperScriptMethod::s_utf8ToUnicode );
        pybind::def_functor( "unicodeToUtf8", helperScriptMethod, &HelperScriptMethod::s_unicodeToUtf8 );
	}
}
