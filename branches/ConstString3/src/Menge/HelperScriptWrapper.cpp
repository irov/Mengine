#	include "Config/Typedef.h"

#	include "ScriptWrapper.h"

#	include "Node.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/rand.h"

#	include "Core/Polygon.h"
#	include "Core/ColourValue.h"

#	include "Account.h"
#	include "Game.h"
#	include "Application.h"
#	include "FileEngine.h"
#	include "EntityManager.h"

#	include "ResourceAnimation.h"

#	include "Player.h"
#	include "TimingManager.h"

#	include "Consts.h"

#	include <ctime>
#	include <sstream>
#	include <iomanip>
#	include "TextManager.h"

#	include "TaskLoadPak.h"
#	include "TaskManager.h"
#	include "ResourceImage.h"
#	include "ResourceManager.h"
#	include "ResourceCursorICO.h"
#	include "AccountManager.h"

#	include "Affector.h"

#	include "Utils/Core/File.h"
#	include "Utils/Core/String.h"

#	include "Math/angle.h"

#	include "ScriptEngine.h"
#	include "EventManager.h"
#	include "TimingManager.h"

#	include "boost/geometry/geometries/box.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ScriptHelper
	{
	public:
		static int mt_rand( int a )
		{	
			return mt::rand( a );
		}

		static int mt_randf( float a )
		{	
			return (int)mt::randf( a );
		}

		static int mt_range_rand( int a, int b )
		{	
			return mt::range_rand( a, b );
		}

		static int mt_range_randf( float a, float b )
		{	
			return (int)mt::range_randf( a, b );
		}

		static float mt_sqrtf( float a )
		{
			return sqrtf(a);
		}

		static float mt_absf( float a )
		{
			return fabsf(a);
		}

		static float mt_cosf( float a )
		{
			return cosf(a);
		}

		static float mt_sinf( float a )
		{
			return sinf(a);
		}

		static float mt_tanf( float a )
		{
			return tanf( a );
		}

		static float mt_acosf( float a )
		{
			return acosf( a );
		}

		static float mt_atanf( float _x )
		{
			return ::atanf( _x );
		}

		static mt::vec2f mt_direction( const mt::vec2f & _from, const mt::vec2f & _to )
		{
			mt::vec2f direction;
			sub_v2_v2(direction, _to, _from);

			mt::vec2f direction_n;
			norm_v2(direction_n, direction);

			return direction_n;
		}

		static mt::vec2f mt_norm_v2( const mt::vec2f & _vec )
		{
			mt::vec2f v_n;
			mt::norm_v2( v_n, _vec );

			return v_n;
		}

		static bool s_intersectsBoxes( const mt::vec2f & _minFirst, const mt::vec2f & _maxFirst, const mt::vec2f & _minSecond, const mt::vec2f & _maxSecond )
		{
			typedef boost::geometry::model::box<mt::vec2f> Box;
			Box box_first(_minFirst, _maxFirst);
			Box box_second(_minSecond, _maxSecond);

			bool result = boost::geometry::intersects(box_first, box_second);

			return result;
		}

		static mt::vec2f projectionPointToLine( const mt::vec2f & _point, const mt::vec2f & _v0, const mt::vec2f & _v1 )
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

		static PyObject * s_angle_correct_interpolate_from_to( float _from, float _to )
		{
			float correct_angle_from;
			float correct_angle_to;

			mt::angle_correct_interpolate_from_to( _from, _to, correct_angle_from, correct_angle_to );

			PyObject * py_result = pybind::build_value("(ff)", correct_angle_from, correct_angle_to);

			return py_result;
		}

		static PyObject * s_getPolygonPoints( const Polygon & _polygon )
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

		static PyObject * s_objects()
		{
			PyObject * py_list = pybind::list_new(0);

			MyObjectVisits mov(py_list);

			pybind::visit_objects(&mov);

			return py_list;
		}

		static bool s_isPointInsidePolygon( const mt::vec2f & _point, const Polygon & _polygon )
		{
			Polygon point_polygon;
			boost::geometry::append(point_polygon, _point);
			boost::geometry::correct(point_polygon);

			Polygon correct_polygon(_polygon);
			boost::geometry::correct(correct_polygon);
			
			bool result = boost::geometry::intersects(point_polygon, correct_polygon);

			return result;
		}

		static String s_getTimeString()
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

		static TVectorString s_getAccounts()
		{
			const TMapAccounts & accounts = AccountManager::get()
				->getAccounts();

			TVectorString v_accounts;

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

		static void s_addSetting( const String& _setting, const String& _defaultValue, PyObject* _applyFunc )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "Error addSetting: currentAccount is none"
					);

				return;
			}
			
			currentAccount->addSetting( _setting, _defaultValue, _applyFunc );
		}

		static void s_changeSetting( const String& _setting, const String& _value )
		{
			AccountManager::get()
				->changeSetting( _setting, _value );
		}

		static const String& s_getSetting( const String& _setting )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "Error getSetting: currentAccount is none"
					);

				static String empty;
				return empty;
			}

			return currentAccount->getSetting( _setting );
		}

		static PyObject* s_getAccountSetting( const String& _accountID, const String& _setting )
		{
			Account* account = AccountManager::get()
				->getAccount( _accountID );
			
			
			if( account == NULL )
			{
				MENGE_LOG_ERROR( "Error getAccountSetting: Account %s is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const String & setting = account->getSetting( _setting );

			PyObject* py_setting = pybind::string_from_char( setting.c_str(), setting.length() );

			return py_setting;
		}


		static PyObject* s_getAccountSettingU( const String& _accountID, const String& _setting )
		{
			Account* account = AccountManager::get()
				->getAccount( _accountID );


			if( account == NULL )
			{
				MENGE_LOG_ERROR( "Error getAccountSetting: Account %s is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const String & settingU = account->getSettingU( _setting );

			PyObject* py_settingU = pybind::unicode_from_utf8( settingU.c_str(), settingU.length() );

			return py_settingU;
		}
		

		static String s_createAccount()
		{
			return AccountManager::get()
				->createAccount();
		}

		static void s_selectAccount( const String& _accountID )
		{
			AccountManager::get()
				->selectAccount( _accountID );
		}
		
		static bool s_hasCurrentAccount()
		{
			return AccountManager::get()
				->hasCurrentAccount();
		}

		static void s_setDefaultAccount( const String & _accountID )
		{
			return AccountManager::get()
				->setDefaultAccount( _accountID );
		}

		static const String & s_getDefaultAccount()
		{
			return AccountManager::get()
				->getDefaultAccount();
		}

		static bool s_selectDefaultAccount()
		{
			return AccountManager::get()
				->selectDefaultAccount();
		}
		
		static void s_saveAccount()
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "Error saveCurrentAccount: currentAccount is none"
					);

				return;
			}

			currentAccount->save();
		}
	
		static void s_saveAccounts()
		{
			AccountManager::get()
				->saveAccounts();
		}

		static void s_saveAccountsInfo()
		{
			AccountManager::get()
				->saveAccountsInfo();
		}

		static void s_deleteAccount( const String& _accountName )
		{
			AccountManager::get()
				->deleteAccount( _accountName );
		}
				
		static const ConstString & s_getCurrentAccountName()
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "Error getCurrentAccountName: currentAccount is none"
					);

				return Consts::get()->c_builtin_empty;
			}

			return currentAccount->getFolder();
		}

		static bool s_writeBinaryFile( const String & _filename, const String & _data )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "Error writeBinaryFile: currentAccount is none"
					);

				return false;
			}

			FileOutputInterface * file = FileEngine::get()
				->createOutputFile( Consts::get()->c_user );

			if( file == 0 )
			{
				return false;
			}

			const ConstString & folder = currentAccount->getFolder();

			String fullpath = Helper::to_str(folder) + "/" + _filename;

			if( file->open( fullpath ) == false )
			{
				file->close();

				return false;
			}

			file->write( _data.c_str(), _data.size() );
			file->close();

			return true;
		}

		static String s_loadBinaryFile( const String & _filename )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			String data;

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "Error loadBinaryFile: currentAccount is none"
					);

				return data;
			}

			FileInputInterface * file = FileEngine::get()
				->createInputFile( Consts::get()->c_user );

			if( file == 0 )
			{
				return data;
			}

			const ConstString & folder = currentAccount->getFolder();

			String fullpath = Helper::to_str(folder) + "/" + _filename;
			
			if( file->open( fullpath ) == false )
			{
				file->close();

				return data;
			}

			int size = file->size();
			
			if( size < 0 )
			{
				file->close();

				return data;
			}

			data.resize(size);
			file->read( &data[0], size );
			
			file->close();

			return data;
		}


		static void s_setParticlesEnabled( bool _enable )
		{
			Application::get()
				->setParticlesEnabled( _enable );
		}

		static PyObject* s_unicode( const String& _string )
		{
			return pybind::ptr( Utils::AToW( _string ) );
		}

		static String s_getTextByKey( const ConstString& _key )
		{
			const TextEntry & entry = TextManager::get()
				->getTextEntry( _key );

			return entry.text;
		}

		//static void s_loadPak( const ConstString& _pakName, PyObject* _doneCallback )
		//{
		//	Game::get()
		//		->getResourcePak(_pakName);

		//	TaskLoadPak* task = new TaskLoadPak( _pakName, _doneCallback );
		//	
		//	TaskManager::get()
		//		->addTask( task );
		//}

		static std::size_t s_getImageCount( const ConstString & _resourceName )
		{
			ResourceImage* resImage = ResourceManager::get()
				->getResourceT<ResourceImage>( _resourceName );

			if( resImage == NULL )
			{
				MENGE_LOG_ERROR( "Error: Image resource not getting '%s'"
					, _resourceName.c_str() 
					);

				return 0;
			}

			std::size_t count = resImage->getCount();

			ResourceManager::get()
				->releaseResource( resImage );

			return count;
		}

		static void s_setVSync( bool _vSync )
		{
			Application::get()
				->setVSync( _vSync );
		}

		static bool s_getVSync()
		{
			return Application::get()
						->getVSync();
		}

		static void s_setCursorMode( bool _mode )
		{
			Application::get()
				->setCursorMode( _mode );
		}

		static bool s_getCursorMode()
		{
			return Application::get()
					->getCursorMode();
		}

		static bool s_setCursorIcon(const ConstString & _resourceName)
		{
			if( ResourceManager::get()
				->hasResource( _resourceName ) == false )
			{
				MENGE_LOG_ERROR( "Error: can't set cursor icon '%s'"
					, _resourceName.c_str() 
					);

				return false;
			}

			ResourceCursorICO * resource = ResourceManager::get()
				->getResourceT<ResourceCursorICO>(_resourceName);

			const ConstString & resourceCategory = resource->getCategory();
			const ConstString & resourcePath = resource->getPath();

			String path;
			if( FileEngine::get()
				->getFullPath( resourceCategory, resourcePath, path ) == false )
			{
				return false;
			}

			Application::get()
				->setCursorIcon( path );

			return true;
		}

		static void s_setAsScreensaver( bool _set )
		{
			return Application::get()
				->setAsScreensaver( _set );
		}

		static bool s_createAnimationSequence( const ConstString & _name, PyObject * _sequence )
		{
			if( ResourceManager::get()->hasResource(_name) == true )
			{
				ResourceManager::get()
					->directResourceRelease(_name);
			}

			if( pybind::list_check(_sequence) == false )
			{
				return false;
			}

			size_t sequnce_size = pybind::list_size(_sequence);

			TVectorAnimationSequence animSequence;

			for( size_t it = 0; it != sequnce_size; ++it )
			{
				PyObject * py_element = pybind::list_getitem( _sequence, it );

				if( pybind::tuple_check(py_element) == false )
				{
					continue;
				}

				PyObject * py_delay = pybind::tuple_getitem( py_element, 0 );
				PyObject * py_index = pybind::tuple_getitem( py_element, 1 );

				AnimationSequence seq;
				seq.delay = pybind::extract<float>(py_delay);
				seq.index = pybind::extract<std::size_t>(py_index);

				animSequence.push_back(seq);
			}

			ResourceAnimation * resource = ResourceManager::get()
				->createResourceT<ResourceAnimation>(Consts::get()->c_builtin_empty, Consts::get()->c_builtin_empty, _name, Consts::get()->c_ResourceAnimation);

			if( resource == NULL )
			{
				return false;
			}

			resource->setSequences( animSequence );

			return true;
		}

		static PyObject * s_importEntity( const String & _type )
		{
			ResourceDesc desc;

			ConstString c_type(_type);

			if( EntityManager::get()
				->getPrototypeDesc( c_type, desc ) == false )
			{
				return pybind::ret_none();
			}

			bool exist = false;

			PyObject * module = ScriptEngine::get()
				->importPrototype( c_type, Consts::get()->c_Entity, desc.pak, desc.path, exist );

			if( exist == false )
			{
				MENGE_LOG_ERROR( "Error: importEntity can't import prototype '%s'(escape module)"
					, _type.c_str() 
					);

				return 0;
			}

			if( module == 0 )
			{
				MENGE_LOG_ERROR( "Error: importEntity can't import prototype '%s'(invalid module)"
					, _type.c_str() 
					);

				return 0;
			}

			pybind::incref(module);

			return module;
		}
	};

	class TimingInterpolatorLinearVector
		: public TimingListener
		, public Eventable
	{
	public:
		TimingInterpolatorLinearVector( float _time, const mt::vec2f & _from, const mt::vec2f & _to, PyObject * _cb )
			: m_cb(_cb)
		{
			pybind::incref(m_cb);

			m_interpolator.start( _from, _to, _time, &mt::length_v2 );
		}

		~TimingInterpolatorLinearVector()
		{
			pybind::decref(m_cb);
		}

	protected:
		bool update( std::size_t _id, float _timing ) override
		{
			mt::vec2f out;
			bool done = m_interpolator.update( _timing, &out );


			PyObject * py_out = pybind::ptr(out);
			EventManager::get()
				->addEventFormat( EVENT_TIMING, m_cb, "(iO)", _id, py_out );

			pybind::decref( py_out );

			return done;
		}

	protected:
		PyObject * m_cb;

		ValueInterpolatorLinear<mt::vec2f> m_interpolator;		
	};

	static std::size_t addInterpolatorLinearVector( float _time, const mt::vec2f & _from, const mt::vec2f & _to, PyObject * _cb )
	{
		TimingManager * timingManager = Player::get()
			->getTimingManager();

		TimingListener * timing =
			new TimingInterpolatorLinearVector( _time, _from, _to, _cb );

		std::size_t id = timingManager->timing( false, false, 0.f, timing );

		return id;
	}

	static std::size_t addGlobalInterpolatorLinearVector( float _time, const mt::vec2f & _from, const mt::vec2f & _to, PyObject * _cb )
	{
		TimingManager * timingManager = Player::get()
			->getTimingManager();

		TimingListener * timing =
			new TimingInterpolatorLinearVector( _time, _from, _to, _cb );

		std::size_t id = timingManager->timing( false, true, 0.f, timing );

		return id;
	}

	class TimingInterpolatorLinearFloat
		: public TimingListener
		, public Eventable
	{
	public:
		TimingInterpolatorLinearFloat( float _time, float _from, float _to, PyObject * _cb )
			: m_cb(_cb)
		{
			pybind::incref(m_cb);
			m_interpolator.start( _from, _to, _time, &fabsf );
		}

		~TimingInterpolatorLinearFloat()
		{
			pybind::decref(m_cb);
		}

	protected:
		bool update( std::size_t _id, float _timing ) override
		{
			float out;
			bool done = m_interpolator.update( _timing, &out );

			EventManager::get()
				->addEventFormat( EVENT_TIMING, m_cb, "(ifO)", _id, out, pybind::get_bool(done) );

			return done;
		}

	protected:
		PyObject * m_cb;

		ValueInterpolatorLinear<float> m_interpolator;
	};

	static std::size_t addInterpolatorLinearFloat( float _time, float _from, float _to, PyObject * _cb )
	{
		TimingManager * timingManager = Player::get()
			->getTimingManager();

		TimingListener * timing =
			new TimingInterpolatorLinearFloat( _time, _from, _to, _cb );

		std::size_t id = timingManager->timing( false, false, 0.f, timing );

		return id;
	}

	static std::size_t addGlobalInterpolatorLinearFloat( float _time, float _from, float _to, PyObject * _cb )
	{
		TimingManager * timingManager = Player::get()
			->getTimingManager();

		TimingListener * timing =
			new TimingInterpolatorLinearFloat( _time, _from, _to, _cb );

		std::size_t id = timingManager->timing( false, true, 0.f, timing );

		return id;
	}

	static void removeTiming( std::size_t id )
	{
		TimingManager * timingManager = Player::get()
			->getTimingManager();

		timingManager->remove( id );
	}

	//////////////////////////////////////////////////////////////////////////
	//REGISTER_SCRIPT_CLASS( Menge, ScriptHelper, Base )
	void ScriptWrapper::helperWrap()
	{
		//srand( (unsigned)std::time( NULL ) );

		pybind::def( "addInterpolatorLinearVector", &addInterpolatorLinearVector );
		pybind::def( "addInterpolatorLinearFloat", &addInterpolatorLinearFloat);

		pybind::def( "addGlobalInterpolatorLinearVector", &addGlobalInterpolatorLinearVector );
		pybind::def( "addGlobalInterpolatorLinearFloat", &addGlobalInterpolatorLinearFloat);		

		pybind::def( "removeTiming", &removeTiming );

		pybind::def( "rand", &ScriptHelper::mt_rand );
		pybind::def( "randf", &ScriptHelper::mt_randf );
		pybind::def( "range_rand", &ScriptHelper::mt_range_rand );
		pybind::def( "range_randf", &ScriptHelper::mt_range_randf );
		
		pybind::def( "sqrtf", &ScriptHelper::mt_sqrtf );
		pybind::def( "absf", &ScriptHelper::mt_absf );
		pybind::def( "cosf", &ScriptHelper::mt_cosf );
		pybind::def( "sinf", &ScriptHelper::mt_sinf );
		pybind::def( "tanf", &ScriptHelper::mt_tanf );
		pybind::def( "acosf", &ScriptHelper::mt_acosf );
		pybind::def( "atanf", &ScriptHelper::mt_atanf );

		pybind::def( "direction", &ScriptHelper::mt_direction );
		pybind::def( "angle_norm", &mt::angle_norm );
		

		pybind::def( "length_v2_v2", &mt::length_v2_v2 );
		pybind::def( "sqrlength_v2_v2", &mt::sqrlength_v2_v2 );
		
		pybind::def( "norm_v2", &ScriptHelper::mt_norm_v2 );
		pybind::def( "signed_angle", &mt::signed_angle );
		pybind::def( "angle_length", &mt::angle_length );
		pybind::def( "perp", &mt::perp );
		pybind::def( "perp_left", &mt::perp_left );

		pybind::def( "angle_correct_interpolate_from_to", &ScriptHelper::s_angle_correct_interpolate_from_to );

		pybind::def( "projectionPointToLine", &ScriptHelper::projectionPointToLine );

		pybind::def( "isPointInsidePolygon", &ScriptHelper::s_isPointInsidePolygon );

		pybind::def( "getTimeString", &ScriptHelper::s_getTimeString );

		pybind::def( "getAccounts", &ScriptHelper::s_getAccounts );
		pybind::def( "addSetting", &ScriptHelper::s_addSetting );
		pybind::def( "getSetting", &ScriptHelper::s_getSetting );
		pybind::def( "changeSetting", &ScriptHelper::s_changeSetting );
		

		pybind::def( "getAccountSetting", &ScriptHelper::s_getAccountSetting );
		pybind::def( "getAccountSettingU", &ScriptHelper::s_getAccountSettingU );
				

		pybind::def( "createAccount", &ScriptHelper::s_createAccount );
		pybind::def( "selectAccount", &ScriptHelper::s_selectAccount );
		pybind::def( "deleteAccount", &ScriptHelper::s_deleteAccount );
				
		pybind::def( "saveAccount", &ScriptHelper::s_saveAccount );
		pybind::def( "saveAccounts", &ScriptHelper::s_saveAccounts );
		pybind::def( "saveAccountsInfo", &ScriptHelper::s_saveAccountsInfo );
		//pybind::def( "getDataPath", &ScriptHelper::s_getDataPath );
		pybind::def( "hasCurrentAccount", &ScriptHelper::s_hasCurrentAccount );
		pybind::def( "getCurrentAccountName", &ScriptHelper::s_getCurrentAccountName );

		pybind::def( "setDefaultAccount", &ScriptHelper::s_setDefaultAccount );
		pybind::def( "getDefaultAccount", &ScriptHelper::s_getDefaultAccount );
		pybind::def( "selectDefaultAccount", &ScriptHelper::s_selectDefaultAccount );		

		pybind::def( "writeBinaryFile", &ScriptHelper::s_writeBinaryFile );
		pybind::def( "loadBinaryFile", &ScriptHelper::s_loadBinaryFile );


		pybind::def( "setParticlesEnabled", &ScriptHelper::s_setParticlesEnabled );

		pybind::def( "unicode", &ScriptHelper::s_unicode );
		pybind::def( "getTextByKey", &ScriptHelper::s_getTextByKey );
		//pybind::def( "loadPak", &ScriptHelper::s_loadPak );
		pybind::def( "getImageCount", &ScriptHelper::s_getImageCount );
		pybind::def( "setVSync", &ScriptHelper::s_setVSync );
		pybind::def( "getVSync", &ScriptHelper::s_getVSync );
		pybind::def( "setCursorMode", &ScriptHelper::s_setCursorMode );
		pybind::def( "getCursorMode", &ScriptHelper::s_getCursorMode );
		pybind::def( "setCursorIcon", &ScriptHelper::s_setCursorIcon );
		pybind::def( "setAsScreensaver", &ScriptHelper::s_setAsScreensaver );

		pybind::def( "importEntity", &ScriptHelper::s_importEntity );

		pybind::def( "createAnimationSequence", &ScriptHelper::s_createAnimationSequence );

		pybind::def( "intersectsBoxes", &ScriptHelper::s_intersectsBoxes );

		pybind::def( "getPolygonPoints", &ScriptHelper::s_getPolygonPoints );

		pybind::def( "getrefcount", &pybind::refcount );
		pybind::def( "decref", &pybind::decref );

		pybind::def( "objects", &ScriptHelper::s_objects );
	}
}
