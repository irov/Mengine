package org.Mengine.Build;

import android.content.*;
import android.os.Bundle;

import com.facebook.CallbackManager;
import com.facebook.appevents.AppEventsLogger;

import org.Mengine.Build.AdMob.AdMobInteractionLayer;
import org.Mengine.Build.Facebook.FacebookInteractionLayer;
import org.Mengine.Build.UnityAds.UnityAdsInteractionLayer;
import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

public class MengineActivity extends SDLActivity {

    public FacebookInteractionLayer facebookInteractionLayer;
    public UnityAdsInteractionLayer unityAdsInteractionLayer;
    public AdMobInteractionLayer adMobInteractionLayer;

    private static MengineActivity _instance;
    private CallbackManager _callbackManager;

    static native void AndroidNativeFacebook_setupFacebookJNI();
    static native void AndroidNativeUnity_setupUnityJNI();
    

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

        _callbackManager = CallbackManager.Factory.create();
        
        AndroidNativeFacebook_setupFacebookJNI();
        facebookInteractionLayer = new FacebookInteractionLayer(_callbackManager);
        
        AndroidNativeUnity_setupUnityJNI();
        unityAdsInteractionLayer = new UnityAdsInteractionLayer();

        adMobInteractionLayer = new AdMobInteractionLayer(this);
    }

    @Override
    protected void onStop() {
        super.onStop();

        _instance = null;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        _callbackManager.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected SDLSurface createSDLSurface(Context context) {
        return new MengineSurface(context);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Facebook Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static boolean facebookIsLoggedIn() {
        if (_instance == null) {
            return false;
        }
        return _instance.facebookInteractionLayer.isLoggedIn();
    }

    public static void facebookPerformLogin(String[] readPermissions) {
        if (_instance == null) {
            return;
        }
        _instance.facebookInteractionLayer.performLogin(_instance, readPermissions);
    }

    public static void facebookGetUser() {
        if (_instance == null) {
            return;
        }
        _instance.facebookInteractionLayer.getUser();
    }

    public static void facebookShareLink(String link) {
        if (_instance == null) {
            return;
        }
        _instance.facebookInteractionLayer.shareLink(_instance, link);
    }
  
    public static void facebookGetProfilePictureLink(String typeParameter) {
        if (_instance == null) {
            return;
        }
        _instance.facebookInteractionLayer.getProfilePictureLink(typeParameter);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //UnityAds Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void unitySetupAds (boolean debug) {
        if (_instance == null) {
            return;
        }
        _instance.unityAdsInteractionLayer.setupAds(_instance, debug);
    }

    public static void unityShowAd (String placementId) {
        if (_instance == null) {
            return;
        }
        _instance.unityAdsInteractionLayer.showAd(_instance, placementId);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //AdMob Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////
    public static void admobSetupInterstitialAd () {
        if (_instance == null) {
            return;
        }
        _instance.adMobInteractionLayer.setupInterstitialAd();
    }

    public static void admobShowInterstitialAd () {
        if (_instance == null) {
            return;
        }
        _instance.adMobInteractionLayer.showInterstitialAd();
    }

    public static void admobSetupRewardedVideoAd () {
        if (_instance == null) {
            return;
        }
        _instance.adMobInteractionLayer.setupRewardedVideoAd();
    }

    public static void admobShowRewardedVideoAd () {
        if (_instance == null) {
            return;
        }
        _instance.adMobInteractionLayer.showRewardedVideoAd();
    }
}
