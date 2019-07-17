package org.Mengine.Build;

import android.content.*;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

import com.facebook.CallbackManager;
import com.facebook.appevents.AppEventsLogger;

import org.Mengine.Build.AdMob.AdMobInteractionLayer;
import org.Mengine.Build.DevToDev.DevToDevInteractionLayer;
import org.Mengine.Build.Facebook.FacebookInteractionLayer;
import org.Mengine.Build.UnityAds.UnityAdsInteractionLayer;
import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

public class MengineActivity extends SDLActivity {
    private static final String TAG = "MENGINE";

    public FacebookInteractionLayer facebookInteractionLayer;
    public UnityAdsInteractionLayer unityAdsInteractionLayer;
    public AdMobInteractionLayer adMobInteractionLayer;
    public DevToDevInteractionLayer devToDevInteractionLayer;

    public CallbackManager callbackManager;

    private static MengineActivity _instance;

    static native void AndroidNativeFacebook_setupFacebookJNI();
    static native void AndroidNativeFacebook_onSDKInitialized();

    static native void AndroidNativeUnity_setupUnityJNI();
    static native void AndroidNativeUnity_onSDKInitialized();

    static native void AndroidNativeAdMob_setupAdMobJNI();
    static native void AndroidNativeAdMob_onSDKInitialized();

    static native void AndroidNativeDevToDev_setupDevToDevJNI();
    static native void AndroidNativeDevToDev_onSDKInitialized();

    static native void AndroidNativeLinking_setupLinkingJNI();

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL2",
                "SDLApplication"
        };
    }

    protected void initPlugins()
    {
        AppEventsLogger.activateApp(getApplication());

        _instance = this;

        AndroidNativeFacebook_setupFacebookJNI();
        AndroidNativeUnity_setupUnityJNI();
        AndroidNativeAdMob_setupAdMobJNI();
        AndroidNativeDevToDev_setupDevToDevJNI();
        AndroidNativeLinking_setupLinkingJNI();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.e(TAG, "MengineActivity.onCreate()");

        initPlugins();
    }

    @Override
    protected void onStop() {
        super.onStop();

        Log.e(TAG, "MengineActivity.onStop()");

        _instance = null;
    }

    @Override
    public void onPause(){
        super.onPause();

        Log.e(TAG, "MengineActivity.onPause()");
    }

    @Override
    public void onResume(){
        super.onResume();

        Log.e(TAG, "MengineActivity.onResume()");
        if (_instance != null)
        {
            if (_instance.facebookInteractionLayer == null)
            {
                Log.e(TAG, "_instance != null AND _instance.facebookInteractionLayer == null");
            }
            else
            {
                Log.e(TAG, "_instance != null AND _instance.facebookInteractionLayer != null");
            }
        }
        else
        {
            Log.e(TAG, "_instance == null -> init plugins");

            initPlugins();
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
    
    public static void facebookPerformLogin(String[] readPermissions) {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.performLogin(_instance, readPermissions);
        }
    }

    public static void facebookGetUser() {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.getUser();
        }
    }

    public static void facebookShareLink(String link) {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.shareLink(_instance, link);
        }
    }

    public static void facebookGetProfilePictureLink(String typeParameter) {
        if (_instance != null && _instance.facebookInteractionLayer != null) {
            _instance.facebookInteractionLayer.getProfilePictureLink(typeParameter);
        }
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

    public static void unitySetupAds(final boolean debug) {
        if (_instance != null && _instance.unityAdsInteractionLayer != null) {
            _instance.unityAdsInteractionLayer.setupAds(_instance, debug);
        }
    }

    public static void unityShowAd(final String placementId) {

        if (_instance != null && _instance.unityAdsInteractionLayer != null) {
            _instance.unityAdsInteractionLayer.showAd(_instance, placementId);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //AdMob Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void admobInitializePlugin(final String admobAppId, final String interAdUnitId, final String videoAdUnitId) {
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                if (_instance != null && _instance.adMobInteractionLayer == null) {
                    _instance.adMobInteractionLayer = new AdMobInteractionLayer(_instance, admobAppId, interAdUnitId, videoAdUnitId);
                    AndroidNativeAdMob_onSDKInitialized();
                }
            }
        });
    }

    public static void admobSetupInterstitialAd() {
        if (_instance != null && _instance.adMobInteractionLayer != null) {
            _instance.adMobInteractionLayer.setupInterstitialAd();
        }
    }

    public static void admobShowInterstitialAd() {
        if (_instance != null && _instance.adMobInteractionLayer != null) {
            _instance.adMobInteractionLayer.showInterstitialAd();
        }
    }

    public static void admobSetupRewardedVideoAd() {
        if (_instance != null && _instance.adMobInteractionLayer != null) {
            _instance.adMobInteractionLayer.setupRewardedVideoAd();
        }
    }

    public static void admobShowRewardedVideoAd() {
        if (_instance != null && _instance.adMobInteractionLayer != null) {
            _instance.adMobInteractionLayer.showRewardedVideoAd();
        }
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

    public static void devtodevOnTutorialEvent(int stateOrStep) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onTutorialEvent(stateOrStep);
        }
    }

    public static void devtodevSetCurrentLevel(int level) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.setCurrentLevel(level);
        }
    }

    public static void devtodevOnLevelUp(int level) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onLevelUp(level);
        }
    }

    public static void devtodevOnCurrencyAccrual(String currencyName, int currencyAmount, int accrualType) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onCurrencyAccrual(currencyName, currencyAmount, accrualType);
        }
    }

    public static void devtodevOnRealPayment(String paymentId, float inAppPrice, String inAppName, String inAppCurrencyISOCode) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onRealPayment(paymentId, inAppPrice, inAppName, inAppCurrencyISOCode);
        }
    }

    public static void devtodevOnInAppPurchase(String purchaseId, String purchaseType, int purchaseAmount, int purchasePrice, String purchaseCurrency) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onInAppPurchase(purchaseId, purchaseType, purchaseAmount, purchasePrice, purchaseCurrency);
        }
    }

    public static void devtodevOnSimpleCustomEvent(String eventName) {
        if (_instance != null && _instance.devToDevInteractionLayer != null) {
            _instance.devToDevInteractionLayer.onSimpleCustomEvent(eventName);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Linking Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void openURL(final String url) {
        Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse(url));
        _instance.startActivity(Intent.createChooser(intent, ""));
    }

    public static void openMail(final String email, final String subject, final String body) {
        Intent intent = new Intent(Intent.ACTION_SEND);
        intent.setType("plain/text");
        intent.putExtra(Intent.EXTRA_EMAIL, new String[] { email });
        intent.putExtra(Intent.EXTRA_SUBJECT, subject);
        intent.putExtra(Intent.EXTRA_TEXT, body);
        _instance.startActivity(Intent.createChooser(intent, ""));
    }
}
