# respawn-pane

Restart the command in an inactive pane.

Alias: `respawnp`

## USAGE

```text
tmux respawn-pane [OPTIONS] [shell-command [argument ...]]
```

## OPTIONS

- `-c start-directory` — Set the pane's working directory.
- `-e VARIABLE=value` — Set an environment variable. May be repeated.
- `-E` — Leave the pane without a running command.
- `-k` — Kill an existing command before respawning.
- `-t target-pane` — Choose the target pane. Defaults to the active pane.

## NOTES

If `shell-command` is omitted, tmux reruns the command used when the pane was created or last respawned. The pane must be inactive unless `-k` is given. With `-E`, the pane is left without a running command; a non-empty `shell-command` cannot be supplied.

`-c` sets a new working directory. `-e` sets environment variables for the command, in the same form as for `new-window`.

## EXAMPLES

Restart the previous command in pane 3, if it has exited:

```sh
tmux respawn-pane -t %3
```

Run a command in the inactive pane using a specified directory:

```sh
tmux respawn-pane -t %3 -c ~/project 'make test'
```
