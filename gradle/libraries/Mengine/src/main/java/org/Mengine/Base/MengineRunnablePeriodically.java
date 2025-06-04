package org.Mengine.Base;

import android.os.Handler;

public class MengineRunnablePeriodically implements Runnable {
    private final Handler handler;
    private final Runnable runnable;
    private final long period;

    MengineRunnablePeriodically(Handler handler, Runnable runnable, long period) {
        this.handler = handler;
        this.runnable = runnable;
        this.period = period;
    }

    public void start(long delay) {
        handler.postDelayed(this, delay);
    }

    @Override
    public void run() {
        runnable.run();

        handler.postDelayed(this, period);
    }

    public void cancel() {
        handler.removeCallbacks(this);
    }
}