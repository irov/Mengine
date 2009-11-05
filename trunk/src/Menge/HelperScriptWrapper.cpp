#	include "Config/Typedef.h"

#	include "ScriptWrapper.h"

#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/rand.h"
#	include "ColourValue.h"

#	include "Account.h"
#	include "Game.h"
#	include "Application.h"
#	include "FileEngine.h"

#	include <ctime>
#	include <sstream>
#	include <iomanip>
#	include "Menge/Utils.h"
#	include "TextManager.h"

#	include "TaskLoadPak.h"
#	include "TaskManager.h"
#	include "ResourceImage.h"
#	include "ResourceManager.h"

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
			Account* currentAccount = Holder<Game>::hostage()->getCurrentAccount();
			currentAccount->addSetting( _setting, _defaultValue, _applyFunc );
		}

		static void s_changeSetting( const String& _setting, const String& _value )
		{
			Account* currentAccount = Holder<Game>::hostage()->getCurrentAccount();
			currentAccount->changeSetting( _setting, _value );
		}

		static const String& s_getSetting( const String& _setting )
		{
			Account* currentAccount = Holder<Game>::hostage()->getCurrentAccount();
			return currentAccount->getSetting( _setting );
		}

		static PyObject* s_createAccount( const String& _accountName )
		{
			Holder<Game>::hostage()->createAccount( _accountName );
			PyObject* uName = PyUnicode_DecodeUTF8( _accountName.c_str(), _accountName.length(), NULL );
			return uName;
		}

		static void s_selectAccount( const String& _accountName )
		{
			Holder<Game>::hostage()->selectAccount( _accountName );
		}

		static void s_saveAccount( const String& _accountName )
		{
			Holder<Game>::hostage()->saveAccount( _accountName );
		}
	
		static void s_saveAccounts()
		{
			Holder<Game>::hostage()->saveAccounts();
		}

		static void s_saveAccountsInfo()
		{
			Holder<Game>::hostage()->saveAccountsInfo();
		}

		static void s_deleteAccount( const String& _accountName )
		{
			Holder<Game>::hostage()->deleteAccount( _accountName );
		}

		static const String& s_getCurrentAccountName()
		{
			Account* currentAccount = Holder<Game>::hostage()->getCurrentAccount();
			return currentAccount->getName();
		}

		//static String s_getDataPath()
		//{
		//	String path = Holder<FileEngine>::hostage()->getAppDataPath();
		//	Account* currentAccount = Holder<Game>::hostage()->getCurrentAccount();
		//	return path + "\\" + currentAccount->getName();
		//}

		static void s_setParticlesEnabled( bool _enable )
		{
			Holder<Application>::hostage()->setParticlesEnabled( _enable );
		}

		static PyObject* s_unicode( const String& _string )
		{
			return pybind::ptr( Utils::AToW( _string ) );
		}

		static String s_getTextByKey( const String& _key )
		{
			return Holder<TextManager>::hostage()->getTextEntry( _key ).text;
		}

		static void s_loadPak( const String& _pakName, PyObject* _doneCallback )
		{
			TaskLoadPak* task = new TaskLoadPak( _pakName, _doneCallback );
			Holder<TaskManager>::hostage()
				->addTask( task );
		}

		static std::size_t s_getImageCount( const String& _resourceName )
		{
			ResourceImage* resImage = Holder<ResourceManager>::hostage()
										->getResourceT<ResourceImage>( _resourceName );
			if( resImage == NULL )
			{
				MENGE_LOG_ERROR( "Error: Image resource not getting '%s'"
					, _resourceName.c_str() 
					);

				return 0;
			}
			return resImage->getCount();
		}

		static void s_setVSync( bool _vSync )
		{
			Application::hostage()
				->setVSync( _vSync );
		}

		static bool s_getVSync()
		{
			return Application::hostage()
						->getVSync();
		}

		static void s_setCursorMode( bool _mode )
		{
			Application::hostage()
				->setCursorMode( _mode );
		}

		static bool s_getCursorMode()
		{
			return Application::hostage()
					->getCursorMode();
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

		pybind::def( "getA", &ScriptHelper::getA );
		pybind::def( "getR", &ScriptHelper::getR );
		pybind::def( "getG", &ScriptHelper::getG );
		pybind::def( "getB", &ScriptHelper::getB );

		pybind::def( "getTimeString", &ScriptHelper::s_getTimeString );

		pybind::def( "addSetting", &ScriptHelper::s_addSetting );
		pybind::def( "changeSetting", &ScriptHelper::s_changeSetting );
		pybind::def( "getSetting", &ScriptHelper::s_getSetting );
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
	}
}
