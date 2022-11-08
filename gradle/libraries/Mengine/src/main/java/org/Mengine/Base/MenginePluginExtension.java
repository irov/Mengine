package org.Mengine.Base;

public interface MenginePluginExtension {
    public boolean onInitialize(MengineActivity activity, MenginePlugin plugin);
    public void onFinalize(MengineActivity activity, MenginePlugin plugin);
    public void onRun(MengineActivity activity, MenginePlugin plugin);
}
