# next-window

Move to the next window in a session.

Alias: `next`

## USAGE

```text
tmux next-window [OPTIONS]
```

## OPTIONS

- `-a` — Select the next window with an alert.
- `-t target-session` — Set the target session.

## NOTES

Without `-t`, the current session is used. Normal movement follows window index order and wraps to the first window after the last. With `-a`, tmux searches forward for a window with an alert (bell, activity, or silence); if none is found, the command reports an error.

## EXAMPLES

Move to the next window in the current session:

```sh
tmux next-window
```

Move to the next window with an alert in the `work` session:

```sh
tmux next-window -a -t work
```
