#!/usr/bin/env python3

import argparse
import signal
import subprocess
import sys
import time
from pathlib import Path
from typing import Optional


def executable(root: Path, name: str, configuration: str, binary_directory: Optional[Path]) -> Path:
    suffix = ".exe" if sys.platform == "win32" else ""
    if binary_directory is not None:
        return binary_directory / f"{name}{suffix}"
    if sys.platform == "win32":
        candidates = sorted((root / "bin").glob(f"**/{configuration}/{name}{suffix}"))
        if candidates:
            return candidates[0]
        return root / "bin" / "Arena3D_WIN64_DX11" / "Visual_Studio_17_2022" / "x64" / configuration / f"{name}{suffix}"
    return root / "bin" / "Mengine_Xcode_MacOS" / "Xcode" / configuration / name


def main() -> int:
    parser = argparse.ArgumentParser(description="Run Arena3DServer and independent bot client processes")
    parser.add_argument("--bots", type=int, default=3)
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=27960)
    parser.add_argument("--ticks", type=int, default=6000, help="100 Hz server ticks; 0 runs until interrupted")
    parser.add_argument("--configuration", choices=("Debug", "Release"), default="Debug")
    parser.add_argument("--bin-dir", type=Path, help="directory containing Arena3DServer/BotClient/NetProxy")
    parser.add_argument("--latency-ms", type=int, default=0, help="one-way proxy latency per bot")
    parser.add_argument("--loss-permille", type=int, default=0)
    args = parser.parse_args()
    if not 0 <= args.bots <= 15 or not 1 <= args.port <= 65535:
        parser.error("bots must be 0..15 and port must be 1..65535")

    root = Path(__file__).resolve().parents[3]
    binary_directory = args.bin_dir.resolve() if args.bin_dir is not None else None
    server = executable(root, "Arena3DServer", args.configuration, binary_directory)
    bot = executable(root, "Arena3DBotClient", args.configuration, binary_directory)
    proxy = executable(root, "Arena3DNetProxy", args.configuration, binary_directory)
    required = [server, bot] + ([proxy] if args.latency_ms or args.loss_permille else [])
    missing = [str(path) for path in required if not path.is_file()]
    if missing:
        print("Missing Arena3D executable(s):", *missing, sep="\n  ", file=sys.stderr)
        return 2

    processes: list[subprocess.Popen] = []

    def launch(command: list[str]) -> subprocess.Popen:
        process = subprocess.Popen(command, cwd=root)
        processes.append(process)
        print(f"pid={process.pid}: {' '.join(command)}")
        return process

    def stop_all(*_unused: object) -> None:
        for process in reversed(processes):
            if process.poll() is None:
                process.terminate()

    signal.signal(signal.SIGINT, stop_all)
    signal.signal(signal.SIGTERM, stop_all)

    server_process = launch([
        str(server), "--host", args.host, "--port", str(args.port),
        "--max-players", "16", "--ticks", str(args.ticks),
    ])
    time.sleep(0.25)

    for index in range(args.bots):
        bot_port = args.port
        if args.latency_ms or args.loss_permille:
            bot_port = args.port + index + 1
            launch([
                str(proxy), "--listen-host", args.host, "--listen-port", str(bot_port),
                "--server-host", args.host, "--server-port", str(args.port),
                "--latency-ms", str(args.latency_ms), "--loss-permille", str(args.loss_permille),
            ])
        launch([
            str(bot), "--host", args.host, "--port", str(bot_port),
            "--name", f"bot{index + 1}", "--ticks", str(args.ticks),
        ])

    try:
        result = server_process.wait()
    finally:
        stop_all()
        for process in processes:
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
    return result


if __name__ == "__main__":
    raise SystemExit(main())
