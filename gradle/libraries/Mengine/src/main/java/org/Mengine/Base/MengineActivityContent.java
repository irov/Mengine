package org.Mengine.Base;

import android.content.Intent;
import android.view.Surface;
import android.widget.RelativeLayout;

import org.Mengine.Base.MengineActivity.ELifecycleState;

public abstract class MengineActivityContent implements MengineFactorable {
    protected final MengineActivity m_activity;

    protected MengineActivityContent(MengineActivity activity) {
        m_activity = activity;
    }

    public MengineActivity getMengineActivity() {
        return m_activity;
    }

    public abstract boolean isDevelopmentMode();
    public abstract String getCompanyName();
    public abstract String getProjectName();
    public abstract int getProjectVersion();

    public abstract Surface getSurface();

    public abstract void showKeyboard();
    public abstract void hideKeyboard();
    public abstract boolean isShowKeyboard();

    public abstract void onActivityCreate(RelativeLayout contentView);
    public abstract void onActivityDestroy();
    public abstract void onActivityPostDestroy();

    public abstract void onContentChanged();
    public abstract void onContentLifecycle(ELifecycleState state);

    public abstract void onPlatformLifecycle(ELifecycleState state);
    public abstract void onPlatformFocus(boolean focus);
    public abstract void onPlatformIntent(Intent intent, boolean start);
    public abstract void onPlatformLocale(String locale);
}
