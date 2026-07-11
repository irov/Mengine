# Unix build

## Ubuntu/Debian prerequisites

```bash
sudo apt update
sudo apt install build-essential cmake git pkg-config python3-jinja2 libx11-dev libgl1-mesa-dev libasound2-dev libssl-dev
```

## Build

Run the scripts from any working directory:

```bash
bash build/downloads/downloads_unix.bash
bash build/unix/build_depends_unix_debug.bash
bash build/unix/build_solution_unix_debug.bash
```

Use the matching `*_release.bash` wrappers for a Release build.
