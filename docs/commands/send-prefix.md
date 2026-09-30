# send-prefix

Send the configured prefix key to a pane.

## USAGE

```text
tmux send-prefix [OPTIONS]
```

## OPTIONS

- `-2` — Send the secondary prefix key. This uses the session's `prefix2` option.
- `-t target-pane` — Choose the destination pane. Defaults to the active pane.

## NOTES

This sends the session's `prefix` key as if it were pressed; `-2` sends `prefix2` instead. The key is sent to the target pane and can, for example, be used to pass a prefix through to a nested tmux instance.

## EXAMPLES

Send the configured prefix key to pane 0 in the current window:

```sh
tmux send-prefix -t 0
```

Send the secondary prefix key to pane 0:

```sh
tmux send-prefix -2 -t 0
```
