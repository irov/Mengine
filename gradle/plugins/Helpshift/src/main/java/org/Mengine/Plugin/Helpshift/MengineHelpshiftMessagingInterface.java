package org.Mengine.Plugin.Helpshift;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

public interface MengineHelpshiftMessagingInterface {
    boolean initializeMessaging(MenginePlugin plugin, MengineActivity activity);
    void finalizeMessaging(MenginePlugin plugin);
}
