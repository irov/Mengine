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
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineUtils;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Timer;

public class MengineAppLovinNonetBanners {
    protected MengineAppLovinPlugin m_plugin;

    protected long m_showBannerDurationTime;

    protected boolean m_visible;
    protected int m_requestId;

    class NonetBanner {
        public ImageView view;
        public String url;
    }

    protected List<NonetBanner> m_nonetBanners;

    protected NonetBanner m_showBanner;

    protected Timer m_refreshTimer;

    public MengineAppLovinNonetBanners(MengineAppLovinPlugin plugin, long showBannerDurationTime) {
        m_plugin = plugin;

        m_nonetBanners = new ArrayList<>();

        m_showBannerDurationTime = showBannerDurationTime;

        m_visible = false;
        m_requestId = 0;
    }

    public void initialize() {
        MengineApplication application = m_plugin.getMengineApplication();

        Resources resources = application.getResources();

        try {
            XmlResourceParser parser = resources.getXml(R.xml.nonet_banners);

            while (parser.next() != XmlPullParser.END_DOCUMENT) {
                String name = parser.getName();

                int ac = parser.getAttributeCount();

                if (ac == -1) {
                    continue;
                }

                if (name.equals("banner") == false) {
                    continue;
                }

                Map<String, String> attributes = new HashMap<>();

                for(int index = 0; index != ac; ++index) {
                    String key = parser.getAttributeName(index);
                    String value = parser.getAttributeValue(index);

                    attributes.put(key, value);
                }

                String image = attributes.get("image");
                String url = attributes.get("url");

                this.addNonetBanner(image, url);
            }
        } catch (XmlPullParserException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (m_nonetBanners.isEmpty() == true) {
            return;
        }

        Timer refreshTimer = m_plugin.scheduleAtFixedRate(m_showBannerDurationTime * 1000, new Runnable() {
            @Override
            public void run() {
                synchronized (MengineAppLovinNonetBanners.this) {
                    if (m_showBanner == null) {
                        return;
                    }

                    m_requestId++;

                    ViewGroup viewGroup = MengineActivity.getContentViewGroup();

                    NonetBanner oldBanner = m_showBanner;

                    viewGroup.removeView(oldBanner.view);

                    NonetBanner newBanner = MengineAppLovinNonetBanners.this.getCurrentBanner();
                    viewGroup.addView(newBanner.view);

                    m_showBanner = newBanner;

                    m_plugin.logMessage("[NONET_BANNER] refresh banner request: %d old: %s new: %s"
                        , m_requestId
                        , oldBanner.url
                        , newBanner.url
                    );

                    m_plugin.buildEvent("mng_ad_nonet_banner_displayed")
                        .addParameterString("url", newBanner.url)
                        .addParameterLong("request_id", m_requestId)
                        .log();
                }
            }
        });

        m_refreshTimer = refreshTimer;
    }

    protected void addNonetBanner(String image, String url) {
        MengineApplication application = m_plugin.getMengineApplication();

        Resources resources = application.getResources();
        Context context = application.getApplicationContext();
        String packageName = application.getPackageName();

        int resId = resources.getIdentifier(image, "drawable", packageName);

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

        m_nonetBanners.add(banner);
    }

    public void destroy() {
        m_plugin = null;
        m_nonetBanners = null;

        synchronized (this) {
            if (m_showBanner != null) {
                ViewGroup viewGroup = MengineActivity.getContentViewGroup();
                viewGroup.removeView(m_showBanner.view);

                m_showBanner = null;
            }
        }

        if (m_refreshTimer != null) {
            m_refreshTimer.cancel();
            m_refreshTimer = null;
        }
    }

    protected NonetBanner getCurrentBanner() {
        long timestamp = MengineUtils.getTimestamp();
        timestamp /= 1000;
        timestamp /= m_showBannerDurationTime;

        int index = (int)timestamp % m_nonetBanners.size();
        NonetBanner banner = m_nonetBanners.get(index);

        return banner;
    }

    public void show() {
        if (m_nonetBanners.isEmpty() == true) {
            return;
        }

        m_visible = true;

        synchronized (this) {
            if (m_showBanner != null) {
                return;
            }

            m_requestId++;

            NonetBanner banner = this.getCurrentBanner();

            ViewGroup viewGroup = MengineActivity.getContentViewGroup();
            viewGroup.addView(banner.view);

            m_showBanner = banner;

            m_plugin.logMessage("[NONET_BANNER] show banner request: %d url: %s"
                , m_requestId
                , banner.url
            );

            m_plugin.buildEvent("mng_ad_nonet_banner_displayed")
                .addParameterString("url", banner.url)
                .addParameterLong("request_id", m_requestId)
                .log();
        }
    }

    public void hide() {
        if (m_nonetBanners.isEmpty() == true) {
            return;
        }

        m_visible = false;

        synchronized (this) {
            if (m_showBanner == null) {
                return;
            }

            NonetBanner oldBanner = m_showBanner;

            ViewGroup viewGroup = MengineActivity.getContentViewGroup();
            viewGroup.removeView(oldBanner.view);

            m_showBanner = null;

            m_plugin.logMessage("[NONET_BANNER] hide banner request: %d url: %s"
                , m_requestId
                , oldBanner.url
            );
        }
    }
}
