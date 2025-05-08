package org.Mengine.Base;

public class MengineFragmentEngine extends MengineFragment<MengineListenerEngine> {
    public static MengineFragmentEngine INSTANCE = null;

    protected boolean m_isMengineInitializeBaseServices = false;
    protected boolean m_isMengineCreateApplication = false;
    protected boolean m_isMenginePlatformRun = false;

    public boolean isMengineInitializeBaseServices() {
        return m_isMengineInitializeBaseServices;
    }

    public boolean isMengineCreateApplication() {
        return m_isMengineCreateApplication;
    }

    public boolean isMenginePlatformRun() {
        return m_isMenginePlatformRun;
    }

    MengineFragmentEngine() {
        super(MengineListenerEngine.class);

        INSTANCE = this;
    }

    public void initializeBaseServices() {
        m_isMengineInitializeBaseServices = true;

        this.propagate(MengineListenerEngine::onMengineInitializeBaseServices);
    }

    public void createApplication() {
        m_isMengineCreateApplication = true;

        this.propagate(MengineListenerEngine::onMengineCreateApplication);
    }

    public void platformRun() {
        m_isMenginePlatformRun = true;

        this.propagate(MengineListenerEngine::onMenginePlatformRun);
    }

    public void platformStop() {
        m_isMenginePlatformRun = false;

        this.propagate(MengineListenerEngine::onMenginePlatformStop);
    }

    public void caughtException(Throwable throwable) {
        String message = throwable.getMessage();
        MengineApplication.INSTANCE.setState("exception.message", message);

        String stacktrace = MengineUtils.getThrowableStackTrace(throwable);
        MengineApplication.INSTANCE.setState("exception.stacktrace", stacktrace);

        this.propagate(MengineListenerEngine::onMengineCaughtException, throwable);
    }
}
