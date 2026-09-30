# swap-pane

Exchange two panes, optionally moving the active pane's neighbors.

Alias: `swapp`

## USAGE

```text
tmux swap-pane [OPTIONS]
```

## OPTIONS

- `-d` — Do not change the active pane after the swap.
- `-D` — Swap with the next pane. Only tiled panes are considered.
- `-U` — Swap with the previous pane. Only tiled panes are considered.
- `-s src-pane` — Choose the source pane.
- `-t dst-pane` — Choose the destination pane.
- `-Z` — Keep the window zoomed if it was zoomed.

## NOTES

If `-s` is omitted, tmux uses the marked pane if one is set, otherwise the current pane. Without `-s`, `-D` or `-U` swaps the destination pane with the next or previous pane respectively. Directional swapping is not available for floating panes.

## EXAMPLES

Swap two panes by their pane indices:

```sh
tmux swap-pane -s 1.0 -t 1.1
```
