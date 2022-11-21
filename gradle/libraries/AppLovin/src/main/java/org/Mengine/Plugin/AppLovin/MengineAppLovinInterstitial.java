package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineUtils;
import org.Mengine.Base.MengineActivity;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdListener;
import com.applovin.mediation.MaxAdRequestListener;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.ads.MaxInterstitialAd;

import java.util.concurrent.TimeUnit;

public class MengineAppLovinInterstitial extends MengineAppLovinBase implements MaxAdRequestListener, MaxAdListener, MaxAdRevenueListener {
    private MaxInterstitialAd m_interstitialAd;

    private int m_retryAttemptInterstitial;

    public MengineAppLovinInterstitial(MengineAppLovinPlugin plugin) throws Exception {
        super(plugin);

        m_retryAttemptInterstitial = 0;

        MengineActivity activity = m_plugin.getActivity();

        String AppLovin_InterstitialAdUnitId = activity.getConfigValue("AppLovinPlugin", "InterstitialAdUnitId", "");

        if (AppLovin_InterstitialAdUnitId.isEmpty() == true) {
            throw new Exception("Need to add config value for [AppLovin] InterstitialAdUnitId");
        }

        MaxInterstitialAd interstitialAd = new MaxInterstitialAd(AppLovin_InterstitialAdUnitId, activity);

        interstitialAd.setRequestListener(this);
        interstitialAd.setListener(this);
        interstitialAd.setRevenueListener(this);

        m_interstitialAd = interstitialAd;
    }

    public void destroy() {
        super.destroy();

        if (m_interstitialAd != null) {
            m_interstitialAd.destroy();
            m_interstitialAd = null;
        }
    }

    public void loadInterstitial() {
        MengineActivity activity = m_plugin.getActivity();

        MengineAppLovinMediationInterface mediationAmazon = m_plugin.getMediationAmazon();

        if (mediationAmazon != null) {
            try {
                mediationAmazon.loadMediatorInterstitial(activity, m_interstitialAd);
            } catch (Exception e) {
            }
        } else {
            m_interstitialAd.loadAd();
        }
    }

    public void showInterstitial() {
        if (m_interstitialAd.isReady()) {
            m_interstitialAd.showAd();
        }
    }

    @Override
    public void onAdRequestStarted(String adUnitId) {
        m_plugin.logInfo("[Banner] onAdRequestStarted %s"
            , adUnitId
        );

        m_plugin.pythonCall("onApplovinBannerOnAdRequestStarted");
    }

    @Override
    public void onAdLoaded(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdLoaded", ad);

        m_retryAttemptInterstitial = 0;

        m_plugin.pythonCall("onApplovinInterstitialOnAdLoaded");
    }

    @Override
    public void onAdDisplayed(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdDisplayed", ad);

        m_plugin.pythonCall("onApplovinInterstitialOnAdDisplayed");
    }

    @Override
    public void onAdHidden(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdHidden", ad);

        m_interstitialAd.loadAd();

        m_plugin.pythonCall("onApplovinInterstitialOnAdHidden");
    }

    @Override
    public void onAdClicked(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdClicked", ad);

        m_plugin.pythonCall("onApplovinInterstitialOnAdClicked");
    }

    @Override
    public void onAdLoadFailed(String adUnitId, MaxError error) {
        this.logMaxError("Interstitial", "onAdLoadFailed", error);

        m_retryAttemptInterstitial++;

        long delayMillis = TimeUnit.SECONDS.toMillis((long) Math.pow(2, Math.min(6, m_retryAttemptInterstitial)));

        MengineUtils.performOnMainThread(() -> {
            m_interstitialAd.loadAd();
        }, delayMillis);

        m_plugin.pythonCall("onApplovinInterstitialOnAdLoadFailed");
    }

    @Override
    public void onAdDisplayFailed(MaxAd ad, MaxError error) {
        this.logMaxError("Interstitial", "onAdDisplayFailed", error);

        m_interstitialAd.loadAd();

        m_plugin.pythonCall("onApplovinInterstitialOnAdDisplayFailed");
    }

    @Override
    public void onAdRevenuePaid(MaxAd ad) {
        this.logMaxAd("Interstitial", "onAdRevenuePaid", ad);

        m_plugin.pythonCall("onApplovinInterstitialOnAdRevenuePaid");

        m_plugin.eventRevenuePaid(ad);
    }
}
