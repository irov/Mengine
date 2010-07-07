#	include "Config/Typedef.h"

#	include "ScriptWrapper.h"

#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/rand.h"
#	include "Math/polygon.h"
#	include "Core/ColourValue.h"

#	include "Account.h"
#	include "Game.h"
#	include "Application.h"
#	include "FileEngine.h"

#	include <ctime>
#	include <sstream>
#	include <iomanip>
#	include "TextManager.h"

#	include "TaskLoadPak.h"
#	include "TaskManager.h"
#	include "ResourceImage.h"
#	include "ResourceManager.h"
#	include "AccountManager.h"

#	include "Utils/Core/File.h"
#	include "Utils/Core/String.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ScriptHelper
	{
	public:
		static int mt_randint( int a, int b )
		{	
			return mt::range_rand( a, b );
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

		static float getVec2fX( const mt::vec2f & _vec2f )
		{
			return _vec2f.x;
		}

		static float getVec2fY( const mt::vec2f & _vec2f )
		{
			return _vec2f.y;
		}

		static float getVec3fX( const mt::vec3f & _vec3f )
		{
			return _vec3f.x;
		}

		static float getVec3fY( const mt::vec3f & _vec3f )
		{
			return _vec3f.y;
		}

		static float getVec3fZ( const mt::vec3f & _vec3f )
		{
			return _vec3f.z;
		}

		static float getVec4fX( const mt::vec4f & _vec )
		{
			return _vec.x;
		}

		static float getVec4fY( const mt::vec4f & _vec )
		{
			return _vec.y;
		}

		static float getVec4fZ( const mt::vec4f & _vec )
		{
			return _vec.z;
		}

		static float getVec4fW( const mt::vec4f & _vec )
		{
			return _vec.w;
		}

		static PyObject * getPolygonPoints( const mt::polygon & _p )
		{
			PyObject * py_list = pybind::list_new(0);

			const mt::TVectorPoints & v = _p.get_points();

			for( mt::TVectorPoints::const_iterator 
				it = v.begin(),
				it_end = v.end();
			it != it_end;
			++it )
			{
				pybind::list_appenditem( py_list, pybind::ptr(*it) );
			}

			return py_list;
		}

		static mt::vec2f getViewportBegin( const Viewport & _v )
		{
			return _v.begin;
		}

		static mt::vec2f getViewportEnd( const Viewport & _v )
		{
			return _v.end;
		}

		static float getA( const ColourValue & _color )
		{
			return _color.getA();
		}

		static float getR( const ColourValue & _color )
		{
			return _color.getR();
		}

		static float getG( const ColourValue & _color )
		{
			return _color.getG();
		}

		static float getB( const ColourValue & _color )
		{
			return _color.getB();
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

		static void s_addSetting( const String& _setting, const String& _defaultValue, PyObject* _applyFunc )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			currentAccount->addSetting( _setting, _defaultValue, _applyFunc );
		}

		static void s_changeSetting( const String& _setting, const String& _value )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			if( currentAccount != NULL )
			{
				currentAccount->changeSetting( _setting, _value );
			}
		}

		static const String& s_getSetting( const String& _setting )
		{
			Account* currentAccount = AccountManager::get()
				->getCurrentAccount();

			return currentAccount->getSetting( _setting );
		}

		static PyObject* s_getAccountSetting( const String& _accountID, const String& _setting )
		{
			Account* account = AccountManager::get()
				->getAccount( _accountID );
			
			String setting;
			if( account != NULL )
			{
				setting = account->getSetting( _setting );
			}

			PyObject* uSetting = PyUnicode_DecodeUTF8( setting.c_str(), setting.length(), NULL );

			return uSetting;
		}

		static String s_createAccount()
		{
			return AccountManager::get()
				->createNewAccount();
		}

		static void s_selectAccount( const String& _accountID )
		{
			AccountManager::get()
				->selectAccount( _accountID );
		}

		static void s_saveAccount( const String& _accountID )
		{
			AccountManager::get()
				->saveAccount( _accountID );
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

			return currentAccount->getFolder();
		}

		static void s_setParticlesEnabled( bool _enable )
		{
			Application::get()->setParticlesEnabled( _enable );
		}

		static PyObject* s_unicode( const String& _string )
		{
			return pybind::ptr( Utils::AToW( _string ) );
		}

		static String s_getTextByKey( const ConstString& _key )
		{
			return TextManager::get()->getTextEntry( _key ).text;
		}

		static void s_loadPak( const ConstString& _pakName, PyObject* _doneCallback )
		{
			TaskLoadPak* task = new TaskLoadPak( _pakName, _doneCallback );
			TaskManager::get()
				->addTask( task );
		}

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

		static void s_setAsScreensaver( bool _set )
		{
			return Application::get()
				->setAsScreensaver( _set );
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//REGISTER_SCRIPT_CLASS( Menge, ScriptHelper, Base )
	void ScriptWrapper::helperWrap()
	{
		//srand( (unsigned)std::time( NULL ) );

		pybind::def( "randint", &ScriptHelper::mt_randint );
		pybind::def( "sqrtf", &ScriptHelper::mt_sqrtf );
		pybind::def( "absf", &ScriptHelper::mt_absf );
		pybind::def( "cosf", &ScriptHelper::mt_cosf );
		pybind::def( "sinf", &ScriptHelper::mt_sinf );
		pybind::def( "tanf", &ScriptHelper::mt_tanf );
		pybind::def( "acosf", &ScriptHelper::mt_acosf );
		pybind::def( "atanf", &ScriptHelper::mt_atanf );

		pybind::def( "getVec2fX", &ScriptHelper::getVec2fX );
		pybind::def( "getVec2fY", &ScriptHelper::getVec2fY );


		pybind::def( "getVec3fX", &ScriptHelper::getVec3fX );
		pybind::def( "getVec3fY", &ScriptHelper::getVec3fY );
		pybind::def( "getVec3fZ", &ScriptHelper::getVec3fZ );

		pybind::def( "getVec4fX", &ScriptHelper::getVec4fX );
		pybind::def( "getVec4fY", &ScriptHelper::getVec4fY );
		pybind::def( "getVec4fZ", &ScriptHelper::getVec4fZ );
		pybind::def( "getVec4fW", &ScriptHelper::getVec4fW );

		pybind::def( "getViewportBegin", &ScriptHelper::getViewportBegin );
		pybind::def( "getViewportEnd", &ScriptHelper::getViewportEnd );
			
		pybind::def( "getA", &ScriptHelper::getA );
		pybind::def( "getR", &ScriptHelper::getR );
		pybind::def( "getG", &ScriptHelper::getG );
		pybind::def( "getB", &ScriptHelper::getB );

		pybind::def( "getTimeString", &ScriptHelper::s_getTimeString );

		pybind::def( "addSetting", &ScriptHelper::s_addSetting );
		pybind::def( "changeSetting", &ScriptHelper::s_changeSetting );
		pybind::def( "getSetting", &ScriptHelper::s_getSetting );
		pybind::def( "getAccountSetting", &ScriptHelper::s_getAccountSetting );
		pybind::def( "createAccount", &ScriptHelper::s_createAccount );
		pybind::def( "selectAccount", &ScriptHelper::s_selectAccount );
		pybind::def( "deleteAccount", &ScriptHelper::s_deleteAccount );
		pybind::def( "saveAccount", &ScriptHelper::s_saveAccount );
		pybind::def( "saveAccounts", &ScriptHelper::s_saveAccounts );
		pybind::def( "saveAccountsInfo", &ScriptHelper::s_saveAccountsInfo );
		//pybind::def( "getDataPath", &ScriptHelper::s_getDataPath );
		pybind::def( "getCurrentAccountName", &ScriptHelper::s_getCurrentAccountName );

		pybind::def( "setParticlesEnabled", &ScriptHelper::s_setParticlesEnabled );

		pybind::def( "unicode", &ScriptHelper::s_unicode );
		pybind::def( "getTextByKey", &ScriptHelper::s_getTextByKey );
		pybind::def( "loadPak", &ScriptHelper::s_loadPak );
		pybind::def( "getImageCount", &ScriptHelper::s_getImageCount );
		pybind::def( "setVSync", &ScriptHelper::s_setVSync );
		pybind::def( "getVSync", &ScriptHelper::s_getVSync );
		pybind::def( "setCursorMode", &ScriptHelper::s_setCursorMode );
		pybind::def( "getCursorMode", &ScriptHelper::s_getCursorMode );
		pybind::def( "setAsScreensaver", &ScriptHelper::s_setAsScreensaver );
	}
}
