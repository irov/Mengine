# Room creates WorkManager database implementations through reflection.
-keep class * extends androidx.room.RoomDatabase {
    public <init>();
}

# WorkManager creates input mergers through reflection.
-keep class * extends androidx.work.InputMerger {
    public <init>();
}
