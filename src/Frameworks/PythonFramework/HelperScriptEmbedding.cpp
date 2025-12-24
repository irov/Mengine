#include "HelperScriptEmbedding.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
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
        //////////////////////////////////////////////////////////////////////////
        class HelperScriptMethod
            : public Factorable
        {
        public:
            HelperScriptMethod()
            {
            }

            ~HelperScriptMethod() override
            {
            }

        public:
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_refcount( pybind::kernel_interface * _kernel, PyObject * _obj )
            {
                return _kernel->refcount( _obj );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_is_class( pybind::kernel_interface * _kernel, PyObject * _obj )
            {
                return _kernel->is_class( _obj );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_is_type_class( pybind::kernel_interface * _kernel, PyTypeObject * _type )
            {
                return _kernel->is_type_class( _type );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_is_wrap( pybind::kernel_interface * _kernel, PyObject * _obj )
            {
                return _kernel->is_wrap( _obj );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_setLocale( const ConstString & _locale )
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
            const ConstString & s_getLocale()
            {
                const ConstString & locale = APPLICATION_SERVICE()
                    ->getLocale();

                return locale;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasLocale( const ConstString & _locale )
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
            VectorConstString s_getLocales()
            {
                const Tags & platformTags = PLATFORM_SERVICE()
                    ->getPlatformTags();

                VectorConstString locales;
                PACKAGE_SERVICE()
                    ->getLocales( platformTags, &locales );

                return locales;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_isometric_length_v3_v3( const mt::vec3f & _v0, const mt::vec3f & _v1 )
            {
                mt::vec3f iso_v0 = _v0;
                iso_v0.y *= 2.f;

                mt::vec3f iso_v1 = _v1;
                iso_v1.y *= 2.f;

                float iso_length = mt::length_v3_v3( iso_v0, iso_v1 );

                return iso_length;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_isometric_sqrlength_v3_v3( const mt::vec3f & _v0, const mt::vec3f & _v1 )
            {
                mt::vec3f iso_v0 = _v0;
                iso_v0.y *= 2.f;

                mt::vec3f iso_v1 = _v1;
                iso_v1.y *= 2.f;

                float iso_length = mt::sqrlength_v3_v3( iso_v0, iso_v1 );

                return iso_length;
            }
            //////////////////////////////////////////////////////////////////////////
            WString s_utf8ToUnicode( const String & _utf8 )
            {
                WString unicode;
                Helper::utf8ToUnicode( _utf8, &unicode );

                return unicode;
            }
            //////////////////////////////////////////////////////////////////////////
            String s_unicodeToUtf8( const WString & _unicode )
            {
                String utf8;
                Helper::unicodeToUtf8( _unicode, &utf8 );

                return utf8;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_debug()
            {
                LOGGER_MESSAGE_RELEASE( "debug!\n" );

#if defined(MENGINE_TOOLCHAIN_MSVC) && !defined(I3D_ARCH_ARM) && !defined(MENGINE_PLATFORM_WINDOWS64)
                _asm int 3;
#endif
            }
            //////////////////////////////////////////////////////////////////////////
            void s_debugOnce()
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
            void s_debugNode( Node * _node )
            {
                MENGINE_UNUSED( _node );

                LOGGER_MESSAGE_RELEASE( "debug node!\n" );

#if defined(MENGINE_TOOLCHAIN_MSVC) && !defined(I3D_ARCH_ARM) && !defined(MENGINE_PLATFORM_WINDOWS64)
                _asm int 3;
#endif
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasGameData( const ConstString & _name )
            {
                bool result = USERDATA_SERVICE()
                    ->hasUserdata( _name );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_writeGameData( pybind::kernel_interface * _kernel, const ConstString & _name, PyObject * _data, PyObject * _pickleTypes )
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
            PyObject * s_loadGameData( pybind::kernel_interface * _kernel, const ConstString & _name, PyObject * _pickleTypes )
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
            bool s_isSpecialDown() const
            {
                return INPUT_SERVICE()
                    ->isSpecialDown();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isAltDown() const
            {
                return INPUT_SERVICE()
                    ->isAltDown();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isShiftDown() const
            {
                return INPUT_SERVICE()
                    ->isShiftDown();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isCtrlDown() const
            {
                return INPUT_SERVICE()
                    ->isControlDown();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isShiftKeyCode( EKeyCode _keyCode ) const
            {
                return _keyCode == KC_SHIFT || _keyCode == KC_LSHIFT || _keyCode == KC_RSHIFT;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isAltKeyCode( EKeyCode _keyCode ) const
            {
                return _keyCode == KC_MENU || _keyCode == KC_LMENU || _keyCode == KC_RMENU;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isControlKeyCode( EKeyCode _keyCode ) const
            {
                return _keyCode == KC_CONTROL || _keyCode == KC_LCONTROL || _keyCode == KC_RCONTROL;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isKeyDown( EKeyCode _keyCode ) const
            {
                return INPUT_SERVICE()
                    ->isKeyDown( _keyCode );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isExclusiveKeyDown( EKeyCode _keyCode ) const
            {
                return INPUT_SERVICE()
                    ->isExclusiveKeyDown( _keyCode );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isAnyKeyDown() const
            {
                return INPUT_SERVICE()
                    ->isAnyKeyDown();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isAnyMouseButtonDown() const
            {
                return INPUT_SERVICE()
                    ->isAnyMouseButtonDown();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isMouseButtonDown( EMouseButtonCode _button ) const
            {
                return INPUT_SERVICE()
                    ->isMouseButtonDown( _button );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_printChildren2( const NodePtr & _node, uint32_t _tab )
            {
                _node->foreachChildrenSlug( [this, _tab]( const NodePtr & _child )
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
            void s_printChildren( const NodePtr & _node )
            {
                s_printChildren2( _node, 0 );
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_getGroupResourcesMemoryUse( const ConstString & _group )
            {
                MENGINE_UNUSED( _group );

                return 0U;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_intersectPathVsCircle( const pybind::list & _path, const mt::vec3f & _position, float _radius )
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
            pybind::object s_selectRandomPointFromPathRadius( pybind::kernel_interface * _kernel, const pybind::list & _path, const mt::vec3f & _position, float _radius )
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
            PyObject * s_reloadModule( pybind::kernel_interface * _kernel, PyObject * _module )
            {
                return _kernel->module_reload( _module );
            }
            //////////////////////////////////////////////////////////////////////////
            String s_makeUID( uint32_t _length )
            {
                MENGINE_ASSERTION_FATAL( _length < 1024, "max UID length equal 1024" );

                Char uid[1024 + 1] = {'\0'};
                Helper::makeUID( _length, uid );

                return String( uid, _length );
            }
            //////////////////////////////////////////////////////////////////////////
            const String & s_getTextFromId( const ConstString & _textId )
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
            void s_setTextAlias( const ConstString & _aliasEnvironment, const ConstString & _aliasId, const ConstString & _textId )
            {
                TEXT_SERVICE()
                    ->setTextAlias( _aliasEnvironment, _aliasId, _textId );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_removeTextAlias( const ConstString & _aliasEnvironment, const ConstString & _aliasId )
            {
                TEXT_SERVICE()
                    ->removeTextAlias( _aliasEnvironment, _aliasId );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasTextAlias( const ConstString & _aliasEnvironment, const ConstString & _aliasId )
            {
                bool exist = TEXT_SERVICE()
                    ->hasTextAlias( _aliasEnvironment, _aliasId );

                return exist;
            }
            //////////////////////////////////////////////////////////////////////////
            const ConstString & s_getTextAlias( const ConstString & _aliasEnvironment, const ConstString & _aliasId )
            {
                const ConstString & textId = TEXT_SERVICE()
                    ->getTextAlias( _aliasEnvironment, _aliasId );

                return textId;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_setTextAliasArguments( const ConstString & _aliasEnvironment, const ConstString & _aliasId, const pybind::args & _args )
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
            void s_removeTextAliasArguments( const ConstString & _aliasEnvironment, const ConstString & _aliasId )
            {
                TEXT_SERVICE()
                    ->removeTextAliasArguments( _aliasEnvironment, _aliasId );
            }
            //////////////////////////////////////////////////////////////////////////
            ChronometerInterfacePtr s_addChronometer( const pybind::object & _cb, const pybind::args & _args )
            {
                ChronometerInterfacePtr chronometer = CHRONOMETER_SERVICE()
                    ->addChronometer( [_cb, _args]( Timestamp _time )
                {
                    _cb.call_args( _time, _args );
                }, MENGINE_DOCUMENT_PYTHON );

                return chronometer;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_removeChronometer( const ChronometerInterfacePtr & _chronometer )
            {
                bool successful = CHRONOMETER_SERVICE()
                    ->removeChronometer( _chronometer );

                return successful;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::box2f s_getHotSpotPolygonBoundingBox( const HotSpotPolygonPtr & _hotspotPolygon )
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
            bool s_hasOption( const Char * _option )
            {
                bool exist = OPTIONS_SERVICE()
                    ->hasOption( _option, false );

                return exist;
            }
            //////////////////////////////////////////////////////////////////////////
            const Char * s_getOptionValue( const Char * _option )
            {
                const Char * value = OPTIONS_SERVICE()
                    ->getOptionValue( _option, "" );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::list s_getOptionValues( pybind::kernel_interface * _kernel, const Char * _option )
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
            int32_t s_getOptionInt32( const Char * _option )
            {
                uint32_t value = OPTIONS_SERVICE()
                    ->getOptionInt32( _option, 0 );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_getOptionUInt32( const Char * _option )
            {
                uint32_t value = OPTIONS_SERVICE()
                    ->getOptionUInt32( _option, 0U );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_crashEngine()
            {
                Helper::crash( "python" );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_freezeEngine()
            {
                uint32_t index = 0;
                for( ;;)
                {
                    ++index;
                }
            }
            //////////////////////////////////////////////////////////////////////////
            void s_memleakEngine( uint32_t _size, uint32_t _count )
            {
                for( uint32_t i = 0; i != _count; ++i )
                {
                    Helper::allocateMemory( _size, "pymemleak" );
                }
            }
            //////////////////////////////////////////////////////////////////////////
            String s_generateUniqueIdentity( uint32_t _length )
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
            bool s_setClipboardText( const Char * _value )
            {
                if( PLATFORM_SERVICE()
                    ->setClipboardText( _value ) == false )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_compressBase64( pybind::kernel_interface * _kernel, const ConstString & _archivatorType, const StringView & _data )
            {
                const Char * str = _data.c_str();
                size_t size = _data.size();

                ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), _archivatorType );

                size_t compressSize2 = archivator->compressBound( size );

                MemoryInputInterfacePtr memory = MEMORY_SERVICE()
                    ->createMemoryInput( MENGINE_DOCUMENT_FACTORABLE );

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
            PyObject * s_decompressBase64( pybind::kernel_interface * _kernel, const ConstString & _archivatorType, const StringView & _base64 )
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
            PyObject * s_getClipboardText( pybind::kernel_interface * _kernel )
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
            PyObject * s_getAndroidId( pybind::kernel_interface * _kernel )
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
            void s_setCursorPosition( const mt::vec2f & _cursorPosition )
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
            bool s_isValidWindowMode()
            {
                //bool check = APPLICATION_SERVICE()
                    //->isValidWindowMode();

                //return check;

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_addGlobalModule( const Char * _name, PyObject * _module )
            {
                SCRIPT_SERVICE()
                    ->addGlobalModule( _name, _module );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_removeGlobalModule( const Char * _name )
            {
                SCRIPT_SERVICE()
                    ->removeGlobalModule( _name );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_log( pybind::kernel_interface * _kernel, const StringView & _message, ELoggerLevel _level, uint32_t _color, uint32_t _flag )
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
                _kernel->get_statetrace_top( filename, MENGINE_MAX_PATH, function, MENGINE_CODE_MAX_FUNCTION_NAME, &lineno );

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
            void s_logDebug( pybind::kernel_interface * _kernel, const StringView & _message )
            {
                MENGINE_UNUSED( _kernel );
                MENGINE_UNUSED( _message );

#if defined(MENGINE_LOGGER_INFO_ENABLE)
                s_log( _kernel, _message, LM_DEBUG, LCOLOR_BLUE, LFLAG_SHORT );
#endif
            }
            //////////////////////////////////////////////////////////////////////////
            void s_logInfo( pybind::kernel_interface * _kernel, const StringView & _message )
            {
                MENGINE_UNUSED( _kernel );
                MENGINE_UNUSED( _message );

#if defined(MENGINE_LOGGER_INFO_ENABLE)
                s_log( _kernel, _message, LM_INFO, LCOLOR_GREEN | LCOLOR_BLUE, LFLAG_SHORT );
#endif
            }
            //////////////////////////////////////////////////////////////////////////
            void s_logMessage( pybind::kernel_interface * _kernel, const StringView & _message )
            {
                s_log( _kernel, _message, LM_MESSAGE, LCOLOR_GREEN, LFLAG_SHORT );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_logWarning( pybind::kernel_interface * _kernel, const StringView & _message )
            {
                s_log( _kernel, _message, LM_WARNING, LCOLOR_RED | LCOLOR_GREEN, LFLAG_SHORT );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_logError( pybind::kernel_interface * _kernel, const StringView & _message )
            {
                s_log( _kernel, _message, LM_ERROR, LCOLOR_RED, LFLAG_SHORT );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_logMessageRelease( pybind::kernel_interface * _kernel, const StringView & _message )
            {
                s_log( _kernel, _message, LM_MESSAGE_RELEASE, LCOLOR_RED | LCOLOR_BLUE, LFLAG_SHORT );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_logFatal( pybind::kernel_interface * _kernel, const StringView & _message )
            {
                s_log( _kernel, _message, LM_FATAL, LCOLOR_RED, LFLAG_FULL );
            }
            //////////////////////////////////////////////////////////////////////////
            UniqueId s_enumerator()
            {
                UniqueId new_id = Helper::generateUniqueIdentity();

                return new_id;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_filterpowf( const pybind::list & l, float _pow )
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
            uint32_t s_rand( uint32_t a )
            {
                const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                    ->getRandomizer();

                uint32_t value = randomizer->getRandom32( a );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            int32_t s_randi( int32_t a )
            {
                const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                    ->getRandomizer();

                uint32_t value = randomizer->getRandom32i( a );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_randf( float a )
            {
                const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                    ->getRandomizer();

                float value = randomizer->getRandomf( a );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_range_rand( uint32_t a, uint32_t b )
            {
                const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                    ->getRandomizer();

                uint32_t value = randomizer->getRandomRange32( a, b );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            int32_t s_range_randi( int32_t a, int32_t b )
            {
                const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                    ->getRandomizer();

                int32_t value = randomizer->getRandomRange32i( a, b );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_range_randf( float a, float b )
            {
                const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                    ->getRandomizer();

                float value = randomizer->getRandomRangef( a, b );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_around_randf( float a, float b )
            {
                float v_low = a - a * b;
                float v_hight = a + a * b;

                float v = s_range_randf( v_low, v_hight );

                return v;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_radius_randf( float _radius )
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
            //////////////////////////////////////////////////////////////////////////
            bool s_intersectsBoxes( const mt::vec2f & _minFirst, const mt::vec2f & _maxFirst, const mt::vec2f & _minSecond, const mt::vec2f & _maxSecond )
            {
                mt::box2f box_first( _minFirst, _maxFirst );
                mt::box2f box_second( _minSecond, _maxSecond );

                bool result = mt::box2_intersect( box_first, box_second );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_projectionPointToLine( const mt::vec2f & _point, const mt::vec2f & _v0, const mt::vec2f & _v1 )
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
            pybind::tuple s_angle_correct_interpolate_from_to( pybind::kernel_interface * _kernel, float _from, float _to )
            {
                float correct_angle_from;
                float correct_angle_to;

                mt::angle_correct_interpolate_from_to( _from, _to, &correct_angle_from, &correct_angle_to );

                pybind::tuple py_result = pybind::make_tuple_t( _kernel, correct_angle_from, correct_angle_to );

                return py_result;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_angle_between_two_vectors( const mt::vec2f & _v1, const mt::vec2f & _v2 )
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
            pybind::list s_getPolygonPoints( pybind::kernel_interface * _kernel, const Polygon & _polygon )
            {
                const VectorPoints & points = _polygon.getPoints();

                pybind::list py_list = pybind::make_list_container_t( _kernel, points );

                return py_list;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_intersectsPolygons( const Polygon & _p1, const Polygon & _p2 )
            {
                bool intersect = Helper::intersects( _p1, _p2 );

                return intersect;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_intersectsPolygonsWM( const mt::mat4f & _wm1, const Polygon & _p1, const mt::mat4f & _wm2, Polygon _p2 )
            {
                Polygon p1wm;
                _p1.mul_wm( &p1wm, _wm1 );

                Polygon p2wm;
                _p2.mul_wm( &p2wm, _wm2 );

                bool intersect = Helper::intersects( p1wm, p2wm );

                return intersect;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_intersectsPolygonsWMP( const mt::vec3f & _wm1, const Polygon & _p1, const mt::vec3f & _wm2, Polygon _p2 )
            {
                Polygon p1wm;
                _p1.transpose( &p1wm, _wm1.to_vec2f() );

                Polygon p2wm;
                _p2.transpose( &p2wm, _wm2.to_vec2f() );

                bool intersect = Helper::intersects( p1wm, p2wm );

                return intersect;
            }
            //////////////////////////////////////////////////////////////////////////
            Tags s_makeTags( const pybind::args & _args )
            {
                Tags t;
                for( const pybind::object & o : _args )
                {
                    t.addTag( o.extract() );
                }

                return t;
            }
            //////////////////////////////////////////////////////////////////////////
            pybind::list s_textures( pybind::kernel_interface * _kernel )
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
            double s_watchdog( const ConstString & _tag )
            {
                MENGINE_UNUSED( _tag );

                double watch = MENGINE_WATCHDOG( _tag );

                return watch;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isPointInsidePolygon( const mt::vec2f & _point, const Polygon & _polygon )
            {
                bool result = Helper::intersects( _polygon, _point );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            Timestamp s_getTimeMs()
            {
                Timestamp ms = Helper::getSystemTimestamp();

                return ms;
            }
            //////////////////////////////////////////////////////////////////////////
            Timestamp s_getTime()
            {
                Timestamp ms = s_getTimeMs();

                Timestamp s = ms / 1000;

                return s;
            }
            //////////////////////////////////////////////////////////////////////////
            Timestamp s_getLocalDateTimeMs()
            {
                Timestamp timestamp = Helper::getLocalTimestamp();

                return timestamp;
            }
            //////////////////////////////////////////////////////////////////////////
            PlatformDateTime s_getLocalDateStruct()
            {
                PlatformDateTime dateTime;
                DATETIME_SYSTEM()
                    ->getLocalDateTime( &dateTime );

                return dateTime;
            }
            //////////////////////////////////////////////////////////////////////////
            PlatformDateTime s_getLocalDateStructFromTimeMs( uint64_t _time )
            {
                PlatformDateTime dateTime;
                DATETIME_SYSTEM()
                    ->getDateTimeFromMilliseconds( _time, &dateTime );

                return dateTime;
            }
            //////////////////////////////////////////////////////////////////////////
            String s_getDatePathTimestamp()
            {
                Char pathTimestamp[1024 + 1] = {'\0'};
                Helper::makeFilePathDateTimestamp( pathTimestamp, 1024 );

                return pathTimestamp;
            }
            //////////////////////////////////////////////////////////////////////////
            String s_getLoggerTimestamp( const Char * _format )
            {
                Timestamp timestamp = Helper::getLocalTimestamp();

                Char shortDate[1024 + 1] = {'\0'};
                Helper::makeLoggerShortDate( timestamp, _format, shortDate, 0, 1024 );

                return shortDate;
            }
            //////////////////////////////////////////////////////////////////////////
            String s_getTimeString()
            {
                Char timestamp[256 + 1] = {'\0'};
                Helper::makeLoggerDateTimestamp( timestamp, 256 );

                return timestamp;
            }
            //////////////////////////////////////////////////////////////////////////
            VectorConstString s_getAccounts()
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
            bool s_addCurrentAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _setting, PyObject * _defaultValue, const pybind::object & _cb, const pybind::args & _args )
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
            bool s_changeCurrentAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _setting, PyObject * _value )
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
            bool s_changeCurrentAccountSettingBool( const ConstString & _setting, bool _value )
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
            bool s_changeCurrentAccountSettingInt( const ConstString & _setting, int32_t _value )
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
            bool s_changeCurrentAccountSettingUInt( const ConstString & _setting, uint32_t _value )
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
            bool s_changeCurrentAccountSettingUInt64( const ConstString & _setting, uint64_t _value )
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
            bool s_changeCurrentAccountSettingFloat( const ConstString & _setting, float _value )
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
            bool s_changeCurrentAccountSettingStrings( const ConstString & _setting, const VectorWString & _values )
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
            bool s_addAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting, PyObject * _defaultValue, const pybind::object & _cb, const pybind::args & _args )
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

                const Char * utf8_defaultValue = _kernel->unicode_to_utf8( _defaultValue );

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
            bool s_hasAccountSetting( const ConstString & _accountId, const ConstString & _setting )
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
            bool s_changeAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting, PyObject * _value )
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

                const Char * utf8_defaultValue = _kernel->unicode_to_utf8( _value );

                bool result = account->changeSetting( _setting, utf8_defaultValue );

                return result;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_changeAccountSettingBool( const ConstString & _accountId, const ConstString & _setting, bool _value )
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
            bool s_changeAccountSettingInt( const ConstString & _accountId, const ConstString & _setting, int32_t _value )
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
            bool s_changeAccountSettingUInt( const ConstString & _accountId, const ConstString & _setting, uint32_t _value )
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
            bool s_changeAccountSettingUInt64( const ConstString & _accountId, const ConstString & _setting, uint64_t _value )
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
            bool s_changeAccountSettingFloat( const ConstString & _accountId, const ConstString & _setting, float _value )
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
            bool s_changeAccountSettingStrings( const ConstString & _accountId, const ConstString & _setting, const VectorWString & _values )
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
            bool s_addGlobalSetting( pybind::kernel_interface * _kernel, const ConstString & _setting, PyObject * _defaultValue, const pybind::object & _cb, const pybind::args & _args )
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
            bool s_hasGlobalSetting( const ConstString & _setting )
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
            bool s_changeGlobalSetting( pybind::kernel_interface * _kernel, const ConstString & _setting, PyObject * _value )
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
            bool s_changeGlobalSettingBool( const ConstString & _setting, bool _value )
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
            bool s_changeGlobalSettingInt( const ConstString & _setting, int32_t _value )
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
            bool s_changeGlobalSettingUInt( const ConstString & _setting, uint32_t _value )
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
            bool s_changeGlobalSettingUInt64( const ConstString & _setting, uint64_t _value )
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
            bool s_changeGlobalSettingFloat( const ConstString & _setting, float _value )
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
            bool s_changeGlobalSettingStrings( const ConstString & _setting, const VectorWString & _values )
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
            bool s_hasCurrentAccountSetting( const ConstString & _setting )
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
            PyObject * s_getCurrentAccountUID( pybind::kernel_interface * _kernel )
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
            PyObject * s_getCurrentAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getCurrentAccountSettingBool( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getCurrentAccountSettingInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getCurrentAccountSettingUInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getCurrentAccountSettingUInt64( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getCurrentAccountSettingFloat( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getCurrentAccountSettingStrings( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            float s_getCurrentAccountSettingFloatDefault( const ConstString & _setting, float _default )
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
            bool s_getConfigBool( const Char * _section, const Char * _key, bool _default )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getMainConfig();

                bool value;
                config->hasValue( _section, _key, _default, &value );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            int32_t s_getConfigInt( const Char * _section, const Char * _key, int32_t _default )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getMainConfig();

                int32_t value;
                config->hasValueInteger( _section, _key, _default, &value );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            uint32_t s_getConfigUInt( const Char * _section, const Char * _key, uint32_t _default )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getMainConfig();

                uint32_t value;
                config->hasValueInteger( _section, _key, _default, &value );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            uint64_t s_getConfigUInt64( const Char * _section, const Char * _key, uint64_t _default )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getMainConfig();

                uint64_t value;
                config->hasValueInteger( _section, _key, _default, &value );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            float s_getConfigFloat( const Char * _section, const Char * _key, float _default )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getMainConfig();

                float value;
                config->hasValueFloat( _section, _key, _default, &value );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            String s_getConfigString( const Char * _section, const Char * _key, const Char * _default )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getMainConfig();

                String value;
                config->hasValue( _section, _key, _default, &value );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            VectorString s_getConfigStrings( const Char * _section, const Char * _key )
            {
                const ConfigInterfacePtr & config = CONFIG_SERVICE()
                    ->getMainConfig();

                VectorString values;
                config->getValues( _section, _key, &values );

                return values;
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_getAccountUID( pybind::kernel_interface * _kernel, const ConstString & _accountId )
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
            PyObject * s_getAccountSetting( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
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
            PyObject * s_getAccountSettingBool( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
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
            PyObject * s_getAccountSettingInt( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
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
            PyObject * s_getAccountSettingUInt( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
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
            PyObject * s_getAccountSettingUInt64( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
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
            PyObject * s_getAccountSettingStrings( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
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
            PyObject * s_getAccountSettingFloat( pybind::kernel_interface * _kernel, const ConstString & _accountId, const ConstString & _setting )
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
            float s_getAccountSettingFloatDefault( const ConstString & _accountId, const ConstString & _setting, float _default )
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
            PyObject * s_getGlobalAccountUID( pybind::kernel_interface * _kernel )
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
            PyObject * s_getGlobalSetting( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getGlobalSettingBool( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getGlobalSettingInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getGlobalSettingUInt( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getGlobalSettingUInt64( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getGlobalSettingFloat( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_getGlobalSettingStrings( pybind::kernel_interface * _kernel, const ConstString & _setting )
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
            PyObject * s_createAccount( pybind::kernel_interface * _kernel )
            {
                AccountInterfacePtr account = ACCOUNT_SERVICE()
                    ->createAccount( MENGINE_DOCUMENT_PYTHON );

                MENGINE_ASSERTION_MEMORY_PANIC( account, "invalid create account" );

                const ConstString & accountId = account->getAccountId();

                PyObject * py_value = pybind::ptr( _kernel, accountId );

                return py_value;
            }
            //////////////////////////////////////////////////////////////////////////
            PyObject * s_createGlobalAccount( pybind::kernel_interface * _kernel )
            {
                AccountInterfacePtr account = ACCOUNT_SERVICE()
                    ->createGlobalAccount( MENGINE_DOCUMENT_PYTHON );

                MENGINE_ASSERTION_MEMORY_PANIC( account, "invalid create account" );

                const ConstString & accountId = account->getAccountId();

                PyObject * py_value = pybind::ptr( _kernel, accountId );

                return py_value;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_selectAccount( const ConstString & _accountId )
            {
                ACCOUNT_SERVICE()
                    ->selectAccount( _accountId );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasCurrentAccount()
            {
                return ACCOUNT_SERVICE()
                    ->hasCurrentAccount();
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setDefaultAccount( const ConstString & _accountId )
            {
                ACCOUNT_SERVICE()
                    ->setDefaultAccount( _accountId );
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setGlobalAccount( const ConstString & _accountId )
            {
                ACCOUNT_SERVICE()
                    ->setGlobalAccount( _accountId );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasGlobalAccount()
            {
                return ACCOUNT_SERVICE()
                    ->hasGlobalAccount();
            }
            //////////////////////////////////////////////////////////////////////////
            const ConstString & s_getGlobalAccountId()
            {
                return ACCOUNT_SERVICE()
                    ->getGlobalAccountId();
            }
            //////////////////////////////////////////////////////////////////////////
            const ConstString & s_getDefaultAccount()
            {
                return ACCOUNT_SERVICE()
                    ->getDefaultAccountId();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasDefaultAccount()
            {
                return ACCOUNT_SERVICE()
                    ->hasDefaultAccount();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_isCurrentDefaultAccount()
            {
                return ACCOUNT_SERVICE()
                    ->isCurrentDefaultAccount();
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_selectDefaultAccount()
            {
                return ACCOUNT_SERVICE()
                    ->selectDefaultAccount();
            }
            //////////////////////////////////////////////////////////////////////////
            void s_saveAccount()
            {
                //ToDo
            }
            //////////////////////////////////////////////////////////////////////////
            void s_saveAccounts()
            {
                ACCOUNT_SERVICE()
                    ->saveAccounts();
            }
            //////////////////////////////////////////////////////////////////////////
            void s_saveAccountsInfo()
            {
                //ToDo
            }
            //////////////////////////////////////////////////////////////////////////
            void s_deleteAccount( const ConstString & _accountName )
            {
                ACCOUNT_SERVICE()
                    ->deleteAccount( _accountName );
            }
            //////////////////////////////////////////////////////////////////////////
            const ConstString & s_getCurrentAccountName()
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
            bool s_writeGlobalPickleFile( pybind::kernel_interface * _kernel, const WString & _filePath, PyObject * _data, PyObject * _pickleTypes )
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

                OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( fileGroup, filePath, true, MENGINE_DOCUMENT_FACTORABLE );

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

                Helper::closeOutputStreamFile( fileGroup, stream );

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
            PyObject * s_loadGlobalPickleFile( pybind::kernel_interface * _kernel, const WString & _filePath, PyObject * _pickleTypes )
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

                InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

                if( stream == nullptr )
                {
                    LOGGER_ERROR( "invalid open file '%s'"
                        , filePath.c_str()
                    );

                    return _kernel->ret_none();
                }

                MemoryInterfacePtr memory = Helper::readStreamArchiveMagic( stream, archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), MENGINE_DOCUMENT_FACTORABLE );

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
            bool s_writeAccountPickleFile( pybind::kernel_interface * _kernel, const ConstString & _accountId, const WString & _filePath, PyObject * _data, PyObject * _pickleTypes )
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
            PyObject * s_loadAccountPickleFile( pybind::kernel_interface * _kernel, const ConstString & _accountId, const WString & _filePath, PyObject * _pickleTypes )
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
            FileGroupInterfacePtr s_getDefaultFileGroup()
            {
                FileGroupInterfacePtr fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );;

                return fileGroup;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasFileGroup( const ConstString & _fileGroup )
            {
                bool exist = FILE_SERVICE()
                    ->hasFileGroup( _fileGroup, nullptr );

                return exist;
            }
            //////////////////////////////////////////////////////////////////////////
            const FileGroupInterfacePtr & s_getFileGroup( const ConstString & _fileGroup )
            {
                const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                    ->getFileGroup( _fileGroup );

                return fileGroup;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_hasAccountPickleFile( const ConstString & _accountId, const WString & _filePath )
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
            void s_setParticlesEnabled( bool _enable )
            {
                APPLICATION_SERVICE()
                    ->setParticleEnable( _enable );
            }
            //////////////////////////////////////////////////////////////////////////
            mt::vec2f s_getNodeScreenAdaptPosition( Node * _node )
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
            mt::vec2f s_fromWorldToScreenPosition( const mt::vec3f & _wp )
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
            bool s_hasTextByKey( const ConstString & _key )
            {
                bool value = TEXT_SERVICE()
                    ->hasTextEntry( _key, nullptr );

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
            WString s_getTextByKey( const ConstString & _key )
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
            size_t s_getTextCharCountByKey( const ConstString & _key )
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
            void s_setVSync( bool _vSync )
            {
                APPLICATION_SERVICE()
                    ->setVSync( _vSync );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_getVSync()
            {
                bool vsync = APPLICATION_SERVICE()
                    ->getVSync();

                return vsync;
            }
            //////////////////////////////////////////////////////////////////////////
            void s_setCursorMode( bool _mode )
            {
                APPLICATION_SERVICE()
                    ->setCursorMode( _mode );
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_getCursorMode()
            {
                bool cursorMode = APPLICATION_SERVICE()
                    ->getCursorMode();

                return cursorMode;
            }
            //////////////////////////////////////////////////////////////////////////
            bool s_setCursorIcon( const ConstString & _resourceName )
            {
                APPLICATION_SERVICE()
                    ->setCursorIcon( _resourceName );

                return true;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<HelperScriptMethod> HelperScriptMethodPtr;
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
            static String Tags_repr( Tags * _tags )
            {
                return Helper::tagsToString( *_tags );
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

        pybind::const_<uint32_t>( _kernel, "MENGINE_INPUT_MAX_TOUCH", MENGINE_INPUT_MAX_TOUCH );

        pybind::registration_stl_optional_type_cast<Optional<mt::box2f>>(_kernel);

        pybind::registration_type_cast<Data>(_kernel, pybind::make_type_cast<extract_Data_type>(_kernel));
        pybind::registration_type_cast<Tags>(_kernel, pybind::make_type_cast<extract_Tags_type>(_kernel));

        pybind::registration_stl_vector_type_cast<VectorResourceImages>(_kernel);
        pybind::registration_stl_vector_type_cast<VectorHotSpotPolygons>(_kernel);

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

        HelperScriptMethodPtr helperScriptMethod = Helper::makeFactorableUnique<HelperScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        pybind::def_functor_kernel( _kernel, "logDebug", helperScriptMethod, &HelperScriptMethod::s_logDebug );
        pybind::def_functor_kernel( _kernel, "logInfo", helperScriptMethod, &HelperScriptMethod::s_logInfo );
        pybind::def_functor_kernel( _kernel, "logMessage", helperScriptMethod, &HelperScriptMethod::s_logMessage );
        pybind::def_functor_kernel( _kernel, "logWarning", helperScriptMethod, &HelperScriptMethod::s_logWarning );
        pybind::def_functor_kernel( _kernel, "logError", helperScriptMethod, &HelperScriptMethod::s_logError );
        pybind::def_functor_kernel( _kernel, "logMessageRelease", helperScriptMethod, &HelperScriptMethod::s_logMessageRelease );
        pybind::def_functor_kernel( _kernel, "logFatal", helperScriptMethod, &HelperScriptMethod::s_logFatal );

        pybind::def_functor( _kernel, "enumerator", helperScriptMethod, &HelperScriptMethod::s_enumerator );
        pybind::def_functor( _kernel, "filterpowf", helperScriptMethod, &HelperScriptMethod::s_filterpowf );

        pybind::def_functor( _kernel, "rand", helperScriptMethod, &HelperScriptMethod::s_rand );
        pybind::def_functor( _kernel, "randi", helperScriptMethod, &HelperScriptMethod::s_randi );
        pybind::def_functor( _kernel, "randf", helperScriptMethod, &HelperScriptMethod::s_randf );
        pybind::def_functor( _kernel, "range_rand", helperScriptMethod, &HelperScriptMethod::s_range_rand );
        pybind::def_functor( _kernel, "range_randi", helperScriptMethod, &HelperScriptMethod::s_range_randi );
        pybind::def_functor( _kernel, "range_randf", helperScriptMethod, &HelperScriptMethod::s_range_randf );
        pybind::def_functor( _kernel, "around_randf", helperScriptMethod, &HelperScriptMethod::s_around_randf );
        pybind::def_functor( _kernel, "radius_randf", helperScriptMethod, &HelperScriptMethod::s_radius_randf );

        pybind::def_functor_kernel( _kernel, "angle_correct_interpolate_from_to", helperScriptMethod, &HelperScriptMethod::s_angle_correct_interpolate_from_to );
        pybind::def_functor( _kernel, "angle_between_two_vectors", helperScriptMethod, &HelperScriptMethod::s_angle_between_two_vectors );

        pybind::def_functor( _kernel, "projectionPointToLine", helperScriptMethod, &HelperScriptMethod::s_projectionPointToLine );
        pybind::def_functor( _kernel, "isPointInsidePolygon", helperScriptMethod, &HelperScriptMethod::s_isPointInsidePolygon );

        pybind::def_functor( _kernel, "getTime", helperScriptMethod, &HelperScriptMethod::s_getTime );
        pybind::def_functor( _kernel, "getTimeMs", helperScriptMethod, &HelperScriptMethod::s_getTimeMs );

        pybind::def_functor_deprecated( _kernel, "getDate", helperScriptMethod, &HelperScriptMethod::s_getTime, "use getTime" );

        pybind::def_functor( _kernel, "getLocalDateTimeMs", helperScriptMethod, &HelperScriptMethod::s_getLocalDateTimeMs );
        pybind::def_functor_deprecated( _kernel, "getDateStruct", helperScriptMethod, &HelperScriptMethod::s_getLocalDateStruct, "use getLocalDateStruct" );
        pybind::def_functor( _kernel, "getLocalDateStruct", helperScriptMethod, &HelperScriptMethod::s_getLocalDateStruct );
        pybind::def_functor( _kernel, "getLocalDateStructFromTimeMs", helperScriptMethod, &HelperScriptMethod::s_getLocalDateStructFromTimeMs );

        pybind::def_functor( _kernel, "getDatePathTimestamp", helperScriptMethod, &HelperScriptMethod::s_getDatePathTimestamp );
        pybind::def_functor( _kernel, "getLoggerTimestamp", helperScriptMethod, &HelperScriptMethod::s_getLoggerTimestamp );

        pybind::def_functor( _kernel, "getTimeString", helperScriptMethod, &HelperScriptMethod::s_getTimeString );

        pybind::def_functor( _kernel, "getConfigBool", helperScriptMethod, &HelperScriptMethod::s_getConfigBool );
        pybind::def_functor( _kernel, "getConfigInt", helperScriptMethod, &HelperScriptMethod::s_getConfigInt );
        pybind::def_functor( _kernel, "getConfigUInt", helperScriptMethod, &HelperScriptMethod::s_getConfigUInt );
        pybind::def_functor( _kernel, "getConfigUInt64", helperScriptMethod, &HelperScriptMethod::s_getConfigUInt64 );
        pybind::def_functor( _kernel, "getConfigFloat", helperScriptMethod, &HelperScriptMethod::s_getConfigFloat );
        pybind::def_functor( _kernel, "getConfigString", helperScriptMethod, &HelperScriptMethod::s_getConfigString );
        pybind::def_functor( _kernel, "getConfigStrings", helperScriptMethod, &HelperScriptMethod::s_getConfigStrings );

        pybind::def_functor( _kernel, "getAccounts", helperScriptMethod, &HelperScriptMethod::s_getAccounts );

        pybind::def_functor_kernel( _kernel, "getCurrentAccountUID", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountUID );

        pybind::def_functor_kernel( _kernel, "getCurrentAccountSetting", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountSetting );
        pybind::def_functor_kernel( _kernel, "getCurrentAccountSettingBool", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountSettingBool );
        pybind::def_functor_kernel( _kernel, "getCurrentAccountSettingInt", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountSettingInt );
        pybind::def_functor_kernel( _kernel, "getCurrentAccountSettingUInt", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountSettingUInt );
        pybind::def_functor_kernel( _kernel, "getCurrentAccountSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountSettingUInt64 );
        pybind::def_functor_kernel( _kernel, "getCurrentAccountSettingFloat", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountSettingFloat );
        pybind::def_functor_kernel( _kernel, "getCurrentAccountSettingStrings", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountSettingStrings );
        pybind::def_functor( _kernel, "getCurrentAccountSettingFloatDefault", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountSettingFloatDefault );

        pybind::def_functor_kernel_args( _kernel, "addCurrentAccountSetting", helperScriptMethod, &HelperScriptMethod::s_addCurrentAccountSetting );
        pybind::def_functor( _kernel, "hasCurrentAccountSetting", helperScriptMethod, &HelperScriptMethod::s_hasCurrentAccountSetting );
        pybind::def_functor_kernel( _kernel, "changeCurrentAccountSetting", helperScriptMethod, &HelperScriptMethod::s_changeCurrentAccountSetting );
        pybind::def_functor( _kernel, "changeCurrentAccountSettingBool", helperScriptMethod, &HelperScriptMethod::s_changeCurrentAccountSettingBool );
        pybind::def_functor( _kernel, "changeCurrentAccountSettingInt", helperScriptMethod, &HelperScriptMethod::s_changeCurrentAccountSettingInt );
        pybind::def_functor( _kernel, "changeCurrentAccountSettingUInt", helperScriptMethod, &HelperScriptMethod::s_changeCurrentAccountSettingUInt );
        pybind::def_functor( _kernel, "changeCurrentAccountSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_changeCurrentAccountSettingUInt64 );
        pybind::def_functor( _kernel, "changeCurrentAccountSettingFloat", helperScriptMethod, &HelperScriptMethod::s_changeCurrentAccountSettingFloat );
        pybind::def_functor( _kernel, "changeCurrentAccountSettingStrings", helperScriptMethod, &HelperScriptMethod::s_changeCurrentAccountSettingStrings );

        pybind::def_functor_kernel( _kernel, "getAccountUID", helperScriptMethod, &HelperScriptMethod::s_getAccountUID );

        pybind::def_functor_kernel( _kernel, "getAccountSetting", helperScriptMethod, &HelperScriptMethod::s_getAccountSetting );
        pybind::def_functor_kernel( _kernel, "getAccountSettingBool", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingBool );
        pybind::def_functor_kernel( _kernel, "getAccountSettingInt", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingInt );
        pybind::def_functor_kernel( _kernel, "getAccountSettingUInt", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingUInt );
        pybind::def_functor_kernel( _kernel, "getAccountSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingUInt64 );
        pybind::def_functor_kernel( _kernel, "getAccountSettingFloat", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingFloat );
        pybind::def_functor_kernel( _kernel, "getAccountSettingStrings", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingStrings );
        pybind::def_functor( _kernel, "getAccountSettingFloatDefault", helperScriptMethod, &HelperScriptMethod::s_getAccountSettingFloatDefault );

        pybind::def_functor_kernel_args( _kernel, "addAccountSetting", helperScriptMethod, &HelperScriptMethod::s_addAccountSetting );
        pybind::def_functor( _kernel, "hasAccountSetting", helperScriptMethod, &HelperScriptMethod::s_hasAccountSetting );
        pybind::def_functor_kernel( _kernel, "changeAccountSetting", helperScriptMethod, &HelperScriptMethod::s_changeAccountSetting );
        pybind::def_functor( _kernel, "changeAccountSettingBool", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingBool );
        pybind::def_functor( _kernel, "changeAccountSettingInt", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingInt );
        pybind::def_functor( _kernel, "changeAccountSettingUInt", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingUInt );
        pybind::def_functor( _kernel, "changeAccountSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingUInt64 );
        pybind::def_functor( _kernel, "changeAccountSettingFloat", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingFloat );
        pybind::def_functor( _kernel, "changeAccountSettingStrings", helperScriptMethod, &HelperScriptMethod::s_changeAccountSettingStrings );

        pybind::def_functor_kernel( _kernel, "getGlobalAccountUID", helperScriptMethod, &HelperScriptMethod::s_getGlobalAccountUID );

        pybind::def_functor_kernel( _kernel, "getGlobalSetting", helperScriptMethod, &HelperScriptMethod::s_getGlobalSetting );
        pybind::def_functor_kernel( _kernel, "getGlobalSettingBool", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingBool );
        pybind::def_functor_kernel( _kernel, "getGlobalSettingInt", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingInt );
        pybind::def_functor_kernel( _kernel, "getGlobalSettingUInt", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingUInt );
        pybind::def_functor_kernel( _kernel, "getGlobalSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingUInt64 );
        pybind::def_functor_kernel( _kernel, "getGlobalSettingFloat", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingFloat );
        pybind::def_functor_kernel( _kernel, "getGlobalSettingStrings", helperScriptMethod, &HelperScriptMethod::s_getGlobalSettingStrings );

        pybind::def_functor_kernel_args( _kernel, "addGlobalSetting", helperScriptMethod, &HelperScriptMethod::s_addGlobalSetting );
        pybind::def_functor( _kernel, "hasGlobalSetting", helperScriptMethod, &HelperScriptMethod::s_hasGlobalSetting );
        pybind::def_functor_kernel( _kernel, "changeGlobalSetting", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSetting );
        pybind::def_functor( _kernel, "changeGlobalSettingBool", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingBool );
        pybind::def_functor( _kernel, "changeGlobalSettingInt", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingInt );
        pybind::def_functor( _kernel, "changeGlobalSettingUint", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingUInt );
        pybind::def_functor( _kernel, "changeGlobalSettingUInt64", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingUInt64 );
        pybind::def_functor( _kernel, "changeGlobalSettingFloat", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingFloat );
        pybind::def_functor( _kernel, "changeGlobalSettingStrings", helperScriptMethod, &HelperScriptMethod::s_changeGlobalSettingStrings );

        pybind::def_functor_kernel( _kernel, "createAccount", helperScriptMethod, &HelperScriptMethod::s_createAccount );
        pybind::def_functor_kernel( _kernel, "createGlobalAccount", helperScriptMethod, &HelperScriptMethod::s_createGlobalAccount );

        pybind::def_functor( _kernel, "selectAccount", helperScriptMethod, &HelperScriptMethod::s_selectAccount );
        pybind::def_functor( _kernel, "deleteAccount", helperScriptMethod, &HelperScriptMethod::s_deleteAccount );

        pybind::def_functor( _kernel, "saveAccount", helperScriptMethod, &HelperScriptMethod::s_saveAccount );
        pybind::def_functor( _kernel, "saveAccounts", helperScriptMethod, &HelperScriptMethod::s_saveAccounts );
        pybind::def_functor( _kernel, "saveAccountsInfo", helperScriptMethod, &HelperScriptMethod::s_saveAccountsInfo );
        pybind::def_functor( _kernel, "hasCurrentAccount", helperScriptMethod, &HelperScriptMethod::s_hasCurrentAccount );
        pybind::def_functor( _kernel, "getCurrentAccountName", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountName );
        pybind::def_functor( _kernel, "getCurrentAccountId", helperScriptMethod, &HelperScriptMethod::s_getCurrentAccountName );

        pybind::def_functor( _kernel, "setGlobalAccount", helperScriptMethod, &HelperScriptMethod::s_setGlobalAccount );
        pybind::def_functor( _kernel, "hasGlobalAccount", helperScriptMethod, &HelperScriptMethod::s_hasGlobalAccount );
        pybind::def_functor( _kernel, "getGlobalAccountName", helperScriptMethod, &HelperScriptMethod::s_getGlobalAccountId );

        pybind::def_functor( _kernel, "setDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_setDefaultAccount );
        pybind::def_functor( _kernel, "getDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_getDefaultAccount );
        pybind::def_functor( _kernel, "hasDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_hasDefaultAccount );
        pybind::def_functor( _kernel, "isCurrentDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_isCurrentDefaultAccount );
        pybind::def_functor( _kernel, "selectDefaultAccount", helperScriptMethod, &HelperScriptMethod::s_selectDefaultAccount );

        pybind::def_functor_kernel( _kernel, "writeGlobalPickleFile", helperScriptMethod, &HelperScriptMethod::s_writeGlobalPickleFile );
        pybind::def_functor_kernel( _kernel, "loadGlobalPickleFile", helperScriptMethod, &HelperScriptMethod::s_loadGlobalPickleFile );

        pybind::def_functor_kernel( _kernel, "writeAccountPickleFile", helperScriptMethod, &HelperScriptMethod::s_writeAccountPickleFile );
        pybind::def_functor_kernel( _kernel, "loadAccountPickleFile", helperScriptMethod, &HelperScriptMethod::s_loadAccountPickleFile );
        pybind::def_functor( _kernel, "hasAccountPickleFile", helperScriptMethod, &HelperScriptMethod::s_hasAccountPickleFile );

        pybind::def_functor( _kernel, "getDefaultFileGroup", helperScriptMethod, &HelperScriptMethod::s_getDefaultFileGroup );
        pybind::def_functor( _kernel, "hasFileGroup", helperScriptMethod, &HelperScriptMethod::s_hasFileGroup );
        pybind::def_functor( _kernel, "getFileGroup", helperScriptMethod, &HelperScriptMethod::s_getFileGroup );

        pybind::def_functor( _kernel, "setParticlesEnabled", helperScriptMethod, &HelperScriptMethod::s_setParticlesEnabled );

        pybind::def_functor( _kernel, "getNodeScreenAdaptPosition", helperScriptMethod, &HelperScriptMethod::s_getNodeScreenAdaptPosition );
        pybind::def_functor( _kernel, "fromWorldToScreenPosition", helperScriptMethod, &HelperScriptMethod::s_fromWorldToScreenPosition );

        pybind::def_functor( _kernel, "hasTextByKey", helperScriptMethod, &HelperScriptMethod::s_hasTextByKey );
        pybind::def_functor( _kernel, "getTextByKey", helperScriptMethod, &HelperScriptMethod::s_getTextByKey );
        pybind::def_functor( _kernel, "getTextCharCountByKey", helperScriptMethod, &HelperScriptMethod::s_getTextCharCountByKey );


        pybind::def_functor( _kernel, "setVSync", helperScriptMethod, &HelperScriptMethod::s_setVSync );
        pybind::def_functor( _kernel, "getVSync", helperScriptMethod, &HelperScriptMethod::s_getVSync );
        pybind::def_functor( _kernel, "setCursorMode", helperScriptMethod, &HelperScriptMethod::s_setCursorMode );
        pybind::def_functor( _kernel, "getCursorMode", helperScriptMethod, &HelperScriptMethod::s_getCursorMode );
        pybind::def_functor( _kernel, "setCursorIcon", helperScriptMethod, &HelperScriptMethod::s_setCursorIcon );

        pybind::def_functor( _kernel, "intersectsEllipseVsPoint", helperScriptMethod, &HelperScriptMethod::s_intersectsEllipseVsPoint );
        pybind::def_functor( _kernel, "intersectsBoxes", helperScriptMethod, &HelperScriptMethod::s_intersectsBoxes );

        pybind::def_functor_kernel( _kernel, "getPolygonPoints", helperScriptMethod, &HelperScriptMethod::s_getPolygonPoints );
        pybind::def_functor( _kernel, "intersectsPolygons", helperScriptMethod, &HelperScriptMethod::s_intersectsPolygons );
        pybind::def_functor( _kernel, "intersectsPolygonsWM", helperScriptMethod, &HelperScriptMethod::s_intersectsPolygonsWM );
        pybind::def_functor( _kernel, "intersectsPolygonsWMP", helperScriptMethod, &HelperScriptMethod::s_intersectsPolygonsWMP );

        pybind::def_functor_args( _kernel, "makeTags", helperScriptMethod, &HelperScriptMethod::s_makeTags );


        pybind::def_functor_kernel( _kernel, "textures", helperScriptMethod, &HelperScriptMethod::s_textures );

        pybind::def_functor( _kernel, "watchdog", helperScriptMethod, &HelperScriptMethod::s_watchdog );

        pybind::def_functor( _kernel, "addGlobalModule", helperScriptMethod, &HelperScriptMethod::s_addGlobalModule );
        pybind::def_functor( _kernel, "removeGlobalModule", helperScriptMethod, &HelperScriptMethod::s_removeGlobalModule );

        pybind::def_functor_kernel( _kernel, "getrefcount", helperScriptMethod, &HelperScriptMethod::s_refcount );
        pybind::def_functor_kernel( _kernel, "is_class", helperScriptMethod, &HelperScriptMethod::s_is_class );
        pybind::def_functor_kernel( _kernel, "is_type_class", helperScriptMethod, &HelperScriptMethod::s_is_type_class );
        pybind::def_functor_kernel( _kernel, "is_wrap", helperScriptMethod, &HelperScriptMethod::s_is_wrap );

        pybind::def_functor( _kernel, "setLocale", helperScriptMethod, &HelperScriptMethod::s_setLocale );
        pybind::def_functor( _kernel, "getLocale", helperScriptMethod, &HelperScriptMethod::s_getLocale );
        pybind::def_functor( _kernel, "hasLocale", helperScriptMethod, &HelperScriptMethod::s_hasLocale );
        pybind::def_functor( _kernel, "getLocales", helperScriptMethod, &HelperScriptMethod::s_getLocales );

        pybind::def_functor( _kernel, "isometric_length_v3_v3", helperScriptMethod, &HelperScriptMethod::s_isometric_length_v3_v3 );
        pybind::def_functor( _kernel, "isometric_sqrlength_v3_v3", helperScriptMethod, &HelperScriptMethod::s_isometric_sqrlength_v3_v3 );

        pybind::def_functor( _kernel, "isValidWindowMode", helperScriptMethod, &HelperScriptMethod::s_isValidWindowMode );

        pybind::def_functor( _kernel, "utf8ToUnicode", helperScriptMethod, &HelperScriptMethod::s_utf8ToUnicode );
        pybind::def_functor( _kernel, "unicodeToUtf8", helperScriptMethod, &HelperScriptMethod::s_unicodeToUtf8 );
        pybind::def_functor( _kernel, "setCursorPosition", helperScriptMethod, &HelperScriptMethod::s_setCursorPosition );

        pybind::def_functor( _kernel, "debug", helperScriptMethod, &HelperScriptMethod::s_debug );
        pybind::def_functor( _kernel, "debugOnce", helperScriptMethod, &HelperScriptMethod::s_debugOnce );
        pybind::def_functor( _kernel, "debugNode", helperScriptMethod, &HelperScriptMethod::s_debugNode );

        pybind::def_functor( _kernel, "hasGameData", helperScriptMethod, &HelperScriptMethod::s_hasGameData );
        pybind::def_functor_kernel( _kernel, "writeGameData", helperScriptMethod, &HelperScriptMethod::s_writeGameData );
        pybind::def_functor_kernel( _kernel, "loadGameData", helperScriptMethod, &HelperScriptMethod::s_loadGameData );

        pybind::def_functor( _kernel, "isSpecialDown", helperScriptMethod, &HelperScriptMethod::s_isSpecialDown );
        pybind::def_functor( _kernel, "isAltDown", helperScriptMethod, &HelperScriptMethod::s_isAltDown );
        pybind::def_functor( _kernel, "isShiftDown", helperScriptMethod, &HelperScriptMethod::s_isShiftDown );
        pybind::def_functor( _kernel, "isCtrlDown", helperScriptMethod, &HelperScriptMethod::s_isCtrlDown );

        pybind::def_functor( _kernel, "isShiftKeyCode", helperScriptMethod, &HelperScriptMethod::s_isShiftKeyCode );
        pybind::def_functor( _kernel, "isAltKeyCode", helperScriptMethod, &HelperScriptMethod::s_isAltKeyCode );
        pybind::def_functor( _kernel, "isControlKeyCode", helperScriptMethod, &HelperScriptMethod::s_isControlKeyCode );
        pybind::def_functor( _kernel, "isKeyDown", helperScriptMethod, &HelperScriptMethod::s_isKeyDown );
        pybind::def_functor( _kernel, "isExclusiveKeyDown", helperScriptMethod, &HelperScriptMethod::s_isExclusiveKeyDown );
        pybind::def_functor( _kernel, "isAnyKeyDown", helperScriptMethod, &HelperScriptMethod::s_isAnyKeyDown );

        pybind::def_functor( _kernel, "isAnyMouseButtonDown", helperScriptMethod, &HelperScriptMethod::s_isAnyMouseButtonDown );
        pybind::def_functor( _kernel, "isMouseButtonDown", helperScriptMethod, &HelperScriptMethod::s_isMouseButtonDown );

        pybind::def_functor( _kernel, "printChildren", helperScriptMethod, &HelperScriptMethod::s_printChildren );

        pybind::def_functor( _kernel, "getGroupResourcesMemoryUse", helperScriptMethod, &HelperScriptMethod::s_getGroupResourcesMemoryUse );

        pybind::def_functor( _kernel, "intersectPathVsCircle", helperScriptMethod, &HelperScriptMethod::s_intersectPathVsCircle );
        pybind::def_functor_kernel( _kernel, "selectRandomPointFromPathRadius", helperScriptMethod, &HelperScriptMethod::s_selectRandomPointFromPathRadius );

        pybind::def_functor_kernel( _kernel, "reloadModule", helperScriptMethod, &HelperScriptMethod::s_reloadModule );

        pybind::def_functor( _kernel, "makeUID", helperScriptMethod, &HelperScriptMethod::s_makeUID );

        pybind::def_functor_deprecated( _kernel, "getTextFromID", helperScriptMethod, &HelperScriptMethod::s_getTextFromId, "use getTextFromId" );
        pybind::def_functor( _kernel, "getTextFromId", helperScriptMethod, &HelperScriptMethod::s_getTextFromId );

        pybind::def_functor( _kernel, "setTextAlias", helperScriptMethod, &HelperScriptMethod::s_setTextAlias );
        pybind::def_functor( _kernel, "removeTextAlias", helperScriptMethod, &HelperScriptMethod::s_removeTextAlias );
        pybind::def_functor( _kernel, "hasTextAlias", helperScriptMethod, &HelperScriptMethod::s_hasTextAlias );
        pybind::def_functor( _kernel, "getTextAlias", helperScriptMethod, &HelperScriptMethod::s_getTextAlias );

        pybind::def_functor_args( _kernel, "setTextAliasArguments", helperScriptMethod, &HelperScriptMethod::s_setTextAliasArguments );
        pybind::def_functor( _kernel, "removeTextAliasArguments", helperScriptMethod, &HelperScriptMethod::s_removeTextAliasArguments );

        pybind::def_functor_args( _kernel, "addChronometer", helperScriptMethod, &HelperScriptMethod::s_addChronometer );
        pybind::def_functor( _kernel, "removeChronometer", helperScriptMethod, &HelperScriptMethod::s_removeChronometer );

        pybind::def_functor( _kernel, "getHotSpotPolygonBoundingBox", helperScriptMethod, &HelperScriptMethod::s_getHotSpotPolygonBoundingBox );

        pybind::def_functor( _kernel, "hasOption", helperScriptMethod, &HelperScriptMethod::s_hasOption );
        pybind::def_functor( _kernel, "getOptionValue", helperScriptMethod, &HelperScriptMethod::s_getOptionValue );
        pybind::def_functor_kernel( _kernel, "getOptionValues", helperScriptMethod, &HelperScriptMethod::s_getOptionValues );
        pybind::def_functor( _kernel, "getOptionInt32", helperScriptMethod, &HelperScriptMethod::s_getOptionInt32 );
        pybind::def_functor( _kernel, "getOptionUInt32", helperScriptMethod, &HelperScriptMethod::s_getOptionUInt32 );

        pybind::def_functor( _kernel, "crashEngine", helperScriptMethod, &HelperScriptMethod::s_crashEngine );
        pybind::def_functor( _kernel, "freezeEngine", helperScriptMethod, &HelperScriptMethod::s_freezeEngine );
        pybind::def_functor( _kernel, "memleakEngine", helperScriptMethod, &HelperScriptMethod::s_memleakEngine );

        pybind::def_functor( _kernel, "generateUniqueIdentity", helperScriptMethod, &HelperScriptMethod::s_generateUniqueIdentity );

        pybind::def_functor( _kernel, "setClipboardText", helperScriptMethod, &HelperScriptMethod::s_setClipboardText );
        pybind::def_functor_kernel( _kernel, "getClipboardText", helperScriptMethod, &HelperScriptMethod::s_getClipboardText );

        pybind::def_functor_kernel( _kernel, "compressBase64", helperScriptMethod, &HelperScriptMethod::s_compressBase64 );
        pybind::def_functor_kernel( _kernel, "decompressBase64", helperScriptMethod, &HelperScriptMethod::s_decompressBase64 );

#if defined(MENGINE_PLATFORM_ANDROID)
        pybind::def_functor_kernel( _kernel, "getAndroidId", helperScriptMethod, &HelperScriptMethod::s_getAndroidId );
#endif

        m_implement = helperScriptMethod;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void HelperScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        m_implement = nullptr;

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
