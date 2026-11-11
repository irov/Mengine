package org.Mengine.Plugin.LocalNotifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.job.JobParameters;
import android.app.job.JobInfo;
import android.app.job.JobScheduler;
import android.app.job.JobService;
import android.content.ComponentName;
import android.content.Context;
import android.os.PersistableBundle;

import java.util.Objects;

public class MengineLocalNotificationsJobService extends JobService {
    private static final int JOB_ID_PREFIX = 0x4D000000;
    private static final long JOB_DEADLINE_WINDOW_MILLIS = 10000;

    public static PersistableBundle notificationBundle(int id, String title, String content){
        PersistableBundle bundle = new PersistableBundle();
        bundle.putInt(MengineLocalNotificationsConstants.KEY_ID, id);
        bundle.putString(MengineLocalNotificationsConstants.KEY_CONTENT, content);
        bundle.putString(MengineLocalNotificationsConstants.KEY_TITLE, title);

        return bundle;
    }

    static boolean scheduleNotification(Context context, long delayMillis, PersistableBundle bundle) {
        int notificationId = bundle.getInt(MengineLocalNotificationsConstants.KEY_ID);

        if (notificationId <= 0) {
            return false;
        }

        if (delayMillis <= 0) {
            return false;
        }

        JobScheduler jobScheduler = context.getSystemService(JobScheduler.class);

        if (jobScheduler == null) {
            return false;
        }

        int jobId = JOB_ID_PREFIX ^ notificationId;
        ComponentName jobService = new ComponentName(context, MengineLocalNotificationsJobService.class);
        JobInfo pendingJob = jobScheduler.getPendingJob(jobId);

        if (pendingJob != null) {
            ComponentName pendingJobService = pendingJob.getService();

            if (Objects.equals(pendingJobService, jobService) == false) {
                return false;
            }
        }

        long maximumLatency = Long.MAX_VALUE - JOB_DEADLINE_WINDOW_MILLIS;
        long latency = Math.min(delayMillis, maximumLatency);
        long deadline = latency + JOB_DEADLINE_WINDOW_MILLIS;
        JobInfo.Builder builder = new JobInfo.Builder(jobId, jobService);
        JobInfo jobInfo = builder
            .setMinimumLatency(delayMillis)
            .setOverrideDeadline(deadline)
            .setPersisted(true)
            .setExtras(bundle)
            .build();

        if (jobScheduler.schedule(jobInfo) != JobScheduler.RESULT_SUCCESS) {
            return false;
        }

        return true;
    }

    @Override
    public boolean onStartJob(JobParameters params) {
        PersistableBundle bundle = params.getExtras();
        String title = bundle.getString(MengineLocalNotificationsConstants.KEY_TITLE);
        String content = bundle.getString(MengineLocalNotificationsConstants.KEY_CONTENT);
        int id = bundle.getInt(MengineLocalNotificationsConstants.KEY_ID);

        Context context = this.getApplicationContext();

        Notification notification = MengineLocalNotificationsPlugin.getNotification(context, id, title, content);
        NotificationManager notificationManager = this.getSystemService(NotificationManager.class);

        if (notificationManager == null) {
            return false;
        }

        notificationManager.notify(MengineLocalNotificationsPlugin.NOTIFICATION_TAG, id, notification);

        int month = bundle.getInt(MengineLocalNotificationsConstants.KEY_MONTH);

        if (month > 0) {
            int day = bundle.getInt(MengineLocalNotificationsConstants.KEY_DAY);
            int hour = bundle.getInt(MengineLocalNotificationsConstants.KEY_HOUR);
            int minute = bundle.getInt(MengineLocalNotificationsConstants.KEY_MINUTE);
            long now = System.currentTimeMillis();
            long delay = MengineLocalNotificationsPlugin.calculateCalendarDelay(month, day, hour, minute, now);
            MengineLocalNotificationsJobService.scheduleNotification(context, delay, bundle);
        }

        return false;
    }

    @Override
    public boolean onStopJob(JobParameters params) {
        return false;
    }
}
