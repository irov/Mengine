package org.Mengine.Base;

import org.Mengine.Base.MengineActivity;

public interface MenginePluginExtension<T> {
    public boolean initialize(MengineActivity activity, T plugin);
    public void finalize(MengineActivity activity, T plugin);

    public void run(MengineActivity activity, T plugin);
}
