package org.Mengine.Plugin.AppLovin.NonetBanners;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.XmlResourceParser;
import android.graphics.drawable.Drawable;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Size;
import androidx.core.content.res.ResourcesCompat;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineAnalyticsEventBuilderInterface;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineRunnablePeriodically;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinNonetBannersInterface;
import org.Mengine.Plugin.AppLovin.Core.MengineAppLovinPluginInterface;

import org.json.JSONObject;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

public class MengineAppLovinNonetBanners implements MengineAppLovinNonetBannersInterface {
    protected MengineAppLovinPluginInterface m_plugin;

    protected boolean m_showBannersEnabled;
    protected long m_showBannerDurationTime;

    protected boolean m_visible;
    protected int m_requestId;

    static protected class NonetBanner {
        public String image;
        public String url;
    }

    protected List<NonetBanner> m_banners;

    protected NonetBanner m_showBanner;
    protected ImageView m_showBannerView;

    protected final Object m_showBannerLock = new Object();

    protected MengineRunnablePeriodically m_refreshTimer;

    protected MengineAnalyticsEventBuilderInterface buildNonetBannersEvent(@Size(min = 1L, max = 40L) String event) {
        MengineAnalyticsEventBuilderInterface builder = m_plugin.buildEvent("mng_applovin_nonet_banners_" + event);

        return builder;
    }

    @Override
    public void onAppCreate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin) throws MengineServiceInvalidInitializeException {
        m_plugin = plugin;

        m_banners = new ArrayList<>();

        m_visible = false;
        m_requestId = 0;

        m_plugin.logMessage("[NONET_BANNERS] initialize");

        Resources resources = application.getResources();

        m_showBannerDurationTime = 30000;

        try {
            XmlResourceParser parser = resources.getXml(R.xml.nonet_banners);

            while (parser.next() != XmlPullParser.END_DOCUMENT) {
                if (parser.getEventType() != XmlResourceParser.START_TAG) {
                    continue;
                }

                String name = parser.getName();

                if (Objects.equals(name, "banner") == false) {
                    continue;
                }

                int attributeCount = parser.getAttributeCount();

                Map<String, String> attributes = new HashMap<>();

                for (int index = 0; index != attributeCount; ++index) {
                    String key = parser.getAttributeName(index);
                    String value = parser.getAttributeValue(index);

                    attributes.put(key, value);
                }

                String image = attributes.get("image");
                String url = attributes.get("url");

                NonetBanner banner = new NonetBanner();
                banner.image = image;
                banner.url = url;

                m_banners.add(banner);
            }

            parser.close();
        } catch (final XmlPullParserException e) {
            m_plugin.logError("[NONET_BANNERS] XmlPullParserException: %s"
                , e.getMessage()
            );

            return;
        } catch (final IOException e) {
            m_plugin.logError("[NONET_BANNERS] IOException: %s"
                , e.getMessage()
            );

            return;
        }
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application, @NonNull MengineAppLovinPluginInterface plugin) {
        m_banners = null;

        m_plugin = null;
    }

    @Override
    public void onMengineRemoteConfigFetch(@NonNull MengineApplication application, boolean updated, @NonNull Map<String, JSONObject> configs) {
        JSONObject applovin_nonetbanner = configs.getOrDefault("applovin_nonetbanner", null);

        if (applovin_nonetbanner != null) {
            boolean enable = applovin_nonetbanner.optBoolean("enable", false);

            m_showBannersEnabled = enable;

            long duration = applovin_nonetbanner.optLong("duration", 30000);

            m_showBannerDurationTime = duration;
        }
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) {
        if (m_showBannersEnabled == false) {
            return;
        }

        if (m_banners.isEmpty() == true) {
            return;
        }

        MengineRunnablePeriodically refreshTimer = MengineUtils.scheduleOnMainThreadFixedRate(0, m_showBannerDurationTime, () -> {
            int refreshRequestId;
            String oldBanenrUrl;
            String newBannerUrl;

            if (this.m_visible == false) {
                return;
            }

            synchronized (m_showBannerLock) {
                if (m_showBanner == null) {
                    return;
                }

                m_requestId++;

                NonetBanner oldBanner = m_showBanner;

                ViewGroup viewGroup = activity.getContentViewGroup();

                if (m_showBannerView != null) {
                    viewGroup.removeView(m_showBannerView);
                }

                NonetBanner newBanner = this.getCurrentBanner();

                ImageView view = this.createNonetBannerView(activity, newBanner.image, newBanner.url);

                viewGroup.addView(view);

                m_showBannerView = view;

                m_showBanner = newBanner;

                refreshRequestId = m_requestId;
                oldBanenrUrl = oldBanner.url;
                newBannerUrl = newBanner.url;
            }

            m_plugin.logMessage("[NONET_BANNERS] refresh banner request: %d old: %s new: %s"
                , refreshRequestId
                , oldBanenrUrl
                , newBannerUrl
            );

            this.buildNonetBannersEvent("displayed")
                .addParameterString("url", newBannerUrl)
                .addParameterLong("request_id", refreshRequestId)
                .log();
        });

        m_refreshTimer = refreshTimer;

        if (m_visible == true) {
            this.show();
        }
    }

    @Override
    public void onActivityDestroy(@NonNull MengineActivity activity) {
        if (m_refreshTimer != null) {
            m_refreshTimer.cancel();
            m_refreshTimer = null;
        }

        synchronized (m_showBannerLock) {
            if (m_showBannerView != null) {
                ViewGroup viewGroup = activity.getContentViewGroup();
                viewGroup.removeView(m_showBannerView);

                m_showBannerView = null;
            }
        }
    }

    protected ImageView createNonetBannerView(@NonNull MengineActivity activity, String image, String url) {
        Resources resources = activity.getResources();
        Context context = activity.getBaseContext();
        String packageName = activity.getPackageName();

        int resId = resources.getIdentifier(image, "drawable", packageName);

        if (resId == 0) {
            m_plugin.logError("[NONET_BANNERS] not found image: %s"
                , image
            );

            return null;
        }

        Resources.Theme theme = context.getTheme();
        Drawable drawable = ResourcesCompat.getDrawable(resources, resId, theme);

        ImageView view = new ImageView(context);
        view.setBackground(drawable);
        view.setScaleType(ImageView.ScaleType.FIT_XY);
        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
        view.setLayoutParams(params);

        view.setOnClickListener(v -> {
            if (activity == null) {
                m_plugin.logError("[NONET_BANNERS] click banner invalid activity");

                return;
            }

            m_plugin.logMessage("[NONET_BANNERS] click banner request: %d url: %s"
                , m_requestId
                , url
            );

            this.buildNonetBannersEvent("clicked")
                .addParameterString("url", url)
                .addParameterLong("request_id", m_requestId)
                .log();

            MengineUtils.openUrl(activity, url);
        });

        m_plugin.logInfo("[NONET_BANNERS] add banner url: %s"
            , url
        );

        return view;
    }

    protected NonetBanner getCurrentBanner() {
        long timestamp = MengineUtils.getTimestamp();
        timestamp /= m_showBannerDurationTime;

        int index = (int)timestamp % m_banners.size();
        NonetBanner banner = m_banners.get(index);

        return banner;
    }

    @Override
    public void show() {
        if (m_banners.isEmpty() == true) {
            return;
        }

        if (m_visible == true) {
            return;
        }

        MengineActivity activity = m_plugin.getMengineActivity();

        if (activity == null) {
            m_plugin.logError("[NONET_BANNERS] show banner invalid activity");

            return;
        }

        m_visible = true;

        int showRequestId;
        String showUrl;

        synchronized (m_showBannerLock) {
            if (m_showBanner != null) {
                return;
            }

            m_requestId++;

            NonetBanner banner = this.getCurrentBanner();

            ViewGroup viewGroup = activity.getContentViewGroup();

            ImageView bannerView = this.createNonetBannerView(activity, banner.image, banner.url);

            viewGroup.addView(bannerView);

            m_showBannerView = bannerView;
            m_showBanner = banner;

            showRequestId = m_requestId;
            showUrl = banner.url;
        }

        m_plugin.logMessage("[NONET_BANNERS] show banner request: %d url: %s"
            , showRequestId
            , showUrl
        );

        this.buildNonetBannersEvent("displayed")
            .addParameterString("url", showUrl)
            .addParameterLong("request_id", showRequestId)
            .log();
    }

    @Override
    public void hide() {
        if (m_banners.isEmpty() == true) {
            return;
        }

        if (m_visible == false) {
            return;
        }

        MengineActivity activity = m_plugin.getMengineActivity();

        if (activity == null) {
            m_plugin.logError("[NONET_BANNERS] hide banner invalid activity");

            return;
        }

        m_visible = false;

        int hideRequestId;
        String hideUrl;

        synchronized (m_showBannerLock) {
            if (m_showBanner == null) {
                return;
            }

            NonetBanner oldBanner = m_showBanner;
            m_showBanner = null;

            if (m_showBannerView != null) {
                ViewGroup viewGroup = activity.getContentViewGroup();
                viewGroup.removeView(m_showBannerView);

                m_showBannerView = null;
            }

            hideRequestId = m_requestId;
            hideUrl = oldBanner.url;
        }

        m_plugin.logMessage("[NONET_BANNERS] hide banner request: %d url: %s"
            , hideRequestId
            , hideUrl
        );
    }
}
