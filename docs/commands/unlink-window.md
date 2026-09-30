# unlink-window

Remove a window from the current session.

Alias: `unlinkw`

## USAGE

```text
tmux unlink-window [OPTIONS]
```

## OPTIONS

- `-k` — Allow unlinking the window's final session link. If it has only one link, the window is destroyed.
- `-t target-window` — Choose the target window. Defaults to the current window.

## NOTES

Without `-k`, a window can be unlinked only if it is linked to multiple sessions; tmux will not leave it linked to no sessions. With `-k`, it can be unlinked from its only session, which destroys it.

## EXAMPLES

Unlink window 3 from the current session when it remains linked elsewhere:

```sh
tmux unlink-window -t :3
```
