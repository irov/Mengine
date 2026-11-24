package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.function.BooleanSupplier;
import java.util.function.Predicate;

public final class MengineFullscreenAdShow {
    private final Predicate<MengineActivity> m_canShowInActivity;
    @Nullable
    private final Predicate<Runnable> m_scheduleShow;

    private volatile boolean m_showing;
    private volatile String m_placement = "";

    public MengineFullscreenAdShow(@NonNull Predicate<MengineActivity> canShowInActivity, @Nullable Predicate<Runnable> scheduleShow) {
        m_canShowInActivity = canShowInActivity;
        m_scheduleShow = scheduleShow;
    }

    public boolean isShowing() {
        return m_showing;
    }

    @NonNull
    public String getPlacement() {
        return m_placement;
    }

    @NonNull
    public synchronized String finishShow() {
        if (m_showing == false) {
            return "";
        }

        String placement = m_placement;
        m_placement = "";
        m_showing = false;

        MenginePlatformEventQueue.pushUnfreezeEvent(MenginePlatformEventQueue.FREEZE_ALL);

        return placement;
    }

    private void runShow(@NonNull Runnable showAd) {
        try {
            showAd.run();
        } catch (final RuntimeException e) {
            this.finishShow();

            throw e;
        }
    }

    public boolean show(@NonNull MengineActivity activity, @NonNull String placement, @NonNull BooleanSupplier isCurrentAd, @NonNull Runnable showAd) {
        synchronized (this) {
            if (m_showing == true) {
                return false;
            }

            if (isCurrentAd.getAsBoolean() == false) {
                return false;
            }

            if (m_canShowInActivity.test(activity) == false) {
                return false;
            }

            if (MenginePlatformEventQueue.pushFreezeEvent(MenginePlatformEventQueue.FREEZE_ALL, null) == false) {
                return false;
            }

            m_placement = placement;
            m_showing = true;
        }

        if (m_scheduleShow == null) {
            this.runShow(showAd);

            return true;
        }

        boolean scheduled = m_scheduleShow.test(() -> {
            if (isCurrentAd.getAsBoolean() == false) {
                return;
            }

            this.runShow(showAd);
        });

        if (scheduled == false) {
            this.finishShow();

            return false;
        }

        return true;
    }
}
