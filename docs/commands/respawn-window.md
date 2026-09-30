# respawn-window

Restart the command in an inactive window.

Alias: `respawnw`

## USAGE

```text
tmux respawn-window [OPTIONS] [shell-command [argument ...]]
```

## OPTIONS

- `-c start-directory` — Set the window's working directory.
- `-e VARIABLE=value` — Set an environment variable. May be repeated.
- `-E` — Leave the window empty. It retains one pane with no running command.
- `-k` — Kill an existing command before respawning.
- `-t target-window` — Choose the target window. Defaults to the current window.

## NOTES

If `shell-command` is omitted, tmux reruns the command used when the window was created or last respawned. The window must be inactive unless `-k` is given. With `-E`, the window is left with one pane and no running command; a non-empty `shell-command` cannot be supplied.

`-c` sets a new working directory. `-e` sets environment variables for the command, in the same form as for `new-window`.

## EXAMPLES

Restart the previous command in window 2, if it has exited:

```sh
tmux respawn-window -t :2
```

Run a command in the inactive window using a specified directory:

```sh
tmux respawn-window -t :2 -c ~/project 'make test'
```
