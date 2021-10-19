package org.Mengine.Build;

import org.Mengine.Build.AdMob.AdMobInteractionLayer;
import org.Mengine.Build.DevToDev.DevToDevInteractionLayer;
import org.Mengine.Build.Facebook.FacebookInteractionLayer;
import org.Mengine.Build.LocalNotifications.LocalNotificationsInteractionLayer;
import org.Mengine.Build.LocalNotifications.NotificationPublisher;
import org.Mengine.Build.UnityAds.UnityAdsInteractionLayer;
import org.Mengine.Build.MarSDK.MarSDKInteractionLayer;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

import com.facebook.CallbackManager;
import com.facebook.appevents.AppEventsLogger;

import io.sentry.Sentry;
import io.sentry.CustomSamplingContext;
import io.sentry.android.core.SentryAndroid;

import android.app.Notification;
import android.content.*;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.provider.Settings.Secure;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;

public class MengineActivity extends SDLActivity {
    public static final String TAG = "MENGINE";

    public FacebookInteractionLayer facebookInteractionLayer;
    public UnityAdsInteractionLayer unityAdsInteractionLayer;
    public MarSDKInteractionLayer marSDKInteractionLayer;
    public AdMobInteractionLayer adMobInteractionLayer;
    public DevToDevInteractionLayer devToDevInteractionLayer;
    public LocalNotificationsInteractionLayer localNotificationsInteractionLayer;

    public CallbackManager callbackManager;

    private static MengineActivity _instance;

    private static native void AndroidNativeMengine_setMengineAndroidActivityJNI(Object Activity);
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

    private static native void AndroidNativeKernel_setupKernelJNI();
    private static native void AndroidNativePython_setupPythonJNI();
    private static native void AndroidNativePython_addPlugin(String name, Object plugin);
    private static native void AndroidNativePython_call(String method, String args);
    private static native void AndroidNativeFacebook_setupFacebookJNI();
    private static native void AndroidNativeFacebook_onSDKInitialized();
    private static native void AndroidNativeUnity_setupUnityJNI();
    private static native void AndroidNativeUnity_onSDKInitialized();
    private static native void AndroidNativeAdMob_setupAdMobJNI();
    private static native void AndroidNativeAdMob_onSDKInitialized();
    private static native void AndroidNativeDevToDev_setupDevToDevJNI();
    private static native void AndroidNativeDevToDev_onSDKInitialized();
    private static native void AndroidNativeLinking_setupLinkingJNI();
    private static native void AndroidNativeLocalNotifications_setupLocalNotificationsJNI();
    private static native void AndroidNativeLocalNotifications_onLocalNotificationsInitialized();
    private static native void AndroidNativeLocalNotifications_onLocalNotificationsPress(int id);

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL2",
                "SDLApplication"
        };
    }

    protected void initPlugins() {
        AppEventsLogger.activateApp(getApplication());

        this.marSDKInteractionLayer = new MarSDKInteractionLayer(this);

        AndroidNativeKernel_setupKernelJNI();
        AndroidNativePython_setupPythonJNI();

        AndroidNativeFacebook_setupFacebookJNI();
        AndroidNativeUnity_setupUnityJNI();
        AndroidNativeAdMob_setupAdMobJNI();
        AndroidNativeDevToDev_setupDevToDevJNI();
        AndroidNativeLinking_setupLinkingJNI();
        AndroidNativeLocalNotifications_setupLocalNotificationsJNI();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        _instance = this;

        super.onCreate(savedInstanceState);

        Log.i(TAG, "MengineActivity.onCreate()");

        AndroidNativeMengine_setMengineAndroidActivityJNI(this);

        initPlugins();

        if (_instance.getIntent().hasExtra(NotificationPublisher.NOTIFICATION_ID)) {
            AndroidNativeLocalNotifications_onLocalNotificationsPress(_instance.getIntent().getIntExtra(NotificationPublisher.NOTIFICATION_ID, 0));
        }
    }

    public void onMengineInitializeBaseServices() {
        Log.i(TAG, "MengineActivity.onMengineInitializeBaseServices()");

        SentryAndroid.init(this, options -> {
            String SENTRY_DNS = AndroidNativeMengine_getConfigValue("Sentry", "DSN", "");
            options.setDsn(SENTRY_DNS);
            Log.i(TAG, "Sentry DNS:" + SENTRY_DNS);

            String buildVersion = AndroidNativeMengine_getBuildVersion();
            options.setRelease(buildVersion);
            Log.i(TAG, "Sentry Build Version:" + buildVersion);
        });
    }

    public void onMengineCreateApplication() {
        Sentry.configureScope(scope -> {
            String SENTRY_APPLICATION = AndroidNativeMengine_getConfigValue( "Sentry", "Application", "Mengine" );

            String companyName = AndroidNativeMengine_getCompanyName();
            String projectName = AndroidNativeMengine_getProjectName();
            int projectVersion = AndroidNativeMengine_getProjectVersion();
            boolean isDebugMode = AndroidNativeMengine_isDebugMode();
            boolean isDevelopmentMode = AndroidNativeMengine_isDevelopmentMode();
            boolean isBuildMaster = AndroidNativeMengine_isBuildMaster();
            boolean isBuildPublish = AndroidNativeMengine_isBuildPublish();
            String engineGITSHA1 = AndroidNativeMengine_getEngineGITSHA1();
            String buildTimestamp = AndroidNativeMengine_getBuildTimestamp();
            String buildUsername = AndroidNativeMengine_getBuildUsername();

            scope.setExtra("Application", SENTRY_APPLICATION);
            scope.setExtra("Company", companyName);
            scope.setExtra("Project", projectName);
            scope.setExtra("Version", String.valueOf(projectVersion));
            scope.setExtra("Debug", String.valueOf(isDebugMode));
            scope.setExtra("Development", String.valueOf(isDevelopmentMode));
            scope.setExtra("Master", String.valueOf(isBuildMaster));
            scope.setExtra("Publish", String.valueOf(isBuildPublish));
            scope.setExtra("Engine Commit", engineGITSHA1);
            scope.setExtra("Build Timestamp", buildTimestamp);
            scope.setExtra("Build Username", buildUsername);
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        Log.i(TAG, "MengineActivity.onActivityResult()");

        if (marSDKInteractionLayer != null) {
            marSDKInteractionLayer.onActivityResult(requestCode, resultCode, data);
        }

        if (callbackManager != null) {
            callbackManager.onActivityResult(requestCode, resultCode, data);
        }
    }

    @Override
    public void onStart(){
        super.onStart();

        Log.i(TAG, "MengineActivity.onStart()");

        if( marSDKInteractionLayer != null ) {
            marSDKInteractionLayer.onStart();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();

        Log.i(TAG, "MengineActivity.onStop()");

        _instance = null;
    }

    @Override
    public void onPause() {
        super.onPause();

        Log.i(TAG, "MengineActivity.onPause()");

        if( marSDKInteractionLayer != null ) {
            marSDKInteractionLayer.onPause();
        }
    }

    @Override
    public void onResume() {
        super.onResume();

        Log.i(TAG, "MengineActivity.onResume()");

        if (_instance == null) {
            Log.i(TAG, "_instance == null -> init plugins");

            _instance = this;

            initPlugins();
        }
        else {
            if (marSDKInteractionLayer != null) {
                marSDKInteractionLayer.onResume();
            }
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        Log.i(TAG, "MengineActivity.onNewIntent()");

        if (marSDKInteractionLayer != null) {
            marSDKInteractionLayer.onNewIntent(intent);
        }

        if (intent.hasExtra(NotificationPublisher.NOTIFICATION_ID)) {
            AndroidNativeLocalNotifications_onLocalNotificationsPress(intent.getIntExtra(NotificationPublisher.NOTIFICATION_ID, 0));
        }
    }

    @Override
    public void onDestroy(){
        super.onDestroy();

        Log.i(TAG, "MengineActivity.onDestroy()");

        if (marSDKInteractionLayer != null) {
            marSDKInteractionLayer.onDestroy();
        }
    }

    @Override
    public void onRestart(){
        super.onRestart();

        Log.i(TAG, "MengineActivity.onRestart()");

        if (marSDKInteractionLayer != null) {
            marSDKInteractionLayer.onRestart();
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig){
        super.onConfigurationChanged(newConfig);

        if (marSDKInteractionLayer != null) {
            marSDKInteractionLayer.onRestart();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults){
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (marSDKInteractionLayer != null) {
            marSDKInteractionLayer.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event){
        if (marSDKInteractionLayer != null) {
            if( marSDKInteractionLayer.dispatchKeyEvent(event) == true )
            {
                return true;
            }
        }

        return false;
    }

    @Override
    protected SDLSurface createSDLSurface(Context context) {
        Log.i(TAG, "MengineActivity.createSDLSurface()");

        return new MengineSurface(context);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Kernel Methods    
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static String kernelGetAndroidId() {
        String android_id = Secure.getString(getContext().getContentResolver(), Secure.ANDROID_ID);

        return android_id;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Python Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void pythonInitializePlugin() {
        _instance.addPythonPlugin("Activity", _instance);

        _instance.marSDKInteractionLayer.onPythonEmbedding();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void pythonCall(String method, Object ... args)
    {
        String py_args = "";

        py_args += "(";

        for( int i = 0; i != args.length; ++i )
        {
            Object a = args[i];

            if( a instanceof String )
            {
                py_args += "\"";
                py_args += a;
                py_args += "\"";
            }
            else
            {
                py_args += a;
            }

            py_args += ",";
        }

        py_args += ")";

        // args example: "(arg1,arg2,arg3)" - string
        AndroidNativePython_call(method, py_args);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public void addPythonPlugin(String name, Object plugin)
    {
        AndroidNativePython_addPlugin(name, plugin);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Facebook Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void facebookInitializePlugin() {
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                if (_instance != null && _instance.facebookInteractionLayer == null) {
                    _instance.callbackManager = CallbackManager.Factory.create();
                    _instance.facebookInteractionLayer = new FacebookInteractionLayer(_instance.callbackManager);
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
    //Linking Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean linkingOpenURL(String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(url));
        _instance.startActivity(Intent.createChooser(intent, ""));
        
        return true;
    }

    public static boolean linkingOpenMail(String email, String subject, String body) {
//        Intent intent = new Intent(Intent.ACTION_SEND);
//        intent.setType("plain/text");
//        intent.putExtra(Intent.EXTRA_EMAIL, new String[] { email });
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(email));
        _instance.startActivity(Intent.createChooser(intent, ""));
        intent.putExtra(Intent.EXTRA_SUBJECT, subject);
        intent.putExtra(Intent.EXTRA_TEXT, body);
        _instance.startActivity(Intent.createChooser(intent, ""));
        
        return true;
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
}
