-keep public class * extends org.Mengine.Base.MengineServiceInvalidInitializeException {
    native <methods>;
    public <methods>;
}

-keep public class * extends org.Mengine.Base.MengineService {
    native <methods>;
    public <methods>;
    public static final java.lang.String SERVICE_NAME;
    public static final boolean SERVICE_EMBEDDING;
}

-keep public class org.Mengine.Base.MengineActivity {
    native <methods>;
    public <methods>;
    public static org.Mengine.Base.MengineActivity INSTANCE;
}

-keep public class org.Mengine.Base.MengineApplication {
    native <methods>;
    public <methods>;
    public static org.Mengine.Base.MengineApplication INSTANCE;
}

-keep public class org.Mengine.Base.MengineSemaphore {
    public java.lang.Object getValue();
}

-keep public class * extends org.Mengine.Base.MengineFragment {
    <init>();
    public static ** INSTANCE;
    public <methods>;
}

-keep public class * implements org.Mengine.Base.MengineFactorable {
    public <init>(...);
}

-keep public class * implements org.Mengine.Base.MenginePluginExtensionInterface {
    public <init>();
}

-keep public interface org.Mengine.Base.MengineCallback {
    void call(boolean, java.util.Map);
}

-keep public class org.Mengine.Base.MengineNativeRunnable {
    public <init>(java.nio.ByteBuffer);
}

-keep public class org.Mengine.Base.MengineNativeCallQueue {
    public static void processCalls(org.Mengine.Base.MengineApplication);
}

-keep public class org.Mengine.Base.MenginePlatformEventQueue {
    public static boolean processEvents(org.Mengine.Base.MengineApplication);
}

-keep public class org.Mengine.Base.MengineNetwork {
    public static void cancelHttpRequest(int);
    public static org.Mengine.Base.MengineParamHttpResponse httpRequest*(...);
}

-keep public class org.Mengine.Base.MengineParamHttpRequest {
    public <init>(int, java.lang.String, java.lang.String, java.util.List, java.lang.String, int);
}

-keep public class org.Mengine.Base.MengineParamHttpResponse {
    public <fields>;
}

-keep public class org.Mengine.Base.MengineParamLoggerMessage {
    public <init>(org.Mengine.Base.MengineLoggerMessageSource, org.Mengine.Base.MengineTag, java.lang.String, int, int, java.lang.String, int, java.lang.String, java.lang.String);
}

-keep public class org.Mengine.Base.MengineTag {
    public static org.Mengine.Base.MengineTag of(java.lang.String);
}

-keep public enum org.Mengine.Base.MengineLoggerMessageSource {
    public static <fields>;
}

-keep public class org.Mengine.Base.MengineParamAnalyticsEvent {
    public <init>(org.Mengine.Base.MengineAnalyticsEventCategory, java.lang.String, long, java.util.Map, java.util.Map);
}

-keep public enum org.Mengine.Base.MengineAnalyticsEventCategory {
    public static <fields>;
}