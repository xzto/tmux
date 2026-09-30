# set-environment

Set, unset, or mark an environment variable for removal.

Alias: `setenv`

## USAGE

```text
tmux set-environment [OPTIONS] variable [value]
```

## OPTIONS

- `-F` — Expand `value` as a format.
- `-g` — Change the global environment. By default, change the target session's environment.
- `-h` — Mark the variable as hidden. Hidden variables are not passed to new processes.
- `-r` — Mark the variable for removal from new processes. Do not provide `value` with this flag.
- `-t target-session` — Select the target session. Defaults to the current session.
- `-u` — Unset the variable. Do not provide `value` with this flag.

## NOTES

A `value` is required unless `-u` or `-r` is used. `-u` removes the variable from the selected environment; `-r` instead keeps a removal mark so the variable is omitted when starting a new process. A hidden variable remains available to tmux, including in formats, but is not passed to new processes.

The global and session environments are merged when tmux starts a window's process. If the same variable is in both, the session value takes precedence.

## EXAMPLES

Set a global editor variable:

```sh
tmux set-environment -g EDITOR vi
```

Unset a variable from the current session environment:

```sh
tmux set-environment -u TEMPORARY
```
