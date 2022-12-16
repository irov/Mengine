package org.Mengine.Plugin.SplashScreen;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineActivity;

import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import android.os.Bundle;

public class MengineSplashScreenPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "SplashScreen";
    public static boolean PLUGIN_EMBEDDING = false;

    protected ImageView m_image;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        ImageView image = new ImageView(activity);
        image.setBackgroundResource(R.drawable.splashscreen);
        image.setScaleType(ImageView.ScaleType.CENTER_CROP);
        image.setLayoutParams(new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT));

        ViewGroup view = (ViewGroup)activity.getContentView();
        view.addView(image);

        m_image = image;

        this.showSplash();
    }

    @Override
    public void onMengineApplicationReady(MengineActivity activity) {
        this.hideSplash();
    }

    public void showSplash() {
        m_image.clearAnimation();

        AlphaAnimation animation = new AlphaAnimation(0.f, 1.f);
        animation.setDuration(800);
        animation.setFillAfter(true);
        animation.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {
                m_image.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                m_image.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationRepeat(Animation animation) {
                //Empty
            }
        });

        m_image.startAnimation(animation);
    }

    public void hideSplash() {
        m_image.clearAnimation();

        AlphaAnimation animation = new AlphaAnimation(1.f, 0.f);
        animation.setDuration(400);
        animation.setFillAfter(true);
        animation.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {
                m_image.setVisibility(View.VISIBLE);
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                m_image.setVisibility(View.GONE);

                MengineSplashScreenPlugin.this.removeSpash();
            }

            @Override
            public void onAnimationRepeat(Animation animation) {
                //Empty
            }
        });

        m_image.startAnimation(animation);
    }

    public void removeSpash() {
        MengineActivity activity = this.getActivity();

        ViewGroup view = (ViewGroup)activity.getContentView();
        view.removeView(m_image);
        m_image = null;
    }
}