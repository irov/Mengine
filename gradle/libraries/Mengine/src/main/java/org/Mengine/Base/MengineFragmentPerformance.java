package org.Mengine.Base;

import java.util.Map;

public class MengineFragmentPerformance extends MengineFragment<MengineListenerPerformance> {
    public static MengineFragmentPerformance INSTANCE = null;

    MengineFragmentPerformance() {
        super(MengineListenerPerformance.class);

        INSTANCE = this;
    }

    public MenginePerformanceTrace startPerformanceTrace(String name, Map<String, String> attributes) {
        MenginePerformanceTrace trace = new MenginePerformanceTrace();

        this.propagate(MengineListenerPerformance::onMenginePerformanceTraceStart, trace, name, attributes);

        return trace;
    }
}
