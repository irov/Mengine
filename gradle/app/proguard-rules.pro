-keep public class com.android.installreferrer.** { *; }

-keep public class * extends org.Mengine.Base.MenginePluginExtension {
    native <methods>;
    public <methods>;
}

-keep public class * extends org.Mengine.Base.MenginePlugin {
    native <methods>;
    public <methods>;
}

-keep public class org.Mengine.Base.MengineActivity {
    native <methods>;
    public <methods>;
}

-keep public class org.Mengine.Base.MengineApplication {
    native <methods>;
    public <methods>;
}