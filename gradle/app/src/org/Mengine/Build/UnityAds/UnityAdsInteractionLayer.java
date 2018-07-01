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

    final private String _defaultGameId = "2654327";

    public UnityAdsInteractionLayer() {
        if (Build.VERSION.SDK_INT >= 19) {
            WebView.setWebContentsDebuggingEnabled(true);
        }

        UnityAds.setListener(this);
    }

    public void setupAds(Activity activity, boolean debug) {
        UnityAds.initialize(activity, _defaultGameId, this, debug);
    }

    public void showAd(Activity activity) {
        UnityAds.show(activity);
    }

    @Override
    public void onUnityAdsClick(String s) {

    }

    @Override
    public void onUnityAdsPlacementStateChanged(String s, UnityAds.PlacementState placementState, UnityAds.PlacementState placementState1) {

    }

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
}
