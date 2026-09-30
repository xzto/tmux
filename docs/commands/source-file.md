# source-file

Execute commands from one or more configuration files.

Alias: `source`

## USAGE

```text
tmux source-file [OPTIONS] path ...
```

## OPTIONS

- `-F` — Expand each `path` as a format.
- `-n` — Parse the files without executing their commands.
- `-q` — Suppress errors when a path does not exist.
- `-t target-pane` — Set the target pane context for loaded commands.
- `-v` — Show parsed commands and line numbers when possible.

## NOTES

Paths may be glob patterns. Relative paths are resolved from the client's working directory, and `-` reads commands from standard input. Multiple paths are processed in order. `-q` suppresses a no-match error, but other file or glob errors are still reported.

## EXAMPLES

Load the user's tmux configuration:

```sh
tmux source-file ~/.tmux.conf
```

Check a configuration file's syntax without running its commands:

```sh
tmux source-file -n ~/.config/tmux/extra.conf
```
