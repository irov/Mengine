package org.Mengine.Build.UnityAds;

import android.app.Activity;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;

import com.unity3d.ads.UnityAds;
import com.unity3d.ads.mediation.IUnityAdsExtendedListener;

import org.Mengine.Build.ThreadUtil;

import java.util.HashSet;

/**
 * Created by sweatcoin7 on 7/1/18.
 */

public class UnityAdsInteractionLayer implements IUnityAdsExtendedListener {

    private static final String TAG = "UnityAds";

    private final String _gameId;
    private HashSet<String> _alreadyInitializedPlacements;
    
    static native void AndroidNativeUnity_onUnityAdsClick(String placementId);
    static native void AndroidNativeUnity_onUnityAdsPlacementStateChanged(String placementId, int placementState, int placementState1);
    static native void AndroidNativeUnity_onUnityAdsReady(String placementId);
    static native void AndroidNativeUnity_onUnityAdsStart(String placementId);
    static native void AndroidNativeUnity_onUnityAdsFinish(String placementId, int finishState);
    static native void AndroidNativeUnity_onUnityAdsError(int unityAdsError, String message);

    public UnityAdsInteractionLayer(String gameId) {
        _gameId = gameId;

        UnityAds.setListener(this);
        _alreadyInitializedPlacements = new HashSet<String>();
    }

    public void setupAds(final Activity activity, final boolean debug) {
        final IUnityAdsExtendedListener listener = this;
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                UnityAds.initialize(activity, _gameId, listener, debug);
            }
        });
    }

    public void showAd(final Activity activity, final String placementId) {
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                if (UnityAds.isReady(placementId)) {
                    UnityAds.show(activity, placementId);
                    if (_alreadyInitializedPlacements.contains(placementId)) {
                        _alreadyInitializedPlacements.remove(placementId);
                    }
                }
            }
        });
    }

    @Override
    public void onUnityAdsClick(final String placementId) {
        AndroidNativeUnity_onUnityAdsClick(placementId);
    }

    @Override
    public void onUnityAdsPlacementStateChanged(final String placementId,
                                                final UnityAds.PlacementState placementState,
                                                final UnityAds.PlacementState placementState1) {
        AndroidNativeUnity_onUnityAdsPlacementStateChanged(placementId, placementState.ordinal(), placementState1.ordinal());
    }

    @Override
    public void onUnityAdsReady(final String placementId) {
        if (_alreadyInitializedPlacements.contains(placementId)) {
            return;
        }
        AndroidNativeUnity_onUnityAdsReady(placementId);
        _alreadyInitializedPlacements.add(placementId);
    }

    @Override
    public void onUnityAdsStart(final String placementId) {
        AndroidNativeUnity_onUnityAdsStart(placementId);
    }

    @Override
    public void onUnityAdsFinish(final String placementId, final UnityAds.FinishState finishState) {
        AndroidNativeUnity_onUnityAdsFinish(placementId, finishState.ordinal());
    }

    @Override
    public void onUnityAdsError(final UnityAds.UnityAdsError unityAdsError, final String message) {
        AndroidNativeUnity_onUnityAdsError(unityAdsError.ordinal(), message);
    }

}
