# XlsExportPlugin

`XlsExportPlugin` runs a project data exporter before the application is created. It is intended for desktop development builds; packaged mobile applications must consume the generated data and must not contact the source service at runtime.

Both desktop platforms provide `FindPython3ServiceInterface` under the service ID `FindPython3Service`. `Win32FindPython3Service` and `MacOSFindPython3Service` implement the same `getPython3ExecutablePath` method, which returns the executable path in UTF-8.

The plugin calls `PlatformServiceInterface::createProcess` with UTF-8 `ArgumentStrings`: up to `MENGINE_MAX_PROCESS_ARGUMENTS` (128 by default), each at most `MENGINE_MAX_PATH` bytes. Argument storage is fixed. Windows handles Unicode conversion and command-line quoting in its process helper; macOS passes an argument array to the POSIX process helper. The plugin contains no platform-specific branches.

The development command is:

```text
python3 XlsxExport/xlsxExporter.py <ProjectCodename>
```

The exporter must return zero only when it produced fresh valid data or deliberately accepted a previously validated cache. A non-zero result stops application bootstrap.

## macOS setup

Enable both plugins from the external project's `MENGINE_PREPARE_FUNCTION`:

```cmake
ENABLE_PLUGIN_DEBUG(MENGINE_PLUGIN_MACOS_FINDPYTHON3)
ENABLE_PLUGIN_DEBUG(MENGINE_PLUGIN_XLSEXPORT)
```

The launcher must set the current working directory to the project root. The child process inherits that directory, so the fixed script path is relative to it. `PlatformServiceInterface::getCurrentPath` is deliberately not used: on macOS it points to bundle resources. No Python path is supplied by CMake. `ENABLE_PLUGIN_DEBUG` keeps Python discovery, credentials, and source-network access out of Release runtime.

The common `FindPython3ServiceInterface` is implemented on macOS by `MacOSFindPython3Service`. It requires Python 3.10 or newer and checks candidates in this order:

1. project-local `<cwd>/.runtime/python/bin/python3`, `.venv/bin/python3`, and `venv/bin/python3`;
2. Homebrew paths under `/opt/homebrew` and `/usr/local`;
3. `Python.framework` Current and numeric versions;
4. the result of `xcrun --find python3`;
5. `/usr/bin/python3`.

The finder service remains available with an empty executable path when no valid interpreter exists. An enabled exporter treats that empty path as a startup error; `--nodataexport` remains a usable escape hatch on machines without Python.

## Runtime controls

`--nodataexport` disables the plugin; legacy `--noxlsexport` is also accepted.

The exporter chooses its output location. The plugin adds `--strict` when `Helper::isDevelopmentMode()` is false.

Development fallback behavior belongs to the exporter. The plugin only accepts its successful exit code, so cached data must be validated by the same schema before the exporter returns zero.

## Release and CI

Release packaging must consume generated data and must not enable this runtime plugin. The project build pipeline is responsible for running the same exporter with `--strict` before packaging and failing the build on any non-zero exit code.

```text
python3 XlsxExport/xlsxExporter.py <ProjectCodename> --strict
```

## Windows setup and migration

For development-only Windows export, enable `MENGINE_PLUGIN_WIN32_FINDPYTHON3` and `MENGINE_PLUGIN_XLSEXPORT` with `ENABLE_PLUGIN_DEBUG`; disable their general defaults first if the selected Windows preset enables them in Release.

`XlsExportPlugin` replaces both `Win32XlsExportPlugin` and `DevelopmentDataExportPlugin`. Use `MENGINE_PLUGIN_XLSEXPORT` in project prepare files with `ENABLE_PLUGIN`, `DISABLE_PLUGIN`, or their Debug variants.

Projects that explicitly list plugin DLLs must replace `Win32XlsExportPlugin.dll` or `DevelopmentDataExportPlugin.dll` with `XlsExportPlugin.dll`. The plugin name is `XlsExport`.

The project codename remains positional for compatibility with existing XLS exporters. Exporters must support `--strict` outside development mode. Both disable options remain supported.
