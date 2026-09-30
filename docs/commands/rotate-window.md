# rotate-window

Rotate the pane positions within a window.

Alias: `rotatew`

## USAGE

```text
tmux rotate-window [OPTIONS]
```

## OPTIONS

- `-D` — Move the last pane to the start of the pane list.
- `-U` — Move the first pane to the end of the pane list.
- `-t target-window` — Choose the window whose panes to rotate.
- `-Z` — Keep the window zoomed if it was already zoomed.

## NOTES

The default rotation uses the first-to-last pane-list path, as does `-U` in this source revision; `-D` selects the last-to-first path. The command also rotates pane layout positions. If the window was zoomed, `-Z` keeps it zoomed.

## EXAMPLES

Rotate the panes in the current window:

```sh
tmux rotate-window
```

Rotate panes in the other pane-list direction in a selected window:

```sh
tmux rotate-window -D -t work:2
```
