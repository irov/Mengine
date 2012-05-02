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
#	include "ResourceSequence.h"

#	include "Player.h"
#	include "TimingManager.h"

#	include "Consts.h"

#	include <ctime>
#	include <sstream>
#	include <iomanip>
#	include "TextManager.h"

#	include "ThreadTaskLoadPak.h"
#	include "ThreadTaskManager.h"
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

#	ifdef PYBIND_VISIT_OBJECTS
			MyObjectVisits mov(py_list);
			pybind::visit_objects(&mov);
#	endif

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

		static unsigned int s_getTime()
		{
			std::time_t ctTime; 
			std::time(&ctTime);

			return static_cast<unsigned int>(ctTime);
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

		static TVectorWString s_getAccounts()
		{
			const TMapAccounts & accounts = AccountManager::get()
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

		static void s_addSetting( const WString& _setting, const WString& _defaultValue, PyObject* _applyFunc )
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

		static void s_changeSetting( const WString& _setting, const WString& _value )
		{
			AccountManager::get()
				->changeSetting( _setting, _value );
		}

		static PyObject * s_getSetting( const WString& _setting )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "getSetting: currentAccount is none [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const WString & value = currentAccount->getSetting( _setting );

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		static PyObject * s_getSettingUInt( const WString& _setting )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "getSettingUInt: currentAccount is none [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = currentAccount->getSetting( _setting );

			unsigned int value;
			if( swscanf( setting.c_str(), L"%u", &value ) != 1 )
			{
				MENGE_LOG_ERROR( "getSettingUInt: can't scanf from [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		static PyObject * s_getSettingFloat( const WString& _setting )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "getSettingFloat: currentAccount is none [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = currentAccount->getSetting( _setting );

			float value;
			if( swscanf( setting.c_str(), L"%f", &value ) != 1 )
			{
				MENGE_LOG_ERROR( "getSettingFloat: can't scanf from [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		static PyObject* s_getAccountSetting( const WString& _accountID, const WString& _setting )
		{
			Account* account = AccountManager::get()
				->getAccount( _accountID );
						
			if( account == NULL )
			{
				MENGE_LOG_ERROR( "Error getAccountSetting: Account '%S' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & value = account->getSetting( _setting );

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		static PyObject* s_getAccountSettingUInt( const WString& _accountID, const WString& _setting )
		{
			Account* account = AccountManager::get()
				->getAccount( _accountID );

			if( account == NULL )
			{
				MENGE_LOG_ERROR( "Error getAccountSetting: Account '%S' is none"
					, _accountID.c_str()
					);

				return pybind::ret_none();
			}

			const WString & setting = account->getSetting( _setting );

			PyObject* py_setting = pybind::ptr( setting );

			unsigned int value;
			if( swscanf( setting.c_str(), L"%u", &value ) != 1 )
			{
				MENGE_LOG_ERROR( "getSettingUInt: can't scanf from [%S]"
					, _setting.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_value = pybind::ptr( value );

			return py_value;
		}

		static WString s_createAccount()
		{
			WString accountId = AccountManager::get()
				->createAccount(); 

			return accountId;
		}

		static void s_selectAccount( const WString& _accountID )
		{
			AccountManager::get()
				->selectAccount( _accountID );
		}
		
		static bool s_hasCurrentAccount()
		{
			return AccountManager::get()
				->hasCurrentAccount();
		}

		static void s_setDefaultAccount( const WString & _accountID )
		{
			return AccountManager::get()
				->setDefaultAccount( _accountID );
		}

		static const WString & s_getDefaultAccount()
		{
			return AccountManager::get()
				->getDefaultAccount();
		}

		static bool s_hasDefaultAccount()
		{
			return AccountManager::get()
				->hasDefaultAccount();
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

		static void s_deleteAccount( const WString& _accountName )
		{
			AccountManager::get()
				->deleteAccount( _accountName );
		}
				
		static const WString & s_getCurrentAccountName()
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "Error getCurrentAccountName: currentAccount is none"
					);

				return Utils::emptyWString();
			}

			const WString & name = currentAccount->getName();

			return name;
		}

		static bool s_writeBinaryFile( const WString & _filename, const String & _data )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "Error writeBinaryFile: currentAccount is none"
					);

				return false;
			}

			FileOutputStreamInterface * file = FileEngine::get()
				->createOutputFile( Consts::get()->c_user );

			if( file == 0 )
			{
				return false;
			}

			const WString & folder = currentAccount->getName();

			WString fullpath = folder + MENGE_FOLDER_DELIM + _filename;

			if( file->open( fullpath ) == false )
			{
				file->close();

				return false;
			}

			file->write( _data.c_str(), _data.size() );
			file->close();

			return true;
		}

		static String s_loadBinaryFile( const WString & _filename )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			String data;

			if( currentAccount == NULL )
			{
				MENGE_LOG_ERROR( "loadBinaryFile: currentAccount is none"
					);

				return data;
			}

			FileInputStreamInterface * file = FileEngine::get()
				->createInputFile( Consts::get()->c_user );

			if( file == 0 )
			{
				MENGE_LOG_ERROR( "loadBinaryFile: invalid create input file '%S'"
					, _filename.c_str()
					);

				return data;
			}

			const WString & folder = currentAccount->getName();

			WString fullpath = folder + MENGE_FOLDER_DELIM + _filename;
			
			if( file->open( fullpath ) == false )
			{
				MENGE_LOG_ERROR( "loadBinaryFile: invalid open input file '%S'"
					, _filename.c_str()
					);
				
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

		//static PyObject* s_unicode( const String& _string )
		//{
		//	PlatformInterface * platform = Application::get()
		//		->getPlatform();

		//	WString unicode = platform->ansiToUnicode( _string );

		//	return pybind::ptr( unicode );
		//}
		//
		//static PyObject* s_ansi( const WString& _string )
		//{
		//	PlatformInterface * platform = Application::get()
		//		->getPlatform();
		//		
		//	String ansi = platform->unicodeToAnsi( _string );

		//	return pybind::ptr( ansi );
		//}

		static WString s_getTextByKey( const ConstString& _key )
		{
			const TextEntry & entry = TextManager::get()
				->getTextEntry( _key );

			return entry.text;
		}

		static size_t s_getTextCharCountByKey( const ConstString& _key )
		{
			const TextEntry & entry = TextManager::get()
				->getTextEntry( _key );

			//size_t count;

			//Application::get()
				//->utf8Count( entry.text, count );

			return entry.text.size();
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

		/*static size_t s_getImageCount( const ConstString & _resourceName )
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

			size_t count = resImage->getCount();

			resImage->decrementReference();

			return count;
		}*/

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
				MENGE_LOG_ERROR( "Error: can't set cursor icon, no find resource '%s'"
					, _resourceName.c_str() 
					);

				return false;
			}

			ResourceCursorICO * resource = ResourceManager::get()
				->getResourceT<ResourceCursorICO>(_resourceName);

			const ConstString & resourceCategory = resource->getCategory();
			const WString & resourcePath = resource->getPath();

			static WString path;
			path.c_str();

			if( FileEngine::get()
				->getFullPath( resourceCategory, resourcePath, path ) == false )
			{
				MENGE_LOG_ERROR( "Error: can't set cursor icon getFullPath '%s' '%S'"
					, resourceCategory.c_str()
					, resourcePath.c_str()
					);

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
			if( ResourceManager::get()->hasResource( _name ) == true )
			{
				return true;
			}

			if( pybind::list_check(_sequence) == false )
			{
				return false;
			}

			size_t sequnce_size = pybind::list_size(_sequence);

			TVectorAnimationSequence sequence;

			for( size_t it = 0; it != sequnce_size; ++it )
			{
				PyObject * py_element = pybind::list_getitem( _sequence, it );

				if( pybind::tuple_check(py_element) == false )
				{
					continue;
				}

				PyObject * py_delay = pybind::tuple_getitem( py_element, 0 );
				PyObject * py_resourceName = pybind::tuple_getitem( py_element, 1 );

				AnimationSequence seq;
				seq.delay = pybind::extract<float>(py_delay);
				seq.resourceName = pybind::extract<ConstString>(py_resourceName);

				sequence.push_back(seq);
			}

			ResourceAnimation * resource = ResourceManager::get()
				->createResourceT<ResourceAnimation>(Consts::get()->c_builtin_empty, Consts::get()->c_builtin_empty, _name, Consts::get()->c_ResourceSequence);

			if( resource == NULL )
			{
				return false;
			}

			resource->setSequences( sequence );

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
		bool update( size_t _id, float _timing ) override
		{
			mt::vec2f out;
			bool done = m_interpolator.update( _timing, &out );


			PyObject * py_out = pybind::ptr(out);

			EventManager::get()
				->addEventFormat( EVENT_TIMING, m_cb, "(iOO)", _id, py_out, pybind::get_bool(done) );

			pybind::decref( py_out );

			return done;
		}

		void remove( size_t _id ) override
		{			
			EventManager::get()
				->addEventFormat( EVENT_TIMING, m_cb, "(iOO)", _id, pybind::get_none(), pybind::get_bool(false) );
		}

	protected:
		PyObject * m_cb;

		ValueInterpolatorLinear<mt::vec2f> m_interpolator;		
	};

	static size_t addInterpolatorLinearVector( float _time, const mt::vec2f & _from, const mt::vec2f & _to, PyObject * _cb )
	{
		TimingManager * timingManager = Player::get()
			->getTimingManager();

		TimingListener * timing =
			new TimingInterpolatorLinearVector( _time, _from, _to, _cb );

		size_t id = timingManager->timing( false, false, 0.f, timing );

		return id;
	}

	static size_t addGlobalInterpolatorLinearVector( float _time, const mt::vec2f & _from, const mt::vec2f & _to, PyObject * _cb )
	{
		TimingManager * timingManager = Player::get()
			->getTimingManager();

		TimingListener * timing =
			new TimingInterpolatorLinearVector( _time, _from, _to, _cb );

		size_t id = timingManager->timing( false, true, 0.f, timing );

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
		bool update( size_t _id, float _timing ) override
		{
			float out;
			bool done = m_interpolator.update( _timing, &out );

			EventManager::get()
				->addEventFormat( EVENT_TIMING, m_cb, "(ifO)", _id, out, pybind::get_bool(done), pybind::get_bool(false) );

			return done;
		}

		void remove( size_t _id ) override
		{			
			EventManager::get()
				->addEventFormat( EVENT_TIMING, m_cb, "(iOO)", _id, pybind::get_none(), pybind::get_bool(false) );
		}

	protected:
		PyObject * m_cb;

		ValueInterpolatorLinear<float> m_interpolator;
	};

	static size_t addInterpolatorLinearFloat( float _time, float _from, float _to, PyObject * _cb )
	{
		TimingManager * timingManager = Player::get()
			->getTimingManager();

		TimingListener * timing =
			new TimingInterpolatorLinearFloat( _time, _from, _to, _cb );

		size_t id = timingManager->timing( false, false, 0.f, timing );

		return id;
	}

	static size_t addGlobalInterpolatorLinearFloat( float _time, float _from, float _to, PyObject * _cb )
	{
		TimingManager * timingManager = Player::get()
			->getTimingManager();

		TimingListener * timing =
			new TimingInterpolatorLinearFloat( _time, _from, _to, _cb );

		size_t id = timingManager->timing( false, true, 0.f, timing );

		return id;
	}

	static void removeTiming( size_t id )
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

		pybind::def_function( "addInterpolatorLinearVector", &addInterpolatorLinearVector );
		pybind::def_function( "addInterpolatorLinearFloat", &addInterpolatorLinearFloat);

		pybind::def_function( "addGlobalInterpolatorLinearVector", &addGlobalInterpolatorLinearVector );
		pybind::def_function( "addGlobalInterpolatorLinearFloat", &addGlobalInterpolatorLinearFloat);		

		pybind::def_function( "removeTiming", &removeTiming );

		pybind::def_function( "rand", &ScriptHelper::mt_rand );
		pybind::def_function( "randf", &ScriptHelper::mt_randf );
		pybind::def_function( "range_rand", &ScriptHelper::mt_range_rand );
		pybind::def_function( "range_randf", &ScriptHelper::mt_range_randf );
		
		pybind::def_function( "sqrtf", &ScriptHelper::mt_sqrtf );
		pybind::def_function( "absf", &ScriptHelper::mt_absf );
		pybind::def_function( "cosf", &ScriptHelper::mt_cosf );
		pybind::def_function( "sinf", &ScriptHelper::mt_sinf );
		pybind::def_function( "tanf", &ScriptHelper::mt_tanf );
		pybind::def_function( "acosf", &ScriptHelper::mt_acosf );
		pybind::def_function( "atanf", &ScriptHelper::mt_atanf );

		pybind::def_function( "direction", &ScriptHelper::mt_direction );
		pybind::def_function( "angle_norm", &mt::angle_norm );
		

		pybind::def_function( "length_v2_v2", &mt::length_v2_v2 );
		pybind::def_function( "sqrlength_v2_v2", &mt::sqrlength_v2_v2 );
		
		pybind::def_function( "norm_v2", &ScriptHelper::mt_norm_v2 );
		pybind::def_function( "signed_angle", &mt::signed_angle );
		pybind::def_function( "angle_length", &mt::angle_length );
		pybind::def_function( "perp", &mt::perp );
		pybind::def_function( "perp_left", &mt::perp_left );

		pybind::def_function( "angle_correct_interpolate_from_to", &ScriptHelper::s_angle_correct_interpolate_from_to );

		pybind::def_function( "projectionPointToLine", &ScriptHelper::projectionPointToLine );

		pybind::def_function( "isPointInsidePolygon", &ScriptHelper::s_isPointInsidePolygon );

		pybind::def_function( "getTime", &ScriptHelper::s_getTime );
		pybind::def_function( "getTimeString", &ScriptHelper::s_getTimeString );

		pybind::def_function( "getAccounts", &ScriptHelper::s_getAccounts );
		pybind::def_function( "addSetting", &ScriptHelper::s_addSetting );
		pybind::def_function( "getSetting", &ScriptHelper::s_getSetting );
		pybind::def_function( "getSettingUInt", &ScriptHelper::s_getSettingUInt );
		pybind::def_function( "getSettingFloat", &ScriptHelper::s_getSettingFloat );
		

		pybind::def_function( "changeSetting", &ScriptHelper::s_changeSetting );
		

		pybind::def_function( "getAccountSetting", &ScriptHelper::s_getAccountSetting );
		pybind::def_function( "getAccountSettingUInt", &ScriptHelper::s_getAccountSettingUInt );
		
				

		pybind::def_function( "createAccount", &ScriptHelper::s_createAccount );
		pybind::def_function( "selectAccount", &ScriptHelper::s_selectAccount );
		pybind::def_function( "deleteAccount", &ScriptHelper::s_deleteAccount );
				
		pybind::def_function( "saveAccount", &ScriptHelper::s_saveAccount );
		pybind::def_function( "saveAccounts", &ScriptHelper::s_saveAccounts );
		pybind::def_function( "saveAccountsInfo", &ScriptHelper::s_saveAccountsInfo );
		//pybind::def_function( "getDataPath", &ScriptHelper::s_getDataPath );
		pybind::def_function( "hasCurrentAccount", &ScriptHelper::s_hasCurrentAccount );
		pybind::def_function( "getCurrentAccountName", &ScriptHelper::s_getCurrentAccountName );

		pybind::def_function( "setDefaultAccount", &ScriptHelper::s_setDefaultAccount );
		pybind::def_function( "getDefaultAccount", &ScriptHelper::s_getDefaultAccount );
		pybind::def_function( "hasDefaultAccount", &ScriptHelper::s_hasDefaultAccount );		
		pybind::def_function( "selectDefaultAccount", &ScriptHelper::s_selectDefaultAccount );		

		pybind::def_function( "writeBinaryFile", &ScriptHelper::s_writeBinaryFile );
		pybind::def_function( "loadBinaryFile", &ScriptHelper::s_loadBinaryFile );


		pybind::def_function( "setParticlesEnabled", &ScriptHelper::s_setParticlesEnabled );

		//pybind::def_function( "unicode", &ScriptHelper::s_unicode );
		//pybind::def_function( "ansi", &ScriptHelper::s_ansi );

		pybind::def_function( "getTextByKey", &ScriptHelper::s_getTextByKey );
		pybind::def_function( "getTextCharCountByKey", &ScriptHelper::s_getTextCharCountByKey );
		

		//pybind::def_function( "loadPak", &ScriptHelper::s_loadPak );
		//pybind::def_function( "getImageCount", &ScriptHelper::s_getImageCount );
		pybind::def_function( "setVSync", &ScriptHelper::s_setVSync );
		pybind::def_function( "getVSync", &ScriptHelper::s_getVSync );
		pybind::def_function( "setCursorMode", &ScriptHelper::s_setCursorMode );
		pybind::def_function( "getCursorMode", &ScriptHelper::s_getCursorMode );
		pybind::def_function( "setCursorIcon", &ScriptHelper::s_setCursorIcon );
		pybind::def_function( "setAsScreensaver", &ScriptHelper::s_setAsScreensaver );

		pybind::def_function( "importEntity", &ScriptHelper::s_importEntity );

		pybind::def_function( "createAnimationSequence", &ScriptHelper::s_createAnimationSequence );

		pybind::def_function( "intersectsBoxes", &ScriptHelper::s_intersectsBoxes );

		pybind::def_function( "getPolygonPoints", &ScriptHelper::s_getPolygonPoints );

		pybind::def_function( "getrefcount", &pybind::refcount );
		pybind::def_function( "decref", &pybind::decref );
		pybind::def_function( "is_wrap", &pybind::is_wrap );

		pybind::def_function( "objects", &ScriptHelper::s_objects );
	}
}
