#!/usr/bin/env python3
"""Exercise a real executable upgrade using isolated sockets and PTYs."""

import fcntl
import os
from pathlib import Path
import pty
import select
import shlex
import shutil
import signal
import struct
import subprocess
import tempfile
import termios
import time


BINARY = Path(os.environ.get("TEST_TMUX", Path(__file__).resolve().parent.parent / "tmux")).resolve()


def wait_for(predicate, message, timeout=10):
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if predicate():
            return
        time.sleep(0.02)
    raise AssertionError(message)


class Server:
    def __init__(self, root, invocation="absolute", config=""):
        self.root = root
        self.root.mkdir()
        self.old = root / "old-tmux"
        self.new = root / "new-tmux"
        shutil.copy2(BINARY, self.old)
        shutil.copy2(BINARY, self.new)
        self.exports = root / "exports"
        self.directory_link = invocation == "directory"
        self.generation = 0
        if self.directory_link:
            initial = root / "exports-0"
            initial.mkdir()
            (initial / "tmux").symlink_to(self.old)
            self.exports.symlink_to(initial)
        else:
            self.exports.mkdir()
            (self.exports / "tmux").symlink_to(self.old)
        self.link = self.exports / "tmux"
        self.socket = str(root / "socket")
        self.env = dict(os.environ, TERM="xterm-256color", LC_ALL="C.UTF-8")
        self.env.pop("TMUX", None)
        self.env.pop("TMUX_PANE", None)
        self.env["PATH"] = str(self.exports) + os.pathsep + self.env.get("PATH", "/bin:/usr/bin")
        self.clients = []
        self.config_files = []
        for i, content in enumerate([config] if isinstance(config, str) else config):
            path = root / f"config-{i}.conf"
            path.write_text(content)
            self.config_files.append(path)
        self.config = self.config_files[0]
        if invocation == "path":
            program = "tmux"
        elif invocation == "relative":
            program = "exports/tmux"
        else:
            program = str(self.link)
        config_args = [arg for path in self.config_files
                       for arg in ("-f", path.name)]
        self.run(*config_args, "new-session", "-d", "-s", "one", "-x", "90", "-y", "28",
                 "exec /bin/sh", program=program)
        self.pid = int(self.run("display-message", "-p", "#{pid}"))

    def run(self, *args, program=None, check=True, timeout=12):
        result = subprocess.run([program or str(BINARY), "-S", self.socket, *args],
                                env=self.env, cwd=self.root, capture_output=True,
                                text=True, timeout=timeout)
        if check and result.returncode:
            raise AssertionError(f"{args}: {result.stderr.strip()}")
        return result.stdout.strip() if check else result

    def point_to(self, path):
        temporary = self.root / "next-link"
        if self.directory_link:
            self.generation += 1
            directory = self.root / f"exports-{self.generation}"
            directory.mkdir()
            (directory / "tmux").symlink_to(path)
            temporary.symlink_to(directory)
            temporary.replace(self.exports)
        else:
            temporary.symlink_to(path)
            temporary.replace(self.link)

    def attach(self, session, control=False):
        master, slave = pty.openpty()
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 28, 90, 0, 0))
        args = [str(self.link), "-S", self.socket]
        if control:
            args.append("-CC" if control == "double" else "-C")
        args.extend(["attach-session", "-t", session])

        def terminal():
            os.setsid()
            fcntl.ioctl(0, termios.TIOCSCTTY, 0)

        process = subprocess.Popen(args, env=self.env, stdin=slave, stdout=slave,
                                   stderr=slave, preexec_fn=terminal, cwd=self.root)
        os.close(slave)
        os.set_blocking(master, False)
        self.clients.append((process, master))
        wait_for(lambda: str(process.pid) in self.run("list-clients", "-F", "#{client_pid}").splitlines(),
                 "client did not attach")
        return process, master

    def drain(self):
        for _, fd in self.clients:
            while select.select([fd], [], [], 0)[0]:
                try:
                    if not os.read(fd, 65536):
                        break
                except (BlockingIOError, OSError):
                    break

    def snapshot(self):
        return self.run("list-panes", "-a", "-F",
                        "#{pid} #{session_id} #{session_name} #{window_index} #{window_id} "
                        "#{pane_id} #{pane_pid} #{pane_width} #{pane_height} #{pane_dead}")

    def executable_is(self, pid, path):
        proc = Path(f"/proc/{pid}/exe")
        if not Path("/proc/self/exe").exists():
            return True
        try:
            return os.path.samefile(proc, path)
        except FileNotFoundError:
            return False

    def cleanup(self):
        self.run("kill-server", check=False)
        for process, fd in self.clients:
            if process.poll() is None:
                process.terminate()
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
            os.close(fd)


def upgrade(root, invocation):
    s = Server(root, invocation, config="""
set-option -g status off
set-option -g window-size manual
set-option -g @restart-test "global value with 'quotes'"
set-option -s command-alias[restart-test] 'display-message OK'
bind-key -T restart-test x display-message 'bound command'
set-hook -g after-new-window 'set-option -g @hook survived'
""")
    try:
        s.run("set-environment", "-g", "RESTART_ENV", "global environment")
        s.run("set-environment", "-t", "one", "RESTART_LOCAL", "session environment")
        s.run("set-option", "-t", "one", "@local", "local value")
        s.run("set-buffer", "-b", "restart-test", "buffer\nwith unicode: 日本語")
        s.run("set-buffer", "automatic-one")
        s.run("set-buffer", "automatic-two")
        binary_data = b"\x00\x1b\xff" + "日本語".encode()
        binary_file = root / "binary-buffer"
        binary_file.write_bytes(binary_data)
        s.run("load-buffer", "-b", "binary", str(binary_file))
        buffers = s.run("list-buffers", "-F", "#{buffer_name} #{buffer_created} #{buffer_size}")
        top_buffer = s.run("show-buffer")
        s.run("split-window", "-h", "-d", "-t", "one", "exec /bin/sh")
        s.run("new-session", "-d", "-s", "two", "exec /bin/sh")
        s.run("new-session", "-d", "-s", "grouped", "-t", "one")
        s.run("link-window", "-s", "two:0", "-t", "one:7")
        s.run("select-window", "-t", "one:0")
        s.run("send-keys", "-t", "one:0.0",
              "i=0; while [ $i -lt 60 ]; do printf 'HISTORY_%s_日本語\\n' \"$i\"; "
              "i=$((i+1)); done; printf 'SCROLLBACK_日本語\\n'; printf '\\033[31mRED\\033[0m\\n'", "Enter")
        wait_for(lambda: "SCROLLBACK_日本語" in s.run("capture-pane", "-p", "-t", "one:0.0").splitlines(),
                 "pane did not produce output")
        normal, terminal = s.attach("one")
        control, _ = s.attach("two", control=True)
        double, _ = s.attach("two", control="double")
        s.run("set-option", "-t", "one", "destroy-unattached", "on")
        s.run("set-option", "-s", "exit-unattached", "on")
        with s.config.open("a") as f:
            f.write("set-option -s exit-unattached on\n")
        before = s.snapshot()
        layout = s.run("list-windows", "-a", "-F", "#{session_id} #{window_id} #{window_layout}")
        contents = s.run("capture-pane", "-pe", "-t", "one:0.0", "-S", "-")
        processes = [int(line.split()[6]) for line in before.splitlines()]
        starts = {pid: Path(f"/proc/{pid}/stat").read_text().split()[21]
                  for pid in set(processes) if Path(f"/proc/{pid}/stat").exists()}
        s.point_to(s.new)
        s.run("restart-server")
        wait_for(lambda: s.executable_is(s.pid, s.new) and
                 all(s.executable_is(c.pid, s.new) for c in (normal, control, double)) and
                 len(s.run("list-clients", "-F", "#{client_pid}").splitlines()) == 3,
                 "server or clients did not exec the new binary")
        assert all(c.poll() is None for c in (normal, control, double))
        assert before == s.snapshot(), "sessions or pane processes changed"
        assert layout == s.run("list-windows", "-a", "-F", "#{session_id} #{window_id} #{window_layout}")
        assert contents == s.run("capture-pane", "-pe", "-t", "one:0.0", "-S", "-")
        for pid, start in starts.items():
            assert Path(f"/proc/{pid}/stat").read_text().split()[21] == start
        assert s.run("show-option", "-gqv", "@restart-test") == "global value with 'quotes'"
        assert s.run("show-option", "-t", "one", "-qv", "@local") == "local value"
        assert "global environment" in s.run("show-environment", "-g", "RESTART_ENV")
        assert "session environment" in s.run("show-environment", "-t", "one", "RESTART_LOCAL")
        assert "bound command" in s.run("list-keys", "-T", "restart-test")
        assert s.run("show-buffer", "-b", "restart-test") == "buffer\nwith unicode: 日本語"
        assert buffers == s.run("list-buffers", "-F", "#{buffer_name} #{buffer_created} #{buffer_size}")
        assert top_buffer == s.run("show-buffer")
        binary_file.unlink()
        s.run("save-buffer", "-b", "binary", str(binary_file))
        assert binary_file.read_bytes() == binary_data
        s.run("set-buffer", "automatic-three")
        assert s.run("show-buffer", "-b", "buffer2") == "automatic-three"
        s.run("new-window", "-d", "-t", "two")
        assert s.run("show-option", "-gqv", "@hook") == "survived"
        # The new process must retain the logical path for the next upgrade.
        s.point_to(s.old)
        s.drain()
        s.run("restart-server")
        wait_for(lambda: all(s.executable_is(pid, s.old) for pid in (s.pid, normal.pid, control.pid, double.pid)),
                 "second restart lost the logical executable path")
        assert all(c.poll() is None for c in (normal, control, double))
        s.run("bind-key", "-n", "F12", "restart-server")
        s.point_to(s.new)
        s.drain()
        os.write(terminal, b"\x1b[24~")
        wait_for(lambda: all(s.executable_is(pid, s.new)
                            for pid in (s.pid, normal.pid, control.pid, double.pid)),
                 "restart from a key binding failed")
        s.run("send-keys", "-t", "one:0.0", "printf 'AFTER_RESTART\\n'", "Enter")
        wait_for(lambda: "AFTER_RESTART" in s.run("capture-pane", "-p", "-t", "one:0.0"),
                 "pane stopped accepting input")
        print(f"PASS upgrade ({invocation}): server/client PIDs, panes, sessions, state, repeated exec")
    finally:
        s.cleanup()


def bad_binary(root):
    s = Server(root)
    try:
        client, _ = s.attach("one")
        before = s.snapshot()
        bad = root / "bad"
        incompatible = root / "incompatible"
        image = BINARY.read_bytes()
        assert b"tmux restart" in image
        incompatible.write_bytes(image.replace(b"tmux restart", b"oops restart"))
        incompatible.chmod(0o755)
        cases = [(incompatible, "incompatible checkpoint format"),
                 (Path("/bin/true"), "success without handshake"),
                 (Path("/bin/false"), "failed process")]
        bad.write_text("not an executable\n")
        bad.chmod(0o755)
        cases += [(bad, "invalid executable"), (root / "missing", "missing executable")]
        for path, label in cases:
            s.point_to(path)
            result = s.run("restart-server", check=False)
            assert result.returncode != 0, label
            assert "sanity check" in result.stderr or "cannot open" in result.stderr, result.stderr
            assert before == s.snapshot(), label
            assert client.poll() is None and s.executable_is(s.pid, s.old), label
        # A candidate that never exits must not block the server indefinitely.
        bad.write_text("#!/bin/sh\nexec sleep 30\n")
        s.point_to(bad)
        start = time.monotonic()
        result = s.run("restart-server", check=False)
        assert result.returncode != 0 and time.monotonic() - start < 8
        assert before == s.snapshot() and client.poll() is None
        # Even an exec failure after a successful probe must restore the tty.
        if Path("/dev/fd").exists():
            bad.write_text('#!/bin/sh\nprintf R > "/dev/fd/$3"\nchmod 600 "$0"\n')
            bad.chmod(0o755)
            result = s.run("restart-server", check=False)
            assert result.returncode != 0 and "restart exec" in result.stderr, result.stderr
            assert before == s.snapshot() and client.poll() is None
        s.point_to(s.new)
        s.drain()
        s.run("restart-server")
        wait_for(lambda: s.executable_is(client.pid, s.new), "healthy retry failed")
        print("PASS bad binaries: incompatible, no-op, failing, invalid, missing, timeout, exec rollback; healthy retry")
    finally:
        s.cleanup()


def state_edges(root):
    s = Server(root, config="""
set-option -g status off
set-option -g window-size manual
set-option -g remain-on-exit on
""")
    pipe = None
    try:
        s.run("split-window", "-h", "-d", "-t", "one", "exec /bin/sh")
        s.run("select-pane", "-m", "-t", "one:0.1")
        s.run("resize-pane", "-Z", "-t", "one:0.1")
        visible = s.run("display-message", "-p", "-t", "one", "#{window_visible_layout}")
        layout = s.run("display-message", "-p", "-t", "one", "#{window_layout}")
        dead = s.run("new-window", "-dP", "-F", "#{pane_id}", "exit 7")
        wait_for(lambda: s.run("display-message", "-p", "-t", dead, "#{pane_dead}") == "1",
                 "dead pane did not exit")
        s.run("wait-for", "-L", "restart-lock")
        s.run("wait-for", "-S", "restart-signal")
        s.run("bind-key", "C-b", "display-message", "overridden default")
        fifo = root / "output"
        os.mkfifo(fifo)
        pipe = os.open(fifo, os.O_RDWR | os.O_NONBLOCK)
        script = root / "writer.py"
        script.write_text("import os,sys\nf=open(sys.argv[1])\nfor line in f:\n os.write(1,bytes.fromhex(line.strip()))\n")
        command = "exec " + shlex.join([shutil.which("python3"), "-u", str(script), str(fifo)])
        raw = s.run("new-window", "-dP", "-F", "#{pane_id}", command)
        os.write(pipe, b"4e4f524d414c\n")
        wait_for(lambda: "NORMAL" in s.run("capture-pane", "-p", "-t", raw), "raw pane did not start")
        os.write(pipe, (b"\x1b[?1049hALT_SCREEN\x1b[31".hex() + "\n").encode())
        wait_for(lambda: "ALT_SCREEN" in s.run("capture-pane", "-p", "-t", raw), "alternate screen not entered")
        before = s.snapshot()
        s.point_to(s.new)
        s.run("restart-server")
        assert before == s.snapshot()
        assert visible == s.run("display-message", "-p", "-t", "one", "#{window_visible_layout}")
        assert layout == s.run("display-message", "-p", "-t", "one", "#{window_layout}")
        assert s.run("display-message", "-p", "-t", "one:0.1", "#{pane_marked}") == "1"
        assert s.run("display-message", "-p", "-t", dead, "#{pane_dead_status}") == "7"
        s.run("wait-for", "-U", "restart-lock")
        s.run("wait-for", "restart-signal", timeout=2)
        assert "send-prefix" in s.run("list-keys", "C-b")
        # Complete the escape sequence that straddled the exec boundary.
        os.write(pipe, (b"mRED\x1b[0m".hex() + "\n").encode())
        wait_for(lambda: "RED" in s.run("capture-pane", "-pe", "-t", raw), "partial CSI was lost")
        assert "\x1b[31mRED" in s.run("capture-pane", "-pe", "-t", raw)
        os.write(pipe, (b"\x1b[?1049l".hex() + "\n").encode())
        wait_for(lambda: "NORMAL" in s.run("capture-pane", "-p", "-t", raw), "saved normal screen was lost")
        # A partial UTF-8 character must survive independently of the parser state.
        os.write(pipe, (b"\xe6\x97".hex() + "\n").encode())
        wait_for(lambda: "NORMAL" in s.run("capture-pane", "-p", "-t", raw), "normal screen missing")
        s.point_to(s.old)
        s.run("restart-server")
        os.write(pipe, (b"\xa5".hex() + "\n").encode())
        wait_for(lambda: "日" in s.run("capture-pane", "-p", "-t", raw), "partial UTF-8 was lost")
        s.run("resize-pane", "-Z", "-t", "one:0.1")
        assert s.run("display-message", "-p", "-t", "one", "#{window_zoomed_flag}") == "0"
        s.run("send-keys", "-t", "one:0.0", "exit 9", "Enter")
        wait_for(lambda: s.run("display-message", "-p", "-t", "one:0.0", "#{pane_dead}") == "1",
                 "reexecuted server did not reap its original child")
        assert s.run("display-message", "-p", "-t", "one:0.0", "#{pane_dead_status}") == "9"
        s.run("new-session", "-d", "-s", "later", "exec /bin/sh")
        ids = s.run("list-panes", "-a", "-F", "#{pane_id}").splitlines()
        assert len(ids) == len(set(ids)), "pane IDs were reused"
        print("PASS state edges: zoom, mark, dead pane, key defaults, wait channels, alternate screen, partial CSI/UTF-8")
    finally:
        if pipe is not None:
            os.close(pipe)
        s.cleanup()


def reload_config(root):
    marker = root / "loads"
    s = Server(root, config=("""
set-option -g status off
set-option -g window-size manual
set-option -s escape-time 10
set-option -g mouse on
set-option -g @removed old
bind-key -T reload-test x display-message old
set-hook -g after-new-window 'set-option -g @old-hook yes'
""", f"run-shell {shlex.quote('printf x >> ' + shlex.quote(str(marker)))}\n"))
    try:
        s.run("set-option", "-t", "one", "@session-local", "session")
        s.run("set-option", "-t", "one", "status", "off")
        s.run("set-option", "-t", "one", "@targeted", "old")
        s.run("set-option", "-w", "-t", "one:0", "@window-local", "window")
        s.run("set-option", "-p", "-t", "one:0.0", "@pane-local", "pane")
        s.run("set-hook", "-t", "one", "after-new-window[local]",
              "set-option -t one @local-hook yes")
        s.run("bind-key", "C-b", "display-message", "overridden default")
        s.run("bind-key", "-T", "discard-test", "x", "display-message", "discarded")
        s.run("set-option", "-s", "command-alias[discard-test]", "discard=display-message old")
        client, _ = s.attach("one")
        before = s.snapshot()
        s.config.write_text("""
set-option -g status on
set-option -g window-size manual
set-option -s escape-time 20
set-option -g mouse off
set-option -g @config-version new
set-option -t one @targeted new
bind-key -T reload-test y display-message new
set-hook -g client-attached 'set-option -gF @attached-config "#{@config-version}"'
""")
        nested = shlex.join([str(s.link), "-S", s.socket, "set-option", "-g", "@nested", "loaded"])
        # Keep configuration active past the client-cleanup timer interval.
        shell = f"sleep 1.1; printf x >> {shlex.quote(str(marker))}; exec {nested}"
        s.config_files[1].write_text(f"run-shell {shlex.quote(shell)}\n")
        s.point_to(s.new)
        for generation in range(2):
            s.drain()
            s.run("restart-server")
            wait_for(lambda: str(client.pid) in s.run("list-clients", "-F", "#{client_pid}").splitlines(),
                     "client did not reattach after config reload")
            assert marker.read_text() == "x" * (generation + 2), "probe executed configuration"
            assert s.run("show-option", "-gqv", "@nested") == "loaded", "nested config client blocked"
            assert s.run("show-option", "-sqv", "escape-time") == "20"
            assert s.run("show-option", "-gqv", "mouse") == "off"
            assert s.run("show-option", "-gqv", "status") == "on"
            assert s.run("show-option", "-gqv", "@removed") == ""
            assert "discard=display-message old" not in s.run("show-option", "-sv", "command-alias")
            assert "@old-hook" not in s.run("show-hooks", "-g")
            assert s.run("list-keys", "-T", "discard-test", check=False).returncode != 0
            bindings = s.run("list-keys", "-T", "reload-test")
            assert "display-message new" in bindings and "display-message old" not in bindings
            assert "send-prefix" in s.run("list-keys", "C-b")
            assert s.run("show-option", "-gqv", "@attached-config") == "new"
            assert s.run("show-option", "-t", "one", "-qv", "@session-local") == "session"
            assert s.run("show-option", "-t", "one", "-qv", "status") == "off"
            assert s.run("show-option", "-t", "one", "-qv", "@targeted") == "new"
            assert s.run("show-option", "-w", "-t", "one:0", "-qv", "@window-local") == "window"
            assert s.run("show-option", "-p", "-t", "one:0.0", "-qv", "@pane-local") == "pane"
            assert before == s.snapshot(), "config reload changed runtime state"
        s.run("new-window", "-d", "-t", "one")
        assert s.run("show-option", "-t", "one", "-qv", "@local-hook") == "yes"
        print("PASS config reload: original -f files, fresh globals/bindings/hooks/aliases, local overrides, probe isolation")
    finally:
        s.cleanup()


def control_sizes(root):
    s = Server(root, config="set-option -g status off\nset-option -g window-size smallest\n")
    try:
        client, _ = s.attach("one", control=True)
        name = s.run("list-clients", "-F", "#{client_name}")
        s.run("refresh-client", "-t", name, "-C", "140x50")
        window = s.run("new-window", "-dP", "-F", "#{window_id}")
        s.run("refresh-client", "-t", name, "-C", f"{window}:100x40")
        before = s.run("list-windows", "-F", "#{window_id} #{window_width}x#{window_height}")
        assert "100x40" in before
        s.point_to(s.new)
        s.run("restart-server")
        wait_for(lambda: s.executable_is(client.pid, s.new) and bool(s.run("list-clients")),
                 "control client did not reattach")
        assert before == s.run("list-windows", "-F", "#{window_id} #{window_width}x#{window_height}")
        assert s.run("new-window", "-dP", "-F", "#{window_width}x#{window_height}") == "140x50"
        print("PASS control sizing: client dimensions and per-window overrides")
    finally:
        s.cleanup()


def silence_monitor(root):
    s = Server(root)
    try:
        s.run("new-window", "-d", "-n", "quiet", "exec sleep 60")
        s.run("set-window-option", "-t", "one:quiet", "monitor-silence", "1")
        s.run("restart-server")
        wait_for(lambda: s.run("display-message", "-p", "-t", "one:quiet", "#{window_silence_flag}") == "1",
                 "window-local silence monitor was not rearmed", timeout=4)
        print("PASS silence monitoring: restored window-local timer")
    finally:
        s.cleanup()


def config_error(root):
    s = Server(root)
    try:
        client, terminal = s.attach("one", control=True)
        s.config.write_text("not-a-tmux-command\n")
        s.point_to(s.new)
        s.run("restart-server")
        output = bytearray()

        def reported():
            while select.select([terminal], [], [], 0)[0]:
                try:
                    chunk = os.read(terminal, 65536)
                except BlockingIOError:
                    break
                if not chunk:
                    break
                output.extend(chunk)
            return b"%config-error" in output and b"unknown command" in output

        wait_for(reported, "configuration error was not reported after handoff")
        assert client.poll() is None
        assert int(s.run("display-message", "-p", "#{pid}")) == s.pid
        s.config.write_text("set-option -g @recovered yes\n")
        s.run("restart-server")
        assert s.run("show-option", "-gqv", "@recovered") == "yes"
        print("PASS configuration errors: report after handoff, retain processes, recover on next reload")
    finally:
        s.cleanup()


def foreground_server(root):
    root.mkdir()
    socket = str(root / "socket")
    command = [str(BINARY), "-S", socket]
    server = subprocess.Popen([*command, "-D", "-f", "/dev/null"],
                              stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    def run(*args):
        return subprocess.check_output([*command, *args], text=True, timeout=12).strip()

    try:
        wait_for(lambda: Path(socket).exists(), "foreground server did not start")
        run("new-session", "-d", "-s", "one", "exec sleep 60")
        run("restart-server")
        assert run("show-option", "-sqv", "exit-empty") == "off"
        run("kill-session", "-t", "one")
        assert int(run("display-message", "-p", "#{pid}")) == server.pid
        assert server.poll() is None
        print("PASS foreground server: retain -D defaults after config reset")
    finally:
        subprocess.run([*command, "kill-server"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        try:
            server.wait(timeout=3)
        except subprocess.TimeoutExpired:
            server.kill()
            server.wait()


def busy_server(root):
    s = Server(root)
    try:
        before = s.snapshot()
        result = s.run("restart-server", ";", "display-message", "-p", "NOT-RUN", check=False)
        assert result.returncode != 0 and "must run alone" in result.stderr
        assert "NOT-RUN" not in result.stdout
        s.run("copy-mode", "-t", "one")
        result = s.run("restart-server", check=False)
        assert result.returncode != 0 and "busy" in result.stderr
        assert before == s.snapshot()
        s.run("send-keys", "-X", "-t", "one", "cancel")
        waiter = subprocess.Popen([str(BINARY), "-S", s.socket, "wait-for", "pending"], env=s.env,
                                  stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        try:
            wait_for(lambda: str(waiter.pid) in s.run("wait-for", "-l", "pending"), "waiter not registered")
            result = s.run("restart-server", check=False)
            assert result.returncode != 0 and "waiting commands" in result.stderr
            assert before == s.snapshot()
        finally:
            s.run("wait-for", "-S", "pending")
            waiter.wait(timeout=3)
        s.run("pipe-pane", "-t", "one", "cat >/dev/null")
        result = s.run("restart-server", check=False)
        assert result.returncode != 0
        s.run("pipe-pane", "-t", "one")
        wait_for(lambda: s.run("restart-server", check=False).returncode == 0,
                 "server did not recover from busy state")
        assert before == s.snapshot()
        print("PASS busy state: reject command sequences, waiters, modes and pipes without loss")
    finally:
        s.cleanup()


if __name__ == "__main__":
    with tempfile.TemporaryDirectory(prefix="tmux-restart-regress-") as directory:
        root = Path(directory)
        for invocation in ("absolute", "relative", "path", "directory"):
            upgrade(root / invocation, invocation)
        bad_binary(root / "bad-binary")
        state_edges(root / "state-edges")
        reload_config(root / "reload-config")
        control_sizes(root / "control-sizes")
        silence_monitor(root / "silence-monitor")
        config_error(root / "config-error")
        foreground_server(root / "foreground-server")
        busy_server(root / "busy")
