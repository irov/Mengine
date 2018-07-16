package org.Mengine.Build;

import android.content.*;
import android.os.Bundle;

import com.facebook.CallbackManager;
import com.facebook.appevents.AppEventsLogger;

import org.Mengine.Build.AdMob.AdMobInteractionLayer;
import org.Mengine.Build.DevToDev.DevToDevInteractionLayer;
import org.Mengine.Build.Facebook.FacebookInteractionLayer;
import org.Mengine.Build.UnityAds.UnityAdsInteractionLayer;
import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

public class MengineActivity extends SDLActivity {

    public FacebookInteractionLayer facebookInteractionLayer;
    public UnityAdsInteractionLayer unityAdsInteractionLayer;
    public AdMobInteractionLayer adMobInteractionLayer;
    public DevToDevInteractionLayer devToDevInteractionLayer;

    public CallbackManager callbackManager;

    private static MengineActivity _instance;

    static native void AndroidNativeFacebook_setupFacebookJNI();

    static native void AndroidNativeUnity_setupUnityJNI();

    static native void AndroidNativeAdMob_setupAdMobJNI();

    static native void AndroidNativeDevToDev_setupDevToDevJNI();

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL2",
                "SDLApplication"
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        //Facebook initialization should be done before setContentView
        AppEventsLogger.activateApp(getApplication());

        super.onCreate(savedInstanceState);

        _instance = this;

        AndroidNativeFacebook_setupFacebookJNI();

        AndroidNativeUnity_setupUnityJNI();

        AndroidNativeAdMob_setupAdMobJNI();

        AndroidNativeDevToDev_setupDevToDevJNI();
    }

    @Override
    protected void onStop() {
        super.onStop();

        _instance = null;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(callbackManager != null) {
            callbackManager.onActivityResult(requestCode, resultCode, data);
        }
    }

    @Override
    protected SDLSurface createSDLSurface(Context context) {
        return new MengineSurface(context);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Facebook Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void facebookInitializePlugin() {
        if (_instance == null) {
            return;
        }

        _instance.callbackManager = CallbackManager.Factory.create();
        _instance.facebookInteractionLayer = new FacebookInteractionLayer(_instance.callbackManager);
    }

    public static boolean facebookIsLoggedIn() {
        if (_instance == null || _instance.facebookInteractionLayer == null) {
            return false;
        }
        return _instance.facebookInteractionLayer.isLoggedIn();
    }

    public static void facebookPerformLogin(String[] readPermissions) {
        if (_instance == null || _instance.facebookInteractionLayer == null) {
            return;
        }
        _instance.facebookInteractionLayer.performLogin(_instance, readPermissions);
    }

    public static void facebookGetUser() {
        if (_instance == null || _instance.facebookInteractionLayer == null) {
            return;
        }
        _instance.facebookInteractionLayer.getUser();
    }

    public static void facebookShareLink(String link) {
        if (_instance == null || _instance.facebookInteractionLayer == null) {
            return;
        }
        _instance.facebookInteractionLayer.shareLink(_instance, link);
    }

    public static void facebookGetProfilePictureLink(String typeParameter) {
        if (_instance == null || _instance.facebookInteractionLayer == null) {
            return;
        }
        _instance.facebookInteractionLayer.getProfilePictureLink(typeParameter);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //UnityAds Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void unityInitializePlugin(String gameId){
        if (_instance == null) {
            return;
        }
        _instance.unityAdsInteractionLayer = new UnityAdsInteractionLayer(gameId);
    }

    public static void unitySetupAds(boolean debug) {
        if (_instance == null || _instance.unityAdsInteractionLayer == null) {
            return;
        }
        _instance.unityAdsInteractionLayer.setupAds(_instance, debug);
    }

    public static void unityShowAd(String placementId) {
        if (_instance == null || _instance.unityAdsInteractionLayer == null) {
            return;
        }
        _instance.unityAdsInteractionLayer.showAd(_instance, placementId);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //AdMob Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void admobInitializePlugin(String admobAppId, String interAdUnitId, String videoAdUnitId){
        if (_instance == null) {
            return;
        }
        _instance.adMobInteractionLayer = new AdMobInteractionLayer(_instance, admobAppId, interAdUnitId, videoAdUnitId);
    }

    public static void admobSetupInterstitialAd() {
        if (_instance == null || _instance.adMobInteractionLayer == null) {
            return;
        }
        _instance.adMobInteractionLayer.setupInterstitialAd();
    }

    public static void admobShowInterstitialAd() {
        if (_instance == null || _instance.adMobInteractionLayer == null) {
            return;
        }
        _instance.adMobInteractionLayer.showInterstitialAd();
    }

    public static void admobSetupRewardedVideoAd() {
        if (_instance == null || _instance.adMobInteractionLayer == null) {
            return;
        }
        _instance.adMobInteractionLayer.setupRewardedVideoAd();
    }

    public static void admobShowRewardedVideoAd() {
        if (_instance == null || _instance.adMobInteractionLayer == null) {
            return;
        }
        _instance.adMobInteractionLayer.showRewardedVideoAd();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //DevToDev Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void devtodevInitializePlugin(String appId, String secret, String apiKey) {
        if (_instance == null) {
            return;
        }
        _instance.devToDevInteractionLayer = new DevToDevInteractionLayer(_instance, appId, secret, apiKey);
    }

    public static void devtodevOnTutorialEvent(int stateOrStep) {
        if (_instance == null || _instance.devToDevInteractionLayer == null) {
            return;
        }
        _instance.devToDevInteractionLayer.onTutorialEvent(stateOrStep);
    }

    public static void devtodevSetCurrentLevel(int level) {
        if (_instance == null || _instance.devToDevInteractionLayer == null) {
            return;
        }
        _instance.devToDevInteractionLayer.setCurrentLevel(level);
    }

    public static void devtodevOnLevelUp(int level) {
        if (_instance == null || _instance.devToDevInteractionLayer == null) {
            return;
        }
        _instance.devToDevInteractionLayer.onLevelUp(level);
    }

    public static void devtodevOnCurrencyAccrual(String currencyName, int currencyAmount, int accrualType) {
        if (_instance == null || _instance.devToDevInteractionLayer == null) {
            return;
        }
        _instance.devToDevInteractionLayer.onCurrencyAccrual(currencyName, currencyAmount, accrualType);
    }

    public static void devtodevOnRealPayment(String paymentId, float inAppPrice, String inAppName, String inAppCurrencyISOCode) {
        if (_instance == null || _instance.devToDevInteractionLayer == null) {
            return;
        }
        _instance.devToDevInteractionLayer.onRealPayment(paymentId, inAppPrice, inAppName, inAppCurrencyISOCode);
    }

    public static void devtodevOnInAppPurchase(String purchaseId, String purchaseType, int purchaseAmount, int purchasePrice, String purchaseCurrency) {
        if (_instance == null || _instance.devToDevInteractionLayer == null) {
            return;
        }
        _instance.devToDevInteractionLayer.onInAppPurchase(purchaseId, purchaseType, purchaseAmount, purchasePrice, purchaseCurrency);
    }

    public static void devtodevOnSimpleCustomEvent(String eventName) {
        if (_instance == null || _instance.devToDevInteractionLayer == null) {
            return;
        }
        _instance.devToDevInteractionLayer.onSimpleCustomEvent(eventName);
    }
}
