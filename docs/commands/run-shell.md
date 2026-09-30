# run-shell

Run a shell command or tmux command without creating a window.

Alias: `run`

## USAGE

```text
tmux run-shell [OPTIONS] [shell-command [argument ...]]
```

## OPTIONS

- `-b` — Run the command without waiting for it.
- `-C` — Run a tmux command instead of a shell command.
- `-d delay` — Delay the command by this many seconds.
- `-E` — Redirect standard error to standard output.
- `-c start-directory` — Set the command's working directory.
- `-s value` — Accept a value that this command does not use.
- `-t target-pane` — Choose where to display command output.

## NOTES

Shell commands run using `/bin/sh` and are expanded as formats. Additional arguments are available in the command as `#{1}`, `#{2}`, and so on. Unless `-C` is used, standard output is shown in view mode in the target pane (or current pane) after the command finishes; a nonzero exit status is also displayed. `-E` redirects standard error to standard output instead of ignoring it.

`-C` interprets the command as a tmux command rather than starting a shell. `-b` runs without waiting for completion. `-d` delays execution by the specified number of seconds. In this source version, `-s` is accepted by the argument parser but is not read by the command execution logic.

## EXAMPLES

Run a shell command and display its output:

```sh
tmux run-shell 'date'
```

Run a tmux command:

```sh
tmux run-shell -C 'display-message "Hello from tmux"'
```
