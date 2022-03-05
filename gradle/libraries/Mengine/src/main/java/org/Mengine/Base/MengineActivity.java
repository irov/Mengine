package org.Mengine.Base;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineApplication;

import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.nio.charset.Charset;
import java.nio.file.Path;
import java.util.ArrayList;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

import org.json.JSONException;
import org.json.JSONObject;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import android.app.Notification;
import android.content.*;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.provider.Settings.Secure;
import android.view.KeyEvent;

public class MengineActivity extends SDLActivity {
    public static final String TAG = "Mengine";

    private Map<String, Integer> _requestCodes;

    class CallbackResponse
    {
        public Integer id;
        public MenginePlugin.CallbackInterface cb;
    };

    private ArrayList<CallbackResponse> _callbackResponses;
    private Integer _callbackResponseEnumerator;

    private static native void AndroidNativeMengine_setMengineAndroidActivityJNI(Object Activity);
    private static native void AndroidNativeMengine_quitMengineAndroidActivityJNI();
    private static native String AndroidNativeMengine_getCompanyName();
    private static native String AndroidNativeMengine_getProjectName();
    private static native int AndroidNativeMengine_getProjectVersion();
    private static native boolean AndroidNativeMengine_isDebugMode();
    private static native boolean AndroidNativeMengine_isDevelopmentMode();
    private static native boolean AndroidNativeMengine_isBuildMaster();
    private static native boolean AndroidNativeMengine_isBuildPublish();
    private static native String AndroidNativeMengine_getEngineGITSHA1();
    private static native String AndroidNativeMengine_getBuildTimestamp();
    private static native String AndroidNativeMengine_getBuildUsername();
    private static native String AndroidNativeMengine_getBuildVersion();
    private static native String AndroidNativeMengine_getConfigValue(String section, String key, String default_value);

    private static native void AndroidNativePython_setupPythonJNI();
    private static native void AndroidNativePython_addPlugin(String name, Object plugin);
    private static native void AndroidNativePython_call(String plugin, String method, Integer responseId, String args);

    public MengineActivity() {
        _requestCodes = new HashMap<String, Integer>();

        _callbackResponseEnumerator = 0;
        _callbackResponses = new ArrayList<CallbackResponse>();
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL2",
                "SDLApplication"
        };
    }

    public static String getCompanyName()
    {
       return AndroidNativeMengine_getCompanyName();
    }

    public static String getProjectName()
    {
        return AndroidNativeMengine_getProjectName();
    }

    public static int getProjectVersion()
    {
        return AndroidNativeMengine_getProjectVersion();
    }

    public static boolean isDebugMode()
    {
        return AndroidNativeMengine_isDebugMode();
    }

    public static boolean isDevelopmentMode()
    {
        return AndroidNativeMengine_isDevelopmentMode();
    }

    public static boolean isBuildMaster()
    {
        return AndroidNativeMengine_isBuildMaster();
    }

    public static boolean isBuildPublish()
    {
        return AndroidNativeMengine_isBuildPublish();
    }

    public static String getEngineGITSHA1()
    {
        return AndroidNativeMengine_getEngineGITSHA1();
    }

    public static String getBuildTimestamp()
    {
        return AndroidNativeMengine_getBuildTimestamp();
    }

    public static String getBuildUsername()
    {
        return AndroidNativeMengine_getBuildUsername();
    }

    public static String getBuildVersion()
    {
        return AndroidNativeMengine_getBuildVersion();
    }

    public static String getConfigValue(String section, String key, String default_value)
    {
        return AndroidNativeMengine_getConfigValue(section, key, default_value);
    }

    protected ArrayList<MenginePlugin> getPlugins() {
        MengineApplication app = (MengineApplication)this.getApplication();

        return app.plugins;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.i(TAG, "MengineActivity.onCreate()");

        if(mBrokenLibraries == true)
        {
            Log.e(TAG, "MengineActivity.onCreate: broken libraries");

            AndroidNativeMengine_quitMengineAndroidActivityJNI();

            return;
        }

        for(MenginePlugin p : this.getPlugins()) {
            p.setActivity(this);
        }

        AndroidNativeMengine_setMengineAndroidActivityJNI(this);
        AndroidNativePython_setupPythonJNI();

        AndroidNativePython_addPlugin("Activity", this);

        for(MenginePlugin p : this.getPlugins()) {
            p.onCreate(this, savedInstanceState);
        }
    }

    public void quitMengineApplication() {
        AndroidNativeMengine_quitMengineAndroidActivityJNI();
    }

    public void onMengineInitializeBaseServices() {
        Log.i(TAG, "MengineActivity.onMengineInitializeBaseServices()");

        for(MenginePlugin p : this.getPlugins()) {
            p.onMengineInitializeBaseServices(this);
        }

        Log.i(TAG, "MengineActivity.onMengineInitializeBaseServices()");
    }

    public void onMengineCreateApplication() {
        Log.i(TAG, "MengineActivity.onMengineCreateApplication()");

        for(MenginePlugin p : this.getPlugins()) {
            p.onMengineCreateApplication(this);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        Log.i(TAG, "MengineActivity.onActivityResult()");

        for(MenginePlugin p : this.getPlugins()) {
            p.onActivityResult(this, requestCode, resultCode, data);
        }
    }

    @Override
    public void onStart(){
        super.onStart();

        Log.i(TAG, "MengineActivity.onStart()");

        for(MenginePlugin p : this.getPlugins()) {
            p.onStart(this);
        }
    }

    @Override
    protected void onStop() {
        super.onStop();

        Log.i(TAG, "MengineActivity.onStop()");

        for(MenginePlugin p : this.getPlugins()) {
            p.onStop(this);
        }
    }

    @Override
    public void onPause() {
        super.onPause();

        Log.i(TAG, "MengineActivity.onPause()");

        for(MenginePlugin p : this.getPlugins()) {
            p.onPause(this);
        }
    }

    @Override
    public void onResume() {
        super.onResume();

        Log.i(TAG, "MengineActivity.onResume()");

        for(MenginePlugin p : this.getPlugins()) {
            p.onResume(this);
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        Log.i(TAG, "MengineActivity.onNewIntent()");

        for(MenginePlugin p : this.getPlugins()) {
            p.onNewIntent(this, intent);
        }
    }

    @Override
    public void onDestroy(){
        super.onDestroy();

        Log.i(TAG, "MengineActivity.onDestroy()");

        for(MenginePlugin p : this.getPlugins()) {
            p.onDestroy(this);
        }
    }

    @Override
    public void onRestart(){
        super.onRestart();

        Log.i(TAG, "MengineActivity.onRestart()");

        for(MenginePlugin p : this.getPlugins()) {
            p.onRestart(this);
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig){
        super.onConfigurationChanged(newConfig);

        for(MenginePlugin p : this.getPlugins()) {
            p.onConfigurationChanged(this, newConfig);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults){
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        for(MenginePlugin p : this.getPlugins()) {
            p.onRequestPermissionsResult(this, requestCode, permissions, grantResults);
        }
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event){
        Log.i(TAG, "MengineActivity.dispatchKeyEvent() action: " + event.getAction() + " code: " + event.getKeyCode());

        for(MenginePlugin p : this.getPlugins()) {
            if (p.dispatchKeyEvent(this, event) == true) {
                return true;
            }
        }

        return super.dispatchKeyEvent(event);
    }

    @Override
    protected SDLSurface createSDLSurface(Context context) {
        Log.i(TAG, "MengineActivity.createSDLSurface()");

        return new MengineSurface(context);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Kernel Methods    
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public String getAndroidId() {
        ContentResolver resolver = this.getContext().getContentResolver();

        String android_id = Secure.getString(resolver, Secure.ANDROID_ID);

        return android_id;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public int genRequestCode(String name) {
        if (!_requestCodes.containsKey(name)) {
            _requestCodes.put(name, _requestCodes.size());
        }
        return _requestCodes.get(name);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Python Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonInitializePlugins() {
        for(MenginePlugin p : this.getPlugins()) {
            p.onPythonEmbedding(this);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    private String pythonCallBuildArgs(Object ... args)
    {
        StringBuilder py_args = new StringBuilder();

        py_args.append("(");

        for(int i = 0; i != args.length; ++i) {
            Object a = args[i];

            if(a instanceof String) {
                py_args.append("\"");
                py_args.append(a);
                py_args.append("\"");
            }
            else if(a instanceof String[]) {
                String[] aa = (String[])a;
                py_args.append("[");
                int index = 0;
                for(String s : aa)
                {
                    if(index != 0)
                    {
                        py_args.append(",");
                    }

                    py_args.append("\"");
                    py_args.append(s);
                    py_args.append("\"");

                    index++;
                }
                py_args.append("]");
            }
            else
            {
                py_args.append(a);
            }

            py_args.append(",");
        }

        py_args.append(")");

        String py_args_str = py_args.toString();

        return py_args_str;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonCall(String plugin, String method, Object ... args)
    {
        String py_args_str = this.pythonCallBuildArgs(args);

        AndroidNativePython_call(plugin, method, 0, py_args_str);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonCallCb(String plugin, String method, MenginePlugin.CallbackInterface cb, Object ... args)
    {
        _callbackResponseEnumerator++;

        Integer id = _callbackResponseEnumerator;

        CallbackResponse cr = new CallbackResponse();
        cr.id = id;
        cr.cb = cb;

        _callbackResponses.add(cr);

        String py_args_str = this.pythonCallBuildArgs(args);

        AndroidNativePython_call(plugin, method, id, py_args_str);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void responseCall(Integer id, Object result)
    {
        Iterator itr = _callbackResponses.iterator();

        while(itr.hasNext())
        {
            CallbackResponse cr = (CallbackResponse)itr.next();

            if( cr.id != id )
            {
                continue;
            }

            cr.cb.callback(result);

            itr.remove();

            return;
        }

        Log.e(TAG, "MengineActivity.responceCall not found id " + id);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void addPythonPlugin(String name, Object plugin)
    {
        AndroidNativePython_addPlugin(name, plugin);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Linking Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public boolean linkingOpenURL(String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(url));
        this.startActivity(Intent.createChooser(intent, ""));

        return true;
    }

    public boolean linkingOpenMail(String email, String subject, String body) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(email));
        this.startActivity(Intent.createChooser(intent, ""));
        intent.putExtra(Intent.EXTRA_SUBJECT, subject);
        intent.putExtra(Intent.EXTRA_TEXT, body);
        this.startActivity(Intent.createChooser(intent, ""));

        return true;
    }

    /***********************************************************************************************
    //OLD PLugins: TODO

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Facebook Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void facebookInitializePlugin() {
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                if (_instance != null && _instance.facebookInteractionLayer == null) {
                    _instance.facebookInteractionLayer = new FacebookInteractionLayer(_instance);
                    AndroidNativeFacebook_onSDKInitialized();
                }
            }
        });
    }

    public static boolean facebookIsLoggedIn() {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            return _instance.facebookInteractionLayer.isLoggedIn();
        } else {
            return false;
        }
    }

    public static String facebookGetAccessToken() {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            return _instance.facebookInteractionLayer.getAccessToken();
        } else {
            return "";
        }
    }

    public static boolean facebookPerformLogin(String[] readPermissions) {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.performLogin(_instance, readPermissions);

            return true;
        }

        return false;
    }

    public static boolean facebookLogout() {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.LogoutFromFacebook();
            return true;
        }

        return false;
    }

    public static boolean facebookGetUser() {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.getUser();

            return true;
        }

        return false;
    }

    public static boolean facebookShareLink(String link, String picture, String message) {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.shareLink(_instance, link, picture, message);

            return true;
        }

        return false;
    }

    public static boolean facebookGetProfilePictureLink(String typeParameter) {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.getProfilePictureLink(typeParameter);

            return true;
        }

        return false;
    }

    public static boolean facebookGetProfileUserPictureLink(String user_id, String typeParameter) {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.getProfileUserPictureLink(user_id, typeParameter);
            return true;
        }

        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //UnityAds Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void unityInitializePlugin(final String gameId) {
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                if (_instance != null && _instance.unityAdsInteractionLayer == null) {
                    _instance.unityAdsInteractionLayer = new UnityAdsInteractionLayer(gameId);
                    AndroidNativeUnity_onSDKInitialized();
                }
            }
        });
    }

    public static boolean unitySetupAds(final boolean debug) {
        if (_instance != null && _instance.unityAdsInteractionLayer != null) {
            _instance.unityAdsInteractionLayer.setupAds(_instance, debug);

            return true;
        }

        return false;
    }

    public static boolean unityShowAd(final String placementId) {
        if (_instance != null && _instance.unityAdsInteractionLayer != null) {
            _instance.unityAdsInteractionLayer.showAd(_instance, placementId);

            return true;
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //AdMob Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void admobInitializePlugin(final String interAdUnitId, final String videoAdUnitId) {
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                if (_instance != null && _instance.adMobInteractionLayer == null) {
                    _instance.adMobInteractionLayer = new AdMobInteractionLayer(_instance, interAdUnitId, videoAdUnitId);
                    AndroidNativeAdMob_onSDKInitialized();
                }
            }
        });
    }

    public static boolean admobSetupInterstitialAd() {
        if (_instance != null && _instance.adMobInteractionLayer != null) {
            _instance.adMobInteractionLayer.setupInterstitialAd();

            return true;
        }

        return false;
    }

    public static boolean admobShowInterstitialAd() {
        if (_instance != null && _instance.adMobInteractionLayer != null) {
            _instance.adMobInteractionLayer.showInterstitialAd();

            return true;
        }

        return false;
    }

    public static boolean admobSetupRewardedVideoAd() {
        if (_instance != null && _instance.adMobInteractionLayer != null) {
            _instance.adMobInteractionLayer.setupRewardedVideoAd();

            return true;
        }

        return false;
    }

    public static boolean admobShowRewardedVideoAd() {
        if (_instance != null && _instance.adMobInteractionLayer != null) {
            _instance.adMobInteractionLayer.showRewardedVideoAd();

            return true;
        }

        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //DevToDev Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void devtodevInitializePlugin(final String appId, final String secret, final String apiKey) {
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                if (_instance != null && _instance.devToDevInteractionLayer == null) {
                    _instance.devToDevInteractionLayer = new DevToDevInteractionLayer(_instance, appId, secret, apiKey);
                    AndroidNativeDevToDev_onSDKInitialized();
                }
            }
        });
    }

    public static boolean devtodevOnTutorialEvent(int stateOrStep) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onTutorialEvent(stateOrStep);

            return true;
        }

        return false;
    }

    public static boolean devtodevSetCurrentLevel(int level) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.setCurrentLevel(level);

            return true;
        }

        return false;
    }

    public static boolean devtodevOnLevelUp(int level) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onLevelUp(level);

            return true;
        }

        return false;
    }

    public static boolean devtodevOnCurrencyAccrual(String currencyName, int currencyAmount, int accrualType) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onCurrencyAccrual(currencyName, currencyAmount, accrualType);

            return true;
        }

        return false;
    }

    public static boolean devtodevOnRealPayment(String paymentId, float inAppPrice, String inAppName, String inAppCurrencyISOCode) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onRealPayment(paymentId, inAppPrice, inAppName, inAppCurrencyISOCode);

            return true;
        }

        return false;
    }

    public static boolean devtodevOnInAppPurchase(String purchaseId, String purchaseType, int purchaseAmount, int purchasePrice, String purchaseCurrency) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onInAppPurchase(purchaseId, purchaseType, purchaseAmount, purchasePrice, purchaseCurrency);

            return true;
        }

        return false;
    }

    public static boolean devtodevOnSimpleCustomEvent(String eventName, String intJSON, String floatJSON, String stringJSON) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onSimpleCustomEvent(eventName, intJSON, floatJSON, stringJSON);

            return true;
        }

        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Local Notifications Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void localNotificationsInitializePlugin() {
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                if (_instance != null) {
                    if (_instance.localNotificationsInteractionLayer == null) {
                        _instance.localNotificationsInteractionLayer = new LocalNotificationsInteractionLayer(_instance);
                        AndroidNativeLocalNotifications_onLocalNotificationsInitialized();
                    }
                }
            }
        });
    }

    public static boolean scheduleLocalNotification(int id, String title, String content, int delay) {
        if (_instance != null && _instance.localNotificationsInteractionLayer != null) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                _instance.localNotificationsInteractionLayer.scheduleJobServiceNotification(id, title, content, delay);
            }else {
                Notification notification = LocalNotificationsInteractionLayer.getNotification(_instance, id, title, content);
                _instance.localNotificationsInteractionLayer.scheduleNotification(notification, id, delay);
            }
            return true;
        }

        return false;
    }

    public static boolean instantlyPresentLocalNotification(int id, String title, String content) {
        if (_instance != null && _instance.localNotificationsInteractionLayer != null) {
            Notification notification = LocalNotificationsInteractionLayer.getNotification(_instance, id, title, content);
            _instance.localNotificationsInteractionLayer.instantlyPresentNotification(notification, id);

            return true;
        }

        return false;
    }

    public static boolean cancelAllLocalNotification() {
        if (_instance != null && _instance.localNotificationsInteractionLayer != null) {
            _instance.localNotificationsInteractionLayer.cancelAll();

            return true;
        }

        return false;
    }
    ***********************************************************************************************/
}
