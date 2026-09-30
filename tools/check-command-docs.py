#!/usr/bin/env python3
"""Check coverage and installed CLI behavior of tmux command references."""

import argparse
import errno
import importlib.util
import os
from pathlib import Path
import pty
import re
import subprocess
import sys


parser = argparse.ArgumentParser()
parser.add_argument("--binary", type=Path)
args = parser.parse_args()

root = Path(__file__).resolve().parent.parent
spec = importlib.util.spec_from_file_location("generator", root / "tools/gen-command-docs.py")
generator = importlib.util.module_from_spec(spec)
spec.loader.exec_module(generator)
pages = sorted((root / "docs/commands").glob("*.md"))
docs = [generator.parse(str(path)) for path in pages]
entries = generator.source_commands(str(root))
generator.check_docs(docs, entries, complete=True)

man_source = (root / "tmux.1").read_text(encoding="utf-8")
index_match = re.search(r"(?ms)^\.Ss Command reference\n(.*?)(?=^\.Sh )", man_source)
assert index_match, "tmux.1 is missing its command reference index"
indexed = set(re.findall(r"(?m)^\.It Xr tmux-([a-z0-9-]+) 1$",
                         index_match.group(1)))
doc_names = {doc["name"] for doc in docs}
assert indexed == doc_names, (
    "command index mismatch", sorted(doc_names - indexed),
    sorted(indexed - doc_names))
index_text = index_match.group(1)
for doc in sorted(docs, key=lambda item: item["name"]):
    summary = generator.clean_inline(doc["description"]).split(". ", 1)[0].rstrip(".") + "."
    alias = f" Alias: {generator.roff_escape(doc['alias'])}." if doc["alias"] else ""
    entry = (f".It Xr tmux-{doc['name']} 1\n" +
             generator.roff_escape(summary + alias))
    assert entry in index_text, (doc["name"], "index summary or alias is stale")

tmux = (args.binary or root / "tmux").resolve()
if not tmux.exists():
    print("Build tmux first with 'make -s'", file=sys.stderr)
    sys.exit(1)
for doc in docs:
    names = [doc["name"]] + ([doc["alias"]] if doc["alias"] else [])
    for name in names:
        result = subprocess.run([str(tmux), name, "--help"], capture_output=True,
                                text=True, check=True)
        expected = f"  tmux {doc['name']}" + (" [OPTIONS]" if doc["options"] else "\n")
        assert expected in result.stdout, (name, result.stdout[:200])
        assert "\nOPTIONS\n" in result.stdout, name
        assert "\\n" not in result.stdout, name
        for flag, _, _ in doc["options"]:
            assert any(line.lstrip().startswith(flag + " ") or
                       line.lstrip() == flag for line in result.stdout.splitlines()), (name, flag)
result = subprocess.run([str(tmux), "--help"], capture_output=True,
                        text=True, check=True)
assert "\nUSAGE\n" in result.stdout and "\\n" not in result.stdout

source_usage = {}
for line in subprocess.check_output([str(tmux), "list-commands"], text=True).splitlines():
    match = re.match(r"^([a-z][a-z0-9-]*)(?: \([^)]*\))?(?: (.*))?$", line)
    assert match, line
    usage = re.sub(r"\[-[^]]*\]", " ", match.group(2) or "")
    source_usage[match.group(1)] = " ".join(usage.split())
assert set(source_usage) == doc_names, "built command list differs from source pages"
usage_exceptions = {
    # These positional arguments are accepted but omitted/overstated by .usage.
    "resize-pane": ("", "[adjustment]"),
    "unbind-key": ("key", "[key]"),
}
for doc in docs:
    line = next(line.strip() for line in doc["sections"]["USAGE"]
                if line.strip().startswith(f"tmux {doc['name']}"))
    documented = line[len(f"tmux {doc['name']}"):].strip()
    if documented.startswith("[OPTIONS]"):
        documented = documented[len("[OPTIONS]"):].strip()
    documented = " ".join(documented.split())
    source = source_usage[doc["name"]]
    if doc["name"] in usage_exceptions:
        assert (source, documented) == usage_exceptions[doc["name"]], doc["name"]
    else:
        assert documented == source, (doc["name"], documented, source)


def tty_help(arguments, **environment):
    master, slave = pty.openpty()
    try:
        child = subprocess.Popen([str(tmux), *arguments], stdout=slave,
                                 stdin=subprocess.DEVNULL, stderr=subprocess.PIPE,
                                 env={**os.environ, "TERM": "xterm-256color",
                                      "NO_COLOR": "", **environment})
        os.close(slave)
        slave = -1
        output = bytearray()
        while True:
            try:
                data = os.read(master, 8192)
            except OSError as error:
                if error.errno == errno.EIO:
                    break
                raise
            if not data:
                break
            output.extend(data)
        assert child.wait() == 0, arguments
        return bytes(output)
    finally:
        os.close(master)
        if slave != -1:
            os.close(slave)


for arguments in (["--help"], ["split-window", "--help"]):
    styled = tty_help(arguments)
    sgr = re.findall(rb"\x1b\[([0-9;]+)m", styled)
    assert b"1;4" in sgr and b"1" in sgr, arguments
    assert set(sgr) <= {b"0", b"1", b"1;4"}, arguments
    assert styled.count(b"\x1b[") == len(sgr), arguments
    if arguments[0] == "split-window":
        notes = styled.split(b"\x1b[1;4mNOTES\x1b[0m", 1)[1]
        assert b"\x1b[" not in notes, arguments
    assert b"\x1b[" not in tty_help(arguments, NO_COLOR="1"), arguments
    assert b"\x1b[" not in tty_help(arguments, TERM="dumb"), arguments
print(f"Checked {len(docs)} command pages, aliases, flag arity, synopses, man index, global help and TTY styling")
