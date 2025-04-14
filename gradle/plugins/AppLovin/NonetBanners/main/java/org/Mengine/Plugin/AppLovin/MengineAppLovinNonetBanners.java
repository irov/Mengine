package org.Mengine.Plugin.AppLovin;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.XmlResourceParser;
import android.graphics.drawable.Drawable;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;
import androidx.core.content.res.ResourcesCompat;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Timer;

public class MengineAppLovinNonetBanners implements MengineAppLovinNonetBannersInterface {
    protected MengineAppLovinPlugin m_plugin;

    protected int m_showBannerDurationTime;

    protected boolean m_visible;
    protected int m_requestId;

    static class NonetBanner {
        public ImageView view;
        public String url;
    }

    protected List<NonetBanner> m_banners;

    protected NonetBanner m_showBanner;

    protected Timer m_refreshTimer;

    @Override
    public void onAppCreate(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin) throws MengineServiceInvalidInitializeException {
        m_plugin = plugin;

        m_banners = new ArrayList<>();

        m_visible = false;
        m_requestId = 0;

        m_plugin.logMessage("[NONET_BANNERS] initialize");

        Resources resources = application.getResources();

        int MengineAppLovinPlugin_NonetBannerDurationTime = resources.getInteger(R.integer.mengine_applovin_nonet_banners_duration_time);

        m_showBannerDurationTime = MengineAppLovinPlugin_NonetBannerDurationTime;

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

                this.addNonetBanner(application, image, url);
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
    public void onAppTerminate(@NonNull MengineApplication application, @NonNull MengineAppLovinPlugin plugin) {
        m_banners = null;

        m_plugin = null;
    }

    @Override
    public void onActivityCreate(@NonNull MengineActivity activity) throws MengineServiceInvalidInitializeException {
        if (m_banners.isEmpty() == true) {
            return;
        }

        Timer refreshTimer = MengineUtils.scheduleOnUiAtFixedRate(activity, 0, m_showBannerDurationTime, () -> {
            int refreshRequestId;
            String oldBanenrUrl;
            String newBannerUrl;

            if (this.m_visible == false) {
                return;
            }

            synchronized (this) {
                if (m_showBanner == null) {
                    return;
                }

                m_requestId++;

                ViewGroup viewGroup = activity.getContentViewGroup();

                NonetBanner oldBanner = m_showBanner;

                viewGroup.removeView(oldBanner.view);

                NonetBanner newBanner = this.getCurrentBanner();
                viewGroup.addView(newBanner.view);

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

            m_plugin.buildEvent("mng_ad_nonet_banners_displayed")
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

        synchronized (this) {
            if (m_showBanner != null) {
                ViewGroup viewGroup = activity.getContentViewGroup();
                viewGroup.removeView(m_showBanner.view);

                m_showBanner = null;
            }
        }
    }

    protected void addNonetBanner(MengineApplication application, String image, String url) {
        Resources resources = application.getResources();
        Context context = application.getBaseContext();
        String packageName = application.getPackageName();

        int resId = resources.getIdentifier(image, "drawable", packageName);

        if (resId == 0) {
            m_plugin.logError("[NONET_BANNERS] not found image: %s"
                , image
            );

            return;
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
            m_plugin.logMessage("[NONET_BANNERS] click banner request: %d url: %s"
                , m_requestId
                , url
            );

            m_plugin.buildEvent("mng_ad_nonet_banners_clicked")
                .addParameterString("url", url)
                .addParameterLong("request_id", m_requestId)
                .log();

            MengineActivity activity = m_plugin.getMengineActivity();

            MengineUtils.openUrl(activity, url);
        });

        m_plugin.logMessage("[NONET_BANNERS] add banner url: %s"
            , url
        );

        NonetBanner banner = new NonetBanner();
        banner.view = view;
        banner.url = url;

        m_banners.add(banner);
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

        m_visible = true;

        int showRequestId;
        String showUrl;

        synchronized (this) {
            if (m_showBanner != null) {
                return;
            }

            m_requestId++;

            NonetBanner banner = this.getCurrentBanner();

            MengineActivity activity = m_plugin.getMengineActivity();

            ViewGroup viewGroup = activity.getContentViewGroup();
            viewGroup.addView(banner.view);

            m_showBanner = banner;

            showRequestId = m_requestId;
            showUrl = banner.url;
        }

        m_plugin.logMessage("[NONET_BANNERS] show banner request: %d url: %s"
            , showRequestId
            , showUrl
        );

        m_plugin.buildEvent("mng_ad_nonet_banners_displayed")
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

        m_visible = false;

        int hideRequestId;
        String hideUrl;

        synchronized (this) {
            if (m_showBanner == null) {
                return;
            }

            NonetBanner oldBanner = m_showBanner;

            MengineActivity activity = m_plugin.getMengineActivity();

            ViewGroup viewGroup = activity.getContentViewGroup();
            viewGroup.removeView(oldBanner.view);

            m_showBanner = null;

            hideRequestId = m_requestId;
            hideUrl = oldBanner.url;
        }

        m_plugin.logMessage("[NONET_BANNERS] hide banner request: %d url: %s"
            , hideRequestId
            , hideUrl
        );
    }
}
