package org.Mengine.Plugin.AdMob.BannerAd;

import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.graphics.Color;

import androidx.annotation.NonNull;
import androidx.annotation.Size;
import androidx.annotation.StringRes;

import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.OnPaidEventListener;
import com.google.android.gms.ads.ResponseInfo;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAdFormat;
import org.Mengine.Base.MengineAdResponseInterface;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineAnalyticsEventBuilderInterface;
import org.Mengine.Base.MengineNative;
import org.Mengine.Base.MengineNetwork;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobBannerAdInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobBase;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobPluginInterface;

import java.util.Map;

public class MengineAdMobBannerAd extends MengineAdMobBase implements MengineAdMobBannerAdInterface, OnPaidEventListener {
    public static final @StringRes int METADATA_BANNER_PLACEMENT = R.string.mengine_admob_banner_placement;
    public static final @StringRes int METADATA_BANNER_ADUNITID = R.string.mengine_admob_banner_adunitid;

    protected final String m_placement;

    protected AdView m_adView;

    protected volatile boolean m_visible = false;
    protected volatile boolean m_loaded = false;

    public MengineAdMobBannerAd(@NonNull MengineAdService adService, @NonNull MengineAdMobPluginInterface plugin) throws MengineServiceInvalidInitializeException {
        super(adService, plugin);

        this.setAdUnitId(METADATA_BANNER_ADUNITID, "BannerAdUnitId");

        String MengineAdMobPlugin_Banner_Placement = plugin.getResourceString(METADATA_BANNER_PLACEMENT);

        m_placement = MengineAdMobPlugin_Banner_Placement;
    }

    protected AdSize getBannerSize(@NonNull MengineActivity activity) {
        AdSize adSize = AdSize.getCurrentOrientationAnchoredAdaptiveBannerAdSize(activity, 320);

        return adSize;
    }

    protected MengineAnalyticsEventBuilderInterface buildBannerAdEvent(@Size(min = 1L, max = 40L) String event) {
        MengineAnalyticsEventBuilderInterface builder = this.buildAdEvent("mng_admob_banner_" + event)
            .addParameterString("placement", m_placement);

        return builder;
    }

    protected void setBannerState(@NonNull String state) {
        this.setState("admob.banner.state." + m_adUnitId, state);
    }

    public int getWidthPx() {
        MengineActivity activity = m_plugin.getMengineActivity();

        if (m_adView == null || activity == null) {
            return 0;
        }

        AdSize adSize = m_adView.getAdSize();

        if (adSize == null) {
            return 0;
        }

        int widthPx = adSize.getWidthInPixels(activity);

        return widthPx;
    }

    public int getHeightPx() {
        MengineActivity activity = m_plugin.getMengineActivity();

        if (m_adView == null || activity == null) {
            return 0;
        }

        AdSize adSize = m_adView.getAdSize();

        if (adSize == null) {
            return 0;
        }

        int heightPx = adSize.getHeightInPixels(activity);

        return heightPx;
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) {
        super.onActivityCreate(activity);

        AdSize adSize = this.getBannerSize(activity);

        AdView adView = new AdView(activity);
        adView.setAdUnitId(m_adUnitId);
        adView.setAdSize(adSize);

        adView.setAdListener(new AdListener() {
            @Override
            public void onAdLoaded() {
                m_loaded = true;

                MengineAdMobBannerAd.this.log("onAdLoaded");

                MengineAdMobBannerAd.this.buildBannerAdEvent("loaded")
                    .log();

                m_requestAttempt = 0;

                MengineAdMobBannerAd.this.setBannerState("loaded." + m_placement);

                if (m_visible == true) {
                    if (m_adView != null) {
                        MengineAdMobBannerAd.this.enableAdView(m_adView);
                    }
                }
            }

            @Override
            public void onAdOpened() {
                MengineAdMobBannerAd.this.log("onAdOpened");

                MengineAdMobBannerAd.this.buildBannerAdEvent("opened")
                    .log();

                MengineAdMobBannerAd.this.setBannerState("opened." + m_placement);
            }

            @Override
            public void onAdClosed() {
                MengineAdMobBannerAd.this.log("onAdClosed");

                MengineAdMobBannerAd.this.buildBannerAdEvent("closed")
                    .log();

                MengineAdMobBannerAd.this.setBannerState("closed." + m_placement);
            }

            @Override
            public void onAdClicked() {
                MengineAdMobBannerAd.this.log("onAdClicked");

                MengineAdMobBannerAd.this.buildBannerAdEvent("clicked")
                    .log();

                MengineAdMobBannerAd.this.setBannerState("clicked." + m_placement);
            }

            @Override
            public void onAdImpression() {
                MengineAdMobBannerAd.this.log("onAdImpression");

                MengineAdMobBannerAd.this.buildBannerAdEvent("impression")
                    .log();

                MengineAdMobBannerAd.this.setBannerState("impression." + m_placement);
            }

            @Override
            public void onAdFailedToLoad(@NonNull LoadAdError error) {
                MengineAdMobBannerAd.this.logLoadAdError("onAdFailedToLoad", error);

                int errorCode = error.getCode();

                MengineAdMobBannerAd.this.buildBannerAdEvent("load_failed")
                    .addParameterJSON("error", MengineAdMobBannerAd.this.getLoadAdErrorParams(error))
                    .addParameterLong("error_code", errorCode)
                    .log();

                m_requestAttempt++;

                MengineAdMobBannerAd.this.setBannerState("load_failed." + m_placement + "." + errorCode);

                if (m_visible == true) {
                    if (m_adView != null) {
                        MengineAdMobBannerAd.this.disableAdView(m_adView);
                    }
                }

                MengineAdMobBannerAd.this.retryLoadAd();
            }
        });
        adView.setOnPaidEventListener(this);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);

        adView.setLayoutParams(params);

        adView.setVisibility(View.GONE);
        adView.setBackgroundColor(Color.TRANSPARENT);

        ViewGroup viewGroup = activity.getContentViewGroup();
        viewGroup.addView(adView);

        m_adView = adView;

        int widthDp = adSize.getWidth();
        int heightDp = adSize.getHeight();

        this.log("create", Map.of("placement", m_placement, "width", widthDp, "height", heightDp));

        this.setBannerState("init." + m_placement);

        this.loadAd();
    }

    @Override
    public void onActivityDestroy(@NonNull MengineActivity activity) {
        super.onActivityDestroy(activity);

        if (m_adView != null) {
            m_adView.setAdListener(null);
            m_adView.setOnPaidEventListener(null);

            ViewGroup viewGroup = activity.getContentViewGroup();
            viewGroup.removeView(m_adView);

            m_adView.destroy();
            m_adView = null;
        }
    }

    @Override
    public void loadAd() {
        if (m_adView == null) {
            return;
        }

        if (m_plugin.hasOption("admob.banner.disable") == true || m_plugin.hasOption("admob.ad.disable") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildBannerAdEvent("load")
            .log();

        this.setBannerState("load");

        try {
            AdRequest adRequest = new AdRequest.Builder()
                .build();

            m_adView.loadAd(adRequest);
        } catch (final Exception e) {
            m_plugin.logError("[Banner] loadAd adUnitId: %s exception: %s"
                , m_adUnitId
                , e.getMessage()
            );

            this.buildBannerAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.setBannerState("load_exception");

            this.retryLoadAd();
        }
    }

    protected void enableAdView(@NonNull AdView adView) {
        if (adView.getVisibility() == View.VISIBLE) {
            return;
        }

        if (m_plugin.hasOption("admob.banner.disable") == true || m_plugin.hasOption("admob.ad.disable") == true) {
            return;
        }

        m_plugin.logInfo("[Banner] show adView: %s", m_adUnitId);

        adView.setVisibility(View.VISIBLE);
        adView.requestLayout();
    }

    protected void disableAdView(@NonNull AdView adView) {
        if (adView.getVisibility() == View.GONE) {
            return;
        }

        if (m_plugin.hasOption("admob.banner.disable") == true || m_plugin.hasOption("admob.ad.disable") == true) {
            return;
        }

        m_plugin.logInfo("[Banner] hide adView: %s", m_adUnitId);

        adView.setVisibility(View.GONE);
        adView.requestLayout();
    }

    private void updateVisible() {
        this.log("updateVisible", Map.of("show", m_visible));

        if (m_visible == true) {
            if (m_adView != null && m_loaded == true) {
                this.enableAdView(m_adView);
            }
        } else {
            if (m_adView != null) {
                this.disableAdView(m_adView);
            }
        }
    }

    public boolean canYouShow() {
        return m_loaded;
    }

    public void show() {
        MengineUtils.performOnMainThread(() -> {
            if (m_visible == true) {
                return;
            }

            m_visible = true;

            this.updateVisible();
        });
    }

    public void hide() {
        MengineUtils.performOnMainThread(() -> {
            if (m_visible == false) {
                return;
            }

            m_visible = false;

            this.updateVisible();
        });
    }


    @Override
    public void onPaidEvent(@NonNull com.google.android.gms.ads.AdValue adValue) {
        this.log("onPaidEvent");

        ResponseInfo responseInfo = m_adView.getResponseInfo();

        if (responseInfo != null) {
            long valueMicros = adValue.getValueMicros();
            double value = valueMicros / 1000000.0;

            this.revenuePaid(responseInfo, MengineAdFormat.ADFORMAT_BANNER, m_placement, value);
        }
    }
}

