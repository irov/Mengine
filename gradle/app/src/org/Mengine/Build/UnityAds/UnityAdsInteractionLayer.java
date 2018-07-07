package org.Mengine.Build.UnityAds;

import android.app.Activity;
import android.os.Build;
import android.util.Log;

import com.unity3d.ads.mediation.IUnityAdsExtendedListener;
import com.unity3d.ads.UnityAds;
import com.unity3d.ads.webview.WebView;

import java.util.HashSet;
import java.util.List;

/**
 * Created by sweatcoin7 on 7/1/18.
 */

public class UnityAdsInteractionLayer implements IUnityAdsExtendedListener {

    private static final String TAG = "UnityAds";

    final private String _gameId = "2654327";

    static native void AndroidNativeUnity_onUnityAdsClick(String placementId);

    static native void AndroidNativeUnity_onUnityAdsPlacementStateChanged(String placementId, int placementState, int placementState1);

    static native void AndroidNativeUnity_onUnityAdsReady(String placementId);

    static native void AndroidNativeUnity_onUnityAdsStart(String placementId);

    static native void AndroidNativeUnity_onUnityAdsFinish(String placementId, int finishState);

    static native void AndroidNativeUnity_onUnityAdsError(int unityAdsError, String message);

    private HashSet<String> _alreadyInitializedPlacements;

    public UnityAdsInteractionLayer() {
        if (Build.VERSION.SDK_INT >= 19) {
            WebView.setWebContentsDebuggingEnabled(true);
        }

        UnityAds.setListener(this);
        _alreadyInitializedPlacements = new HashSet<String>();
    }

    public void setupAds(Activity activity, boolean debug) {
        UnityAds.initialize(activity, _gameId, this, debug);
    }

    public void showAd(Activity activity, String placementId) {
        if (UnityAds.isReady(placementId)) {
            UnityAds.show(activity, placementId);
        }
        if(_alreadyInitializedPlacements.contains(placementId)) {
            _alreadyInitializedPlacements.remove(placementId);
        }
    }

    @Override
    public void onUnityAdsClick(String placementId) {
        AndroidNativeUnity_onUnityAdsClick(placementId);
    }

    @Override
    public void onUnityAdsPlacementStateChanged(String placementId, UnityAds.PlacementState placementState, UnityAds.PlacementState placementState1) {
        AndroidNativeUnity_onUnityAdsPlacementStateChanged(placementId, placementState.ordinal(), placementState1.ordinal());
    }

    @Override
    public void onUnityAdsReady(String placementId) {
        if (_alreadyInitializedPlacements.contains(placementId)) {
            return;
        }
        AndroidNativeUnity_onUnityAdsReady(placementId);
        _alreadyInitializedPlacements.add(placementId);
    }

    @Override
    public void onUnityAdsStart(String placementId) {
        AndroidNativeUnity_onUnityAdsStart(placementId);
    }

    @Override
    public void onUnityAdsFinish(String placementId, UnityAds.FinishState finishState) {
        AndroidNativeUnity_onUnityAdsFinish(placementId, finishState.ordinal());
    }

    @Override
    public void onUnityAdsError(UnityAds.UnityAdsError unityAdsError, String message) {
        AndroidNativeUnity_onUnityAdsError(unityAdsError.ordinal(), message);
    }
}
