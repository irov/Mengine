package org.Mengine.Build.UnityAds;

import android.app.Activity;
import android.os.Build;

import com.unity3d.ads.mediation.IUnityAdsExtendedListener;
import com.unity3d.ads.UnityAds;
import com.unity3d.ads.webview.WebView;

/**
 * Created by sweatcoin7 on 7/1/18.
 */

public class UnityAdsInteractionLayer implements IUnityAdsExtendedListener {

    private static final String TAG = "UnityAds";

    final private String _gameId = "2654327";

    static native void AndroidNativeUnity_onUnityAdsClick(String s);

    static native void AndroidNativeUnity_onUnityAdsPlacementStateChanged(String s, int placementState, int placementState1);

    static native void AndroidNativeUnity_onUnityAdsReady(String s);

    static native void AndroidNativeUnity_onUnityAdsStart(String s);

    static native void AndroidNativeUnity_onUnityAdsFinish(String s, int finishState);

    static native void AndroidNativeFacebook_onUnityAdsError(int unityAdsError, String s);

    public UnityAdsInteractionLayer() {
        if (Build.VERSION.SDK_INT >= 19) {
            WebView.setWebContentsDebuggingEnabled(true);
        }

        UnityAds.setListener(this);
    }

    public void setupAds(Activity activity, boolean debug) {
        UnityAds.initialize(activity, _gameId, this, debug);
    }

    public void showAd(Activity activity) {
        UnityAds.show(activity);
    }

    @Override
    public void onUnityAdsClick(String placementId) {

    }

    @Override
    public void onUnityAdsPlacementStateChanged(String placementId, UnityAds.PlacementState placementState, UnityAds.PlacementState placementState1) {

    }

    @Override
    public void onUnityAdsReady(String placementId) {

    }

    @Override
    public void onUnityAdsStart(String placementId) {

    }

    @Override
    public void onUnityAdsFinish(String placementId, UnityAds.FinishState finishState) {

    }

    @Override
    public void onUnityAdsError(UnityAds.UnityAdsError unityAdsError, String message) {

    }
}
