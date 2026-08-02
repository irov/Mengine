#include "HelperScriptEmbedding.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/UserdataServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/WatchdogServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/ChronometerServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PackageServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/StatisticServiceInterface.h"
#include "Interface/DateTimeSystemInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/EnvironmentServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocument.h"
#include "Environment/Python/PythonCallbackProvider.h"
#include "Environment/Python/PythonTraceback.h"

#if defined(MENGINE_PLATFORM_ANDROID)
#   include "Environment/Android/AndroidKernelServiceInterface.h"
#   include "Environment/Android/AndroidPlatformServiceExtensionInterface.h"
#endif

#include "Engine/HotSpotPolygon.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Node.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/WriteHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/ReadHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TagsHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/StringHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Base64.h"
#include "Kernel/Crash.h"
#include "Kernel/Optional.h"
#include "Kernel/FilePathDateTimeHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/Polygon.h"
#include "Kernel/PolygonHelper.h"
#include "Kernel/Color.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/UID.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/ResolutionHelper.h"
#include "Kernel/Data.h"
#include "Kernel/Stringstream.h"
#include "Kernel/Logger.h"
#include "Kernel/ResourceImageSequence.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/VectorResourceImages.h"
#include "Kernel/CRC32.h"
#include "Kernel/BuildMode.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/StreamHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/EnumeratorHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/StringView.h"
#include "Kernel/AssertionCompilable.h"
#include "Kernel/URLString.h"

#include "Config/StdString.h"
#include "Config/StdMath.h"
#include "Config/Variant.h"
#include "Config/Path.h"

#include "math/angle.h"
#include "math/vec2.h"
#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Vector<HotSpotPolygonPtr> VectorHotSpotPolygons;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool s_addAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting, PyObject * _defaultValue, const pybind::object & _cb, const pybind::args & _args );
        static bool s_changeAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting, PyObject * _value );
        static bool s_changeAccountSettingBool( const ConstString & _accountId, const ConstString & _setting, bool _value );
        static bool s_changeAccountSettingInt( const ConstString & _accountId, const ConstString & _setting, int32_t _value );
        static bool s_changeAccountSettingUInt( const ConstString & _accountId, const ConstString & _setting, uint32_t _value );
        static bool s_changeAccountSettingUInt64( const ConstString & _accountId, const ConstString & _setting, uint64_t _value );
        static bool s_changeAccountSettingFloat( const ConstString & _accountId, const ConstString & _setting, float _value );
        static bool s_changeAccountSettingStrings( const ConstString & _accountId, const ConstString & _setting, const VectorWString & _values );

        static PyObject * s_getAccountUID( pybind::kernel_interface * _kernel, const ConstString & _accountId );
        static PyObject * s_getAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting );
        static PyObject * s_getAccountSettingBool( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting );
        static PyObject * s_getAccountSettingInt( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting );
        static PyObject * s_getAccountSettingUInt( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting );
        static PyObject * s_getAccountSettingUInt64( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting );
        static PyObject * s_getAccountSettingFloat( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting );
        static PyObject * s_getAccountSettingStrings( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting );
        static float s_getAccountSettingFloatDefault( const ConstString & _accountId, const ConstString & _setting, float _default );
        //////////////////////////////////////////////////////////////////////////
        static uint32_t s_refcount( pybind::kernel_interface * _kernel, PyObject * _obj )
        {
            return _kernel->refcount( _obj );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_is_class( pybind::kernel_interface * _kernel, PyObject * _obj )
        {
            return _kernel->is_class( _obj );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_is_type_class( pybind::kernel_interface * _kernel, PyTypeObject * _type )
        {
            return _kernel->is_type_class( _type );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_is_wrap( pybind::kernel_interface * _kernel, PyObject * _obj )
        {
            return _kernel->is_wrap( _obj );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_setLocale( const ConstString & _locale )
        {
            const Tags & platformTags = PLATFORM_SERVICE()
                ->getPlatformTags();

            if( PACKAGE_SERVICE()
                ->existLocalePackage( _locale, platformTags ) == false )
            {
                LOGGER_ERROR( "not found game localization for language '%s'"
                    , _locale.c_str()
                );

                return false;
            }

            APPLICATION_SERVICE()
                ->setLocale( _locale );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static const ConstString & s_getLocale()
        {
            const ConstString & locale = APPLICATION_SERVICE()
                ->getLocale();

            return locale;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasLocale( const ConstString & _locale )
        {
            const Tags & platformTags = PLATFORM_SERVICE()
                ->getPlatformTags();

            if( PACKAGE_SERVICE()
                ->existLocalePackage( _locale, platformTags ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static VectorConstString s_getLocales()
        {
            const Tags & platformTags = PLATFORM_SERVICE()
                ->getPlatformTags();

            VectorConstString locales;
            PACKAGE_SERVICE()
                ->getLocales( platformTags, &locales );

            return locales;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_isometric_length_v3_v3( const mt::vec3f & _v0, const mt::vec3f & _v1 )
        {
            mt::vec3f iso_v0 = _v0;
            iso_v0.y *= 2.f;

            mt::vec3f iso_v1 = _v1;
            iso_v1.y *= 2.f;

            float iso_length = mt::length_v3_v3( iso_v0, iso_v1 );

            return iso_length;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_isometric_sqrlength_v3_v3( const mt::vec3f & _v0, const mt::vec3f & _v1 )
        {
            mt::vec3f iso_v0 = _v0;
            iso_v0.y *= 2.f;

            mt::vec3f iso_v1 = _v1;
            iso_v1.y *= 2.f;

            float iso_length = mt::sqrlength_v3_v3( iso_v0, iso_v1 );

            return iso_length;
        }
        //////////////////////////////////////////////////////////////////////////
        static WString s_utf8ToUnicode( const String & _utf8 )
        {
            WString unicode;
            Helper::utf8ToUnicode( _utf8, &unicode );

            return unicode;
        }
        //////////////////////////////////////////////////////////////////////////
        static String s_unicodeToUtf8( const WString & _unicode )
        {
            String utf8;
            Helper::unicodeToUtf8( _unicode, &utf8 );

            return utf8;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_debug()
        {
            LOGGER_MESSAGE_RELEASE( "debug!\n" );

#if defined(MENGINE_TOOLCHAIN_MSVC) && !defined(I3D_ARCH_ARM) && !defined(MENGINE_PLATFORM_WINDOWS64)
            _asm int 3;
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_debugOnce()
        {
            static bool once = false;

            if( once == true )
            {
                return;
            }

            once = true;

            LOGGER_MESSAGE_RELEASE( "debug once!\n" );

#if defined(MENGINE_TOOLCHAIN_MSVC) && !defined(I3D_ARCH_ARM) && !defined(MENGINE_PLATFORM_WINDOWS64)
            _asm int 3;
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_debugNode( Node * _node )
        {
            MENGINE_UNUSED( _node );

            LOGGER_MESSAGE_RELEASE( "debug node!\n" );

#if defined(MENGINE_TOOLCHAIN_MSVC) && !defined(I3D_ARCH_ARM) && !defined(MENGINE_PLATFORM_WINDOWS64)
            _asm int 3;
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasGameData( const ConstString & _name )
        {
            bool result = USERDATA_SERVICE()
                ->hasUserdata( _name );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_writeGameData( pybind::kernel_interface * _kernel, const ConstString & _name, PyObject * _data, PyObject * _pickleTypes )
        {
            size_t size;
            if( pybind::pickle( _kernel, _data, _pickleTypes, nullptr, 0, &size ) == false )
            {
                LOGGER_ERROR( "data '%s' invalid pickle [size]"
                    , _name.c_str()
                );

                return false;
            }

            MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_PYTHON );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "data '%s' invalid create buffer"
                , _name.c_str()
            );

            void * memory_buffer = buffer->getBuffer();
            size_t memory_size = buffer->getSize();

            if( pybind::pickle( _kernel, _data, _pickleTypes, memory_buffer, memory_size, &size ) == false )
            {
                LOGGER_ERROR( "data '%s' invalid pickle"
                    , _name.c_str()
                );

                return false;
            }

            if( USERDATA_SERVICE()
                ->writeUserdata( _name, memory_buffer, memory_size ) == false )
            {
                LOGGER_ERROR( "data '%s' invalid write"
                    , _name.c_str()
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_loadGameData( pybind::kernel_interface * _kernel, const ConstString & _name, PyObject * _pickleTypes )
        {
            MemoryInterfacePtr binaryBuffer = USERDATA_SERVICE()
                ->loadUserdata( _name, MENGINE_DOCUMENT_PYTHON );

            MENGINE_ASSERTION_MEMORY_PANIC( binaryBuffer, "data '%s' invalid load"
                , _name.c_str()
            );

            void * binaryBuffer_memory = binaryBuffer->getBuffer();
            size_t binaryBuffer_size = binaryBuffer->getSize();

            PyObject * py_data = pybind::unpickle( _kernel, binaryBuffer_memory, binaryBuffer_size, _pickleTypes );

            MENGINE_ASSERTION_MEMORY_PANIC( py_data, "data '%s' invalid unpickle"
                , _name.c_str()
            );

            return py_data;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isSpecialDown()
        {
            return INPUT_SERVICE()
                ->isSpecialDown();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isAltDown()
        {
            return INPUT_SERVICE()
                ->isAltDown();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isShiftDown()
        {
            return INPUT_SERVICE()
                ->isShiftDown();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isCtrlDown()
        {
            return INPUT_SERVICE()
                ->isControlDown();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isShiftKeyCode( EKeyCode _keyCode )
        {
            return _keyCode == KC_SHIFT || _keyCode == KC_LSHIFT || _keyCode == KC_RSHIFT;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isAltKeyCode( EKeyCode _keyCode )
        {
            return _keyCode == KC_MENU || _keyCode == KC_LMENU || _keyCode == KC_RMENU;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isControlKeyCode( EKeyCode _keyCode )
        {
            return _keyCode == KC_CONTROL || _keyCode == KC_LCONTROL || _keyCode == KC_RCONTROL;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isKeyDown( EKeyCode _keyCode )
        {
            return INPUT_SERVICE()
                ->isKeyDown( _keyCode );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isExclusiveKeyDown( EKeyCode _keyCode )
        {
            return INPUT_SERVICE()
                ->isExclusiveKeyDown( _keyCode );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isAnyKeyDown()
        {
            return INPUT_SERVICE()
                ->isAnyKeyDown();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isAnyMouseButtonDown()
        {
            return INPUT_SERVICE()
                ->isAnyMouseButtonDown();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isMouseButtonDown( EMouseButtonCode _button )
        {
            return INPUT_SERVICE()
                ->isMouseButtonDown( _button );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_printChildren2( const NodePtr & _node, uint32_t _tab )
        {
            _node->foreachChildrenSlug( [_tab]( const NodePtr & _child )
            {
                Color color;

                Node * nodeRender;
                if( RenderInterface * render = Helper::getNodeRenderInheritance( _child.get(), &nodeRender ) )
                {
                    render->calcTotalColor( &color );
                }

                LOGGER_MESSAGE_RELEASE( "%.*s%s%s [%s] (%.0f, %.0f) %.2f\n"
                    , _tab
                    , "                                         "
                    , _child->isActivate() == true ? _child->isEnable() ? "+" : "-" : "#"
                    , _child->getName().c_str()
                    , _child->getType().c_str()
                    , _child->getTransformation()->getWorldPosition().x
                    , _child->getTransformation()->getWorldPosition().y
                    , color.getA()
                );

                s_printChildren2( _child, _tab + 1 );
            } );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_printChildren( const NodePtr & _node )
        {
            s_printChildren2( _node, 0 );
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t s_getGroupResourcesMemoryUse( const ConstString & _group )
        {
            MENGINE_UNUSED( _group );

            return MENGINE_UINT32_C(0);
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_intersectPathVsCircle( const pybind::list & _path, const mt::vec3f & _position, float _radius )
        {
            pybind::list::size_type path_count = _path.size();

            if( path_count < 2 )
            {
                return false;
            }

            float dradius = _radius * _radius;

            for( pybind::list::size_type i = 1; i != path_count; ++i )
            {
                mt::vec3f v0 = _path[i - 1];
                mt::vec3f v1 = _path[i - 0];

                mt::vec3f vt;
                float d = mt::project_to_line_v3_v3( &vt, v0, v1, _position );

                if( d < 0.f )
                {
                    if( mt::sqrlength_v3_v3( v0, _position ) < dradius )
                    {
                        return true;
                    }
                }
                else if( d > 1.f )
                {
                    if( mt::sqrlength_v3_v3( v1, _position ) < dradius )
                    {
                        return true;
                    }
                }
                else
                {
                    if( mt::sqrlength_v3_v3( vt, _position ) < dradius )
                    {
                        return true;
                    }
                }
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::object s_selectRandomPointFromPathRadius( pybind::kernel_interface * _kernel, const pybind::list & _path, const mt::vec3f & _position, float _radius )
        {
            pybind::list::size_type path_count = _path.size();

            float dradius = _radius * _radius;

            Vector<mt::vec3f> points;

            for( pybind::list::size_type i = 0; i != path_count; ++i )
            {
                mt::vec3f v = _path[i];

                if( mt::sqrlength_v3_v3( v, _position ) > dradius )
                {
                    continue;
                }

                points.emplace_back( v );
            }

            if( points.empty() == true )
            {
                return pybind::make_none_t( _kernel );
            }

            uint32_t size = (uint32_t)points.size();

            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            uint32_t index = randomizer->getRandom32( size );

            const mt::vec3f & vr = points[index];

            return pybind::make_object_t( _kernel, vr );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_reloadModule( pybind::kernel_interface * _kernel, PyObject * _module )
        {
            return _kernel->module_reload( _module );
        }
        //////////////////////////////////////////////////////////////////////////
        static String s_makeUID( uint32_t _length )
        {
            MENGINE_ASSERTION_FATAL( _length < 1024, "max UID length equal 1024" );

            Char uid[1024 + 1] = {'\0'};
            Helper::makeUID( _length, uid );

            return String( uid, _length );
        }
        //////////////////////////////////////////////////////////////////////////
        static const String & s_getTextFromId( const ConstString & _textId )
        {
            const TextEntryInterfacePtr & textEntry = TEXT_SERVICE()
                ->getTextEntry( _textId );

            MENGINE_ASSERTION_MEMORY_PANIC( textEntry, "text entry '%s' not found (doc: %s)"
                , _textId.c_str()
                , MENGINE_DOCUMENT_STR( MENGINE_DOCUMENT_PYTHON )
            );

            const String & value = textEntry->getValue();

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_setTextAlias( const ConstString & _aliasEnvironment, const ConstString & _aliasId, const ConstString & _textId )
        {
            TEXT_SERVICE()
                ->setTextAlias( _aliasEnvironment, _aliasId, _textId );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_removeTextAlias( const ConstString & _aliasEnvironment, const ConstString & _aliasId )
        {
            TEXT_SERVICE()
                ->removeTextAlias( _aliasEnvironment, _aliasId );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasTextAlias( const ConstString & _aliasEnvironment, const ConstString & _aliasId )
        {
            bool exist = TEXT_SERVICE()
                ->hasTextAlias( _aliasEnvironment, _aliasId );

            return exist;
        }
        //////////////////////////////////////////////////////////////////////////
        static const ConstString & s_getTextAlias( const ConstString & _aliasEnvironment, const ConstString & _aliasId )
        {
            const ConstString & textId = TEXT_SERVICE()
                ->getTextAlias( _aliasEnvironment, _aliasId );

            return textId;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_setTextAliasArguments( const ConstString & _aliasEnvironment, const ConstString & _aliasId, const pybind::args & _args )
        {
            size_t args_count = _args.size();

            VectorTextArguments arguments;
            arguments.reserve( args_count );

            for( const pybind::object & py_obj : _args )
            {
                if( py_obj.is_string() == true )
                {
                    String value = py_obj.extract();

                    TextArgumentInterfacePtr argument = TEXT_SERVICE()
                        ->createTextArgumentValue( value, MENGINE_DOCUMENT_PYTHON );

                    arguments.emplace_back( argument );
                }
                else if( py_obj.is_unicode() == true )
                {
                    WString value = py_obj.extract();

                    String utf8_value;
                    Helper::unicodeToUtf8( value, &utf8_value );

                    TextArgumentInterfacePtr argument = TEXT_SERVICE()
                        ->createTextArgumentValue( utf8_value, MENGINE_DOCUMENT_PYTHON );

                    arguments.emplace_back( argument );
                }
                else if( py_obj.is_callable() == true )
                {
                    LambdaTextArgumentContext context = [py_obj]( String * _value )
                    {
                        pybind::object new_value = py_obj.call();

                        pybind::string_view new_value_str = new_value.str();

                        if( *_value == new_value_str.c_str() )
                        {
                            return false;
                        }

                        _value->assign( new_value_str.c_str(), new_value_str.size() );

                        return true;
                    };

                    TextArgumentInterfacePtr argument = TEXT_SERVICE()
                        ->createTextArgumentContext( context, MENGINE_DOCUMENT_PYTHON );

                    arguments.emplace_back( argument );
                }
                else if( py_obj.is_dict() == true )
                {
                    pybind::dict py_params = py_obj.extract();

                    ConstString textId = py_params.get_default( "TextId", ConstString::none() );

                    if( textId != ConstString::none() )
                    {
                        TextArgumentInterfacePtr argument = TEXT_SERVICE()
                            ->createTextArgumentId( textId, MENGINE_DOCUMENT_PYTHON );

                        arguments.emplace_back( argument );
                    }
                    else
                    {
                        LOGGER_ERROR( "textfield_setTextFormatArgs '%s' not support params"
                            , py_params.repr().c_str()
                        );

                        return false;
                    }
                }
                else
                {
                    MENGINE_ASSERTION_FATAL( py_obj.is_invalid() == false, "textfield_setTextFormatArgs '%s' not suport arg '%s'"
                        , py_obj.repr().c_str()
                        , _args.repr().c_str()
                    );

                    pybind::string_view value = py_obj.str();

                    const Char * value_str = value.c_str();

                    TextArgumentInterfacePtr argument = TEXT_SERVICE()
                        ->createTextArgumentValue( value_str, MENGINE_DOCUMENT_PYTHON );

                    arguments.emplace_back( argument );
                }
            }

            TEXT_SERVICE()
                ->setTextAliasArguments( _aliasEnvironment, _aliasId, arguments );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_removeTextAliasArguments( const ConstString & _aliasEnvironment, const ConstString & _aliasId )
        {
            TEXT_SERVICE()
                ->removeTextAliasArguments( _aliasEnvironment, _aliasId );
        }
        //////////////////////////////////////////////////////////////////////////
        static ChronometerInterfacePtr s_addChronometer( const pybind::object & _cb, const pybind::args & _args )
        {
            ChronometerInterfacePtr chronometer = CHRONOMETER_SERVICE()
                ->addChronometer( [_cb, _args]( Timestamp _time )
            {
                _cb.call_args( _time, _args );
            }, MENGINE_DOCUMENT_PYTHON );

            return chronometer;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_removeChronometer( const ChronometerInterfacePtr & _chronometer )
        {
            bool successful = CHRONOMETER_SERVICE()
                ->removeChronometer( _chronometer );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::box2f s_getHotSpotPolygonBoundingBox( const HotSpotPolygonPtr & _hotspotPolygon )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _hotspotPolygon, "hotspot is nullptr" );

            mt::box2f bb;
            mt::box2_insideout( &bb );

            const Polygon & polygon = _hotspotPolygon->getPolygon();

            const VectorPoints & points = polygon.getPoints();

            if( points.empty() == true )
            {
                return bb;
            }

            const TransformationInterface * transformation = _hotspotPolygon->getTransformation();

            const mt::mat4f & wm = transformation->getWorldMatrix();

            for( const mt::vec2f & v : points )
            {
                mt::vec2f wmp_it;
                mt::mul_v2_v2_m4( &wmp_it, v, wm );

                mt::box2_add_internal_point( &bb, wmp_it );
            }

            return bb;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasOption( const Char * _option )
        {
            bool exist = OPTIONS_SERVICE()
                ->hasOption( _option, false );

            return exist;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * s_getOptionValue( const Char * _option )
        {
            const Char * value = OPTIONS_SERVICE()
                ->getOptionValue( _option, "" );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list s_getOptionValues( pybind::kernel_interface * _kernel, const Char * _option )
        {
            pybind::list r = pybind::make_list_t( _kernel );

            const Char * option_values[MENGINE_OPTIONS_VALUES_MAX];
            uint32_t option_count;
            if( OPTIONS_SERVICE()
                ->getOptionValues( _option, option_values, &option_count ) == false )
            {
                return r;
            }

            for( uint32_t index = 0; index != option_count; ++index )
            {
                const Char * value = option_values[index];

                r.append( value );
            }

            return r;
        }
        //////////////////////////////////////////////////////////////////////////
        static int32_t s_getOptionInt32( const Char * _option )
        {
            uint32_t value = OPTIONS_SERVICE()
                ->getOptionInt32( _option, 0 );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t s_getOptionUInt32( const Char * _option )
        {
            uint32_t value = OPTIONS_SERVICE()
                ->getOptionUInt32( _option, MENGINE_UINT32_C(0) );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_crashEngine()
        {
            Helper::crash( "python" );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_freezeEngine()
        {
            uint32_t index = 0;
            for( ;;)
            {
                ++index;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_memleakEngine( uint32_t _size, uint32_t _count )
        {
            for( uint32_t i = 0; i != _count; ++i )
            {
                Helper::allocateMemory( _size, "pymemleak" );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static String s_generateUniqueIdentity( uint32_t _length )
        {
            MENGINE_ASSERTION_FATAL( _length < 1024, "max UID length equal 1024" );

            Stringstream ss;

            Char uid[1024 + 1] = {'\0'};
            Helper::makeUID( _length, uid );
            uid[_length] = '\0';

            ss << uid;

            String str = ss.str();

            return str;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_setClipboardText( const Char * _value )
        {
            if( PLATFORM_SERVICE()
                ->setClipboardText( _value ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t s_makeCRC32( const StringView & _data )
        {
            uint32_t crc32 = Helper::makeCRC32( _data.c_str(), _data.size() );

            return crc32;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_compressBase64( pybind::kernel_interface * _kernel, const ConstString & _archivatorType, const StringView & _data )
        {
            const Char * str = _data.c_str();
            size_t size = _data.size();

            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), _archivatorType );

            size_t compressSize2 = archivator->compressBound( size );

            MemoryInputInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryInput( MENGINE_DOCUMENT_PYTHON );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory cache" );

            void * memory_buffer = memory->newBuffer( sizeof( uint32_t ) + sizeof( uint64_t ) + compressSize2 );

            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer, "invalid new memory size '%zu'"
                , compressSize2
            );

            Helper::writeUint32( memory_buffer, MENGINE_FOURCC( 'C', 'B', '6', '4' ) );
            Helper::writeUint64( MENGINE_PVOID_OFFSET( memory_buffer, sizeof( uint32_t ) ), size );

            size_t compressSize;
            if( archivator->compress( MENGINE_PVOID_OFFSET( memory_buffer, sizeof( uint32_t ) + sizeof( uint64_t ) ), compressSize2, str, size, &compressSize, EAC_BEST ) == false )
            {
                return _kernel->ret_none();
            }

            void * new_memory = memory->newBuffer( sizeof( uint32_t ) + sizeof( uint64_t ) + compressSize );
            MENGINE_UNUSED( new_memory );

            MENGINE_ASSERTION_MEMORY_PANIC( new_memory, "invalid new memory '%zu'"
                , compressSize
            );

            const void * memoryBuffer = memory->getBuffer();
            size_t memorySize = memory->getSize();

            size_t base64Size = Helper::getBase64EncodeSize( memorySize );

            MemoryInterfacePtr base64Memory = Helper::createMemoryCacheBuffer( base64Size, MENGINE_DOCUMENT_PYTHON );

            Char * base64MemoryBuffer = base64Memory->getBuffer();
            size_t base64MemorySize = base64Memory->getSize();

            Helper::encodeBase64( memoryBuffer, memorySize, base64MemoryBuffer );

            PyObject * py_base64 = _kernel->string_from_char_size( base64MemoryBuffer, base64MemorySize );

            return py_base64;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_decompressBase64( pybind::kernel_interface * _kernel, const ConstString & _archivatorType, const StringView & _base64 )
        {
            const char * base64String = _base64.c_str();
            size_t base64Size = _base64.size();

            size_t size = Helper::getBase64DecodeSize( base64String, base64Size );

            MemoryInterfacePtr dataMemory = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_PYTHON );

            void * dataMemoryBuffer = dataMemory->getBuffer();
            size_t dataMemorySize = dataMemory->getSize();

            Helper::decodeBase64( base64String, base64Size, dataMemoryBuffer );

            uint32_t magic;
            Helper::readUint32( dataMemoryBuffer, &magic );

            if( magic != MENGINE_FOURCC( 'C', 'B', '6', '4' ) )
            {
                LOGGER_ERROR( "invalid magic '%u'"
                    , magic
                );

                return _kernel->ret_none();
            }

            uint64_t decompressSizeU64;
            Helper::readUint64( MENGINE_PVOID_OFFSET( dataMemoryBuffer, sizeof( uint32_t ) ), &decompressSizeU64 );

            size_t decompressSize = (size_t)decompressSizeU64;

            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), _archivatorType );

            MemoryBufferInterfacePtr decompressMemory = MEMORY_SERVICE()
                ->createMemoryCacheBuffer( MENGINE_DOCUMENT_PYTHON );

            MENGINE_ASSERTION_MEMORY_PANIC( decompressMemory, "invalid create memory cache" );

            void * decompressMemoryBuffer = decompressMemory->newBuffer( decompressSize );

            MENGINE_ASSERTION_MEMORY_PANIC( decompressMemoryBuffer, "invalid get memory '%zu' (binary)"
                , decompressSize
            );

            size_t uncompressSize;
            if( archivator->decompress( decompressMemoryBuffer, decompressSize, MENGINE_PVOID_OFFSET( dataMemoryBuffer, sizeof( uint32_t ) + sizeof( uint64_t ) ), dataMemorySize - sizeof( uint32_t ) - sizeof( uint64_t ), &uncompressSize ) == false )
            {
                LOGGER_ERROR( "invalid decompress buffer" );

                return _kernel->ret_none();
            }

            PyObject * py_base64 = _kernel->string_from_char_size( (char *)decompressMemoryBuffer, uncompressSize );

            return py_base64;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getClipboardText( pybind::kernel_interface * _kernel )
        {
            Char value[1024 + 1] = {'\0'};
            if( PLATFORM_SERVICE()
                ->getClipboardText( value, 1024 ) == false )
            {
                return _kernel->ret_none();
            }

            PyObject * py_value = _kernel->ptr_string( value );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_ANDROID)
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getAndroidId( pybind::kernel_interface * _kernel )
        {
            AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
                ->getUnknown();

            Char androidId[128 + 1] = {'\0'};
            size_t androidIdLen = platformExtension->androidNativeGetAndroidId( androidId, 128 );

            return _kernel->string_from_char_size( androidId, androidIdLen );
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
        static void s_setCursorPosition( const mt::vec2f & _cursorPosition )
        {
            const Resolution & contentResolution = APPLICATION_SERVICE()
                ->getContentResolution();

            const Resolution & currentResolution = APPLICATION_SERVICE()
                ->getCurrentWindowResolution();

            mt::vec2f adaptPosition;
            adaptPosition.x = _cursorPosition.x / contentResolution.getWidth() * currentResolution.getWidth();
            adaptPosition.y = _cursorPosition.y / contentResolution.getHeight() * currentResolution.getHeight();

            PLATFORM_SERVICE()
                ->setCursorPosition( adaptPosition );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isValidWindowMode()
        {
            //bool check = APPLICATION_SERVICE()
                //->isValidWindowMode();

            //return check;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_addGlobalModule( const Char * _name, PyObject * _module )
        {
            SCRIPT_SERVICE()
                ->addGlobalModule( _name, _module );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_removeGlobalModule( const Char * _name )
        {
            SCRIPT_SERVICE()
                ->removeGlobalModule( _name );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_log( pybind::kernel_interface * _kernel, const StringView & _message, ELoggerLevel _level, uint32_t _color, uint32_t _flag )
        {
            ELoggerLevel verboseLevel = LOGGER_SERVICE()
                ->getVerboseLevel();

            if( verboseLevel < _level )
            {
                return;
            }

            Path filename = {'\0'};
            Char function[MENGINE_CODE_MAX_FUNCTION_NAME + 1] = {'\0'};
            uint32_t lineno = 0;
            _kernel->get_traceback_top( filename, MENGINE_MAX_PATH, function, MENGINE_CODE_MAX_FUNCTION_NAME, &lineno );

            LoggerMessage msg;
            msg.timestamp = Helper::getLocalTimestamp();
            msg.category = "python";
            msg.thread = Helper::getCurrentThreadName();
            msg.level = _level;
            msg.filter = LFILTER_NONE;
            msg.color = _color;
            msg.flag = _flag;
            msg.file = filename;
            msg.line = lineno;
            msg.function = function;
            msg.data = _message.c_str();
            msg.size = _message.size();

            LOGGER_SERVICE()
                ->logMessage( msg );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_logDebug( pybind::kernel_interface * _kernel, const StringView & _message )
        {
            MENGINE_UNUSED( _kernel );
            MENGINE_UNUSED( _message );

#if defined(MENGINE_LOGGER_INFO_ENABLE)
            s_log( _kernel, _message, LM_DEBUG, LCOLOR_BLUE, LFLAG_SHORT );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_logInfo( pybind::kernel_interface * _kernel, const StringView & _message )
        {
            MENGINE_UNUSED( _kernel );
            MENGINE_UNUSED( _message );

#if defined(MENGINE_LOGGER_INFO_ENABLE)
            s_log( _kernel, _message, LM_INFO, LCOLOR_GREEN | LCOLOR_BLUE, LFLAG_SHORT );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_logMessage( pybind::kernel_interface * _kernel, const StringView & _message )
        {
            s_log( _kernel, _message, LM_MESSAGE, LCOLOR_GREEN, LFLAG_SHORT );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_logWarning( pybind::kernel_interface * _kernel, const StringView & _message )
        {
            s_log( _kernel, _message, LM_WARNING, LCOLOR_RED | LCOLOR_GREEN, LFLAG_SHORT );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_logError( pybind::kernel_interface * _kernel, const StringView & _message )
        {
            s_log( _kernel, _message, LM_ERROR, LCOLOR_RED, LFLAG_SHORT );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_logMessageRelease( pybind::kernel_interface * _kernel, const StringView & _message )
        {
            s_log( _kernel, _message, LM_MESSAGE_RELEASE, LCOLOR_RED | LCOLOR_BLUE, LFLAG_SHORT );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_logFatal( pybind::kernel_interface * _kernel, const StringView & _message )
        {
            s_log( _kernel, _message, LM_FATAL, LCOLOR_RED, LFLAG_FULL );
        }
        //////////////////////////////////////////////////////////////////////////
        static UniqueId s_enumerator()
        {
            UniqueId new_id = Helper::generateUniqueIdentity();

            return new_id;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_filterpowf( const pybind::list & l, float _pow )
        {
            if( l.empty() == true )
            {
                return 0.f;
            }

            if( _pow == 0.f )
            {
                return 0.f;
            }

            float inv_pow = 1.f / _pow;

            float s = 0.f;

            for( float v : l )
            {
                s += StdMath::powf( v, inv_pow );
            }

            s /= (float)l.size();

            s = StdMath::powf( s, _pow );

            return s;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t s_rand( uint32_t a )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            uint32_t value = randomizer->getRandom32( a );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static int32_t s_randi( int32_t a )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            uint32_t value = randomizer->getRandom32i( a );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_randf( float a )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            float value = randomizer->getRandomf( a );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t s_range_rand( uint32_t a, uint32_t b )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            uint32_t value = randomizer->getRandomRange32( a, b );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static int32_t s_range_randi( int32_t a, int32_t b )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            int32_t value = randomizer->getRandomRange32i( a, b );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_range_randf( float a, float b )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            float value = randomizer->getRandomRangef( a, b );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_around_randf( float a, float b )
        {
            float v_low = a - a * b;
            float v_hight = a + a * b;

            float v = s_range_randf( v_low, v_hight );

            return v;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec2f s_radius_randf( float _radius )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            float rp = randomizer->getRandomf( mt::constant::two_pi );
            float rr = randomizer->getRandomf( 1.f );

            float rr2 = StdMath::sqrtf( rr ) * _radius;

            float x = StdMath::cosf( rp );
            float y = StdMath::sinf( rp );

            float xr = x * rr2;
            float yr = y * rr2;

            return mt::vec2f( xr, yr );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_intersectsEllipseVsPoint( const mt::vec2f & _pos, float _radius, float _ellipse, const mt::vec2f & _point )
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
        //////////////////////////////////////////////////////////////////////////
        static bool s_intersectsBoxes( const mt::vec2f & _minFirst, const mt::vec2f & _maxFirst, const mt::vec2f & _minSecond, const mt::vec2f & _maxSecond )
        {
            mt::box2f box_first( _minFirst, _maxFirst );
            mt::box2f box_second( _minSecond, _maxSecond );

            bool result = mt::box2_intersect( box_first, box_second );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec2f s_projectionPointToLine( const mt::vec2f & _point, const mt::vec2f & _v0, const mt::vec2f & _v1 )
        {
            mt::vec2f dir = _v1 - _v0;

            if( mt::dot_v2_v2( dir, dir ) < 0.0001f )
            {
                return _v0;
            }

            mt::vec2f dir_norm;
            float dir_length = mt::norm_v2_f( &dir_norm, dir );

            mt::vec2f dir_point = _point - _v0;

            float dist = mt::dot_v2_v2( dir_point, dir_norm );

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
        //////////////////////////////////////////////////////////////////////////
        static pybind::tuple s_angle_correct_interpolate_from_to( pybind::kernel_interface * _kernel, float _from, float _to )
        {
            float correct_angle_from;
            float correct_angle_to;

            mt::angle_correct_interpolate_from_to( _from, _to, &correct_angle_from, &correct_angle_to );

            pybind::tuple py_result = pybind::make_tuple_t( _kernel, correct_angle_from, correct_angle_to );

            return py_result;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_angle_between_two_vectors( const mt::vec2f & _v1, const mt::vec2f & _v2 )
        {
            mt::vec2f v1_norm;
            mt::norm_v2_v2( &v1_norm, _v1 );

            mt::vec2f v2_norm;
            mt::norm_v2_v2( &v2_norm, _v2 );

            float cos_v = mt::dot_v2_v2( v1_norm, v2_norm );

            float angle = StdMath::acosf( cos_v );

            return angle;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list s_getPolygonPoints( pybind::kernel_interface * _kernel, const Polygon & _polygon )
        {
            const VectorPoints & points = _polygon.getPoints();

            pybind::list py_list = pybind::make_list_container_t( _kernel, points );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_intersectsPolygons( const Polygon & _p1, const Polygon & _p2 )
        {
            bool intersect = Helper::intersects( _p1, _p2 );

            return intersect;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_intersectsPolygonsWM( const mt::mat4f & _wm1, const Polygon & _p1, const mt::mat4f & _wm2, Polygon _p2 )
        {
            Polygon p1wm;
            _p1.mul_wm( &p1wm, _wm1 );

            Polygon p2wm;
            _p2.mul_wm( &p2wm, _wm2 );

            bool intersect = Helper::intersects( p1wm, p2wm );

            return intersect;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_intersectsPolygonsWMP( const mt::vec3f & _wm1, const Polygon & _p1, const mt::vec3f & _wm2, Polygon _p2 )
        {
            Polygon p1wm;
            _p1.transpose( &p1wm, _wm1.to_vec2f() );

            Polygon p2wm;
            _p2.transpose( &p2wm, _wm2.to_vec2f() );

            bool intersect = Helper::intersects( p1wm, p2wm );

            return intersect;
        }
        //////////////////////////////////////////////////////////////////////////
        static Tags s_makeTags( const pybind::args & _args )
        {
            Tags t;
            for( const pybind::object & o : _args )
            {
                t.addTag( o.extract() );
            }

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list s_textures( pybind::kernel_interface * _kernel )
        {
            pybind::list py_list( _kernel );

            RENDERTEXTURE_SERVICE()
                ->foreachTexture( [&py_list]( const RenderTextureInterfacePtr & _texture )
            {
                const ContentInterfacePtr & content = _texture->getContent();

                const FilePath & filePath = content->getFilePath();

                py_list.append( filePath );
            } );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        static double s_watchdog( const ConstString & _tag )
        {
            MENGINE_UNUSED( _tag );

            double watch = MENGINE_WATCHDOG( _tag );

            return watch;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isPointInsidePolygon( const mt::vec2f & _point, const Polygon & _polygon )
        {
            bool result = Helper::intersects( _polygon, _point );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static Timestamp s_getTimeMs()
        {
            Timestamp ms = Helper::getSystemTimestamp();

            return ms;
        }
        //////////////////////////////////////////////////////////////////////////
        static Timestamp s_getTime()
        {
            Timestamp ms = s_getTimeMs();

            Timestamp s = ms / 1000;

            return s;
        }
        //////////////////////////////////////////////////////////////////////////
        static Timestamp s_getLocalDateTimeMs()
        {
            Timestamp timestamp = Helper::getLocalTimestamp();

            return timestamp;
        }
        //////////////////////////////////////////////////////////////////////////
        static PlatformDateTime s_getLocalDateStruct()
        {
            PlatformDateTime dateTime;
            DATETIME_SYSTEM()
                ->getLocalDateTime( &dateTime );

            return dateTime;
        }
        //////////////////////////////////////////////////////////////////////////
        static PlatformDateTime s_getLocalDateStructFromTimeMs( uint64_t _time )
        {
            PlatformDateTime dateTime;
            DATETIME_SYSTEM()
                ->getDateTimeFromMilliseconds( _time, &dateTime );

            return dateTime;
        }
        //////////////////////////////////////////////////////////////////////////
        static String s_getDatePathTimestamp()
        {
            Char pathTimestamp[1024 + 1] = {'\0'};
            Helper::makeFilePathDateTimestamp( pathTimestamp, 1024 );

            return pathTimestamp;
        }
        //////////////////////////////////////////////////////////////////////////
        static String s_getLoggerTimestamp( const Char * _format )
        {
            Timestamp timestamp = Helper::getLocalTimestamp();

            Char shortDate[1024 + 1] = {'\0'};
            Helper::makeLoggerShortDate( timestamp, _format, shortDate, 0, 1024 );

            return shortDate;
        }
        //////////////////////////////////////////////////////////////////////////
        static String s_getTimeString()
        {
            Char timestamp[256 + 1] = {'\0'};
            Helper::makeLoggerDateTimestamp( timestamp, 256 );

            return timestamp;
        }
        //////////////////////////////////////////////////////////////////////////
        static VectorConstString s_getAccounts()
        {
            VectorConstString accounts;

            ACCOUNT_SERVICE()
                ->foreachAccounts( [&accounts]( const AccountInterfacePtr & _account )
            {
                const ConstString & accountId = _account->getAccountId();

                accounts.emplace_back( accountId );
            } );

            return accounts;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_addCurrentAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _setting, PyObject * _defaultValue, const pybind::object & _cb, const pybind::args & _args )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            bool successful = s_addAccountSetting( _kernel, accountId, _setting, _defaultValue, _cb, _args );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeCurrentAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _setting, PyObject * _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            bool successful = s_changeAccountSetting( _kernel, accountId, _setting, _value );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeCurrentAccountSettingBool( const ConstString & _setting, bool _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            bool successful = s_changeAccountSettingBool( accountId, _setting, _value );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeCurrentAccountSettingInt( const ConstString & _setting, int32_t _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            bool successful = s_changeAccountSettingInt( accountId, _setting, _value );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeCurrentAccountSettingUInt( const ConstString & _setting, uint32_t _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            bool successful = s_changeAccountSettingUInt( accountId, _setting, _value );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeCurrentAccountSettingUInt64( const ConstString & _setting, uint64_t _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            bool successful = s_changeAccountSettingUInt64( accountId, _setting, _value );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeCurrentAccountSettingFloat( const ConstString & _setting, float _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            bool successful = s_changeAccountSettingFloat( accountId, _setting, _value );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeCurrentAccountSettingStrings( const ConstString & _setting, const VectorWString & _values )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return s_changeAccountSettingStrings( accountId, _setting, _values );
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonAccountSettingProvider
            : public AccountSettingProviderInterface
            , public PythonCallbackProvider
            , public Factorable
        {
            DECLARE_FACTORABLE( PythonAccountSettingProvider );

        public:
            PythonAccountSettingProvider( pybind::kernel_interface * _kernel, const ConstString & _accountId, const pybind::object & _cb, const pybind::args & _args )
                : m_kernel( _kernel )
                , m_accountId( _accountId )
                , PythonCallbackProvider( _cb, _args )
            {
            }

            ~PythonAccountSettingProvider() override
            {
            }

        protected:
            void onChangeSetting( const Char * _value ) override
            {
                PyObject * pyunicode_value = m_kernel->unicode_from_utf8( _value );

                this->call_cb( m_accountId, pyunicode_value );

                m_kernel->decref( pyunicode_value );
            }

        protected:
            pybind::kernel_interface * m_kernel;

            ConstString m_accountId;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonAccountSettingProvider, AccountSettingProviderInterface> PyAccountSettingProviderPtr;
        //////////////////////////////////////////////////////////////////////////
        static bool s_addAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting, PyObject * _defaultValue, const pybind::object & _cb, const pybind::args & _args )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account not found '%s'"
                , _accountId.c_str()
            );

            if( _kernel->unicode_check( _defaultValue ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' default value is not UNICODE '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                    , _kernel->object_repr( _defaultValue ).c_str()
                );

                return false;
            }

            PyAccountSettingProviderPtr provider = Helper::makeFactorableUnique<PythonAccountSettingProvider>( MENGINE_DOCUMENT_PYTHON, _kernel, _accountId, _cb, _args );

            PyObject * py_utf8_defaultValue = _kernel->unicode_encode_utf8( _defaultValue );

            if( py_utf8_defaultValue == nullptr )
            {
                return false;
            }

            pybind::object utf8_defaultValueObject( _kernel, py_utf8_defaultValue, pybind::borrowed );
            const Char * utf8_defaultValue = _kernel->string_to_char( py_utf8_defaultValue );

            bool result = account->addSetting( _setting, utf8_defaultValue, provider );

            LOGGER_INFO( "account", "%s add account '%s' setting '%s' default value '%s'"
                , result == true ? "success" : "failed"
                , _accountId.c_str()
                , _setting.c_str()
                , utf8_defaultValue
            );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasAccountSetting( const ConstString & _accountId, const ConstString & _setting )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account not found '%s'"
                , _accountId.c_str()
            );

            bool result = account->hasSetting( _setting, nullptr );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting, PyObject * _value )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account not found '%s'"
                , _accountId.c_str()
            );

            if( account->hasSetting( _setting, nullptr ) == false )
            {
                LOGGER_ERROR( "account '%s' not found setting '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return false;
            }

            if( _kernel->unicode_check( _value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' value is not UNICODE '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                    , _kernel->object_repr( _value ).c_str()
                );

                return false;
            }

            PyObject * py_utf8_defaultValue = _kernel->unicode_encode_utf8( _value );

            if( py_utf8_defaultValue == nullptr )
            {
                return false;
            }

            pybind::object utf8_defaultValueObject( _kernel, py_utf8_defaultValue, pybind::borrowed );
            const Char * utf8_defaultValue = _kernel->string_to_char( py_utf8_defaultValue );

            bool result = account->changeSetting( _setting, utf8_defaultValue );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeAccountSettingBool( const ConstString & _accountId, const ConstString & _setting, bool _value )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account not found '%s'"
                , _accountId.c_str()
            );

            if( account->hasSetting( _setting, nullptr ) == false )
            {
                LOGGER_ERROR( "account '%s' not found setting '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return false;
            }

            const Char * value_str = _value == true ? "True" : "False";

            bool result = account->changeSetting( _setting, value_str );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeAccountSettingInt( const ConstString & _accountId, const ConstString & _setting, int32_t _value )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account not found '%s'"
                , _accountId.c_str()
            );

            if( account->hasSetting( _setting, nullptr ) == false )
            {
                LOGGER_ERROR( "account '%s' not found setting '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return false;
            }

            Char setting_value[64 + 1] = {'\0'};
            if( Helper::stringalized( _value, setting_value, 64 ) == false )
            {
                return false;
            }

            bool result = account->changeSetting( _setting, setting_value );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeAccountSettingUInt( const ConstString & _accountId, const ConstString & _setting, uint32_t _value )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account not found '%s'"
                , _accountId.c_str()
            );

            if( account->hasSetting( _setting, nullptr ) == false )
            {
                LOGGER_ERROR( "account '%s' not found setting '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return false;
            }

            Char setting_value[64 + 1] = {'\0'};
            if( Helper::stringalized( _value, setting_value, 64 ) == false )
            {
                return false;
            }

            bool result = account->changeSetting( _setting, setting_value );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeAccountSettingUInt64( const ConstString & _accountId, const ConstString & _setting, uint64_t _value )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account not found '%s'"
                , _accountId.c_str()
            );

            if( account->hasSetting( _setting, nullptr ) == false )
            {
                LOGGER_ERROR( "account '%s' not found setting '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return false;
            }

            Char setting_value[64 + 1] = {'\0'};
            if( Helper::stringalized( _value, setting_value, 64 ) == false )
            {
                return false;
            }

            bool result = account->changeSetting( _setting, setting_value );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeAccountSettingFloat( const ConstString & _accountId, const ConstString & _setting, float _value )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account not found '%s'"
                , _accountId.c_str()
            );

            if( account->hasSetting( _setting, nullptr ) == false )
            {
                LOGGER_ERROR( "account '%s' not found setting '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return false;
            }

            Char setting_value[256 + 1] = {'\0'};
            if( Helper::stringalized( _value, setting_value, 256 ) == false )
            {
                return false;
            }

            bool result = account->changeSetting( _setting, setting_value );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeAccountSettingStrings( const ConstString & _accountId, const ConstString & _setting, const VectorWString & _values )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account not found '%s'"
                , _accountId.c_str()
            );

            if( account->hasSetting( _setting, nullptr ) == false )
            {
                LOGGER_ERROR( "account '%s' not found setting '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return false;
            }

            String setting_value;

            for( const WString & value : _values )
            {
                if( setting_value.empty() == false )
                {
                    setting_value += " ,,, ";
                }

                String utf8_value;
                Helper::unicodeToUtf8( value, &utf8_value );

                setting_value += utf8_value;
            }

            bool result = account->changeSetting( _setting, setting_value.c_str() );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_addGlobalSetting( pybind::kernel_interface * _kernel, const ConstString & _setting, PyObject * _defaultValue, const pybind::object & _cb, const pybind::args & _args )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account not found" );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_addAccountSetting( _kernel, accountId, _setting, _defaultValue, _cb, _args );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasGlobalSetting( const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account not found" );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_hasAccountSetting( accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeGlobalSetting( pybind::kernel_interface * _kernel, const ConstString & _setting, PyObject * _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account not found" );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_changeAccountSetting( _kernel, accountId, _setting, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeGlobalSettingBool( const ConstString & _setting, bool _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account not found" );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_changeAccountSettingBool( accountId, _setting, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeGlobalSettingInt( const ConstString & _setting, int32_t _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account not found" );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_changeAccountSettingInt( accountId, _setting, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeGlobalSettingUInt( const ConstString & _setting, uint32_t _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account not found" );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_changeAccountSettingUInt( accountId, _setting, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeGlobalSettingUInt64( const ConstString & _setting, uint64_t _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account not found" );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_changeAccountSettingUInt64( accountId, _setting, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeGlobalSettingFloat( const ConstString & _setting, float _value )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account not found" );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_changeAccountSettingFloat( accountId, _setting, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_changeGlobalSettingStrings( const ConstString & _setting, const VectorWString & _values )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account not found" );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_changeAccountSettingStrings( accountId, _setting, _values );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasCurrentAccountSetting( const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return false;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return s_hasAccountSetting( accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getCurrentAccountUID( pybind::kernel_interface * _kernel )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none" );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return s_getAccountUID( _kernel, accountId );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getCurrentAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return s_getAccountSetting( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getCurrentAccountSettingBool( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return s_getAccountSettingBool( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getCurrentAccountSettingInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return s_getAccountSettingInt( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getCurrentAccountSettingUInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return s_getAccountSettingUInt( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getCurrentAccountSettingUInt64( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return s_getAccountSettingUInt64( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getCurrentAccountSettingFloat( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return s_getAccountSettingFloat( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getCurrentAccountSettingStrings( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return s_getAccountSettingStrings( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_getCurrentAccountSettingFloatDefault( const ConstString & _setting, float _default )
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none [%s]"
                    , _setting.c_str()
                );

                return _default;
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return s_getAccountSettingFloatDefault( accountId, _setting, _default );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_getConfigBool( const Char * _section, const Char * _key, bool _default )
        {
            const ConfigInterfacePtr & config = CONFIG_SERVICE()
                ->getMainConfig();

            bool value;
            config->hasValue( _section, _key, _default, &value );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static int32_t s_getConfigInt( const Char * _section, const Char * _key, int32_t _default )
        {
            const ConfigInterfacePtr & config = CONFIG_SERVICE()
                ->getMainConfig();

            int32_t value;
            config->hasValueInteger( _section, _key, _default, &value );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t s_getConfigUInt( const Char * _section, const Char * _key, uint32_t _default )
        {
            const ConfigInterfacePtr & config = CONFIG_SERVICE()
                ->getMainConfig();

            uint32_t value;
            config->hasValueInteger( _section, _key, _default, &value );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint64_t s_getConfigUInt64( const Char * _section, const Char * _key, uint64_t _default )
        {
            const ConfigInterfacePtr & config = CONFIG_SERVICE()
                ->getMainConfig();

            uint64_t value;
            config->hasValueInteger( _section, _key, _default, &value );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_getConfigFloat( const Char * _section, const Char * _key, float _default )
        {
            const ConfigInterfacePtr & config = CONFIG_SERVICE()
                ->getMainConfig();

            float value;
            config->hasValueFloat( _section, _key, _default, &value );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static String s_getConfigString( const Char * _section, const Char * _key, const Char * _default )
        {
            const ConfigInterfacePtr & config = CONFIG_SERVICE()
                ->getMainConfig();

            String value;
            config->hasValue( _section, _key, _default, &value );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static VectorString s_getConfigStrings( const Char * _section, const Char * _key )
        {
            const ConfigInterfacePtr & config = CONFIG_SERVICE()
                ->getMainConfig();

            VectorString values;
            config->getValues( _section, _key, &values );

            return values;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getAccountUID( pybind::kernel_interface * _kernel, const ConstString & _accountId )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account '%s' is none"
                , _accountId.c_str()
            );

            const AccountUID & accountUID = account->getUID();

            PyObject * py_value = _kernel->string_from_char_size( accountUID.data, AccountUID::size_data );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account '%s' is none"
                , _accountId.c_str()
            );

            const Char * value = nullptr;
            if( account->getSetting( _setting, &value ) == false )
            {
                return _kernel->ret_none();
            }

            PyObject * py_value = _kernel->unicode_from_utf8( value );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getAccountSettingBool( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account '%s' is none"
                , _accountId.c_str()
            );

            const Char * setting_value = nullptr;
            if( account->getSetting( _setting, &setting_value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' not found"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            if( StdString::strlen( setting_value ) == 0 )
            {
                return _kernel->ret_none();
            }

            if( MENGINE_STRICMP( setting_value, "true" ) == 0 )
            {
                return _kernel->ret_true();
            }
            else if( MENGINE_STRICMP( setting_value, "false" ) == 0 )
            {
                return _kernel->ret_false();
            }

            LOGGER_ERROR( "account '%s' setting '%s' value '%s' is not bool [True|False]"
                , _accountId.c_str()
                , _setting.c_str()
                , setting_value
            );

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getAccountSettingInt( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account '%s' is none"
                , _accountId.c_str()
            );

            const Char * setting_value = nullptr;
            if( account->getSetting( _setting, &setting_value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' not found"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            if( StdString::strlen( setting_value ) == 0 )
            {
                return _kernel->ret_none();
            }

            int32_t value;
            if( Helper::stringalized( setting_value, &value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' can't scanf from '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                    , setting_value
                );

                return _kernel->ret_none();
            }

            PyObject * py_value = pybind::ptr( _kernel, value );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getAccountSettingUInt( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account '%s' is none"
                , _accountId.c_str()
            );

            const Char * setting_value = nullptr;
            if( account->getSetting( _setting, &setting_value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' not found"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            if( StdString::strlen( setting_value ) == 0 )
            {
                return _kernel->ret_none();
            }

            uint32_t value;
            if( Helper::stringalized( setting_value, &value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' can't scanf from '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                    , setting_value
                );

                return _kernel->ret_none();
            }

            PyObject * py_value = pybind::ptr( _kernel, value );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getAccountSettingUInt64( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account '%s' is none"
                , _accountId.c_str()
            );

            const Char * setting_value = nullptr;
            if( account->getSetting( _setting, &setting_value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' not found"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            if( StdString::strlen( setting_value ) == 0 )
            {
                return _kernel->ret_none();
            }

            uint64_t value;
            if( Helper::stringalized( setting_value, &value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' can't scanf from '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                    , setting_value
                );

                return _kernel->ret_none();
            }

            PyObject * py_value = pybind::ptr( _kernel, value );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getAccountSettingStrings( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account '%s' is none"
                , _accountId.c_str()
            );

            const Char * setting_value = nullptr;
            if( account->getSetting( _setting, &setting_value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' not found"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            size_t setting_value_len = StdString::strlen( setting_value );

            if( setting_value_len == 0 )
            {
                pybind::list l( _kernel );

                return l.ret();
            }

            WString setting_valueW;
            Helper::utf8ToUnicode( setting_value, &setting_valueW );

            VectorWString strings;
            Helper::wsplit( &strings, setting_valueW, true, L" ,,, " );

            pybind::list l = pybind::make_list_container_t( _kernel, strings );

            return l.ret();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getAccountSettingFloat( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account '%s' is none"
                , _accountId.c_str()
            );

            const Char * setting_value = nullptr;
            if( account->getSetting( _setting, &setting_value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' not found"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return _kernel->ret_none();
            }

            if( StdString::strlen( setting_value ) == 0 )
            {
                return _kernel->ret_none();
            }

            float value;
            if( Helper::stringalized( setting_value, &value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' can't scanf from '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                    , setting_value
                );

                return _kernel->ret_none();
            }

            PyObject * py_value = pybind::ptr( _kernel, value );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_getAccountSettingFloatDefault( const ConstString & _accountId, const ConstString & _setting, float _default )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "account '%s' is none"
                , _accountId.c_str()
            );

            if( account->hasSetting( _setting, nullptr ) == false )
            {
                return _default;
            }

            const Char * setting_value = nullptr;
            if( account->getSetting( _setting, &setting_value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' not found"
                    , _accountId.c_str()
                    , _setting.c_str()
                );

                return _default;
            }

            if( StdString::strlen( setting_value ) == 0 )
            {
                return _default;
            }

            float value;
            if( Helper::stringalized( setting_value, &value ) == false )
            {
                LOGGER_ERROR( "account '%s' setting '%s' can't scanf from '%s'"
                    , _accountId.c_str()
                    , _setting.c_str()
                    , setting_value
                );

                return _default;
            }

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getGlobalAccountUID( pybind::kernel_interface * _kernel )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account is none" );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_getAccountUID( _kernel, accountId );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getGlobalSetting( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account is none" );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_getAccountSetting( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getGlobalSettingBool( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            MENGINE_UNUSED( _kernel );

            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account is none" );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_getAccountSettingBool( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getGlobalSettingInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account is none" );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_getAccountSettingInt( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getGlobalSettingUInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account is none" );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_getAccountSettingUInt( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getGlobalSettingUInt64( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account is none" );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_getAccountSettingUInt64( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getGlobalSettingFloat( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "vaccount is none" );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_getAccountSettingFloat( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getGlobalSettingStrings( pybind::kernel_interface * _kernel, const ConstString & _setting )
        {
            if( ACCOUNT_SERVICE()
                ->hasGlobalAccount() == false )
            {
                LOGGER_ERROR( "global account is none" );

                return _kernel->ret_none();
            }

            const ConstString & accountId = ACCOUNT_SERVICE()
                ->getGlobalAccountId();

            return s_getAccountSettingStrings( _kernel, accountId, _setting );
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_createAccount( pybind::kernel_interface * _kernel )
        {
            AccountInterfacePtr account = ACCOUNT_SERVICE()
                ->createAccount( MENGINE_DOCUMENT_PYTHON );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "invalid create account" );

            const ConstString & accountId = account->getAccountId();

            PyObject * py_value = pybind::ptr( _kernel, accountId );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_createGlobalAccount( pybind::kernel_interface * _kernel )
        {
            AccountInterfacePtr account = ACCOUNT_SERVICE()
                ->createGlobalAccount( MENGINE_DOCUMENT_PYTHON );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "invalid create account" );

            const ConstString & accountId = account->getAccountId();

            PyObject * py_value = pybind::ptr( _kernel, accountId );

            return py_value;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_selectAccount( const ConstString & _accountId )
        {
            ACCOUNT_SERVICE()
                ->selectAccount( _accountId );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasCurrentAccount()
        {
            return ACCOUNT_SERVICE()
                ->hasCurrentAccount();
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_setDefaultAccount( const ConstString & _accountId )
        {
            ACCOUNT_SERVICE()
                ->setDefaultAccount( _accountId );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_setGlobalAccount( const ConstString & _accountId )
        {
            ACCOUNT_SERVICE()
                ->setGlobalAccount( _accountId );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasGlobalAccount()
        {
            return ACCOUNT_SERVICE()
                ->hasGlobalAccount();
        }
        //////////////////////////////////////////////////////////////////////////
        static const ConstString & s_getGlobalAccountId()
        {
            return ACCOUNT_SERVICE()
                ->getGlobalAccountId();
        }
        //////////////////////////////////////////////////////////////////////////
        static const ConstString & s_getDefaultAccount()
        {
            return ACCOUNT_SERVICE()
                ->getDefaultAccountId();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasDefaultAccount()
        {
            return ACCOUNT_SERVICE()
                ->hasDefaultAccount();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isCurrentDefaultAccount()
        {
            return ACCOUNT_SERVICE()
                ->isCurrentDefaultAccount();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_selectDefaultAccount()
        {
            return ACCOUNT_SERVICE()
                ->selectDefaultAccount();
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_saveAccount()
        {
            //ToDo
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_saveAccounts()
        {
            ACCOUNT_SERVICE()
                ->saveAccounts();
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_saveAccountsInfo()
        {
            //ToDo
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_deleteAccount( const ConstString & _accountName )
        {
            ACCOUNT_SERVICE()
                ->deleteAccount( _accountName );
        }
        //////////////////////////////////////////////////////////////////////////
        static const ConstString & s_getCurrentAccountName()
        {
            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "currentAccount is none" );

                return ConstString::none();
            }

            const ConstString & name = ACCOUNT_SERVICE()
                ->getCurrentAccountId();

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_writeGlobalPickleFile( pybind::kernel_interface * _kernel, const WString & _filePath, PyObject * _data, PyObject * _pickleTypes )
        {
            size_t size;
            if( pybind::pickle( _kernel, _data, _pickleTypes, nullptr, 0, &size ) == false )
            {
                LOGGER_ERROR( "file '%ls' invalid get pickle size"
                    , _filePath.c_str()
                );

                return false;
            }

            MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_PYTHON );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid get memory for '%zu' size"
                , size
            );

            void * memory_buffer = buffer->getBuffer();
            size_t memory_size = buffer->getSize();

            if( pybind::pickle( _kernel, _data, _pickleTypes, memory_buffer, memory_size, &size ) == false )
            {
                LOGGER_ERROR( "file '%ls' invalid pickle"
                    , _filePath.c_str()
                );

                return false;
            }

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

            String utf8_filePath;
            if( Helper::unicodeToUtf8( _filePath, &utf8_filePath ) == false )
            {
                LOGGER_ERROR( "invalid file '%ls' convert to utf8"
                    , _filePath.c_str()
                );

                return false;
            }

            FilePath filePath = Helper::stringizeFilePath( utf8_filePath );

            OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( fileGroup, filePath, true, MENGINE_DOCUMENT_FUNCTION );

            if( stream == nullptr )
            {
                LOGGER_ERROR( "invalid open file '%s'"
                    , filePath.c_str()
                );

                return false;
            }

            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

            if( archivator == nullptr )
            {
                LOGGER_ERROR( "invalid get archivator '%s'"
                    , "lz4"
                );

                return false;
            }

            //TODO create global data save
            bool successful = Helper::writeStreamArchiveMagic( stream, archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), true, memory_buffer, memory_size, EAC_NORMAL );

            if( Helper::closeOutputStreamFile( fileGroup, stream ) == false )
            {
                LOGGER_ERROR( "invalid close file '%s'"
                    , filePath.c_str()
                );

                return false;
            }

            if( successful == false )
            {
                LOGGER_ERROR( "invalid write file '%s'"
                    , filePath.c_str()
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_loadGlobalPickleFile( pybind::kernel_interface * _kernel, const WString & _filePath, PyObject * _pickleTypes )
        {
            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

            if( archivator == nullptr )
            {
                LOGGER_ERROR( "invalid get archivator '%s'"
                    , "lz4"
                );

                return _kernel->ret_none();
            }

            String utf8_filePath;
            if( Helper::unicodeToUtf8( _filePath, &utf8_filePath ) == false )
            {
                LOGGER_ERROR( "invalid convert file '%ls' to utf8"
                    , _filePath.c_str()
                );

                return _kernel->ret_none();
            }

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

            FilePath filePath = Helper::stringizeFilePath( utf8_filePath );

            if( fileGroup->existFile( filePath, true ) == false )
            {
                return _kernel->ret_none();
            }

            InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FUNCTION );

            if( stream == nullptr )
            {
                LOGGER_ERROR( "invalid open file '%s'"
                    , filePath.c_str()
                );

                return _kernel->ret_none();
            }

            MemoryInterfacePtr memory = Helper::readStreamArchiveMagic( stream, archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), MENGINE_DOCUMENT_FUNCTION );

            if( memory == nullptr )
            {
                LOGGER_ERROR( "invalid load stream archive '%s'"
                    , filePath.c_str()
                );

                return _kernel->ret_none();
            }

            void * memory_buffer = memory->getBuffer();
            size_t memory_size = memory->getSize();

            PyObject * py_data = pybind::unpickle( _kernel, memory_buffer, memory_size, _pickleTypes );

            if( py_data == nullptr )
            {
                LOGGER_ERROR( "invalid unpickle file '%s'"
                    , filePath.c_str()
                );

                return _kernel->ret_none();
            }

            memory = nullptr;

            return py_data;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_writeAccountPickleFile( pybind::kernel_interface * _kernel, const ConstString & _accountId, const WString & _filePath, PyObject * _data, PyObject * _pickleTypes )
        {
            String utf8_filePath;
            if( Helper::unicodeToUtf8( _filePath, &utf8_filePath ) == false )
            {
                LOGGER_ERROR( "account '%s' invalid file '%ls' convert to utf8"
                    , _accountId.c_str()
                    , _filePath.c_str()
                );

                return false;
            }

            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "invalid account '%s'"
                , _accountId.c_str()
            );

            FilePath filePath = Helper::stringizeFilePath( utf8_filePath );

            size_t size;
            if( pybind::pickle( _kernel, _data, _pickleTypes, nullptr, 0, &size ) == false )
            {
                LOGGER_ERROR( "account '%s' invalid file '%ls' get pickle size"
                    , _accountId.c_str()
                    , _filePath.c_str()
                );

                return false;
            }

            MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_PYTHON );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "'%s' invalid get memory for '%zu' size"
                , _accountId.c_str()
                , size
            );

            void * memory_buffer = buffer->getBuffer();
            size_t memory_size = buffer->getSize();

            if( pybind::pickle( _kernel, _data, _pickleTypes, memory_buffer, memory_size, &size ) == false )
            {
                LOGGER_ERROR( "account '%s' invalid file '%ls' pickle"
                    , _accountId.c_str()
                    , _filePath.c_str()
                );

                return false;
            }

            if( account->writeBinaryFile( filePath, memory_buffer, memory_size ) == false )
            {
                LOGGER_ERROR( "account '%s' invalid write file '%ls'"
                    , _accountId.c_str()
                    , _filePath.c_str()
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_loadAccountPickleFile( pybind::kernel_interface * _kernel, const ConstString & _accountId, const WString & _filePath, PyObject * _pickleTypes )
        {
            String utf8_filePath;
            if( Helper::unicodeToUtf8( _filePath, &utf8_filePath ) == false )
            {
                LOGGER_ERROR( "invalid convert file '%ls' to utf8"
                    , _filePath.c_str()
                );

                return _kernel->ret_none();
            }

            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            if( account == nullptr )
            {
                LOGGER_ERROR( "invalid get account '%s'"
                    , _accountId.c_str()
                );

                return _kernel->ret_none();
            }

            FilePath filePath = Helper::stringizeFilePath( utf8_filePath );

            MemoryInterfacePtr binaryBuffer = account->loadBinaryFile( filePath );

            if( binaryBuffer == nullptr )
            {
                LOGGER_ERROR( "account '%s' invalid load file '%ls'"
                    , _accountId.c_str()
                    , _filePath.c_str()
                );

                return _kernel->ret_none();
            }

            void * binaryBuffer_memory = binaryBuffer->getBuffer();
            size_t binaryBuffer_size = binaryBuffer->getSize();

            PyObject * py_data = pybind::unpickle( _kernel, binaryBuffer_memory, binaryBuffer_size, _pickleTypes );

            if( py_data == nullptr )
            {
                LOGGER_ERROR( "account '%s' invalid unpickle file '%ls'"
                    , _accountId.c_str()
                    , _filePath.c_str()
                );

                return _kernel->ret_none();
            }

            binaryBuffer = nullptr;

            return py_data;
        }
        //////////////////////////////////////////////////////////////////////////
        static FileGroupInterfacePtr s_getDefaultFileGroup()
        {
            FileGroupInterfacePtr fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );;

            return fileGroup;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasFileGroup( const ConstString & _fileGroup )
        {
            bool exist = FILE_SERVICE()
                ->hasFileGroup( _fileGroup, nullptr );

            return exist;
        }
        //////////////////////////////////////////////////////////////////////////
        static const FileGroupInterfacePtr & s_getFileGroup( const ConstString & _fileGroup )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _fileGroup );

            return fileGroup;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasAccountPickleFile( const ConstString & _accountId, const WString & _filePath )
        {
            const AccountInterfacePtr & account = ACCOUNT_SERVICE()
                ->getAccount( _accountId );

            if( ACCOUNT_SERVICE()
                ->hasCurrentAccount() == false )
            {
                LOGGER_ERROR( "invalid get account '%s'"
                    , _accountId.c_str()
                );

                return false;
            }

            String utf8_filePath;
            if( Helper::unicodeToUtf8( _filePath, &utf8_filePath ) == false )
            {
                LOGGER_ERROR( "invalid convert file '%ls' to utf8"
                    , _filePath.c_str()
                );

                return false;
            }

            FilePath filePath = Helper::stringizeFilePath( utf8_filePath );

            bool exist = account->hasBinaryFile( filePath );

            return exist;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_setParticlesEnabled( bool _enable )
        {
            APPLICATION_SERVICE()
                ->setParticleEnable( _enable );
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec2f s_getNodeScreenAdaptPosition( Node * _node )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _node, "node is nullptr" );

            const RenderCameraInterface * camera = Helper::getNodeRenderCameraInheritance( _node );

            if( camera == nullptr )
            {
                const RenderCameraInterfacePtr & player_camera = PLAYER_SERVICE()
                    ->getRenderCamera();

                camera = player_camera.get();
            }

            const TransformationInterface * transformation = _node->getTransformation();

            const mt::mat4f & wm = transformation->getWorldMatrix();

            mt::vec2f screen;
            camera->fromWorldToScreenPosition( wm, &screen );

            mt::vec2f adaptSceen;
            Helper::adaptWorldPosition( screen, &adaptSceen );

            return adaptSceen;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec2f s_fromWorldToScreenPosition( const mt::vec3f & _wp )
        {
            const RenderCameraInterfacePtr & camera = PLAYER_SERVICE()
                ->getRenderCamera();

            mt::mat4f wm;
            mt::make_translation_m4_v3( &wm, _wp );

            mt::vec2f screen;
            camera->fromWorldToScreenPosition( wm, &screen );

            mt::vec2f adaptSceen;
            Helper::adaptWorldPosition( screen, &adaptSceen );

            return adaptSceen;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasTextByKey( const ConstString & _key )
        {
            bool value = TEXT_SERVICE()
                ->hasTextEntry( _key, nullptr );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static WString s_getTextByKey( const ConstString & _key )
        {
            TextEntryInterfacePtr entry;
            if( TEXT_SERVICE()
                ->hasTextEntry( _key, &entry ) == false )
            {
                pybind::throw_exception( "invalid get key '%s'"
                    , _key.c_str()
                );
            }

            const String & text = entry->getValue();

            WString unicode;
            if( Helper::utf8ToUnicode( text, &unicode ) == false )
            {
                pybind::throw_exception( "invalid text key '%s' convert '%s' to unicode"
                    , _key.c_str()
                    , text.c_str()
                );
            }

            return unicode;
        }
        //////////////////////////////////////////////////////////////////////////
        static size_t s_getTextCharCountByKey( const ConstString & _key )
        {
            TextEntryInterfacePtr entry;
            if( TEXT_SERVICE()
                ->hasTextEntry( _key, &entry ) == false )
            {
                pybind::throw_exception( "invalid get key '%s'"
                    , _key.c_str()
                );
            }

            const String & text = entry->getValue();

            size_t text_size = text.size();

            return text_size;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_setVSync( bool _vSync )
        {
            APPLICATION_SERVICE()
                ->setVSync( _vSync );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_getVSync()
        {
            bool vsync = APPLICATION_SERVICE()
                ->getVSync();

            return vsync;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_setCursorMode( bool _mode )
        {
            APPLICATION_SERVICE()
                ->setCursorMode( _mode );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_getCursorMode()
        {
            bool cursorMode = APPLICATION_SERVICE()
                ->getCursorMode();

            return cursorMode;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_setCursorIcon( const ConstString & _resourceName )
        {
            APPLICATION_SERVICE()
                ->setCursorIcon( _resourceName );

            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        struct extract_Data_type
            : public pybind::type_cast_result<Data>
        {
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, value_type & _value, bool _nothrow ) override
            {
                MENGINE_UNUSED( _kernel );
                MENGINE_UNUSED( _nothrow );

                if( _kernel->bytearray_check( _obj ) == true )
                {
                    size_t size = _kernel->bytearray_size( _obj );
                    const uint8_t * value_byte = (const uint8_t *)_kernel->bytearray_to_data( _obj );

                    if( value_byte == nullptr )
                    {
                        return false;
                    }

                    _value.assign( value_byte, value_byte + size );
                }
                else if( _kernel->string_check( _obj ) == true )
                {
                    size_t size = 0;
                    const uint8_t * value_byte = (const uint8_t *)_kernel->string_to_char_and_size( _obj, &size );

                    if( value_byte == nullptr )
                    {
                        return false;
                    }

                    _value.assign( value_byte, value_byte + size );
                }
                else
                {
                    return false;
                }

                return true;
            }

            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<value_type>::TCastRef _value ) override
            {
                MENGINE_UNUSED( _kernel );

                const uint8_t * value_byte = reinterpret_cast<const uint8_t *>(_value.data());
                value_type::size_type value_size = _value.size();

                PyObject * py_value = _kernel->bytearray_from_data( value_byte, (size_t)value_size );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        struct extract_Tags_type
            : public pybind::type_cast_result<Tags>
        {
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, Tags & _tags, bool _nothrow ) override
            {
                MENGINE_UNUSED( _kernel );
                MENGINE_UNUSED( _nothrow );

                if( _kernel->list_check( _obj ) == true )
                {
                    pybind::list l( _kernel, _obj, pybind::borrowed );

                    size_t tags_size = l.size();

                    for( size_t i = 0; i != tags_size; ++i )
                    {
                        ConstString tag = l[i];

                        _tags.addTag( tag );
                    }
                }
                else
                {
                    return false;
                }

                return true;
            }

            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<Tags>::TCastRef _value ) override
            {
                MENGINE_UNUSED( _kernel );

                const VectorConstString & tags = _value.getValues();

                PyObject * py_tags = _kernel->list_new( 0 );

                for( const ConstString & tag : tags )
                {
                    PyObject * py_tag = pybind::ptr( _kernel, tag );

                    _kernel->list_appenditem( py_tags, py_tag );
                    _kernel->decref( py_tag );
                }

                return py_tags;
            }
        };        
        //////////////////////////////////////////////////////////////////////////
        struct extract_URLString_type
            : public pybind::type_cast_result<URLString>
        {
        public:
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, URLString & _value, bool _nothrow ) override
            {
                MENGINE_UNUSED( _kernel );
                MENGINE_UNUSED( _nothrow );

                if( _kernel->string_check( _obj ) == true )
                {
                    size_t size = 0;
                    const String::value_type * string_char = _kernel->string_to_char_and_size( _obj, &size );

                    if( string_char == nullptr )
                    {
                        return false;
                    }

                    _value.assign( string_char, size );
                }
                else
                {
                    return false;
                }

                return true;
            }

        public:
            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<URLString>::TCastRef _value ) override
            {
                MENGINE_UNUSED( _kernel );

                const String::value_type * value_str = _value.c_str();
                String::size_type value_size = _value.size();

                PyObject * py_value = _kernel->string_from_char_size( value_str, (uint32_t)value_size );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        struct extract_String_type
            : public pybind::type_cast_result<String>
        {
        public:
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, String & _value, bool _nothrow ) override
            {
                MENGINE_UNUSED( _kernel );
                MENGINE_UNUSED( _nothrow );

                if( _kernel->string_check( _obj ) == true )
                {
                    size_t size = 0;
                    const String::value_type * string_char = _kernel->string_to_char_and_size( _obj, &size );

                    if( string_char == nullptr )
                    {
                        return false;
                    }

                    _value.assign( string_char, size );
                }
                else
                {
                    return false;
                }

                return true;
            }

        public:
            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<String>::TCastRef _value ) override
            {
                MENGINE_UNUSED( _kernel );

                const String::value_type * value_str = _value.c_str();
                String::size_type value_size = _value.size();

                PyObject * py_value = _kernel->string_from_char_size( value_str, (uint32_t)value_size );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        struct extract_WString_type
            : public pybind::type_cast_result<WString>
        {
        public:
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, WString & _value, bool _nothrow ) override
            {
                MENGINE_UNUSED( _kernel );
                MENGINE_UNUSED( _nothrow );

                if( _kernel->unicode_check( _obj ) == true )
                {
                    size_t size = 0;
                    const WString::value_type * value_char = _kernel->unicode_to_wchar_and_size( _obj, &size );

                    if( value_char == nullptr )
                    {
                        return false;
                    }

                    _value.assign( value_char, size );
                }
                else
                {
                    return false;
                }

                return true;
            }

        public:
            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<WString>::TCastRef _value ) override
            {
                MENGINE_UNUSED( _kernel );

                const WString::value_type * value_str = _value.c_str();
                WString::size_type value_size = _value.size();

                PyObject * py_value = _kernel->unicode_from_wchar_size( value_str, (uint32_t)value_size );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        struct extract_ParamVariant_type
            : public pybind::type_cast_result<ParamVariant>
        {
        public:
            bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, ParamVariant & _value, bool _nothrow ) override
            {
                MENGINE_UNUSED( _kernel );
                MENGINE_UNUSED( _nothrow );

                if( _kernel->is_none( _obj ) == true )
                {
                    _value.emplace<ParamNull>( nullptr );
                }
                else if( _kernel->bool_check( _obj ) == true )
                {
                    bool bool_value;
                    _kernel->extract_bool( _obj, bool_value );

                    _value.emplace<ParamBool>( bool_value );
                }
                else if( _kernel->int_check( _obj ) == true )
                {
                    int64_t int_value;
                    _kernel->extract_int64( _obj, int_value );

                    _value.emplace<ParamInteger>( int_value );
                }
                else if( _kernel->float_check( _obj ) == true )
                {
                    double double_value;
                    _kernel->extract_double( _obj, double_value );

                    _value.emplace<ParamDouble>( double_value );
                }
                else if( _kernel->string_check( _obj ) == true )
                {
                    size_t size = 0;
                    const String::value_type * string_char = _kernel->string_to_char_and_size( _obj, &size );

                    if( string_char == nullptr )
                    {
                        return false;
                    }

                    _value.emplace<ParamString>( string_char, size );
                }
                else if( _kernel->unicode_check( _obj ) == true )
                {
                    size_t size = 0;
                    const WString::value_type * string_wchar = _kernel->unicode_to_wchar_and_size( _obj, &size );

                    if( string_wchar == nullptr )
                    {
                        return false;
                    }

                    _value.emplace<ParamWString>( string_wchar, size );
                }
                else
                {
                    return false;
                }

                return true;
            }

        public:
            PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<ParamVariant>::TCastRef _value ) override
            {
                MENGINE_UNUSED( _kernel );

                PyObject * py_value = nullptr;

                Helper::visit( _value
                    , [_kernel, &py_value]( const ParamNull & _element )
                {
                    MENGINE_UNUSED( _element );

                    py_value = _kernel->ret_none();
                }
                    , [_kernel, &py_value]( const ParamBool & _element )
                {
                    py_value = pybind::ptr( _kernel, _element );
                }
                    , [_kernel, &py_value]( const ParamInteger & _element )
                {
                    py_value = pybind::ptr( _kernel, _element );
                }
                    , [_kernel, &py_value]( const ParamDouble & _element )
                {
                    py_value = pybind::ptr( _kernel, _element );
                }
                    , [_kernel, &py_value]( const ParamString & _element )
                {
                    py_value = pybind::ptr( _kernel, _element );
                }
                    , [_kernel, &py_value]( const ParamWString & _element )
                {
                    py_value = pybind::ptr( _kernel, _element );
                }
                    , [_kernel, &py_value]( const ParamConstString & _element )
                {
                    py_value = pybind::ptr( _kernel, _element );
                }
                    , [_kernel, &py_value]( const ParamFilePath & _element )
                {
                    py_value = pybind::ptr( _kernel, _element );
                }
                    , [_kernel, &py_value]( const ParamFactorablePtr & _element )
                {
                    py_value = pybind::ptr( _kernel, _element );
                } );

                return py_value;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        namespace ScriptMethod
        {
            //////////////////////////////////////////////////////////////////////////
            static bool Tags_convert( pybind::kernel_interface * _kernel, PyObject * _obj, Tags * const _place )
            {
                if( _kernel->tuple_check( _obj ) == true )
                {
                    size_t size = _kernel->tuple_size( _obj );

                    for( size_t index = 0; index != size; ++index )
                    {
                        ConstString tag = pybind::tuple_getitem_t( _kernel, _obj, index );

                        _place->addTag( tag );
                    }

                    return true;
                }
                else if( _kernel->list_check( _obj ) == true )
                {
                    size_t size = _kernel->list_size( _obj );

                    for( size_t index = 0; index != size; ++index )
                    {
                        ConstString tag = pybind::list_getitem_t( _kernel, _obj, index );

                        _place->addTag( tag );
                    }

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static String Tags_repr( const Tags * _t )
            {
                return Helper::tagsToString( *_t );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    HelperScriptEmbedding::HelperScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HelperScriptEmbedding::~HelperScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool HelperScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::enum_<EKeyCode>( _kernel, "KeyCode" )
            .def( "KC_UNASSIGNED", KC_UNASSIGNED )
            .def( "KC_LBUTTON", KC_LBUTTON )
            .def( "KC_RBUTTON", KC_RBUTTON )
            .def( "KC_CANCEL", KC_CANCEL )
            .def( "KC_MBUTTON", KC_MBUTTON )
            .def( "KC_XBUTTON1", KC_XBUTTON1 )
            .def( "KC_XBUTTON2", KC_XBUTTON2 )
            .def( "KC_BACK", KC_BACK )
            .def( "KC_TAB", KC_TAB )
            .def( "KC_CLEAR", KC_CLEAR )
            .def( "KC_RETURN", KC_RETURN )
            .def( "KC_SHIFT", KC_SHIFT )
            .def( "KC_CONTROL", KC_CONTROL )
            .def( "KC_MENU", KC_MENU )
            .def( "KC_PAUSE", KC_PAUSE )
            .def( "KC_CAPITAL", KC_CAPITAL )
            .def( "KC_KANA", KC_KANA )
            .def( "KC_HANGUEL", KC_HANGUEL )
            .def( "KC_HANGUL", KC_HANGUL )
            .def( "KC_JUNJA", KC_JUNJA )
            .def( "KC_FINAL", KC_FINAL )
            .def( "KC_HANJA", KC_HANJA )
            .def( "KC_KANJI", KC_KANJI )
            .def( "KC_ESCAPE", KC_ESCAPE )
            .def( "KC_CONVERT", KC_CONVERT )
            .def( "KC_NOCONVERT", KC_NOCONVERT )
            .def( "KC_ACCEPT", KC_ACCEPT )
            .def( "KC_MODECHANGE", KC_MODECHANGE )
            .def( "KC_SPACE", KC_SPACE )
            .def( "KC_PRIOR", KC_PRIOR )
            .def( "KC_NEXT", KC_NEXT )
            .def( "KC_END", KC_END )
            .def( "KC_HOME", KC_HOME )
            .def( "KC_LEFT", KC_LEFT )
            .def( "KC_UP", KC_UP )
            .def( "KC_RIGHT", KC_RIGHT )
            .def( "KC_DOWN", KC_DOWN )
            .def( "KC_SELECT", KC_SELECT )
            .def( "KC_PRINT", KC_PRINT )
            .def( "KC_EXECUTE", KC_EXECUTE )
            .def( "KC_SNAPSHOT", KC_SNAPSHOT )
            .def( "KC_INSERT", KC_INSERT )
            .def( "KC_DELETE", KC_DELETE )
            .def( "KC_HELP", KC_HELP )
            .def( "KC_0", KC_0 )
            .def( "KC_1", KC_1 )
            .def( "KC_2", KC_2 )
            .def( "KC_3", KC_3 )
            .def( "KC_4", KC_4 )
            .def( "KC_5", KC_5 )
            .def( "KC_6", KC_6 )
            .def( "KC_7", KC_7 )
            .def( "KC_8", KC_8 )
            .def( "KC_9", KC_9 )
            .def( "KC_A", KC_A )
            .def( "KC_B", KC_B )
            .def( "KC_C", KC_C )
            .def( "KC_D", KC_D )
            .def( "KC_E", KC_E )
            .def( "KC_F", KC_F )
            .def( "KC_G", KC_G )
            .def( "KC_H", KC_H )
            .def( "KC_I", KC_I )
            .def( "KC_J", KC_J )
            .def( "KC_K", KC_K )
            .def( "KC_L", KC_L )
            .def( "KC_M", KC_M )
            .def( "KC_N", KC_N )
            .def( "KC_O", KC_O )
            .def( "KC_P", KC_P )
            .def( "KC_Q", KC_Q )
            .def( "KC_R", KC_R )
            .def( "KC_S", KC_S )
            .def( "KC_T", KC_T )
            .def( "KC_U", KC_U )
            .def( "KC_V", KC_V )
            .def( "KC_W", KC_W )
            .def( "KC_X", KC_X )
            .def( "KC_Y", KC_Y )
            .def( "KC_Z", KC_Z )
            .def( "KC_LWIN", KC_LWIN )
            .def( "KC_RWIN", KC_RWIN )
            .def( "KC_APPS", KC_APPS )
            .def( "KC_SLEEP", KC_SLEEP )
            .def( "KC_NUMPAD0", KC_NUMPAD0 )
            .def( "KC_NUMPAD1", KC_NUMPAD1 )
            .def( "KC_NUMPAD2", KC_NUMPAD2 )
            .def( "KC_NUMPAD3", KC_NUMPAD3 )
            .def( "KC_NUMPAD4", KC_NUMPAD4 )
            .def( "KC_NUMPAD5", KC_NUMPAD5 )
            .def( "KC_NUMPAD6", KC_NUMPAD6 )
            .def( "KC_NUMPAD7", KC_NUMPAD7 )
            .def( "KC_NUMPAD8", KC_NUMPAD8 )
            .def( "KC_NUMPAD9", KC_NUMPAD9 )
            .def( "KC_MULTIPLY", KC_MULTIPLY )
            .def( "KC_ADD", KC_ADD )
            .def( "KC_SEPARATOR", KC_SEPARATOR )
            .def( "KC_SUBTRACT", KC_SUBTRACT )
            .def( "KC_DECIMAL", KC_DECIMAL )
            .def( "KC_DIVIDE", KC_DIVIDE )
            .def( "KC_F1", KC_F1 )
            .def( "KC_F2", KC_F2 )
            .def( "KC_F3", KC_F3 )
            .def( "KC_F4", KC_F4 )
            .def( "KC_F5", KC_F5 )
            .def( "KC_F6", KC_F6 )
            .def( "KC_F7", KC_F7 )
            .def( "KC_F8", KC_F8 )
            .def( "KC_F9", KC_F9 )
            .def( "KC_F10", KC_F10 )
            .def( "KC_F11", KC_F11 )
            .def( "KC_F12", KC_F12 )
            .def( "KC_F13", KC_F13 )
            .def( "KC_F14", KC_F14 )
            .def( "KC_F15", KC_F15 )
            .def( "KC_F16", KC_F16 )
            .def( "KC_F17", KC_F17 )
            .def( "KC_F18", KC_F18 )
            .def( "KC_F19", KC_F19 )
            .def( "KC_F20", KC_F20 )
            .def( "KC_F21", KC_F21 )
            .def( "KC_F22", KC_F22 )
            .def( "KC_F23", KC_F23 )
            .def( "KC_F24", KC_F24 )
            .def( "KC_NUMLOCK", KC_NUMLOCK )
            .def( "KC_SCROLL", KC_SCROLL )
            .def( "KC_LSHIFT", KC_LSHIFT )
            .def( "KC_RSHIFT", KC_RSHIFT )
            .def( "KC_LCONTROL", KC_LCONTROL )
            .def( "KC_RCONTROL", KC_RCONTROL )
            .def( "KC_LMENU", KC_LMENU )
            .def( "KC_RMENU", KC_RMENU )
            .def( "KC_BROWSER_BACK", KC_BROWSER_BACK )
            .def( "KC_BROWSER_FORWARD", KC_BROWSER_FORWARD )
            .def( "KC_BROWSER_REFRESH", KC_BROWSER_REFRESH )
            .def( "KC_BROWSER_STOP", KC_BROWSER_STOP )
            .def( "KC_BROWSER_SEARCH", KC_BROWSER_SEARCH )
            .def( "KC_BROWSER_FAVORITES", KC_BROWSER_FAVORITES )
            .def( "KC_BROWSER_HOME", KC_BROWSER_HOME )
            .def( "KC_VOLUME_MUTE", KC_VOLUME_MUTE )
            .def( "KC_VOLUME_DOWN", KC_VOLUME_DOWN )
            .def( "KC_VOLUME_UP", KC_VOLUME_UP )
            .def( "KC_MEDIA_NEXT_TRACK", KC_MEDIA_NEXT_TRACK )
            .def( "KC_MEDIA_PREV_TRACK", KC_MEDIA_PREV_TRACK )
            .def( "KC_MEDIA_STOP", KC_MEDIA_STOP )
            .def( "KC_MEDIA_PLAY_PAUSE", KC_MEDIA_PLAY_PAUSE )
            .def( "KC_LAUNCH_MAIL", KC_LAUNCH_MAIL )
            .def( "KC_LAUNCH_MEDIA_SELECT", KC_LAUNCH_MEDIA_SELECT )
            .def( "KC_LAUNCH_APP1", KC_LAUNCH_APP1 )
            .def( "KC_LAUNCH_APP2", KC_LAUNCH_APP2 )
            .def( "KC_OEM_1", KC_OEM_1 )
            .def( "KC_OEM_PLUS", KC_OEM_PLUS )
            .def( "KC_OEM_COMMA", KC_OEM_COMMA )
            .def( "KC_OEM_MINUS", KC_OEM_MINUS )
            .def( "KC_OEM_PERIOD", KC_OEM_PERIOD )
            .def( "KC_OEM_2", KC_OEM_2 )
            .def( "KC_OEM_3", KC_OEM_3 )
            .def( "KC_OEM_4", KC_OEM_4 )
            .def( "KC_OEM_5", KC_OEM_5 )
            .def( "KC_OEM_6", KC_OEM_6 )
            .def( "KC_OEM_7", KC_OEM_7 )
            .def( "KC_OEM_8", KC_OEM_8 )
            .def( "KC_OEM_102", KC_OEM_102 )
            .def( "KC_PROCESSKEY", KC_PROCESSKEY )
            .def( "KC_PACKET", KC_PACKET )
            .def( "KC_ATTN", KC_ATTN )
            .def( "KC_CRSEL", KC_CRSEL )
            .def( "KC_EXSEL", KC_EXSEL )
            .def( "KC_EREOF", KC_EREOF )
            .def( "KC_PLAY", KC_PLAY )
            .def( "KC_ZOOM", KC_ZOOM )
            .def( "KC_NONAME", KC_NONAME )
            .def( "KC_PA1", KC_PA1 )
            .def( "KC_OEM_CLEAR", KC_OEM_CLEAR )
            ;

        pybind::enum_<EMouseButtonCode>( _kernel, "MouseButtonCode" )
            .def( "MC_LBUTTON", MC_LBUTTON )
            .def( "MC_RBUTTON", MC_RBUTTON )
            .def( "MC_MBUTTON", MC_MBUTTON )
            .def( "MC_X1BUTTON", MC_X1BUTTON )
            .def( "MC_X2BUTTON", MC_X2BUTTON )
            ;

        pybind::enum_<ETouchCode>( _kernel, "TouchCode" )
            .def( "TC_TOUCH0", TC_TOUCH0 )
            .def( "TC_TOUCH1", TC_TOUCH1 )
            .def( "TC_TOUCH2", TC_TOUCH2 )
            .def( "TC_TOUCH3", TC_TOUCH3 )
            .def( "TC_TOUCH4", TC_TOUCH4 )
            .def( "TC_TOUCH5", TC_TOUCH5 )
            .def( "TC_TOUCH6", TC_TOUCH6 )
            .def( "TC_TOUCH7", TC_TOUCH7 )
            .def( "TC_TOUCH8", TC_TOUCH8 )
            .def( "TC_TOUCH9", TC_TOUCH9 )
            .def( "TC_TOUCH10", TC_TOUCH10 )
            .def( "TC_TOUCH11", TC_TOUCH11 )
            .def( "TC_TOUCH12", TC_TOUCH12 )
            .def( "TC_TOUCH13", TC_TOUCH13 )
            .def( "TC_TOUCH14", TC_TOUCH14 )
            .def( "TC_TOUCH15", TC_TOUCH15 )
            ;

        pybind::def_const<uint32_t>( _kernel, "MENGINE_INPUT_MAX_TOUCH", MENGINE_INPUT_MAX_TOUCH );

        pybind::registration_stl_optional_type_cast<Optional<mt::box2f>>(_kernel);

        pybind::registration_type_cast<Data>(_kernel, pybind::make_type_cast<extract_Data_type>(_kernel));
        pybind::registration_type_cast<Tags>(_kernel, pybind::make_type_cast<extract_Tags_type>(_kernel));

        pybind::registration_stl_vector_type_cast<VectorResourceImages>(_kernel);
        pybind::registration_stl_vector_type_cast<VectorHotSpotPolygons>(_kernel);

        pybind::registration_type_cast<URLString>(_kernel, pybind::make_type_cast<extract_URLString_type>(_kernel));

        pybind::registration_type_cast<String>(_kernel, pybind::make_type_cast<extract_String_type>(_kernel));
        pybind::registration_type_cast<WString>(_kernel, pybind::make_type_cast<extract_WString_type>(_kernel));

        pybind::registration_stl_vector_type_cast<VectorString>(_kernel);
        pybind::registration_stl_vector_type_cast<VectorWString>(_kernel);

        pybind::registration_type_cast<ParamVariant>(_kernel, pybind::make_type_cast<extract_ParamVariant_type>(_kernel));

        pybind::registration_stl_map_type_cast<Params>(_kernel);

        pybind::registration_stl_vector_type_cast<VectorRenderIndex>(_kernel);

        pybind::registration_stl_vector_type_cast<Vector<mt::vec2f>>(_kernel);
        pybind::registration_stl_vector_type_cast<Vector<mt::vec3f>>(_kernel);
        pybind::registration_stl_vector_type_cast<Vector<mt::vec4f>>(_kernel);

        pybind::struct_<Tags>( _kernel, "Tags" )
            .def_constructor( pybind::init<>() )
            .def_convert( &ScriptMethod::Tags_convert )
            .def_repr( &ScriptMethod::Tags_repr )
            .def( "addTag", &Tags::addTag )
            .def( "removeTag", &Tags::removeTag )
            .def( "hasTag", &Tags::hasTag )
            .def( "hasTags", &Tags::hasTags )
            .def( "empty", &Tags::empty )
            .def( "clear", &Tags::clear )
            ;

        pybind::struct_<PlatformDateTime>( _kernel, "PlatformDateTime" )
            .def_member( "year", &PlatformDateTime::year )
            .def_member( "month", &PlatformDateTime::month )
            .def_member( "day", &PlatformDateTime::day )
            .def_member( "hour", &PlatformDateTime::hour )
            .def_member( "minute", &PlatformDateTime::minute )
            .def_member( "second", &PlatformDateTime::second )
            .def_member( "milliseconds", &PlatformDateTime::milliseconds )
            ;

        pybind::def_function_kernel( _kernel, "logDebug", &Detail::s_logDebug );
        pybind::def_function_kernel( _kernel, "logInfo", &Detail::s_logInfo );
        pybind::def_function_kernel( _kernel, "logMessage", &Detail::s_logMessage );
        pybind::def_function_kernel( _kernel, "logWarning", &Detail::s_logWarning );
        pybind::def_function_kernel( _kernel, "logError", &Detail::s_logError );
        pybind::def_function_kernel( _kernel, "logMessageRelease", &Detail::s_logMessageRelease );
        pybind::def_function_kernel( _kernel, "logFatal", &Detail::s_logFatal );

        pybind::def_function( _kernel, "enumerator", &Detail::s_enumerator );
        pybind::def_function( _kernel, "filterpowf", &Detail::s_filterpowf );

        pybind::def_function( _kernel, "rand", &Detail::s_rand );
        pybind::def_function( _kernel, "randi", &Detail::s_randi );
        pybind::def_function( _kernel, "randf", &Detail::s_randf );
        pybind::def_function( _kernel, "range_rand", &Detail::s_range_rand );
        pybind::def_function( _kernel, "range_randi", &Detail::s_range_randi );
        pybind::def_function( _kernel, "range_randf", &Detail::s_range_randf );
        pybind::def_function( _kernel, "around_randf", &Detail::s_around_randf );
        pybind::def_function( _kernel, "radius_randf", &Detail::s_radius_randf );

        pybind::def_function_kernel( _kernel, "angle_correct_interpolate_from_to", &Detail::s_angle_correct_interpolate_from_to );
        pybind::def_function( _kernel, "angle_between_two_vectors", &Detail::s_angle_between_two_vectors );

        pybind::def_function( _kernel, "projectionPointToLine", &Detail::s_projectionPointToLine );
        pybind::def_function( _kernel, "isPointInsidePolygon", &Detail::s_isPointInsidePolygon );

        pybind::def_function( _kernel, "getTime", &Detail::s_getTime );
        pybind::def_function( _kernel, "getTimeMs", &Detail::s_getTimeMs );


        pybind::def_function( _kernel, "getLocalDateTimeMs", &Detail::s_getLocalDateTimeMs );
        pybind::def_function( _kernel, "getLocalDateStruct", &Detail::s_getLocalDateStruct );
        pybind::def_function( _kernel, "getLocalDateStructFromTimeMs", &Detail::s_getLocalDateStructFromTimeMs );

        pybind::def_function( _kernel, "getDatePathTimestamp", &Detail::s_getDatePathTimestamp );
        pybind::def_function( _kernel, "getLoggerTimestamp", &Detail::s_getLoggerTimestamp );

        pybind::def_function( _kernel, "getTimeString", &Detail::s_getTimeString );

        pybind::def_function( _kernel, "getConfigBool", &Detail::s_getConfigBool );
        pybind::def_function( _kernel, "getConfigInt", &Detail::s_getConfigInt );
        pybind::def_function( _kernel, "getConfigUInt", &Detail::s_getConfigUInt );
        pybind::def_function( _kernel, "getConfigUInt64", &Detail::s_getConfigUInt64 );
        pybind::def_function( _kernel, "getConfigFloat", &Detail::s_getConfigFloat );
        pybind::def_function( _kernel, "getConfigString", &Detail::s_getConfigString );
        pybind::def_function( _kernel, "getConfigStrings", &Detail::s_getConfigStrings );

        pybind::def_function( _kernel, "getAccounts", &Detail::s_getAccounts );

        pybind::def_function_kernel( _kernel, "getCurrentAccountUID", &Detail::s_getCurrentAccountUID );

        pybind::def_function_kernel( _kernel, "getCurrentAccountSetting", &Detail::s_getCurrentAccountSetting );
        pybind::def_function_kernel( _kernel, "getCurrentAccountSettingBool", &Detail::s_getCurrentAccountSettingBool );
        pybind::def_function_kernel( _kernel, "getCurrentAccountSettingInt", &Detail::s_getCurrentAccountSettingInt );
        pybind::def_function_kernel( _kernel, "getCurrentAccountSettingUInt", &Detail::s_getCurrentAccountSettingUInt );
        pybind::def_function_kernel( _kernel, "getCurrentAccountSettingUInt64", &Detail::s_getCurrentAccountSettingUInt64 );
        pybind::def_function_kernel( _kernel, "getCurrentAccountSettingFloat", &Detail::s_getCurrentAccountSettingFloat );
        pybind::def_function_kernel( _kernel, "getCurrentAccountSettingStrings", &Detail::s_getCurrentAccountSettingStrings );
        pybind::def_function( _kernel, "getCurrentAccountSettingFloatDefault", &Detail::s_getCurrentAccountSettingFloatDefault );

        pybind::def_function_kernel_args( _kernel, "addCurrentAccountSetting", &Detail::s_addCurrentAccountSetting );
        pybind::def_function( _kernel, "hasCurrentAccountSetting", &Detail::s_hasCurrentAccountSetting );
        pybind::def_function_kernel( _kernel, "changeCurrentAccountSetting", &Detail::s_changeCurrentAccountSetting );
        pybind::def_function( _kernel, "changeCurrentAccountSettingBool", &Detail::s_changeCurrentAccountSettingBool );
        pybind::def_function( _kernel, "changeCurrentAccountSettingInt", &Detail::s_changeCurrentAccountSettingInt );
        pybind::def_function( _kernel, "changeCurrentAccountSettingUInt", &Detail::s_changeCurrentAccountSettingUInt );
        pybind::def_function( _kernel, "changeCurrentAccountSettingUInt64", &Detail::s_changeCurrentAccountSettingUInt64 );
        pybind::def_function( _kernel, "changeCurrentAccountSettingFloat", &Detail::s_changeCurrentAccountSettingFloat );
        pybind::def_function( _kernel, "changeCurrentAccountSettingStrings", &Detail::s_changeCurrentAccountSettingStrings );

        pybind::def_function_kernel( _kernel, "getAccountUID", &Detail::s_getAccountUID );

        pybind::def_function_kernel( _kernel, "getAccountSetting", &Detail::s_getAccountSetting );
        pybind::def_function_kernel( _kernel, "getAccountSettingBool", &Detail::s_getAccountSettingBool );
        pybind::def_function_kernel( _kernel, "getAccountSettingInt", &Detail::s_getAccountSettingInt );
        pybind::def_function_kernel( _kernel, "getAccountSettingUInt", &Detail::s_getAccountSettingUInt );
        pybind::def_function_kernel( _kernel, "getAccountSettingUInt64", &Detail::s_getAccountSettingUInt64 );
        pybind::def_function_kernel( _kernel, "getAccountSettingFloat", &Detail::s_getAccountSettingFloat );
        pybind::def_function_kernel( _kernel, "getAccountSettingStrings", &Detail::s_getAccountSettingStrings );
        pybind::def_function( _kernel, "getAccountSettingFloatDefault", &Detail::s_getAccountSettingFloatDefault );

        pybind::def_function_kernel_args( _kernel, "addAccountSetting", &Detail::s_addAccountSetting );
        pybind::def_function( _kernel, "hasAccountSetting", &Detail::s_hasAccountSetting );
        pybind::def_function_kernel( _kernel, "changeAccountSetting", &Detail::s_changeAccountSetting );
        pybind::def_function( _kernel, "changeAccountSettingBool", &Detail::s_changeAccountSettingBool );
        pybind::def_function( _kernel, "changeAccountSettingInt", &Detail::s_changeAccountSettingInt );
        pybind::def_function( _kernel, "changeAccountSettingUInt", &Detail::s_changeAccountSettingUInt );
        pybind::def_function( _kernel, "changeAccountSettingUInt64", &Detail::s_changeAccountSettingUInt64 );
        pybind::def_function( _kernel, "changeAccountSettingFloat", &Detail::s_changeAccountSettingFloat );
        pybind::def_function( _kernel, "changeAccountSettingStrings", &Detail::s_changeAccountSettingStrings );

        pybind::def_function_kernel( _kernel, "getGlobalAccountUID", &Detail::s_getGlobalAccountUID );

        pybind::def_function_kernel( _kernel, "getGlobalSetting", &Detail::s_getGlobalSetting );
        pybind::def_function_kernel( _kernel, "getGlobalSettingBool", &Detail::s_getGlobalSettingBool );
        pybind::def_function_kernel( _kernel, "getGlobalSettingInt", &Detail::s_getGlobalSettingInt );
        pybind::def_function_kernel( _kernel, "getGlobalSettingUInt", &Detail::s_getGlobalSettingUInt );
        pybind::def_function_kernel( _kernel, "getGlobalSettingUInt64", &Detail::s_getGlobalSettingUInt64 );
        pybind::def_function_kernel( _kernel, "getGlobalSettingFloat", &Detail::s_getGlobalSettingFloat );
        pybind::def_function_kernel( _kernel, "getGlobalSettingStrings", &Detail::s_getGlobalSettingStrings );

        pybind::def_function_kernel_args( _kernel, "addGlobalSetting", &Detail::s_addGlobalSetting );
        pybind::def_function( _kernel, "hasGlobalSetting", &Detail::s_hasGlobalSetting );
        pybind::def_function_kernel( _kernel, "changeGlobalSetting", &Detail::s_changeGlobalSetting );
        pybind::def_function( _kernel, "changeGlobalSettingBool", &Detail::s_changeGlobalSettingBool );
        pybind::def_function( _kernel, "changeGlobalSettingInt", &Detail::s_changeGlobalSettingInt );
        pybind::def_function( _kernel, "changeGlobalSettingUint", &Detail::s_changeGlobalSettingUInt );
        pybind::def_function( _kernel, "changeGlobalSettingUInt64", &Detail::s_changeGlobalSettingUInt64 );
        pybind::def_function( _kernel, "changeGlobalSettingFloat", &Detail::s_changeGlobalSettingFloat );
        pybind::def_function( _kernel, "changeGlobalSettingStrings", &Detail::s_changeGlobalSettingStrings );

        pybind::def_function_kernel( _kernel, "createAccount", &Detail::s_createAccount );
        pybind::def_function_kernel( _kernel, "createGlobalAccount", &Detail::s_createGlobalAccount );

        pybind::def_function( _kernel, "selectAccount", &Detail::s_selectAccount );
        pybind::def_function( _kernel, "deleteAccount", &Detail::s_deleteAccount );

        pybind::def_function( _kernel, "saveAccount", &Detail::s_saveAccount );
        pybind::def_function( _kernel, "saveAccounts", &Detail::s_saveAccounts );
        pybind::def_function( _kernel, "saveAccountsInfo", &Detail::s_saveAccountsInfo );
        pybind::def_function( _kernel, "hasCurrentAccount", &Detail::s_hasCurrentAccount );
        pybind::def_function( _kernel, "getCurrentAccountName", &Detail::s_getCurrentAccountName );
        pybind::def_function( _kernel, "getCurrentAccountId", &Detail::s_getCurrentAccountName );

        pybind::def_function( _kernel, "setGlobalAccount", &Detail::s_setGlobalAccount );
        pybind::def_function( _kernel, "hasGlobalAccount", &Detail::s_hasGlobalAccount );
        pybind::def_function( _kernel, "getGlobalAccountName", &Detail::s_getGlobalAccountId );

        pybind::def_function( _kernel, "setDefaultAccount", &Detail::s_setDefaultAccount );
        pybind::def_function( _kernel, "getDefaultAccount", &Detail::s_getDefaultAccount );
        pybind::def_function( _kernel, "hasDefaultAccount", &Detail::s_hasDefaultAccount );
        pybind::def_function( _kernel, "isCurrentDefaultAccount", &Detail::s_isCurrentDefaultAccount );
        pybind::def_function( _kernel, "selectDefaultAccount", &Detail::s_selectDefaultAccount );

        pybind::def_function_kernel( _kernel, "writeGlobalPickleFile", &Detail::s_writeGlobalPickleFile );
        pybind::def_function_kernel( _kernel, "loadGlobalPickleFile", &Detail::s_loadGlobalPickleFile );

        pybind::def_function_kernel( _kernel, "writeAccountPickleFile", &Detail::s_writeAccountPickleFile );
        pybind::def_function_kernel( _kernel, "loadAccountPickleFile", &Detail::s_loadAccountPickleFile );
        pybind::def_function( _kernel, "hasAccountPickleFile", &Detail::s_hasAccountPickleFile );

        pybind::def_function( _kernel, "getDefaultFileGroup", &Detail::s_getDefaultFileGroup );
        pybind::def_function( _kernel, "hasFileGroup", &Detail::s_hasFileGroup );
        pybind::def_function( _kernel, "getFileGroup", &Detail::s_getFileGroup );

        pybind::def_function( _kernel, "setParticlesEnabled", &Detail::s_setParticlesEnabled );

        pybind::def_function( _kernel, "getNodeScreenAdaptPosition", &Detail::s_getNodeScreenAdaptPosition );
        pybind::def_function( _kernel, "fromWorldToScreenPosition", &Detail::s_fromWorldToScreenPosition );

        pybind::def_function( _kernel, "hasTextByKey", &Detail::s_hasTextByKey );
        pybind::def_function( _kernel, "getTextByKey", &Detail::s_getTextByKey );
        pybind::def_function( _kernel, "getTextCharCountByKey", &Detail::s_getTextCharCountByKey );


        pybind::def_function( _kernel, "setVSync", &Detail::s_setVSync );
        pybind::def_function( _kernel, "getVSync", &Detail::s_getVSync );
        pybind::def_function( _kernel, "setCursorMode", &Detail::s_setCursorMode );
        pybind::def_function( _kernel, "getCursorMode", &Detail::s_getCursorMode );
        pybind::def_function( _kernel, "setCursorIcon", &Detail::s_setCursorIcon );

        pybind::def_function( _kernel, "intersectsEllipseVsPoint", &Detail::s_intersectsEllipseVsPoint );
        pybind::def_function( _kernel, "intersectsBoxes", &Detail::s_intersectsBoxes );

        pybind::def_function_kernel( _kernel, "getPolygonPoints", &Detail::s_getPolygonPoints );
        pybind::def_function( _kernel, "intersectsPolygons", &Detail::s_intersectsPolygons );
        pybind::def_function( _kernel, "intersectsPolygonsWM", &Detail::s_intersectsPolygonsWM );
        pybind::def_function( _kernel, "intersectsPolygonsWMP", &Detail::s_intersectsPolygonsWMP );

        pybind::def_function_args( _kernel, "makeTags", &Detail::s_makeTags );


        pybind::def_function_kernel( _kernel, "textures", &Detail::s_textures );

        pybind::def_function( _kernel, "watchdog", &Detail::s_watchdog );

        pybind::def_function( _kernel, "addGlobalModule", &Detail::s_addGlobalModule );
        pybind::def_function( _kernel, "removeGlobalModule", &Detail::s_removeGlobalModule );

        pybind::def_function_kernel( _kernel, "getrefcount", &Detail::s_refcount );
        pybind::def_function_kernel( _kernel, "is_class", &Detail::s_is_class );
        pybind::def_function_kernel( _kernel, "is_type_class", &Detail::s_is_type_class );
        pybind::def_function_kernel( _kernel, "is_wrap", &Detail::s_is_wrap );

        pybind::def_function( _kernel, "setLocale", &Detail::s_setLocale );
        pybind::def_function( _kernel, "getLocale", &Detail::s_getLocale );
        pybind::def_function( _kernel, "hasLocale", &Detail::s_hasLocale );
        pybind::def_function( _kernel, "getLocales", &Detail::s_getLocales );

        pybind::def_function( _kernel, "isometric_length_v3_v3", &Detail::s_isometric_length_v3_v3 );
        pybind::def_function( _kernel, "isometric_sqrlength_v3_v3", &Detail::s_isometric_sqrlength_v3_v3 );

        pybind::def_function( _kernel, "isValidWindowMode", &Detail::s_isValidWindowMode );

        pybind::def_function( _kernel, "utf8ToUnicode", &Detail::s_utf8ToUnicode );
        pybind::def_function( _kernel, "unicodeToUtf8", &Detail::s_unicodeToUtf8 );
        pybind::def_function( _kernel, "setCursorPosition", &Detail::s_setCursorPosition );

        pybind::def_function( _kernel, "debug", &Detail::s_debug );
        pybind::def_function( _kernel, "debugOnce", &Detail::s_debugOnce );
        pybind::def_function( _kernel, "debugNode", &Detail::s_debugNode );

        pybind::def_function( _kernel, "hasGameData", &Detail::s_hasGameData );
        pybind::def_function_kernel( _kernel, "writeGameData", &Detail::s_writeGameData );
        pybind::def_function_kernel( _kernel, "loadGameData", &Detail::s_loadGameData );

        pybind::def_function( _kernel, "isSpecialDown", &Detail::s_isSpecialDown );
        pybind::def_function( _kernel, "isAltDown", &Detail::s_isAltDown );
        pybind::def_function( _kernel, "isShiftDown", &Detail::s_isShiftDown );
        pybind::def_function( _kernel, "isCtrlDown", &Detail::s_isCtrlDown );

        pybind::def_function( _kernel, "isShiftKeyCode", &Detail::s_isShiftKeyCode );
        pybind::def_function( _kernel, "isAltKeyCode", &Detail::s_isAltKeyCode );
        pybind::def_function( _kernel, "isControlKeyCode", &Detail::s_isControlKeyCode );
        pybind::def_function( _kernel, "isKeyDown", &Detail::s_isKeyDown );
        pybind::def_function( _kernel, "isExclusiveKeyDown", &Detail::s_isExclusiveKeyDown );
        pybind::def_function( _kernel, "isAnyKeyDown", &Detail::s_isAnyKeyDown );

        pybind::def_function( _kernel, "isAnyMouseButtonDown", &Detail::s_isAnyMouseButtonDown );
        pybind::def_function( _kernel, "isMouseButtonDown", &Detail::s_isMouseButtonDown );

        pybind::def_function( _kernel, "printChildren", &Detail::s_printChildren );

        pybind::def_function( _kernel, "getGroupResourcesMemoryUse", &Detail::s_getGroupResourcesMemoryUse );

        pybind::def_function( _kernel, "intersectPathVsCircle", &Detail::s_intersectPathVsCircle );
        pybind::def_function_kernel( _kernel, "selectRandomPointFromPathRadius", &Detail::s_selectRandomPointFromPathRadius );

        pybind::def_function_kernel( _kernel, "reloadModule", &Detail::s_reloadModule );

        pybind::def_function( _kernel, "makeUID", &Detail::s_makeUID );

        pybind::def_function( _kernel, "getTextFromId", &Detail::s_getTextFromId );

        pybind::def_function( _kernel, "setTextAlias", &Detail::s_setTextAlias );
        pybind::def_function( _kernel, "removeTextAlias", &Detail::s_removeTextAlias );
        pybind::def_function( _kernel, "hasTextAlias", &Detail::s_hasTextAlias );
        pybind::def_function( _kernel, "getTextAlias", &Detail::s_getTextAlias );

        pybind::def_function_args( _kernel, "setTextAliasArguments", &Detail::s_setTextAliasArguments );
        pybind::def_function( _kernel, "removeTextAliasArguments", &Detail::s_removeTextAliasArguments );

        pybind::def_function_args( _kernel, "addChronometer", &Detail::s_addChronometer );
        pybind::def_function( _kernel, "removeChronometer", &Detail::s_removeChronometer );

        pybind::def_function( _kernel, "getHotSpotPolygonBoundingBox", &Detail::s_getHotSpotPolygonBoundingBox );

        pybind::def_function( _kernel, "hasOption", &Detail::s_hasOption );
        pybind::def_function( _kernel, "getOptionValue", &Detail::s_getOptionValue );
        pybind::def_function_kernel( _kernel, "getOptionValues", &Detail::s_getOptionValues );
        pybind::def_function( _kernel, "getOptionInt32", &Detail::s_getOptionInt32 );
        pybind::def_function( _kernel, "getOptionUInt32", &Detail::s_getOptionUInt32 );

        pybind::def_function( _kernel, "crashEngine", &Detail::s_crashEngine );
        pybind::def_function( _kernel, "freezeEngine", &Detail::s_freezeEngine );
        pybind::def_function( _kernel, "memleakEngine", &Detail::s_memleakEngine );

        pybind::def_function( _kernel, "generateUniqueIdentity", &Detail::s_generateUniqueIdentity );

        pybind::def_function( _kernel, "setClipboardText", &Detail::s_setClipboardText );
        pybind::def_function_kernel( _kernel, "getClipboardText", &Detail::s_getClipboardText );

        pybind::def_function( _kernel, "makeCRC32", &Detail::s_makeCRC32 );
        pybind::def_function_kernel( _kernel, "compressBase64", &Detail::s_compressBase64 );
        pybind::def_function_kernel( _kernel, "decompressBase64", &Detail::s_decompressBase64 );

#if defined(MENGINE_PLATFORM_ANDROID)
        pybind::def_function_kernel( _kernel, "getAndroidId", &Detail::s_getAndroidId );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HelperScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        pybind::unregistration_stl_optional_type_cast<Optional<mt::box2f>>(_kernel);

        pybind::unregistration_type_cast<Data>(_kernel);
        pybind::unregistration_type_cast<Tags>(_kernel);

        pybind::unregistration_stl_vector_type_cast<VectorResourceImages>(_kernel);
        pybind::unregistration_stl_vector_type_cast<VectorHotSpotPolygons>(_kernel);

        pybind::unregistration_type_cast<ParamVariant>(_kernel);

        pybind::unregistration_stl_map_type_cast<Params>(_kernel);

        pybind::unregistration_type_cast<String>(_kernel);
        pybind::unregistration_type_cast<WString>(_kernel);

        pybind::unregistration_stl_vector_type_cast<Vector<String>>(_kernel);
        pybind::unregistration_stl_vector_type_cast<Vector<WString>>(_kernel);

        pybind::unregistration_stl_vector_type_cast<VectorRenderIndex>(_kernel);

        pybind::unregistration_stl_vector_type_cast<Vector<mt::vec2f>>(_kernel);
        pybind::unregistration_stl_vector_type_cast<Vector<mt::vec3f>>(_kernel);
        pybind::unregistration_stl_vector_type_cast<Vector<mt::vec4f>>(_kernel);
    }
    //////////////////////////////////////////////////////////////////////////
}
