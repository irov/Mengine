package org.Mengine.Plugin.AdMob.InterstitialAd;

import androidx.annotation.NonNull;
import androidx.annotation.Size;
import androidx.annotation.StringRes;

import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.OnPaidEventListener;
import com.google.android.gms.ads.ResponseInfo;
import com.google.android.gms.ads.interstitial.InterstitialAd;
import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdMediation;
import org.Mengine.Base.MengineAdResponseInterface;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineAnalyticsEventBuilderInterface;
import org.Mengine.Base.MengineNative;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobBase;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobInterstitialAdInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobPluginInterface;

import java.util.Map;

public class MengineAdMobInterstitialAd extends MengineAdMobBase implements MengineAdMobInterstitialAdInterface {
    public static final @StringRes int METADATA_INTERSTITIAL_ADUNITID = R.string.mengine_admob_interstitial_adunitid;

    private InterstitialAd m_interstitialAd;
    private OnPaidEventListener m_onPaidEventListener;

    private boolean m_showing = false;

    public MengineAdMobInterstitialAd(@NonNull MengineAdService adService, @NonNull MengineAdMobPluginInterface plugin) throws MengineServiceInvalidInitializeException {
        super(adService, plugin, MengineAdFormat.ADFORMAT_INTERSTITIAL);

        this.setAdUnitId(METADATA_INTERSTITIAL_ADUNITID, "InterstitialAdUnitId");
    }

    protected MengineAnalyticsEventBuilderInterface buildInterstitialAdEvent(@Size(min = 1L, max = 40L) String event) {
        MengineAnalyticsEventBuilderInterface builder = this.buildAdEvent("mng_admob_interstitial_" + event);

        return builder;
    }

    protected void setInterstitialState(@NonNull String state) {
        this.setState("admob.interstitial.state." + m_adUnitId, state);
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) {
        super.onActivityCreate(activity);

        this.log("create");

        this.setInterstitialState("init");

        this.loadAd();
    }

    @Override
    public void onActivityDestroy(@NonNull MengineActivity activity) {
        super.onActivityDestroy(activity);

        this.destroyInterstitialAd();
    }

    private void destroyInterstitialAd() {
        if (m_interstitialAd != null) {
            m_interstitialAd.setFullScreenContentCallback(null);
            m_interstitialAd.setOnPaidEventListener(null);

            m_interstitialAd = null;
        }
    }

    @Override
    public void loadAd() {
        if (m_plugin.hasOption("admob.interstitial.disable") == true || m_plugin.hasOption("admob.ad.disable") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildInterstitialAdEvent("load")
            .log();

        this.setInterstitialState("load");

        MengineActivity activity = m_plugin.getMengineActivity();

        if (activity == null) {
            return;
        }

        try {
            AdRequest adRequest = new AdRequest.Builder().build();

            InterstitialAd.load(activity, m_adUnitId, adRequest, new InterstitialAdLoadCallback() {
                @Override
                public void onAdLoaded(@NonNull InterstitialAd interstitialAd) {
                    MengineAdMobInterstitialAd.this.destroyInterstitialAd();

                    m_interstitialAd = interstitialAd;

                    m_interstitialAd.setFullScreenContentCallback(new FullScreenContentCallback() {
                        @Override
                        public void onAdDismissedFullScreenContent() {
                            m_interstitialAd = null;

                            MengineAdMobInterstitialAd.this.log("onAdDismissedFullScreenContent");

                            MengineAdMobInterstitialAd.this.buildInterstitialAdEvent("dismissed")
                                .log();

                            MengineAdMobInterstitialAd.this.setInterstitialState("dismissed");

                            MengineNative.AndroidPlatform_unfreezeEvent(true, true);

                            m_showing = false;

                            MengineUtils.performOnMainThread(() -> {
                                MengineAdResponseInterface adResponse = m_adService.getAdResponse();

                                adResponse.onAdShowSuccess(MengineAdMediation.ADMEDIATION_ADMOB, MengineAdFormat.ADFORMAT_INTERSTITIAL, null);

                                MengineAdMobInterstitialAd.this.loadAd();
                            });
                        }

                        @Override
                        public void onAdFailedToShowFullScreenContent(@NonNull AdError adError) {
                            m_interstitialAd = null;

                            MengineAdMobInterstitialAd.this.logAdError("onAdFailedToShowFullScreenContent", adError);

                            int errorCode = adError.getCode();

                            MengineAdMobInterstitialAd.this.buildInterstitialAdEvent("show_failed")
                                .addParameterJSON("error", MengineAdMobInterstitialAd.this.getAdErrorParams(adError))
                                .addParameterLong("error_code", errorCode)
                                .log();

                            MengineAdMobInterstitialAd.this.setInterstitialState("show_failed." + errorCode);

                            m_showing = false;

                            MengineUtils.performOnMainThread(() -> {
                                MengineAdResponseInterface adResponse = m_adService.getAdResponse();

                                adResponse.onAdShowFailed(MengineAdMediation.ADMEDIATION_ADMOB, MengineAdFormat.ADFORMAT_INTERSTITIAL, null, errorCode);

                                MengineAdMobInterstitialAd.this.loadAd();
                            });
                        }

                        @Override
                        public void onAdShowedFullScreenContent() {
                            MengineAdMobInterstitialAd.this.log("onAdShowedFullScreenContent");

                            MengineAdMobInterstitialAd.this.buildInterstitialAdEvent("showed")
                                .log();

                            MengineAdMobInterstitialAd.this.setInterstitialState("showed");

                            MengineNative.AndroidPlatform_freezeEvent(true, true);
                        }

                        @Override
                        public void onAdClicked() {
                            MengineAdMobInterstitialAd.this.log("onAdClicked");

                            MengineAdMobInterstitialAd.this.buildInterstitialAdEvent("clicked")
                                .log();

                            MengineAdMobInterstitialAd.this.setInterstitialState("clicked");
                        }

                        @Override
                        public void onAdImpression() {
                            MengineAdMobInterstitialAd.this.log("onAdImpression");

                            MengineAdMobInterstitialAd.this.buildInterstitialAdEvent("impression")
                                .log();

                            MengineAdMobInterstitialAd.this.setInterstitialState("impression");
                        }
                    });

                    m_interstitialAd.setOnPaidEventListener(adValue -> {
                        MengineAdMobInterstitialAd.this.log("onPaidEvent");

                        if (m_interstitialAd != null) {
                            ResponseInfo responseInfo = m_interstitialAd.getResponseInfo();

                            if (responseInfo != null) {
                                long valueMicros = adValue.getValueMicros();
                                double value = valueMicros / 1000000.0;

                                MengineAdMobInterstitialAd.this.revenuePaid(responseInfo, MengineAdFormat.ADFORMAT_INTERSTITIAL, null, value);
                            }
                        }
                    });

                    MengineAdMobInterstitialAd.this.log("onAdLoaded");

                    MengineAdMobInterstitialAd.this.buildInterstitialAdEvent("loaded")
                        .log();

                    MengineAdMobInterstitialAd.this.setInterstitialState("loaded");

                    m_requestAttempt = 0;
                }

                @Override
                public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                    MengineAdMobInterstitialAd.this.destroyInterstitialAd();

                    MengineAdMobInterstitialAd.this.logLoadAdError("onAdFailedToLoad", loadAdError);

                    int errorCode = loadAdError.getCode();

                    MengineAdMobInterstitialAd.this.buildInterstitialAdEvent("load_failed")
                        .addParameterJSON("error", MengineAdMobInterstitialAd.this.getLoadAdErrorParams(loadAdError))
                        .addParameterLong("error_code", errorCode)
                        .log();

                    MengineAdMobInterstitialAd.this.setInterstitialState("load_failed." + errorCode);

                    MengineAdMobInterstitialAd.this.retryLoadAd();
                }
            });
        } catch (final Exception e) {
            this.logError("loadAd", e);

            this.buildInterstitialAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.setInterstitialState("load_exception");

            this.retryLoadAd();
        }
    }

    public boolean canYouShowInterstitial(String placement) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_interstitialAd != null;

        this.log("canYouShowInterstitial", Map.of("placement", placement, "ready", ready));

        if (ready == false) {
            this.buildInterstitialAdEvent("show")
                .addParameterString("placement", placement)
                .addParameterBoolean("ready", false)
                .log();

            return false;
        }

        return true;
    }

    public boolean showInterstitial(@NonNull MengineActivity activity, String placement) {
        if (MengineNetwork.isNetworkAvailable() == false) {
            return false;
        }

        boolean ready = m_interstitialAd != null;

        this.log("showInterstitial", Map.of("placement", placement, "ready", ready));

        this.buildInterstitialAdEvent("show")
            .addParameterString("placement", placement)
            .addParameterBoolean("ready", ready)
            .log();

        if (ready == false) {
            return false;
        }

        m_showing = true;

        InterstitialAd show_interstitialAd = m_interstitialAd;

        MengineUtils.performOnMainThread(() -> {
            show_interstitialAd.show(activity);
        });

        return true;
    }

    @Override
    public boolean isShowingInterstitial() {
        return m_showing;
    }
}

