package org.Mengine.Plugin.AppLovin.NativeAd;

import android.view.View;
import android.view.ViewGroup;
import android.graphics.Color;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.Size;
import androidx.annotation.StringRes;

import com.applovin.mediation.MaxAd;
import com.applovin.mediation.MaxAdFormat;
import com.applovin.mediation.MaxAdRevenueListener;
import com.applovin.mediation.MaxAdReviewListener;
import com.applovin.mediation.MaxError;
import com.applovin.mediation.nativeAds.MaxNativeAdListener;
import com.applovin.mediation.nativeAds.MaxNativeAdLoader;
import com.applovin.mediation.nativeAds.MaxNativeAdView;
import com.applovin.mediation.nativeAds.MaxNativeAdViewBinder;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAnalyticsEventBuilderInterface;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinBase;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinNativeAdInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinPluginInterface;

import java.util.Map;

public class MengineAppLovinNativeAd extends MengineAppLovinBase implements MengineAppLovinNativeAdInterface, MaxAdRevenueListener, MaxAdReviewListener {
    public static final @StringRes int METADATA_NATIVE_PLACEMENT = R.string.mengine_applovin_native_placement;
    public static final @StringRes int METADATA_NATIVE_ADUNITID = R.string.mengine_applovin_native_adunitid;

    protected final String m_placement;

    protected MaxNativeAdView m_adView;
    protected MaxNativeAdLoader m_adLoader;
    protected MaxAd m_cachedAd;

    protected volatile boolean m_visible = false;
    protected volatile boolean m_loaded = false;

    public MengineAppLovinNativeAd(@NonNull MengineAppLovinPluginInterface plugin) throws MengineServiceInvalidInitializeException {
        super(plugin, MaxAdFormat.NATIVE);

        String MengineAppLovinPlugin_Native_AdUnitId = plugin.getResourceString(METADATA_NATIVE_ADUNITID);

        if (MengineAppLovinPlugin_Native_AdUnitId.isEmpty() == true) {
            this.invalidInitialize("meta %s is empty"
                , METADATA_NATIVE_ADUNITID
            );
        }

        this.setAdUnitId(MengineAppLovinPlugin_Native_AdUnitId);

        String MengineAppLovinPlugin_Native_Placement = plugin.getResourceString(METADATA_NATIVE_PLACEMENT);

        m_placement = MengineAppLovinPlugin_Native_Placement;
    }

    protected MengineAnalyticsEventBuilderInterface buildNativeAdEvent(@Size(min = 1L,max = 40L) String event) {
        MengineAnalyticsEventBuilderInterface builder = this.buildAdEvent("mng_applovin_native_" + event)
            .addParameterString("placement", m_placement);

        return builder;
    }

    protected void setNativeState(@NonNull String state) {
        this.setState("applovin.native.state." + m_adUnitId, state);
    }

    public int getLeftMarginPx() {
        //ToDo

        return 0;
    }

    public int getTopMarginPx() {
        //ToDo

        return 0;
    }

    public int getWidthPx() {
        //ToDo

        return 0;
    }

    public int getHeightPx() {
        //ToDo

        return 0;
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) {
        super.onActivityCreate(activity);

        MaxNativeAdViewBinder binder = new MaxNativeAdViewBinder.Builder(R.layout.native_ad_view)
            .setTitleTextViewId(R.id.title_text_view)
            .setBodyTextViewId(R.id.body_text_view)
            .setStarRatingContentViewGroupId(R.id.star_rating_view)
            .setAdvertiserTextViewId(R.id.advertiser_text_view)
            .setIconImageViewId(R.id.icon_image_view)
            .setMediaContentViewGroupId(R.id.media_view_container)
            .setOptionsContentViewGroupId(R.id.options_view)
            .setCallToActionButtonId(R.id.cta_button)
            .build();

        MaxNativeAdView adView = new MaxNativeAdView(binder, activity);

        adView.setVisibility(View.GONE);
        adView.setBackgroundColor(Color.TRANSPARENT);

        ViewGroup viewGroup = activity.getContentViewGroup();
        viewGroup.addView(adView);

        m_adView = adView;

        MaxNativeAdLoader adLoader = new MaxNativeAdLoader(m_adUnitId);

        adLoader.setPlacement(m_placement);

        adLoader.setNativeAdListener(new MaxNativeAdListener() {
            @Override
            public void onNativeAdLoaded(@Nullable MaxNativeAdView nativeAdView, @NonNull MaxAd nativeAd) {
                if (m_cachedAd != null) {
                    m_adLoader.destroy(m_cachedAd);
                    m_cachedAd = null;
                }

                MengineAppLovinNativeAd.this.logMaxAd("onNativeAdLoaded", nativeAd);

                MengineAppLovinNativeAd.this.buildNativeAdEvent("loaded")
                        .addParameterJSON("ad", MengineAppLovinNativeAd.this.getMAAdParams(nativeAd))
                        .log();

                m_requestAttempt = 0;

                m_loaded = true;

                m_cachedAd = nativeAd;

                MengineAppLovinNativeAd.this.setNativeState("loaded." + m_placement + "." + nativeAd.getNetworkName());
            }

            @Override
            public void onNativeAdLoadFailed(@NonNull String adUnitId, @NonNull MaxError error) {
                MengineAppLovinNativeAd.this.logMaxError("onNativeAdLoadFailed", error);

                int errorCode = error.getCode();

                MengineAppLovinNativeAd.this.buildNativeAdEvent("load_failed")
                    .addParameterJSON("error", MengineAppLovinNativeAd.this.getMaxErrorParams(error))
                    .addParameterLong("error_code", errorCode)
                    .log();

                MengineAppLovinNativeAd.this.setNativeState("load_failed." + m_placement + "." + errorCode);

                MengineAppLovinNativeAd.this.retryLoadAd();
            }

            @Override
            public void onNativeAdClicked(@NonNull MaxAd ad) {
                MengineAppLovinNativeAd.this.logMaxAd("onNativeAdClicked", ad);

                MengineAppLovinNativeAd.this.buildNativeAdEvent("clicked")
                    .addParameterJSON("ad", MengineAppLovinNativeAd.this.getMAAdParams(ad))
                    .log();

                MengineAppLovinNativeAd.this.setNativeState("clicked." + m_placement + "." + ad.getNetworkName());
            }

            @Override
            public void onNativeAdExpired(@NonNull MaxAd ad) {
                MengineAppLovinNativeAd.this.logMaxAd("onNativeAdExpired", ad);

                MengineAppLovinNativeAd.this.buildNativeAdEvent("expired")
                    .addParameterJSON("ad", MengineAppLovinNativeAd.this.getMAAdParams(ad))
                    .log();

                m_loaded = false;

                MengineAppLovinNativeAd.this.setNativeState("expired." + m_placement + "." + ad.getNetworkName());

                MengineAppLovinNativeAd.this.retryLoadAd();
            }
        });

        adLoader.setRevenueListener(this);
        adLoader.setAdReviewListener(this);

        m_adLoader = adLoader;

        this.log("create", Map.of("placement", m_placement));

        this.setNativeState("init." + m_placement);

        this.firstLoadAd((mediation, callback) -> {
            mediation.loadMediatorNative(activity, m_plugin, m_adLoader, callback);
        });
    }

    @Override
    public void onActivityDestroy(@NonNull MengineActivity activity) {
        super.onActivityDestroy(activity);

        if (m_adView != null) {
            ViewGroup viewGroup = activity.getContentViewGroup();
            viewGroup.removeView(m_adView);

            m_adView = null;
        }

        if (m_adLoader != null) {
            if (m_cachedAd != null) {
                m_adLoader.destroy(m_cachedAd);
                m_cachedAd = null;
            }

            m_adLoader.setNativeAdListener(null);
            m_adLoader.setRevenueListener(null);
            m_adLoader.setAdReviewListener(null);

            m_adLoader.destroy();
            m_adLoader = null;
        }
    }

    @Override
    public void loadAd() {
        if (m_adView == null) {
            return;
        }

        if (m_plugin.hasOption("applovin.native.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        this.increaseRequestId();

        this.log("loadAd");

        this.buildNativeAdEvent("load")
            .log();

        this.setNativeState("load");

        try {
            m_adLoader.loadAd(m_adView);
        } catch (final Exception e) {
            m_plugin.logError("[Native] loadAd adUnitId: %s exception: %s"
                , m_adUnitId
                , e.getMessage()
            );

            this.buildNativeAdEvent("load_exception")
                .addParameterException("exception", e)
                .log();

            this.setNativeState("load_exception");

            this.retryLoadAd();
        }
    }

    protected void enableAdView(@NonNull MaxNativeAdView adView) {
        if (adView.getVisibility() == View.VISIBLE) {
            return;
        }

        if (m_plugin.hasOption("applovin.native.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        adView.setVisibility(View.VISIBLE);
        adView.requestLayout();
    }

    protected void disableAdView(@NonNull MaxNativeAdView adView) {
        if (adView.getVisibility() == View.GONE) {
            return;
        }

        if (m_plugin.hasOption("applovin.native.disable") == true || m_plugin.hasOption("applovin.ad.disable") == true) {
            return;
        }

        adView.setVisibility(View.GONE);
        adView.requestLayout();
    }

    private void updateVisible() {
        this.log("updateVisible", Map.of("show", m_visible));

        if (m_visible == true) {
            if (m_adView != null) {
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
        MengineUtils.performOnMainThread("show", () -> {
            if (m_visible == true) {
                return;
            }

            m_visible = true;

            this.updateVisible();
        });
    }

    public void hide() {
        MengineUtils.performOnMainThread("hide", () -> {
            if (m_visible == false) {
                return;
            }

            m_visible = false;

            this.updateVisible();
        });
    }

    @Override
    public void onAdRevenuePaid(@NonNull MaxAd ad) {
        this.logMaxAd("onAdRevenuePaid", ad);

        this.revenuePaid(ad);
    }

    @Override
    public void onCreativeIdGenerated(@NonNull String creativeId, @NonNull MaxAd ad) {
        //ToDo
    }
}
