# switch-mode

Choose a session or window from an interactive list.

## USAGE

```text
tmux switch-mode [OPTIONS] [command]
```

## OPTIONS

- `-F format` — Set the format for each item.
- `-k` — Kill the pane when the mode exits.
- `-s` — List sessions.
- `-w` — List windows instead of sessions.
- `-t target-pane` — Choose the target pane.
- `-Z` — Zoom the pane.

## NOTES

The target defaults to the active pane. The list starts with sessions unless `-w` is used; typing narrows it with fuzzy matching. Press Enter to choose an item, the arrow keys to move, or Escape to exit.

After choosing a session or window, the first `%%` and every `%1` in `command` are replaced with the selection and the result is run as a tmux command. If omitted, `command` defaults to `switch-client -Zt '%%'`.

## EXAMPLES

Open a session picker in the active pane:

```sh
tmux switch-mode
```

Open a window picker in pane `%3`:

```sh
tmux switch-mode -wt %3
```
