# kill-window

Destroy a window and remove it from all sessions.

Alias: `killw`

## USAGE

```text
tmux kill-window [OPTIONS]
```

## OPTIONS

- `-a` — Kill the other windows in the session. The target window is retained.
- `-f filter` — Restrict which windows are killed. Only valid with `-a`; only windows for which the filter is true are killed.
- `-t target-window` — Choose the target window. Defaults to the current window.

## NOTES

Without `-a`, the target window is destroyed and removed from every session to which it is linked. With `-a`, other windows in the target session are killed; if the session has only one window, nothing is killed. `-f` is a format filter applied to each window and may only be used with `-a`.

## EXAMPLES

Destroy window 3:

```sh
tmux kill-window -t :3
```
