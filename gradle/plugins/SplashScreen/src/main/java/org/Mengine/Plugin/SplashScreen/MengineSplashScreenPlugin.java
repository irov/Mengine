package org.Mengine.Plugin.SplashScreen;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.Transformation;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;
import androidx.core.content.res.ResourcesCompat;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginEngineListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MengineUtils;

public class MengineSplashScreenPlugin extends MenginePlugin implements MenginePluginEngineListener, MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "SplashScreen";
    public static final boolean PLUGIN_EMBEDDING = false;

    protected ImageView m_image;
    protected TextView m_text;

    private Drawable getDrawableSplashScreen(Context context) {
        Resources resources = context.getResources();
        Resources.Theme theme = context.getTheme();
        Drawable drawable = ResourcesCompat.getDrawable(resources, R.drawable.mengine_splashscreen, theme);

        return drawable;
    }

    private ImageView createBackground(Context context) {
        ImageView image = new ImageView(context);
        Drawable mengine_splashscreen = this.getDrawableSplashScreen(context);
        image.setBackground(mengine_splashscreen);
        image.setScaleType(ImageView.ScaleType.CENTER_CROP);

        ViewGroup.LayoutParams params = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
        image.setLayoutParams(params);

        return image;
    }

    private TextView createTextSessionId(Context context, String message) {
        TextView text = new TextView(context);
        text.setText(message);
        int mengine_splashscreen_text_color = ContextCompat.getColor(context, R.color.mengine_splashscreen_text_color);
        text.setTextColor(mengine_splashscreen_text_color);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
        params.addRule(RelativeLayout.ALIGN_PARENT_START);

        Resources resources = context.getResources();

        int mengine_splashscreen_text_margin_left = resources.getInteger(R.integer.mengine_splashscreen_text_margin_left);
        int mengine_splashscreen_text_margin_bottom = resources.getInteger(R.integer.mengine_splashscreen_text_margin_bottom);
        float density = resources.getDisplayMetrics().density;
        int margin_left = (int)(mengine_splashscreen_text_margin_left * density);
        int margin_bottom = (int)(mengine_splashscreen_text_margin_bottom * density);
        params.setMargins(margin_left, 0, 0, margin_bottom);
        text.setLayoutParams(params);

        return text;
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        this.setState("splashscreen.state", "init");

        Context context = MengineActivity.getContext();
        ViewGroup viewGroup = MengineActivity.getContentViewGroup();

        ImageView image = this.createBackground(context);

        viewGroup.addView(image);

        m_image = image;

        Resources resources = context.getResources();

        boolean mengine_splashscreen_text_enable = resources.getBoolean(R.bool.mengine_splashscreen_text_enable);

        if (mengine_splashscreen_text_enable == true) {
            String sessionId = activity.getSessionId();
            String versionName = activity.getVersionName();

            String message = String.format("ID: %s | %s", sessionId, versionName);

            TextView text = this.createTextSessionId(context, message);

            viewGroup.addView(text);

            m_text = text;
        }

        this.showSplash(activity);
    }

    @Override
    public void onMenginePlatformRun(MengineActivity activity) {
        MengineUtils.performOnMainThread(() -> {
            this.hideSplash(activity);
        });
    }

    private void showSplash(@NonNull MengineActivity activity) {
        this.setState("splashscreen.state", "show");

        AlphaAnimation showAnimation = new AlphaAnimation(0.f, 1.f);
        long durationMillis = MengineActivity.getContext().getResources().getInteger(R.integer.mengine_splashscreen_show_duration);
        showAnimation.setDuration(durationMillis);
        showAnimation.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {
                MengineSplashScreenPlugin.this.logMessage("show splash screen");

                m_image.setVisibility(View.VISIBLE);

                if (m_text != null) {
                    m_text.setVisibility(View.VISIBLE);
                }
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                MengineSplashScreenPlugin.this.logMessage("shown splash screen");
            }

            @Override
            public void onAnimationRepeat(Animation animation) {
                //Empty
            }
        });

        m_image.startAnimation(showAnimation);
    }

    private void hideSplash(@NonNull MengineActivity activity) {
        this.setState("splashscreen.state", "hide");

        Animation showAnimation = m_image.getAnimation();

        float alphaFrom = 1.f;
        if (showAnimation != null) {
            long drawTime = m_image.getDrawingTime();
            Transformation transformation = new Transformation();
            showAnimation.getTransformation(drawTime, transformation);
            float imageAlpha = transformation.getAlpha();
            alphaFrom = imageAlpha;
        }

        AlphaAnimation hideAnimation = new AlphaAnimation(alphaFrom, 0.f);
        long durationMillis = MengineActivity.getContext().getResources().getInteger(R.integer.mengine_splashscreen_hide_duration);
        hideAnimation.setDuration(durationMillis);
        hideAnimation.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {
                MengineSplashScreenPlugin.this.logMessage("hide splash screen");
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                MengineSplashScreenPlugin.this.logMessage("hided splash screen");

                m_image.setVisibility(View.GONE);

                if (m_text != null) {
                    m_text.setVisibility(View.GONE);
                }

                MengineUtils.performOnMainThread(() -> {
                    MengineSplashScreenPlugin.this.removeSpash(activity);
                });
            }

            @Override
            public void onAnimationRepeat(Animation animation) {
                //Empty
            }
        });

        m_image.clearAnimation();
        m_image.startAnimation(hideAnimation);
    }

    private void removeSpash(@NonNull MengineActivity activity) {
        ViewGroup viewGroup = MengineActivity.getContentViewGroup();

        viewGroup.removeView(m_image);
        m_image = null;

        if (m_text != null) {
            viewGroup.removeView(m_text);
            m_text = null;
        }

        this.setState("splashscreen.state", "removed");
    }
}