package org.Mengine.Base;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

public interface MenginePluginExtension {
    public boolean onInitialize(MengineActivity activity, MenginePlugin plugin);
    public void onFinalize(MengineActivity activity, MenginePlugin plugin);
    public void onRun(MengineActivity activity, MenginePlugin plugin);
}
