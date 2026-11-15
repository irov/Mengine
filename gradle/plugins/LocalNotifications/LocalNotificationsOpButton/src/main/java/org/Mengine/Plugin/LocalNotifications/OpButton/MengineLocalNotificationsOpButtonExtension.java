package org.Mengine.Plugin.LocalNotifications.OpButton;

import android.content.res.Resources;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePluginExtensionInterface;
import org.Mengine.Base.MenginePluginExtensionManager;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Plugin.LocalNotifications.MengineLocalNotificationsPlugin;

import org.opbutton.sdk.OpButton;
import org.opbutton.sdk.OpButtonButton;
import org.opbutton.sdk.OpButtonLayout;
import org.opbutton.sdk.OpButtonTab;
import org.opbutton.sdk.OpButtonUI;

public class MengineLocalNotificationsOpButtonExtension implements MenginePluginExtensionInterface {
    private OpButtonButton m_queueButton;

    static {
        MenginePluginExtensionManager.registerExtension("OpButton", MengineLocalNotificationsOpButtonExtension.class);
    }

    @Override
    public void onExtensionInitialize(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        if (OpButton.isInitialized() == false) {
            return;
        }

        Resources resources = application.getResources();
        String packageName = application.getPackageName();
        int resourceId = resources.getIdentifier("mengine_opbutton_notifications_interval_seconds", "integer", packageName);

        if (resourceId == 0) {
            return;
        }

        int intervalSeconds = resources.getInteger(resourceId);

        if (intervalSeconds <= 0) {
            return;
        }

        MengineLocalNotificationsPlugin notifications = application.getService(MengineLocalNotificationsPlugin.class);

        if (notifications == null) {
            throw new MengineServiceInvalidInitializeException("LocalNotifications service not found");
        }

        OpButtonUI ui = OpButton.getUI();
        OpButtonTab tab = ui.addTab("Notifications");
        tab.setId("Notifications");
        OpButtonLayout layout = tab.vertical();
        String title = "Queue notifications every " + intervalSeconds + " seconds";
        m_queueButton = layout.addButton(title);
        m_queueButton.setId("notification_queue");
        m_queueButton.onClick(action -> notifications.queueNotifications(intervalSeconds));
    }

    @Override
    public void onExtensionFinalize(@NonNull MengineApplication application) {
        if (m_queueButton == null) {
            return;
        }

        m_queueButton.onClick(null);
        m_queueButton = null;

        if (OpButton.isInitialized() == false) {
            return;
        }

        OpButtonUI ui = OpButton.getUI();
        ui.removeTab("Notifications");
    }
}
