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

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

public class MengineSplashScreenPlugin extends MenginePlugin {
    public static final String PLUGIN_NAME = "SplashScreen";
    public static final boolean PLUGIN_EMBEDDING = false;

    protected ImageView m_image;

    private Drawable getDrawableSplashScreen(Context context) {
        Resources resources = context.getResources();
        Resources.Theme theme = context.getTheme();
        Drawable drawable = resources.getDrawable(R.drawable.mengine_splashscreen, theme);

        return drawable;
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        this.setState("splashscreen.state", "init");

        ImageView image = new ImageView(activity);
        Context context = MengineActivity.getContext();
        Drawable mengine_splashscreen = this.getDrawableSplashScreen(context);
        image.setBackground(mengine_splashscreen);
        image.setScaleType(ImageView.ScaleType.CENTER_CROP);
        ViewGroup.LayoutParams params = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
        image.setLayoutParams(params);

        ViewGroup view = (ViewGroup)MengineActivity.getContentView();
        view.addView(image);

        m_image = image;

        this.showSplash(activity);
    }

    @Override
    public void onMenginePlatformReady(MengineActivity activity) {
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
        ViewGroup view = (ViewGroup)MengineActivity.getContentView();
        view.removeView(m_image);

        m_image = null;

        this.setState("splashscreen.state", "remove");
    }
}