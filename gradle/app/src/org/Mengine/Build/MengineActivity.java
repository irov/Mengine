package org.Mengine.Build;

import android.app.Notification;
import android.content.*;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.provider.Settings.Secure;

import com.facebook.CallbackManager;
import com.facebook.appevents.AppEventsLogger;

import org.Mengine.Build.Kernel.KernelInteractionLayer;
import org.Mengine.Build.AdMob.AdMobInteractionLayer;
import org.Mengine.Build.DevToDev.DevToDevInteractionLayer;
import org.Mengine.Build.Facebook.FacebookInteractionLayer;
import org.Mengine.Build.LocalNotifications.LocalNotificationsInteractionLayer;
import org.Mengine.Build.LocalNotifications.NotificationPublisher;
import org.Mengine.Build.UnityAds.UnityAdsInteractionLayer;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

public class MengineActivity extends SDLActivity {
    public static final String TAG = "MENGINE";

    public FacebookInteractionLayer facebookInteractionLayer;
    public UnityAdsInteractionLayer unityAdsInteractionLayer;
    public AdMobInteractionLayer adMobInteractionLayer;
    public DevToDevInteractionLayer devToDevInteractionLayer;
    public LocalNotificationsInteractionLayer localNotificationsInteractionLayer;

    public CallbackManager callbackManager;

    private static MengineActivity _instance;

    private static native void AndroidNativeKernel_setupKernelJNI();
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

        AndroidNativeKernel_setupKernelJNI();
        
        AndroidNativeFacebook_setupFacebookJNI();
        AndroidNativeUnity_setupUnityJNI();
        AndroidNativeAdMob_setupAdMobJNI();
        AndroidNativeDevToDev_setupDevToDevJNI();
        AndroidNativeLinking_setupLinkingJNI();
        AndroidNativeLocalNotifications_setupLocalNotificationsJNI();
    }
    
    @Override
    protected void brokenLoadLibraries(String msg) {
        Log.e(TAG, "An error occurred while trying to start the application. Please try again and/or reinstall."
            + System.getProperty("line.separator")
            + System.getProperty("line.separator")
            + "Error: " + msg);
        
        throw new UnsatisfiedLinkError(msg);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        _instance = this;

        super.onCreate(savedInstanceState);

        Log.e(TAG, "MengineActivity.onCreate()");

        initPlugins();

        if (_instance.getIntent().hasExtra(NotificationPublisher.NOTIFICATION_ID)) {
            AndroidNativeLocalNotifications_onLocalNotificationsPress(_instance.getIntent().getIntExtra(NotificationPublisher.NOTIFICATION_ID, 0));
        }
    }

    @Override
    protected void onStop() {
        super.onStop();

        Log.e(TAG, "MengineActivity.onStop()");

        _instance = null;
    }

    @Override
    public void onPause() {
        super.onPause();

        Log.e(TAG, "MengineActivity.onPause()");
    }

    @Override
    public void onResume() {
        super.onResume();

        Log.e(TAG, "MengineActivity.onResume()");
        if (_instance != null) {
            if (_instance.facebookInteractionLayer == null) {
                Log.e(TAG, "_instance != null AND _instance.facebookInteractionLayer == null");
            } else {
                Log.e(TAG, "_instance != null AND _instance.facebookInteractionLayer != null");
            }
        } else {
            Log.e(TAG, "_instance == null -> init plugins");

            _instance = this;

            initPlugins();
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        if (intent.hasExtra(NotificationPublisher.NOTIFICATION_ID)) {
            AndroidNativeLocalNotifications_onLocalNotificationsPress(intent.getIntExtra(NotificationPublisher.NOTIFICATION_ID, 0));
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        Log.e(TAG, "MengineActivity.onActivityResult()");

        if (callbackManager != null) {
            callbackManager.onActivityResult(requestCode, resultCode, data);
        }
    }

    @Override
    protected SDLSurface createSDLSurface(Context context) {
        Log.e(TAG, "MengineActivity.createSDLSurface()");

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

    public static boolean facebookGetUser() {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.getUser();
            
            return true;
        }
        
        return false;
    }

    public static boolean facebookShareLink(String link) {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.shareLink(_instance, link);
            
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
    public static boolean openURL(final String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(url));
        _instance.startActivity(Intent.createChooser(intent, ""));
        
        return true;
    }

    public static boolean openMail(final String email, final String subject, final String body) {
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
