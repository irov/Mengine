# iOSVibratorPlugin

Enable with `ENABLE_PLUGIN(MENGINE_PLUGIN_IOS_VIBRATOR)` in the iOS deploy
configuration. The plugin owns a single `CHHapticEngine` and plays one transient
haptic event per impact. The engine is created and started on the first prepare
or impact request and keeps running until the game mutes it, the application
enters background, or the plugin stops. UIKit state is read and the engine is
driven on the main queue.

With script embedding enabled, the Python API is:

```python
if Mengine.isAvailablePlugin("iOSVibratorPlugin"):
    Mengine.iOSVibratorPrepare()            # Prepare when a drag begins.
    # Later, when the item is dropped:
    Mengine.iOSVibratorImpact(0.5, 0.7)
    Mengine.iOSVibratorMute(True)
    muted = Mengine.iOSVibratorIsMute()
```

An impact takes the two Core Haptics parameters of its event, each between `0.0`
and `1.0`: intensity is how strong the pulse feels, sharpness how crisp it is —
low values feel round and dull, high values feel precise and clicky. The game
owns the table of its own effects; the plugin adds no styles of its own.

Preparation is optional and takes no arguments: Core Haptics prepares the
hardware by running the engine, which then serves every impact. Prepare ahead of
an isolated impact anyway — starting the engine on the impact itself adds its
startup to the first pulse.

The engine plays haptics only and never mixes into the game's audio session. It
restarts itself after a haptic server reset. Requests are ignored while muted,
inactive, stopped, or on hardware without haptics support; engine, pattern, and
player failures are logged as errors, and an inactive application as a warning.
The plugin starts unmuted; persist the player's preference in the game settings.

Simulator or API-call success does not confirm physical feedback on a device.
