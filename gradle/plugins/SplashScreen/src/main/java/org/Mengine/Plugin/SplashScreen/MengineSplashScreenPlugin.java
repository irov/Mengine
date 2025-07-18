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
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFragmentEngine;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerEngine;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineUtils;

public class MengineSplashScreenPlugin extends MengineService implements MengineListenerEngine, MengineListenerActivity {
    public static final String SERVICE_NAME = "SplashScreen";
    public static final boolean SERVICE_EMBEDDING = false;

    protected ImageView m_image;
    protected TextView m_text;

    protected volatile boolean m_differedHideOnResume;

    private Drawable getDrawableSplashScreen(Context context) {
        Resources resources = context.getResources();
        Resources.Theme theme = context.getTheme();
        Drawable drawable = ResourcesCompat.getDrawable(resources, R.drawable.mengine_launchscreen, theme);

        return drawable;
    }

    private ImageView createBackground(Context context) {
        ImageView image = new ImageView(context);
        Drawable mengine_splashscreen = this.getDrawableSplashScreen(context);
        image.setBackground(mengine_splashscreen);

        boolean mengine_splashscreen_background_inside = context.getResources().getBoolean(R.bool.mengine_splashscreen_background_inside);

        if (mengine_splashscreen_background_inside == true) {
            image.setScaleType(ImageView.ScaleType.CENTER_INSIDE);
        } else {
            image.setScaleType(ImageView.ScaleType.CENTER_CROP);
        }

        ViewGroup.LayoutParams params = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
        image.setLayoutParams(params);

        image.setVisibility(View.GONE);

        return image;
    }

    @NonNull
    private TextView createTextUserId(Context context, String message) {
        TextView text = new TextView(context);
        text.setText(message);
        int mengine_splashscreen_text_color = ContextCompat.getColor(context, R.color.mengine_splashscreen_text_color);
        text.setTextColor(mengine_splashscreen_text_color);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
        params.addRule(RelativeLayout.ALIGN_PARENT_START);

        Resources resources = context.getResources();

        int mengine_splashscreen_text_margin_left = resources.getDimensionPixelSize(R.dimen.mengine_splashscreen_text_margin_left);
        int mengine_splashscreen_text_margin_bottom = resources.getDimensionPixelSize(R.dimen.mengine_splashscreen_text_margin_bottom);

        params.setMargins(mengine_splashscreen_text_margin_left, 0, 0, mengine_splashscreen_text_margin_bottom);
        text.setLayoutParams(params);

        text.setVisibility(View.GONE);

        return text;
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        m_differedHideOnResume = false;

        if (MengineFragmentEngine.INSTANCE.isMenginePlatformRun() == true) {
            this.setState("splashscreen.state", "skip");

            return;
        }

        this.setState("splashscreen.state", "init");

        ViewGroup viewGroup = activity.getContentViewGroup();

        ImageView image = this.createBackground(activity);

        if (image == null) {
            this.setState("splashscreen.state", "failed");

            this.invalidInitialize("splash screen image is null");

            return;
        }

        viewGroup.addView(image);

        m_image = image;

        Resources resources = activity.getResources();

        boolean mengine_splashscreen_text_enable = resources.getBoolean(R.bool.mengine_splashscreen_text_enable);

        if (mengine_splashscreen_text_enable == true) {
            MengineApplication application = MengineApplication.INSTANCE;

            String userId = application.getUserId();
            String versionName = application.getVersionName();

            String message = String.format("ID: %s | %s", userId, versionName);

            TextView text = this.createTextUserId(activity, message);

            viewGroup.addView(text);

            m_text = text;
        }

        this.showSplash(activity);
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        if (m_image != null) {
            m_image.clearAnimation();
            m_image = null;
        }

        if (m_text != null) {
            m_text.clearAnimation();
            m_text = null;
        }
    }

    @Override
    public void onPause(@NonNull MengineActivity activity) {
        this.logInfo("onPause");

    }

    @Override
    public void onResume(@NonNull MengineActivity activity) {
        this.logInfo("onResume");

        if (m_differedHideOnResume == true) {
            this.logInfo("deferred hide splash screen on resume");

            m_differedHideOnResume = false;

            this.hideSplash(activity);
        }
    }

    @Override
    public void onMenginePlatformRun(@NonNull MengineApplication application) {
        this.logInfo("Mengine platform run, skip splash screen");

        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logWarning("MengineActivity is null, cannot hide splash screen");

            return;
        }

        MengineActivity.ELifecycleState lifecycleState = activity.getLifecycleState();

        if (lifecycleState != MengineActivity.ELifecycleState.ACTIVITY_RESUME) {
            this.logInfo("activity lifecycle state is not ACTIVITY_RESUME, defer hide splash screen");

            m_differedHideOnResume = true;

            return;
        }

        this.logInfo("hide splash screen on Mengine platform run");

        this.hideSplash(activity);
    }

    private void showSplash(@NonNull MengineActivity activity) {
        this.setState("splashscreen.state", "show");

        if (m_image == null) {
            this.logWarning("show splash screen failed, image is null");

            return;
        }

        AlphaAnimation showAnimation = new AlphaAnimation(0.f, 1.f);
        Resources resources = activity.getResources();

        long durationMillis = resources.getInteger(R.integer.mengine_splashscreen_show_duration);

        showAnimation.setDuration(durationMillis);
        showAnimation.setAnimationListener(new Animation.AnimationListener() {
            @Override
            public void onAnimationStart(Animation animation) {
                MengineSplashScreenPlugin.this.logInfo("start show splash screen");

                if (m_image != null) {
                    m_image.setVisibility(View.VISIBLE);
                    m_image.requestLayout();
                }

                if (m_text != null) {
                    m_text.setVisibility(View.VISIBLE);
                    m_text.requestLayout();
                }
            }

            @Override
            public void onAnimationEnd(Animation animation) {
                MengineSplashScreenPlugin.this.logInfo("shown splash screen");
            }

            @Override
            public void onAnimationRepeat(Animation animation) {
                //Empty
            }
        });

        this.logInfo("show splash screen animation duration: %d ms"
            , durationMillis
        );

        m_image.clearAnimation();
        m_image.startAnimation(showAnimation);
    }

    private void hideSplash(@NonNull MengineActivity activity) {
        this.setState("splashscreen.state", "hide");

        if (m_image == null) {
            this.logWarning("hide splash screen failed, image is null");

            return;
        }

        MengineUtils.performOnMainThread(() -> {
            if (activity.isFinishing() == true || activity.isDestroyed() == true) {
                MengineSplashScreenPlugin.this.logInfo("activity is finishing, skip hide splash screen");

                return;
            }

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

            Resources resources = activity.getResources();

            long durationMillis = resources.getInteger(R.integer.mengine_splashscreen_hide_duration);

            hideAnimation.setDuration(durationMillis);
            hideAnimation.setAnimationListener(new Animation.AnimationListener() {
                @Override
                public void onAnimationStart(Animation animation) {
                    MengineSplashScreenPlugin.this.logInfo("start hide splash screen");
                }

                @Override
                public void onAnimationEnd(Animation animation) {
                    MengineSplashScreenPlugin.this.logInfo("hided splash screen");

                    if (m_image != null) {
                        m_image.setVisibility(View.GONE);
                        m_image.requestLayout();
                    }

                    if (m_text != null) {
                        m_text.setVisibility(View.GONE);
                        m_text.requestLayout();
                    }

                    MengineSplashScreenPlugin.this.removeSplash(activity);
                }

                @Override
                public void onAnimationRepeat(Animation animation) {
                    //Empty
                }
            });

            this.logInfo("hide splash screen animation duration: %d ms"
                , durationMillis
            );

            m_image.clearAnimation();
            m_image.startAnimation(hideAnimation);
        });
    }

    private void removeSplash(@NonNull MengineActivity activity) {
        this.logInfo("remove splash screen");

        this.setState("splashscreen.state", "removed");

        ViewGroup viewGroup = activity.getContentViewGroup();

        if (m_image != null) {
            m_image.clearAnimation();
            if (viewGroup != null) {
                viewGroup.removeView(m_image);
            }
            m_image = null;
        }

        if (m_text != null) {
            m_text.clearAnimation();
            if (viewGroup != null) {
                viewGroup.removeView(m_text);
            }
            m_text = null;
        }
    }
}