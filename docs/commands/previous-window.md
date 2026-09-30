# previous-window

Move to the previous window in a session.

Alias: `prev`

## USAGE

```text
tmux previous-window [OPTIONS]
```

## OPTIONS

- `-a` — Select the previous window with an alert.
- `-t target-session` — Set the target session.

## NOTES

Without `-t`, the current session is used. Normal movement follows window index order and wraps to the last window before the first. With `-a`, tmux searches backward for a window with an alert (bell, activity, or silence); if none is found, the command reports an error.

## EXAMPLES

Move to the previous window in the current session:

```sh
tmux previous-window
```

Move to the previous window with an alert in the `work` session:

```sh
tmux previous-window -a -t work
```
