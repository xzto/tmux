# if-shell

Run a tmux command according to a shell command's result.

Alias: `if`

## USAGE

```text
tmux if-shell [OPTIONS] shell-command command [command]
```

## OPTIONS

- `-b` — Run the shell command without waiting for it.
- `-F` — Test the condition without running a shell.
- `-t target-pane` — Set the target pane for formats and shell context.

## NOTES

The `shell-command` is expanded using formats, including values from `target-pane`, then run with `/bin/sh`. A zero exit status runs the first `command`; a nonzero status runs the optional second `command`. Without a second command, a false result does nothing.

With `-F`, the expanded `shell-command` is treated as true unless it is empty or `0`; no shell is started. With `-b`, the shell command runs in the background and the invoking command queue does not wait for its result.

## EXAMPLES

Show a message depending on whether the current directory is a Git checkout:

```sh
tmux if-shell 'test -d .git' 'display-message "Git checkout"' 'display-message "No .git directory"'
```

Test a format without running a shell command:

```sh
tmux if-shell -F '#{session_attached}' 'display-message "Attached"' 'display-message "Detached"'
```
