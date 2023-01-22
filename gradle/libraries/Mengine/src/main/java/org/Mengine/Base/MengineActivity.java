package org.Mengine.Base;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Map;

import android.content.*;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.provider.Settings.Secure;
import android.view.KeyEvent;

public class MengineActivity extends SDLActivity {
    public static final String TAG = "MengineActivity";

    private boolean m_initializeBaseServices;
    private boolean m_initializePython;

    private String m_androidId;

    private Map<Integer, InputStream> m_openFiles;
    private int m_fileEnumerator;

    private Map<String, MengineSemaphore> m_semaphores;

    private Map<String, Integer> m_requestCodes;

    class CallbackResponse {
        public Integer id;
        public MengineCallbackInterface cb;
    };

    private ArrayList<CallbackResponse> m_callbackResponses;
    private int m_callbackResponseEnumerator;

    public ArrayList<MenginePlugin> m_loggerPlugins;
    public ArrayList<MenginePlugin> m_analyticsPlugins;

    private static native void AndroidEnvironmentService_setMengineAndroidActivityJNI(Object Activity);
    private static native void AndroidEnvironmentService_quitMengineAndroidActivityJNI();
    private static native String AndroidEnvironmentService_getCompanyName();
    private static native String AndroidEnvironmentService_getProjectName();
    private static native int AndroidEnvironmentService_getProjectVersion();
    private static native boolean AndroidEnvironmentService_isDebugMode();
    private static native boolean AndroidEnvironmentService_isDevelopmentMode();
    private static native boolean AndroidEnvironmentService_isMasterRelease();
    private static native boolean AndroidEnvironmentService_isBuildPublish();
    private static native String AndroidEnvironmentService_getEngineGITSHA1();
    private static native String AndroidEnvironmentService_getBuildTimestamp();
    private static native String AndroidEnvironmentService_getBuildUsername();
    private static native String AndroidEnvironmentService_getBuildVersion();

    private static native void AndroidNativePython_addPlugin(String name, Object plugin);
    private static native void AndroidNativePython_call(String plugin, String method, int responseId, Object args[]);
    private static native void AndroidNativePython_activateSemaphore(String name);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    public MengineActivity() {
        m_initializeBaseServices = false;
        m_initializePython = false;

        m_openFiles = new HashMap<>();
        m_fileEnumerator = 0;

        m_semaphores = new HashMap<>();

        m_requestCodes = new HashMap<>();

        m_callbackResponses = new ArrayList<>();
        m_callbackResponseEnumerator = 0;

        m_loggerPlugins = new ArrayList<>();
        m_analyticsPlugins = new ArrayList<>();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    protected String[] getLibraries() {
        return new String[]{
            "SDL2",
            "SDLApplication"
        };
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getCompanyName() {
       return AndroidEnvironmentService_getCompanyName();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getProjectName() {
        return AndroidEnvironmentService_getProjectName();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public int getProjectVersion() {
        return AndroidEnvironmentService_getProjectVersion();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public boolean isDebugMode() {
        return AndroidEnvironmentService_isDebugMode();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public boolean isDevelopmentMode() {
        return AndroidEnvironmentService_isDevelopmentMode();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public boolean isMasterRelease() {
        return AndroidEnvironmentService_isMasterRelease();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public boolean isBuildPublish() {
        return AndroidEnvironmentService_isBuildPublish();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getEngineGITSHA1() {
        return AndroidEnvironmentService_getEngineGITSHA1();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getBuildTimestamp() {
        return AndroidEnvironmentService_getBuildTimestamp();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getBuildUsername() {
        return AndroidEnvironmentService_getBuildUsername();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getBuildVersion() {
        return AndroidEnvironmentService_getBuildVersion();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    protected ArrayList<MenginePlugin> getPlugins() {
        MengineApplication app = (MengineApplication)this.getApplication();

        ArrayList<MenginePlugin> plugins = app.getPlugins();

        return plugins;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public <T> T findPlugin(Class<T> cls) {
        String name;

        try {
            Field PLUGIN_NAME = cls.getField("PLUGIN_NAME");

            name = (String)PLUGIN_NAME.get(null);
        } catch (NoSuchFieldException ex) {
            Log.e(TAG, "MengineActivity plugin not found field PLUGIN_NAME: " + cls.getName());

            return null;
        } catch (IllegalAccessException ex) {
            Log.e(TAG, "MengineActivity plugin invalid field PLUGIN_NAME: " + cls.getName());

            return null;
        }

        MengineApplication app = (MengineApplication)this.getApplication();

        MenginePlugin plugin = app.findPlugin(name);

        return (T)plugin;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void addLoggerPlugin(MenginePlugin plugin) {
        m_loggerPlugins.add(plugin);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void removeLoggerPlugin(MenginePlugin plugin) {
        m_loggerPlugins.remove(plugin);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void addAnalyticsPlugin(MenginePlugin plugin) {
        m_analyticsPlugins.add(plugin);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void removeAnalyticsPlugin(MenginePlugin plugin) {
        m_analyticsPlugins.remove(plugin);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void sendEvent(String id, Object ... args) {
        for(MenginePlugin p : this.getPlugins()) {
            p.onEvent(this, id, args);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public Bundle getMetaDataBundle() {
        Context context = this.getContext();
        String packageName = context.getPackageName();
        PackageManager packageManager = context.getPackageManager();

        try {
            ApplicationInfo ai = packageManager.getApplicationInfo(packageName, PackageManager.GET_META_DATA);
            Bundle bundle = ai.metaData;

            return bundle;
        } catch (PackageManager.NameNotFoundException e) {
            Log.e(TAG, "Unable to load meta-data: " + e.getMessage());
        }

        return null;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getMetaDataString(String name) {
        Bundle bundle = this.getMetaDataBundle();

        if (bundle == null) {
            return null;
        }

        String value = bundle.getString(name);

        return value;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public boolean getMetaDataBoolean(String name, boolean d) {
        Bundle bundle = this.getMetaDataBundle();

        if (bundle == null) {
            return d;
        }

        boolean value = bundle.getBoolean(name, d);

        return value;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public int getMetaDataInteger(String name, int d) {
        Bundle bundle = this.getMetaDataBundle();

        if (bundle == null) {
            return d;
        }

        int value = bundle.getInt(name, d);

        return value;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.i(TAG, "onCreate");

        if (mBrokenLibraries == true) {
            Log.e(TAG, "onCreate: broken libraries");

            return;
        }

        AndroidEnvironmentService_setMengineAndroidActivityJNI(this);

        Context context = this.getContext();
        ContentResolver resolver = context.getContentResolver();
        m_androidId = Secure.getString(resolver, Secure.ANDROID_ID);

        for (MenginePlugin p : this.getPlugins()) {
            p.setActivity(this);

            p.onCreate(this, savedInstanceState);
        }

        for (MenginePlugin p : this.getPlugins()) {
            p.onExtension(this);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void quitMengineApplication() {
        AndroidEnvironmentService_quitMengineAndroidActivityJNI();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void onMengineInitializeBaseServices() {
        m_initializeBaseServices = true;

        MengineLog.onMengineInitializeBaseServices();

        MengineLog.logInfo(TAG, "onMengineInitializeBaseServices");

        for (MenginePlugin p : this.getPlugins()) {
            p.onMengineInitializeBaseServices(this);
        }

        MengineApplication app = (MengineApplication)this.getApplication();
        app.onMengineInitializeBaseServices(this);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void onMengineCreateApplication() {
        MengineLog.logInfo(TAG, "onMengineCreateApplication");

        for (MenginePlugin p : this.getPlugins()) {
            p.onMengineCreateApplication(this);
        }

        for (MenginePlugin p : this.getPlugins()) {
            p.onExtensionRun(this);
        }

        MengineApplication app = (MengineApplication)this.getApplication();
        app.onMengineCreateApplication(this);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void onMengineApplicationRun() {
        MengineLog.logInfo(TAG, "onMengineApplicationRun");

        for (MenginePlugin p : this.getPlugins()) {
            p.onMengineApplicationRun(this);
        }

        MengineApplication app = (MengineApplication)this.getApplication();
        app.onMengineApplicationRun(this);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void onMengineApplicationReady() {
        MengineLog.logInfo(TAG, "onMengineApplicationReady");

        for (MenginePlugin p : this.getPlugins()) {
            p.onMengineApplicationReady(this);
        }

        MengineApplication app = (MengineApplication)this.getApplication();
        app.onMengineApplicationReady(this);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void onMengineApplicationStop() {
        MengineLog.logInfo(TAG, "onMengineApplicationStop");

        for (MenginePlugin p : this.getPlugins()) {
            p.onMengineApplicationStop(this);
        }

        MengineApplication app = (MengineApplication)this.getApplication();
        app.onMengineApplicationStop(this);

        m_initializeBaseServices = false;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void onMengineAnalyticsEvent(String eventName, long timestamp, Map<String, Object> parameters) {
        MengineLog.logInfo(TAG, "onMengineAnalyticsEvent %s %d [%s]"
            , eventName
            , timestamp
            , parameters
        );

        for (MenginePlugin p : m_analyticsPlugins) {
            p.onMengineAnalyticsEvent(this, eventName, timestamp, parameters);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        MengineLog.logInfo(TAG, "onActivityResult");

        for (MenginePlugin p : this.getPlugins()) {
            p.onActivityResult(this, requestCode, resultCode, data);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    public void onStart(){
        super.onStart();

        MengineLog.logInfo(TAG, "onStart");

        if (mBrokenLibraries == true) {
            Log.e(TAG, "onStart: broken libraries");

            return;
        }

        for (MenginePlugin p : this.getPlugins()) {
            p.onStart(this);
        }

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onStart");
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    protected void onStop() {
        super.onStop();

        MengineLog.logInfo(TAG, "onStop");

        if (mBrokenLibraries == true) {
            Log.e(TAG, "onStop: broken libraries");

            return;
        }

        for(MenginePlugin p : this.getPlugins()) {
            p.onStop(this);
        }

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onStop");
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    public void onPause() {
        super.onPause();

        MengineLog.logInfo(TAG, "onPause");

        if (mBrokenLibraries == true) {
            Log.e(TAG, "onPause: broken libraries");

            return;
        }

        for(MenginePlugin p : this.getPlugins()) {
            p.onPause(this);
        }

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onPause");
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    public void onResume() {
        super.onResume();

        MengineLog.logInfo(TAG, "onResume");

        if (mBrokenLibraries == true) {
            Log.e(TAG, "onResume: broken libraries");

            return;
        }

        for(MenginePlugin p : this.getPlugins()) {
            p.onResume(this);
        }

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onResume");
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        MengineLog.logInfo(TAG, "onNewIntent");

        if (mBrokenLibraries == true) {
            Log.e(TAG, "onNewIntent: broken libraries");

            return;
        }

        for(MenginePlugin p : this.getPlugins()) {
            p.onNewIntent(this, intent);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    public void onDestroy() {
        super.onDestroy();

        MengineLog.logInfo(TAG, "onDestroy");

        if (mBrokenLibraries == true) {
            Log.e(TAG, "onDestroy: broken libraries");

            return;
        }

        for(MenginePlugin p : this.getPlugins()) {
            p.onDestroy(this);
        }

        m_openFiles = null;
        m_semaphores = null;
        m_requestCodes = null;
        m_callbackResponses = null;

        m_analyticsPlugins = null;
        m_loggerPlugins = null;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    public void onRestart() {
        super.onRestart();

        MengineLog.logInfo(TAG, "onRestart");

        if (mBrokenLibraries == true) {
            Log.e(TAG, "onRestart: broken libraries");

            return;
        }

        for(MenginePlugin p : this.getPlugins()) {
            p.onRestart(this);
        }

        if (m_initializeBaseServices == true) {
            this.pythonCall(TAG, "onRestart");
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        if (mBrokenLibraries == true) {
            Log.e(TAG, "onConfigurationChanged: broken libraries");

            return;
        }

        for(MenginePlugin p : this.getPlugins()) {
            p.onConfigurationChanged(this, newConfig);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (mBrokenLibraries == true) {
            Log.e(TAG, "onRequestPermissionsResult: broken libraries");

            return;
        }

        for(MenginePlugin p : this.getPlugins()) {
            p.onRequestPermissionsResult(this, requestCode, permissions, grantResults);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        MengineLog.logInfo(TAG, "dispatchKeyEvent action: %d code: %d"
            , event.getAction()
            , event.getKeyCode()
        );

        if (mBrokenLibraries == true) {
            Log.e(TAG, "dispatchKeyEvent: broken libraries");

            return false;
        }

        for(MenginePlugin p : this.getPlugins()) {
            if (p.dispatchKeyEvent(this, event) == true) {
                return true;
            }
        }

        return super.dispatchKeyEvent(event);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    @Override
    protected SDLSurface createSDLSurface(Context context) {
        return new MengineSurface(context);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Kernel Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getAndroidId() {
        return m_androidId;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getDeviceLanguage() {
        String language = Locale.getDefault().getLanguage();

        return language;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getDeviceName() {
        String deviceName = android.os.Build.MODEL;

        return deviceName;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public int genRequestCode(String name) {
        if (m_requestCodes.containsKey(name) == false) {
            int new_code = m_requestCodes.size();

            m_requestCodes.put(name, new_code);

            return new_code;
        }

        int code = m_requestCodes.get(name);

        return code;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void onMengineLogger(String category, int level, int filter, int color, String msg) {
        for(MenginePlugin plugin : m_loggerPlugins) {
            plugin.onMengineLogger(category, level, filter, color, msg);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Python Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonInitializePlugins() {
        MengineLog.logInfo(TAG, "Python initialize");

        m_initializePython = true;

        AndroidNativePython_addPlugin("Activity", this);

        for (MenginePlugin p : this.getPlugins()) {
            try {
                Class<?> cls = p.getClass();

                Field PLUGIN_EMBEDDING = cls.getField("PLUGIN_EMBEDDING");

                if( PLUGIN_EMBEDDING.getBoolean(null) == false ) {
                    continue;
                }
            } catch (NoSuchFieldException ex) {
                continue;
            } catch (IllegalAccessException ex) {
                continue;
            }

            String name = p.getPluginName();

            this.addPythonPlugin(name, p);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonFinalizePlugins() {
        m_initializePython = false;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonCall(String plugin, String method, Object ... args) {
        if (m_initializePython == false) {
            MengineLog.logError(TAG,"invalid python plugin [%s] method [%s] args [%s] call before embedding"
                , plugin
                , method
                , args
            );

            return;
        }

        if (BuildConfig.DEBUG) {
            MengineLog.logInfo(TAG, "pythonCall plugin [%s] method [%s] args [%s]"
                , plugin
                , method
                , args
            );
        }

        AndroidNativePython_call(plugin, method, 0, args);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    protected int pythonCallResponseCreate(MengineCallbackInterface cb) {
        m_callbackResponseEnumerator++;

        int id = m_callbackResponseEnumerator;

        CallbackResponse cr = new CallbackResponse();
        cr.id = id;
        cr.cb = cb;

        m_callbackResponses.add(cr);

        return id;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonCallCb(String plugin, String method, MengineCallbackInterface cb, Object ... args) {
        if (m_initializePython == false) {
            MengineLog.logError(TAG, "invalid python plugin [%s] method [%s] args [%s] call before embedding"
                , plugin
                , method
                , args
            );

            return;
        }

        int id = this.pythonCallResponseCreate(cb);

        if (BuildConfig.DEBUG) {
            MengineLog.logInfo(TAG, "pythonCall plugin [%s] method [%s] response [%d] args [%s]"
                , plugin
                , method
                , id
                , args
            );
        }

        AndroidNativePython_call(plugin, method, id, args);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    protected CallbackResponse pythonCallResponseGet(int id) {
        Iterator itr = m_callbackResponses.iterator();

        CallbackResponse response;

        while(itr.hasNext())
        {
            response = (CallbackResponse)itr.next();

            if( response.id != id )
            {
                continue;
            }

            itr.remove();

            return response;
        }

        return null;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonCallResponse(int id, Object result) {
        if (BuildConfig.DEBUG) {
            MengineLog.logInfo(TAG, "pythonCallResponse [%d] result [%s]"
                , id
                , result
            );
        }

        CallbackResponse response = this.pythonCallResponseGet(id);

        if (response == null) {
            MengineLog.logError(TAG, "responceCall [%d] not found"
                , id
            );

            return;
        }

        response.cb.callback(result);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void addPythonPlugin(String name, Object plugin) {
        MengineLog.logInfo(TAG, "addPythonPlugin [%s] plugin: %s"
            , name
            , plugin
        );

        AndroidNativePython_addPlugin(name, plugin);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Semaphore Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void activateSemaphore(String name) {
        MengineSemaphore semaphore = m_semaphores.get(name);

        if (semaphore == null) {
            MengineSemaphore new_semaphore = new MengineSemaphore(true);

            m_semaphores.put(name, new_semaphore);

            return;
        }

        semaphore.activate();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void waitSemaphore(String name, MengineCallbackInterface cb) {
        MengineSemaphore semaphore = m_semaphores.get(name);

        if (semaphore == null) {
            MengineSemaphore new_semaphore = new MengineSemaphore(false);

            new_semaphore.addListener(cb);

            m_semaphores.put(name, new_semaphore);

            return;
        }

        semaphore.addListener(cb);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void waitAndroidSemaphore(String name) {
        MengineCallbackInterface cb = (Object) -> {
            AndroidNativePython_activateSemaphore(name);
        };

        this.waitSemaphore(name, cb);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Linking Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public boolean linkingOpenURL(String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(url));
        this.startActivity(Intent.createChooser(intent, ""));

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public boolean linkingOpenMail(String email, String subject, String body) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(email));
        this.startActivity(Intent.createChooser(intent, ""));
        intent.putExtra(Intent.EXTRA_SUBJECT, subject);
        intent.putExtra(Intent.EXTRA_TEXT, body);
        this.startActivity(Intent.createChooser(intent, ""));

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Asset Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public int openAssetFile(String path) {
        try {
            InputStream stream = getAssets().open(path);

            m_fileEnumerator++;
            int id = m_fileEnumerator;

            m_openFiles.put(id, stream);

            return id;
        } catch(IOException e) {
            MengineLog.logError(TAG, "open asset file [%s] ex: %s"
                , path
                , e.getLocalizedMessage()
            );

            return 0;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public int availableAssetFile(int id) {
        InputStream stream = m_openFiles.get(id);

        int size = 0;

        try {
            size = stream.available();
        } catch (IOException e) {
            MengineLog.logError(TAG, "available asset file [%d] ex: %s"
                , id
                , e.getLocalizedMessage()
            );

            return 0;
        }

        return size;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public byte[] readAssetFile(int id, int offset, int size) {
        InputStream stream = m_openFiles.get(id);

        byte[] buffer = new byte[size];

        int read = 0;

        try {
            read = stream.read(buffer, offset, size);
        } catch (IOException e) {
            MengineLog.logError(TAG, "read asset file [%d] offset [%d] size [%d] ex: %s"
                , id
                , offset
                , size
                , e.getLocalizedMessage()
            );

            return null;
        }

        return buffer;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public long skipAssetFile(int id, long offset) {
        InputStream stream = m_openFiles.get(id);

        long skip;

        try {
            skip = stream.skip(offset);
        } catch (IOException e) {
            MengineLog.logError(TAG, "skip asset file [%d] offset [%d] ex: %s"
                , id
                , offset
                , e.getLocalizedMessage()
            );

            return 0;
        }

        return skip;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void resetAssetFile(int id) {
        InputStream stream = m_openFiles.get(id);

        try {
            stream.reset();
        } catch (IOException e) {
            MengineLog.logError(TAG, "reset asset file [%d] ex: %s"
                , id
                , e.getLocalizedMessage()
            );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void closeAssetFile(int id) {
        InputStream stream = m_openFiles.get(id);

        try {
            stream.close();
        } catch (IOException e) {
            MengineLog.logError(TAG, "close asset file [%d] ex: %s"
                , id
                , e.getLocalizedMessage()
            );
        }

        m_openFiles.remove(id);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
}
