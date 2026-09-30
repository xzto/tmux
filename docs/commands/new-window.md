# new-window

Create a window in the current session. The new window becomes current unless `-d` is used.

Alias: `neww`

## USAGE

```text
tmux new-window [OPTIONS] [shell-command [argument ...]]
```

## OPTIONS

- `-a` — Insert after `target-window`. Later windows are shifted up.
- `-b` — Insert before `target-window`. Later windows are shifted up.
- `-c start-directory` — Set the command's working directory.
- `-d` — Leave the new window in the background.
- `-e VARIABLE=value` — Set an environment variable. May be repeated.
- `-E` — Create the initial pane without a running command.
- `-F format` — Set the format used by `-P`.
- `-k` — Destroy the target window if it exists. Otherwise, an existing target is an error.
- `-n window-name` — Set the new window's name.
- `-P` — Print the new window's identifier. Use `-F` to set the format; the default is `#{session_name}:#{window_index}.#{pane_index}`.
- `-S` — Select an existing window instead of creating one. Prefer the target if it exists; otherwise use a window named by `-n`. With `-d`, do nothing if a match is found.
- `-t target-window` — Set the target session and window index. Defaults to the current session's next available index.

## NOTES

If `shell-command` is omitted, tmux uses the `default-command` option. The new window is created in the current session unless `-t` specifies another target.

When the command exits, the window closes; use the `remain-on-exit` option to change this behavior.

Programs inside tmux should have `TERM` set to `screen` or `tmux`. New windows get `TERM=screen` automatically; avoid overriding it with `-e` or in shell startup files.

## EXAMPLES

Create a window running the default command:

```sh
tmux new-window
```

Create a named window running a command:

```sh
tmux new-window -n logs 'tail -f /var/log/syslog'
```

Create a window after the current one and leave it in the background:

```sh
tmux new-window -ad
```

Print the new window's identifier:

```sh
tmux new-window -P
```
