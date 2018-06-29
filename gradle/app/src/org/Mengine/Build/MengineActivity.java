package org.Mengine.Build;

import android.content.*;
import android.os.Bundle;

import com.facebook.CallbackManager;
import com.facebook.appevents.AppEventsLogger;

import org.Mengine.Build.Facebook.FacebookInteractionLayer;
import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLSurface;

import com.unity3d.ads.mediation.IUnityAdsExtendedListener;
import com.unity3d.ads.UnityAds;

public class MengineActivity extends SDLActivity implements IUnityAdsExtendedListener {

    public FacebookInteractionLayer facebookInteractionLayer;

    private static MengineActivity _instance;
    private CallbackManager _callbackManager;

    static native void AndroidNativeFacebook_setupFacebookJNI();

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

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Unity Methods
    ////////////////////////////////////////////////////////////////////////////////////////////////

    @Override
    public void onUnityAdsReady(String s) {

    }

    @Override
    public void onUnityAdsStart(String s) {

    }

    @Override
    public void onUnityAdsFinish(String s, UnityAds.FinishState finishState) {

    }

    @Override
    public void onUnityAdsError(UnityAds.UnityAdsError unityAdsError, String s) {

    }

    @Override
    public void onUnityAdsClick(String s) {

    }

    @Override
    public void onUnityAdsPlacementStateChanged(String s, UnityAds.PlacementState placementState, UnityAds.PlacementState placementState1) {

    }
}
