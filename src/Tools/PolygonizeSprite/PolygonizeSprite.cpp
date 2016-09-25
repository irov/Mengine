#	include <Windows.h>

#	include <shellapi.h>

#	include <stdio.h>

#	include <string>
#	include <sstream>
#	include <vector>

#	include "Interface/PluginInterface.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/LoggerInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/DataInterface.h"
#	include "Interface/MemoryInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/ImageCodecInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ThreadSystemInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/ConfigInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#   include "AutoPolygon.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT(MengeWin32FileGroup);
PLUGIN_EXPORT(MengeImageCodec);
PLUGIN_EXPORT(MengeZip);
PLUGIN_EXPORT(MengeLZ4);
//////////////////////////////////////////////////////////////////////////
static void message_error(const char * _format, ...)
{
    va_list argList;

    va_start(argList, _format);

    char str[2048];

    vsnprintf(str, 2048 - 1, _format, argList);

    va_end(argList);

    printf(str);
}
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN(ServiceProvider)

SERVICE_EXTERN(UnicodeSystem);
SERVICE_EXTERN(UnicodeService);
SERVICE_EXTERN(StringizeService);
SERVICE_EXTERN(ArchiveService);
SERVICE_EXTERN(LoggerService);
SERVICE_EXTERN(CodecService);
SERVICE_EXTERN(DataService);
SERVICE_EXTERN(ConfigService);
SERVICE_EXTERN(ThreadSystem);
SERVICE_EXTERN(ThreadService);
SERVICE_EXTERN(MemoryService);
SERVICE_EXTERN(WindowsLayer);
SERVICE_EXTERN(FileService);
SERVICE_EXTERN(PluginSystem);
SERVICE_EXTERN(PluginService);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    static bool initializeEngine(Menge::ServiceProviderInterface ** _serviceProvider)
    {
        Menge::ServiceProviderInterface * serviceProvider;
        SERVICE_PROVIDER_CREATE(ServiceProvider, &serviceProvider);

        SERVICE_CREATE(serviceProvider, UnicodeSystem);
        SERVICE_CREATE(serviceProvider, UnicodeService);

        SERVICE_CREATE(serviceProvider, StringizeService);
        SERVICE_CREATE(serviceProvider, ArchiveService);

        SERVICE_CREATE(serviceProvider, LoggerService);

        class MyLogger
            : public LoggerInterface
        {
        public:
            bool initialize() override
            {
                return true;
            }

            void finalize() override
            {
            };

            void setServiceProvider(ServiceProviderInterface * _serviceProvider) override
            {
                m_serviceProvider = _serviceProvider;
            }

            ServiceProviderInterface * getServiceProvider() const override
            {
                return m_serviceProvider;
            }

            MyLogger()
                : m_verboseLevel(LM_WARNING)
                , m_verboseFlag(0xFFFFFFFF)
            {
            }

        public:
            void setVerboseLevel(EMessageLevel _level) override
            {
                m_verboseLevel = _level;
            };

            void setVerboseFlag(size_t _flag) override
            {
                m_verboseFlag = _flag;
            };

        public:
            bool validMessage(EMessageLevel _level, size_t _flag) const override
            {
                if( m_verboseLevel < _level )
                {
                    return false;
                }

                if( _flag == 0 )
                {
                    return true;
                }

                if( (m_verboseFlag & _flag) == 0 )
                {
                    return false;
                }

                return true;
            };

        public:
            void log(EMessageLevel _level, size_t _flag, const char * _data, size_t _count) override
            {
                (void)_level;
                (void)_flag;
                (void)_count;

                message_error("%s"
                    , _data
                );
            }

            void flush() override
            {
            }

        protected:
            EMessageLevel m_verboseLevel;
            uint32_t m_verboseFlag;

        private:
            ServiceProviderInterface * m_serviceProvider;
        };

        LOGGER_SERVICE(serviceProvider)
            ->setVerboseLevel(LM_WARNING);

        LOGGER_SERVICE(serviceProvider)
            ->registerLogger(new MyLogger);

        SERVICE_CREATE(serviceProvider, CodecService);
        SERVICE_CREATE(serviceProvider, DataService);
        SERVICE_CREATE(serviceProvider, ConfigService);

        SERVICE_CREATE(serviceProvider, ThreadSystem);

        SERVICE_CREATE(serviceProvider, ThreadService);
        SERVICE_CREATE(serviceProvider, MemoryService);
        SERVICE_CREATE(serviceProvider, PluginSystem);
        SERVICE_CREATE(serviceProvider, PluginService);

        SERVICE_CREATE(serviceProvider, WindowsLayer);
        SERVICE_CREATE(serviceProvider, FileService);

        PLUGIN_CREATE(serviceProvider, MengeWin32FileGroup);

        PLUGIN_CREATE(serviceProvider, MengeZip);
        PLUGIN_CREATE(serviceProvider, MengeLZ4);
        PLUGIN_CREATE(serviceProvider, MengeImageCodec);

        if( FILE_SERVICE(serviceProvider)
            ->mountFileGroup(ConstString::none(), ConstString::none(), Helper::stringizeString(serviceProvider, "dir")) == false )
        {
            return false;
        }

        ConstString dev = Helper::stringizeString(serviceProvider, "dev");

        if( FILE_SERVICE(serviceProvider)
            ->mountFileGroup(dev, ConstString::none(), Helper::stringizeString(serviceProvider, "dir")) == false )
        {
            return false;
        }

        *_serviceProvider = serviceProvider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool polygonizeSprite(Menge::ServiceProviderInterface * serviceProvider, const WString & in, const WString & info)
    {
        String utf8_in;
        Helper::unicodeToUtf8(serviceProvider, in, utf8_in);

        ConstString c_in = Helper::stringizeString(serviceProvider, utf8_in);

        InputStreamInterfacePtr input_stream =
            FILE_SERVICE(serviceProvider)->openInputFile(ConstString::none(), c_in, false);

        if( input_stream == nullptr )
        {
            return false;
        }

        const ConstString & codecType = CODEC_SERVICE(serviceProvider)
            ->findCodecType(c_in);

        if( codecType.empty() == true )
        {
            return false;
        }

        //message_error( "test" );

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(serviceProvider)
            ->createDecoderT<ImageDecoderInterfacePtr>(codecType);

        if( imageDecoder == nullptr )
        {
            return false;
        }

        if( imageDecoder->prepareData(input_stream) == false )
        {
            return false;
        }

        const ImageCodecDataInfo* decode_dataInfo = imageDecoder->getCodecDataInfo();

        if( decode_dataInfo->width == 0 ||
            decode_dataInfo->height == 0 )
        {
            return false;
        }

        ImageCodecOptions decode_options;

        decode_options.channels = decode_dataInfo->channels;
        decode_options.pitch = decode_dataInfo->width * decode_dataInfo->channels;

        imageDecoder->setOptions(&decode_options);

        size_t width = decode_dataInfo->width;
        size_t height = decode_dataInfo->height;

        size_t bufferSize = width * height * decode_dataInfo->channels;

        MemoryInterfacePtr memory_textureBuffer = MEMORY_SERVICE(serviceProvider)
            ->createMemory();

        if( memory_textureBuffer == nullptr )
        {
            return false;
        }

        unsigned char * textureBuffer = memory_textureBuffer->newMemory(bufferSize);

        if( textureBuffer == nullptr )
        {
            return false;
        }

        if( imageDecoder->decode(textureBuffer, bufferSize) == 0U )
        {
            return false;
        }

        AutoPolygon ap(textureBuffer, width, height);

        Triangles tri = ap.generateTriangles(mt::rectf(0.f, 0.f, (float)width, (float)height));

        if( info.empty() == false )
        {
            String utf8_info;
            Helper::unicodeToUtf8(serviceProvider, info, utf8_info);

            ConstString c_info = Helper::stringizeString(serviceProvider, utf8_info);

            OutputStreamInterfacePtr info_stream = FILE_SERVICE(serviceProvider)
                ->openOutputFile(ConstString::none(), c_info);

            if( info_stream == nullptr )
            {
                return false;
            }

            std::stringstream ss;

            ss << tri.vertCount << std::endl;
            ss << tri.indexCount << std::endl;

            for( size_t i = 0; i != tri.vertCount; ++i )
            {
                Vertex * v = tri.verts + i;

                ss << v->pos.x << std::endl;
                ss << v->pos.y << std::endl;
                ss << v->uv.x << std::endl;
                ss << v->uv.y << std::endl;
            }

            for( size_t i = 0; i != tri.indexCount; ++i )
            {
                uint16_t index = tri.indices[i];

                ss << index << std::endl;
            }

            ss << std::endl;

            std::string str = ss.str();

            if( info_stream->write(str.c_str(), str.size()) == false )
            {
                return false;
            }
        }

        return true;
    }
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd)
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)nShowCmd;

    stdex_allocator_initialize();

    int cmd_num;
    LPWSTR * cmd_args = CommandLineToArgvW(lpCmdLine, &cmd_num);

    Menge::WString command;
    Menge::WString in;
    Menge::WString info;

    for( int i = 0; i < cmd_num; i += 2 )
    {
        LPWSTR arg = cmd_args[i + 0];
        LPWSTR value = cmd_args[i + 1];

        if( wcscmp(arg, L"-command") == 0 )
        {
            command = value;
        }
        else if( wcscmp(arg, L"-in") == 0 )
        {
            in = value;
        }
        else if( wcscmp(arg, L"-info") == 0 )
        {
            info = value;
        }
    }

    if( in.empty() == true )
    {
        message_error("not found 'in' param"
        );

        return 0;
    }

    Menge::ServiceProviderInterface * serviceProvider;

    try
    {
        if( Menge::initializeEngine(&serviceProvider) == false )
        {
            message_error("ImageTrimmer invalid initialize");

            return 0;
        }
    }
    catch( const std::exception & se )
    {
        message_error("Mengine exception %s"
            , se.what()
        );

        return 0;
    }

    if( Menge::polygonizeSprite(serviceProvider, in, info) == false )
    {
        message_error("ImageTrimmer invalid trim %ls"
            , in.c_str()
        );

        return 0;
    }

    return 0;
}