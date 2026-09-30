# select-window

Select a window in the target session.

Alias: `selectw`

## USAGE

```text
tmux select-window [OPTIONS]
```

## OPTIONS

- `-l` — Select the last-used window. Equivalent to `last-window`.
- `-n` — Select the next window. Equivalent to `next-window`.
- `-p` — Select the previous window. Equivalent to `previous-window`.
- `-T` — Switch to the last window if the target is current.
- `-t target-window` — Select the target window.

## NOTES

Without `-t`, the current window is the target. The `-l`, `-n`, and `-p` flags select the last, next, or previous window in the target session. If `-T` is given and the target is already current, tmux selects the last window instead.

## EXAMPLES

Select window 3 in the current session:

```sh
tmux select-window -t :3
```

Select the window named `editor` in the `work` session:

```sh
tmux select-window -t work:editor
```
