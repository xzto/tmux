# switch-client

Switch a client to another session, window, or pane.

Alias: `switchc`

## USAGE

```text
tmux switch-client [OPTIONS]
```

## OPTIONS

- `-c target-client` — Choose the client to switch.
- `-E` — Skip the environment update.
- `-F` — Accepted without effect.
- `-l` — Switch to the client's last session.
- `-n` — Switch to the next session.
- `-O sort-order` — Set the sort field for session selection.
- `-p` — Switch to the previous session.
- `-r` — Toggle the client's access flags.
- `-t target-session` — Choose a session, window, or pane.
- `-T key-table` — Set the client's key table for the next key.
- `-Z` — Keep the target window zoomed if it was zoomed.

## NOTES

`-t` normally selects a session. A target containing `:`, `.`, or `%` may identify a pane; tmux then switches the client to its session, window, and pane. With a pane target, `-Z` keeps the window zoomed if it was already zoomed.

`-l`, `-n`, and `-p` select the last, next, or previous session. `-O` sets the sort field for next/previous selection: `name`, `size`, `creation`, or `activity`.

`-E` prevents applying the target session's `update-environment` option. `-r` toggles the client's read-only and ignore-size flags. `-T` sets the key table for the client; after the next key, it returns to the default table.

## EXAMPLES

Switch the attached client to a named session:

```sh
tmux switch-client -t work
```

Switch to the next session:

```sh
tmux switch-client -n
```
