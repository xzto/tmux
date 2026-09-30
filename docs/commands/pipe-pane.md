# pipe-pane

Pipe pane output to a shell command or send command output to a pane.

Alias: `pipep`

## USAGE

```text
tmux pipe-pane [OPTIONS] [shell-command]
```

## OPTIONS

- `-I` — Send the command's output to the pane. The command's standard output is written to the pane as input.
- `-O` — Send pane output to the command. The pane's output is connected to the command's standard input.
- `-o` — Open a pipe only if none exists. With an existing pipe, the command closes it and does not open another.
- `-t target-pane` — Choose the pane to pipe. Defaults to the active pane.

## NOTES

A pane can be connected to one command at a time; an existing pipe is closed before a new command is run. If `shell-command` is omitted or empty, tmux closes the current pipe. With neither `-I` nor `-O`, pane output is sent to the command; using both connects both directions.

The command is run by a shell and supports the format sequences available to `status-left`. The `-o` flag is useful for toggling a pipe from a key binding.

## EXAMPLES

Append output from the active pane to a log file:

```sh
tmux pipe-pane -O 'cat >> /tmp/tmux-pane.log'
```

Close the pipe for the active pane:

```sh
tmux pipe-pane
```
