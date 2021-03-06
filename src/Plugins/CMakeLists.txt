ADD_SUBDIRECTORY(ImageCodecPlugin)
ADD_SUBDIRECTORY(SoundCodecPlugin)
ADD_SUBDIRECTORY(AmplifierPlugin)
ADD_SUBDIRECTORY(OggVorbisPlugin)

if(MENGINE_PLUGIN_ZIP)
    ADD_SUBDIRECTORY(ZipPlugin)
endif()

if(MENGINE_PLUGIN_LZ4)
    ADD_SUBDIRECTORY(LZ4Plugin)
endif()

if(MENGINE_PLUGIN_GOAP)
    ADD_SUBDIRECTORY(GOAPPlugin)
endif()

if(MENGINE_PLUGIN_TEXTUREPACKER)
    ADD_SUBDIRECTORY(TexturepackerPlugin)
endif()

if(MENGINE_PLUGIN_JSON)
    ADD_SUBDIRECTORY(JSONPlugin)
endif()

if(MENGINE_PLUGIN_MOVIE1)
    ADD_SUBDIRECTORY(Movie1Plugin)
endif()

if(MENGINE_PLUGIN_MOVIE)
    ADD_SUBDIRECTORY(MoviePlugin)
endif()

if(MENGINE_PLUGIN_NODEDEBUGGER)
    ADD_SUBDIRECTORY(NodeDebuggerPlugin)
endif()

if(MENGINE_PLUGIN_METABUFLOADER)
    ADD_SUBDIRECTORY(MetabufLoaderPlugin)
endif()

if(MENGINE_PLUGIN_CURL)
    ADD_SUBDIRECTORY(cURLPlugin)
endif()

if(MENGINE_PLUGIN_PVRTC)
    ADD_SUBDIRECTORY(PVRTCPlugin)
endif()

if(MENGINE_PLUGIN_ETC1)
    ADD_SUBDIRECTORY(ETC1Plugin)
endif()

if(MENGINE_PLUGIN_DDS)
    ADD_SUBDIRECTORY(DDSPlugin)
endif()

if(MENGINE_PLUGIN_JPEG)
    ADD_SUBDIRECTORY(JPEGPlugin)
endif()

if(MENGINE_PLUGIN_PNG)
    ADD_SUBDIRECTORY(PNGPlugin)
endif()

if(MENGINE_PLUGIN_SPNG)
    ADD_SUBDIRECTORY(SPNGPlugin)
endif()

if(MENGINE_PLUGIN_WEBP)
    ADD_SUBDIRECTORY(WebPPlugin)
endif()

if(MENGINE_PLUGIN_VIDEO)
    ADD_SUBDIRECTORY(VideoPlugin)
endif()

if(MENGINE_PLUGIN_AV1)
    #ADD_SUBDIRECTORY(AV1Plugin)
endif()

if(MENGINE_PLUGIN_THEORA)
    ADD_SUBDIRECTORY(TheoraPlugin)
endif()

if(MENGINE_PLUGIN_OZZ_ANIMATION)
    ADD_SUBDIRECTORY(OzzAnimationPlugin)
endif()

if(MENGINE_PLUGIN_ANDROID_NATIVE_KERNEL)
    ADD_SUBDIRECTORY(AndroidNativeKernelPlugin)
endif()

if(MENGINE_PLUGIN_ANDROID_NATIVE_FACEBOOK)
    ADD_SUBDIRECTORY(AndroidNativeFacebookPlugin)
endif()

if(MENGINE_PLUGIN_ANDROID_NATIVE_UNITYADS)
    ADD_SUBDIRECTORY(AndroidNativeUnityAdsPlugin)
endif()

if(MENGINE_PLUGIN_ANDROID_NATIVE_ADMOB)
    ADD_SUBDIRECTORY(AndroidNativeAdMobPlugin)
endif()

if(MENGINE_PLUGIN_ANDROID_NATIVE_DEVTODEV)
    ADD_SUBDIRECTORY(AndroidNativeDevToDevPlugin)
endif()

if(MENGINE_PLUGIN_ANDROID_NATIVE_LINKING)
    ADD_SUBDIRECTORY(AndroidNativeLinkingPlugin)
endif()

if(MENGINE_PLUGIN_ANDROID_NATIVE_LOCAL_NOTIFICATIONS)
    ADD_SUBDIRECTORY(AndroidNativeLocalNotificationsPlugin)
endif()

if(MENGINE_PLUGIN_DAZZLE)
    ADD_SUBDIRECTORY(DazzlePlugin)
endif()

if(MENGINE_PLUGIN_ASTRALAX)
    ADD_SUBDIRECTORY(AstralaxPlugin)
endif()

if(MENGINE_PLUGIN_SPINE)
    ADD_SUBDIRECTORY(SpinePlugin)
endif()

if(MENGINE_PLUGIN_STEAM)
    ADD_SUBDIRECTORY(SteamPlugin)
endif()

if(MENGINE_PLUGIN_NODEDEBUGRENDER)
    ADD_SUBDIRECTORY(NodeDebugRenderPlugin)
endif()

if(MENGINE_PLUGIN_RESOURCEVALIDATE)
    ADD_SUBDIRECTORY(ResourceValidatePlugin)
endif()

if(MENGINE_PLUGIN_RESOURCEPREFETCHER)
    ADD_SUBDIRECTORY(ResourcePrefetcherPlugin)
endif()

if(MENGINE_PLUGIN_RESOURCECONVERT)
    ADD_SUBDIRECTORY(ResourceConvertPlugin)
endif()

if(MENGINE_PLUGIN_RESOURCEDEBUGGER)
    ADD_SUBDIRECTORY(ResourceDebuggerPlugin)
endif()

if(MENGINE_PLUGIN_POSIX_SOCKET)
    ADD_SUBDIRECTORY(PosixSocketPlugin)
endif()

if(MENGINE_PLUGIN_WIN32_SOCKET)
    ADD_SUBDIRECTORY(Win32SocketPlugin)
endif()

if(MENGINE_PLUGIN_WIN32_FILEGROUP)
    ADD_SUBDIRECTORY(Win32FileGroupPlugin)
endif()

if(MENGINE_PLUGIN_WIN32_ANTIFREEZEMONITOR)
    ADD_SUBDIRECTORY(Win32AntifreezeMonitorPlugin)
endif()

if(MENGINE_PLUGIN_WIN32_CRITICALERRORSMONITOR)
    ADD_SUBDIRECTORY(Win32CriticalErrorsMonitorPlugin)
endif()

if(MENGINE_PLUGIN_SDL_FILEGROUP)
    ADD_SUBDIRECTORY(SDLFileGroupPlugin)
endif()

if(MENGINE_PLUGIN_WIN32_CONSOLELOGGER)
    ADD_SUBDIRECTORY(Win32ConsoleLoggerPlugin)
endif()

if(MENGINE_PLUGIN_DEVELOPMENTCONVERTER)
    ADD_SUBDIRECTORY(DevelopmentConverterPlugin)
endif()

if(MENGINE_PLUGIN_XMLTOBIN)
    ADD_SUBDIRECTORY(XmlToBinPlugin)
endif()

if(MENGINE_PLUGIN_XLS_EXPORT)
    ADD_SUBDIRECTORY(XlsExportPlugin)
endif()

if(MENGINE_PLUGIN_BITMAPFONT)
    ADD_SUBDIRECTORY(BitmapFontPlugin)
endif()

if(MENGINE_PLUGIN_TTF)
    ADD_SUBDIRECTORY(TTFPlugin)
endif()

if(MENGINE_PLUGIN_FE)
    ADD_SUBDIRECTORY(FEPlugin)
endif()

if(MENGINE_PLUGIN_IMGUI)
    ADD_SUBDIRECTORY(ImGUIPlugin)
endif()

if(MENGINE_PLUGIN_BOX2D)
    ADD_SUBDIRECTORY(Box2DPlugin)
endif()

if(MENGINE_PLUGIN_GRAPHICS)
    ADD_SUBDIRECTORY(GraphicsPlugin)
endif()

if(MENGINE_PLUGIN_SENTRY)
    ADD_SUBDIRECTORY(SentryPlugin)
endif()

if(MENGINE_PLUGIN_AREAOFINTEREST)
    ADD_SUBDIRECTORY(AreaOfInterestPlugin)
endif()

if(MENGINE_PLUGIN_CAMERADEBUGGIZMO)
    ADD_SUBDIRECTORY(CameraDebugGizmoPlugin)
endif()

if(MENGINE_PLUGIN_PATHFINDER)
    ADD_SUBDIRECTORY(PathfinderPlugin)
endif()

SET(APPLICATION_LINK_LIBRARIES ${APPLICATION_LINK_LIBRARIES} PARENT_SCOPE)
SET(APPLICATION_DEPENDENCIES ${APPLICATION_DEPENDENCIES} PARENT_SCOPE)