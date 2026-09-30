# kill-session

Destroy a session, or clear its window alerts.

## USAGE

```text
tmux kill-session [OPTIONS]
```

## OPTIONS

- `-a` — Kill all sessions except the target session.
- `-C` — Clear alerts for the target session.
- `-f filter` — Filter sessions killed by `-a`. Only matching sessions are killed.
- `-g` — Kill the target's session group, if grouped.
- `-t target-session` — Choose the session to kill or clear.

## NOTES

By default, tmux destroys the target session, detaches its clients, and destroys windows linked only to it. The target defaults to the current session.

`-a` kills all sessions except the target; `-f` may limit this to sessions for which the format filter is true. `-f` is valid only with `-a` and cannot be used with `-C`.

`-g` kills every session in the target's session group if it belongs to one. `-C` clears bell, activity, and silence alerts on all windows linked to the target instead of killing sessions; it takes precedence over other mode flags.

## EXAMPLES

Kill a specifically named session:

```sh
tmux kill-session -t finished-build
```
