# Local notifications

Android reads `res/xml/local_notifications.xml`. iOS reads a root array from
`local_notifications.plist` in the application bundle, not from `Info.plist`.
The iOS plugin adds this file to application resources when it exists at the root
of `MENGINE_DEPLOY_PATH` during CMake configuration. Files elsewhere can use
`APPLICATION_APPLE_RESOURCES`.

Both platforms use the same notification fields and scheduling rules:

| Field | Value |
| --- | --- |
| `id` | Required unique positive signed 32-bit integer. |
| `title`, `content` | Required native localization keys: Android string resource names or iOS `Localizable.strings` keys. |
| `delay` | Positive integer delay in seconds after the application enters the background. |
| `month`, `day` | Calendar date in the local Gregorian calendar. |
| `hour`, `minute` | Calendar time; both default to zero. |
| `script` | Boolean; use a deadline supplied by `setNotificationDelay(name, seconds)`. Missing or expired deadlines are skipped. |
| `name` | Required nonempty string for a script notification. |
| `repeat` | `once` (default) or `yearly`; `yearly` is valid only for calendar notifications. |
| `languages` | Optional comma-separated language codes, for example `en,ru`. Whitespace around codes is ignored. |

Specify exactly one trigger: `delay`, calendar `month`/`day`, or `script=true`.
A calendar notification uses its next future occurrence. `once` delivers only that
occurrence for the current schedule; `yearly` also schedules subsequent years
without requiring another application launch. February 29 skips non-leap years.
Returning to the foreground cancels pending local notifications; entering the
background builds a new schedule. An iOS launch in the background (a silent push
or a background fetch) keeps the current schedule.

The notification permission is requested from launch N on, where a launch is a
start of the game by the user. A value of zero or less never requests it on launch.

| Platform | Setting | Default |
| --- | --- | --- |
| Android | `integer/mengine_local_notifications_request_launch` resource | `1`, the first launch |
| iOS | `RequestAuthorizationLaunch` in the `MengineiOSUserNotificationCenterPlugin` dictionary of `Info.plist` | `0`, never; scripts call `iOSUserNotificationCenterRequestAuthorization` |

Debug builds calculate the real next occurrence from the current moment for every eligible notification,
sort by that time (then by `id` for ties), and schedule a one-shot preview at 5,
10, 15, ... seconds. Yearly notifications appear once in each preview. Release
builds use the real delays and calendar dates. `DebugDelay`, `debug_delay` and
automatic delay division are no longer used. Delivery remains subject to OS background
scheduling and notification settings.

Scripts use these calls:

| Action | Android (`LNotifications` plugin) | iOS |
| --- | --- | --- |
| Request the permission | `requestPermission()`; the result arrives in `onLocalNotificationsPermission(granted)` | `iOSUserNotificationCenterRequestAuthorization(cb, *args)` calls `cb(granted, *args)` |
| Set a script deadline | `setNotificationDelay(name, seconds)`, integer or float seconds | `iOSUserNotificationCenterSetNotificationDelay(name, seconds)`, integer or float seconds |
| Notification tap | `onLocalNotificationsPress(id)` | not reported |

The iOS plist uses the same lowercase keys as Android XML attributes. Boolean and
integer values use native plist types; `languages` is a comma-separated string on
both platforms. To migrate a `Notifications` array from the plugin dictionary in
`Info.plist`, move its entries to `local_notifications.plist`, replace `Identifier`
with a numeric `id` plus `name` for script entries, rename `Title` to `title` and
`Body` to `content`, join the `Languages` array into a comma-separated `languages`
string, and lowercase the remaining keys. Keep `RequestAuthorizationLaunch` in the
plugin settings in `Info.plist`.
