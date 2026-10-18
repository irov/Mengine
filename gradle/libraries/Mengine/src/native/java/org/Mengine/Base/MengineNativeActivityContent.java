package org.Mengine.Base;

import android.content.Intent;
import android.view.Surface;
import android.widget.RelativeLayout;

import org.Mengine.Base.MengineActivity.ELifecycleState;

public class MengineNativeActivityContent extends MengineActivityContent {
    private MengineSurfaceView m_surfaceView;
    private MengineSoftInput m_softInput;

    public MengineNativeActivityContent(MengineActivity activity) {
        super(activity);
    }

    @Override
    public boolean isDevelopmentMode() {
        if (MengineNative.AndroidEnvironmentService_isDevelopmentMode() == false) {
            return false;
        }

        return true;
    }

    @Override
    public String getCompanyName() {
        String companyName = MengineNative.AndroidEnvironmentService_getCompanyName();

        return companyName;
    }

    @Override
    public String getProjectName() {
        String projectName = MengineNative.AndroidEnvironmentService_getProjectName();

        return projectName;
    }

    @Override
    public int getProjectVersion() {
        int projectVersion = MengineNative.AndroidEnvironmentService_getProjectVersion();

        return projectVersion;
    }

    @Override
    public Surface getSurface() {
        if (m_surfaceView == null) {
            return null;
        }

        Surface surface = m_surfaceView.getSurface();

        return surface;
    }

    @Override
    public void showKeyboard() {
        m_activity.postCommand(() -> {
            m_softInput.showKeyboard();
        });
    }

    @Override
    public void hideKeyboard() {
        m_activity.postCommand(() -> {
            m_softInput.hideKeyboard();
        });
    }

    @Override
    public boolean isShowKeyboard() {
        if (m_softInput == null) {
            return false;
        }

        if (m_softInput.isShowKeyboard() == false) {
            return false;
        }

        return true;
    }

    @Override
    public void onActivityCreate(RelativeLayout contentView) {
        m_surfaceView = new MengineSurfaceView(m_activity);
        contentView.addView(m_surfaceView);
        m_softInput = new MengineSoftInput(m_activity, m_surfaceView);
        contentView.addView(m_softInput);
    }

    @Override
    public void onActivityDestroy() {
        if (m_surfaceView != null) {
            m_surfaceView.handleDestroy();
            m_surfaceView = null;
        }

        if (m_softInput == null) {
            return;
        }

        m_softInput.handleDestroy();
        m_softInput = null;
    }

    @Override
    public void onActivityPostDestroy() {
        //Empty
    }

    @Override
    public void onContentChanged() {
        //Empty
    }

    @Override
    public void onContentLifecycle(ELifecycleState state) {
        if (m_surfaceView == null) {
            return;
        }

        switch (state) {
            case ACTIVITY_START: {
                m_surfaceView.handleStart();
                break;
            }
            case ACTIVITY_STOP: {
                m_surfaceView.handleStop();
                break;
            }
            case ACTIVITY_PAUSE: {
                m_surfaceView.handlePause();
                break;
            }
            case ACTIVITY_RESUME: {
                m_surfaceView.handleResume();
                break;
            }
            default: {
                break;
            }
        }
    }

    @Override
    public void onPlatformLifecycle(ELifecycleState state) {
        switch (state) {
            case ACTIVITY_START: {
                MenginePlatformEventQueue.pushStartEvent();
                break;
            }
            case ACTIVITY_STOP: {
                MenginePlatformEventQueue.pushStopEvent();
                break;
            }
            case ACTIVITY_RESTART: {
                MenginePlatformEventQueue.pushRestartEvent();
                break;
            }
            case ACTIVITY_DESTROY: {
                MenginePlatformEventQueue.pushDestroyEvent();
                break;
            }
            case ACTIVITY_PAUSE: {
                float pauseX = MengineNative.AndroidPlatform_getLastFingerX();
                float pauseY = MengineNative.AndroidPlatform_getLastFingerY();
                MenginePlatformEventQueue.pushPauseEvent(pauseX, pauseY);
                break;
            }
            case ACTIVITY_RESUME: {
                float resumeX = MengineNative.AndroidPlatform_getLastFingerX();
                float resumeY = MengineNative.AndroidPlatform_getLastFingerY();
                MenginePlatformEventQueue.pushResumeEvent(resumeX, resumeY);
                break;
            }
            default: {
                break;
            }
        }
    }

    @Override
    public void onPlatformFocus(boolean focus) {
        MenginePlatformEventQueue.pushWindowFocusChangedEvent(focus);
    }

    @Override
    public void onPlatformIntent(Intent intent, boolean start) {
        if (start == true) {
            MenginePlatformEventQueue.pushIntentStartEvent(intent);
        } else {
            MenginePlatformEventQueue.pushIntentNewEvent(intent);
        }
    }

    @Override
    public void onPlatformLocale(String locale) {
        MenginePlatformEventQueue.pushChangeLocaleEvent(locale);
    }
}
