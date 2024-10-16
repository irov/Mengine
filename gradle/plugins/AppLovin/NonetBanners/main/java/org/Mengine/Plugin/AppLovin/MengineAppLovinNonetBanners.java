package org.Mengine.Plugin.AppLovin;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.XmlResourceParser;
import android.graphics.drawable.Drawable;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import androidx.core.content.res.ResourcesCompat;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Timer;

public class MengineAppLovinNonetBanners implements MengineAppLovinNonetBannersInterface {
    protected MengineAppLovinPlugin m_plugin;

    protected int m_showBannerDurationTime;

    protected boolean m_visible;
    protected int m_requestId;

    class NonetBanner {
        public ImageView view;
        public String url;
    }

    protected List<NonetBanner> m_banners;

    protected NonetBanner m_showBanner;

    protected Timer m_refreshTimer;

    @Override
    public void initializeNonetBanners(MengineActivity activity, MengineAppLovinPlugin plugin) throws MenginePluginInvalidInitializeException {
        m_plugin = plugin;

        m_banners = new ArrayList<>();

        m_visible = false;
        m_requestId = 0;

        this.m_plugin.logMessage("[NONET_BANNER] initialize");

        Resources resources = activity.getResources();

        int MengineAppLovinPlugin_NonetBannerDurationTime = resources.getInteger(R.integer.mengine_applovin_nonet_banner_duration_time);

        m_showBannerDurationTime = MengineAppLovinPlugin_NonetBannerDurationTime;

        try {
            XmlResourceParser parser = resources.getXml(R.xml.nonet_banners);

            while (parser.next() != XmlPullParser.END_DOCUMENT) {
                String name = parser.getName();

                int attributeCount = parser.getAttributeCount();

                if (attributeCount == -1) {
                    continue;
                }

                if (name.equals("banner") == false) {
                    continue;
                }

                Map<String, String> attributes = new HashMap<>();

                for(int index = 0; index != attributeCount; ++index) {
                    String key = parser.getAttributeName(index);
                    String value = parser.getAttributeValue(index);

                    attributes.put(key, value);
                }

                String image = attributes.get("image");
                String url = attributes.get("url");

                this.addNonetBanner(activity, image, url);
            }
        } catch (final XmlPullParserException e) {
            m_plugin.logError("[NONET_BANNER] XmlPullParserException: %s"
                , e.getMessage()
            );

            return;
        } catch (final IOException e) {
            m_plugin.logError("[NONET_BANNER] IOException: %s"
                , e.getMessage()
            );

            return;
        }

        if (m_banners.isEmpty() == true) {
            return;
        }

        Timer refreshTimer = m_plugin.scheduleAtFixedRate(m_showBannerDurationTime, () -> {
            int refreshRequestId;
            String oldBanenrUrl;
            String newBannerUrl;

            if (MengineAppLovinNonetBanners.this.m_visible == false) {
                return;
            }

            synchronized (MengineAppLovinNonetBanners.this) {
                if (MengineAppLovinNonetBanners.this.m_showBanner == null) {
                    return;
                }

                MengineAppLovinNonetBanners.this.m_requestId++;

                ViewGroup viewGroup = activity.getContentViewGroup();

                NonetBanner oldBanner = MengineAppLovinNonetBanners.this.m_showBanner;

                viewGroup.removeView(oldBanner.view);

                NonetBanner newBanner = MengineAppLovinNonetBanners.this.getCurrentBanner();
                viewGroup.addView(newBanner.view);

                MengineAppLovinNonetBanners.this.m_showBanner = newBanner;

                refreshRequestId = MengineAppLovinNonetBanners.this.m_requestId;
                oldBanenrUrl = oldBanner.url;
                newBannerUrl = newBanner.url;
            }

            MengineAppLovinNonetBanners.this.m_plugin.logMessage("[NONET_BANNER] refresh banner request: %d old: %s new: %s"
                , refreshRequestId
                , oldBanenrUrl
                , newBannerUrl
            );

            MengineAppLovinNonetBanners.this.m_plugin.buildEvent("mng_ad_nonet_banner_displayed")
                .addParameterString("url", newBannerUrl)
                .addParameterLong("request_id", refreshRequestId)
                .log();
        });

        m_refreshTimer = refreshTimer;
    }

    protected void addNonetBanner(MengineActivity activity, String image, String url) {
        Resources resources = activity.getResources();
        Context context = activity.getBaseContext();
        String packageName = activity.getPackageName();

        int resId = resources.getIdentifier(image, "drawable", packageName);

        if (resId == 0) {
            m_plugin.logError("[NONET_BANNER] not found image: %s"
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

        view.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                m_plugin.logMessage("[NONET_BANNER] click banner request: %d url: %s"
                    , m_requestId
                    , url
                );

                m_plugin.buildEvent("mng_ad_nonet_banner_clicked")
                    .addParameterString("url", url)
                    .addParameterLong("request_id", m_requestId)
                    .log();

                MengineActivity activity = m_plugin.getMengineActivity();

                MengineUtils.openUrl(activity, url);
            }
        });

        m_plugin.logMessage("[NONET_BANNER] add banner url: %s"
            , url
        );

        NonetBanner banner = new NonetBanner();
        banner.view = view;
        banner.url = url;

        m_banners.add(banner);
    }

    @Override
    public void finalizeNonetBanners(MengineActivity activity, MengineAppLovinPlugin plugin) {
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

        m_plugin = null;
        m_banners = null;
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

        m_plugin.logMessage("[NONET_BANNER] show banner request: %d url: %s"
            , showRequestId
            , showUrl
        );

        m_plugin.buildEvent("mng_ad_nonet_banner_displayed")
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

        m_plugin.logMessage("[NONET_BANNER] hide banner request: %d url: %s"
            , hideRequestId
            , hideUrl
        );
    }
}
