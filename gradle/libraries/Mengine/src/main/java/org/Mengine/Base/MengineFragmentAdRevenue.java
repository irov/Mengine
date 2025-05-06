package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineFragmentAdRevenue extends MengineFragment<MengineListenerAdRevenue> {
    public static MengineFragmentAdRevenue INSTANCE = null;

    MengineFragmentAdRevenue() {
        super(MengineListenerAdRevenue.class);

        INSTANCE = this;
    }

    public void adRevenue(@NonNull MengineParamAdRevenue revenue) {
        this.propagate(MengineListenerAdRevenue::onMengineAdRevenue, revenue);
    }
}