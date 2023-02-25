package org.Mengine.Plugin.SplashScreen;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineActivity;

import android.graphics.drawable.Drawable;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.Transformation;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import android.os.Bundle;

import androidx.annotation.NonNull;

public class MengineSplashScreenPlugin extends MenginePlugin {
    public static final String PLUGIN_NAME = "SplashScreen";
    public static final boolean PLUGIN_EMBEDDING = false;

    protected ImageView m_image;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        ImageView image = new ImageView(activity);
        Drawable mengine_splashscreen = activity.getContext().getResources().getDrawable(R.drawable.mengine_splashscreen);
        image.setBackground(mengine_splashscreen);
        image.setScaleType(ImageView.ScaleType.CENTER_CROP);
        ViewGroup.LayoutParams params = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
        image.setLayoutParams(params);

        ViewGroup view = (ViewGroup)activity.getContentView();
        view.addView(image);

        m_image = image;

        this.showSplash(activity);
    }

    @Override
    public void onMengineApplicationReady(MengineActivity activity) {
        this.hideSplash(activity);
    }

    private void showSplash(@NonNull MengineActivity activity) {
        AlphaAnimation showAnimation = new AlphaAnimation(0.f, 1.f);
        long durationMillis = activity.getContext().getResources().getInteger(R.integer.mengine_splashscreen_show_duration);
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
            }
        });

        m_image.startAnimation(showAnimation);
    }

    private void hideSplash(@NonNull MengineActivity activity) {
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
        long durationMillis = activity.getContext().getResources().getInteger(R.integer.mengine_splashscreen_hide_duration);
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

                MengineSplashScreenPlugin.this.removeSpash(activity);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {
            }
        });

        m_image.clearAnimation();
        m_image.startAnimation(hideAnimation);
    }

    private void removeSpash(@NonNull MengineActivity activity) {
        ViewGroup view = (ViewGroup)activity.getContentView();
        view.removeView(m_image);

        m_image = null;
    }
}