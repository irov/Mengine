package org.Mengine.Build.LocalNotifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.job.JobParameters;
import android.app.job.JobService;
import android.content.Context;
import android.os.Build;
import android.os.PersistableBundle;
import android.util.Log;

import androidx.annotation.RequiresApi;

import org.Mengine.Build.MengineActivity;

@RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
public class NotificationJobService extends JobService {
    public static final String KEY_TITLE = "title";
    public static final String KEY_CONTENT = "content";
    public static final String KEY_ID = "id";

    public static PersistableBundle notificationBundle(int id, String title, String content){
        PersistableBundle bundle = new PersistableBundle();
        bundle.putInt(KEY_ID, id);
        bundle.putString(KEY_CONTENT, content);
        bundle.putString(KEY_TITLE, title);
        return bundle;
    }

    @Override
    public boolean onStartJob(JobParameters params) {
        Log.d(MengineActivity.TAG, "Job started");
        PersistableBundle bundle = params.getExtras();
        String title = bundle.getString(KEY_TITLE);
        String content = bundle.getString(KEY_CONTENT);
        int id = bundle.getInt(KEY_ID);

        Notification notification = MengineLocalNotificationsPlugin.getNotification(getApplicationContext(), id, title, content);
        NotificationManager notificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify(id, notification);

        return false;
    }

    @Override
    public boolean onStopJob(JobParameters params) {
        return false;
    }
}
