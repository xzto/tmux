# last-pane

Select the previously active pane in a window.

Alias: `lastp`

## USAGE

```text
tmux last-pane [OPTIONS]
```

## OPTIONS

- `-d` — Disable input to the previous pane. It does not select that pane.
- `-e` — Enable input to the previous pane. It does not select that pane.
- `-t target-window` — Choose the window. Defaults to the current window.
- `-Z` — Keep the window zoomed if it was zoomed. This applies when selecting the pane.

## NOTES

If the previous pane cannot be found but the window has exactly two panes, tmux uses the other pane. Otherwise, the command reports an error when no previous pane is available. With no `-d` or `-e`, the previous pane is selected; `-d` disables input to it and `-e` enables input without changing the selection.

## EXAMPLES

Return to the previously active pane:

```sh
tmux last-pane
```

Select the previous pane while preserving an existing zoom:

```sh
tmux last-pane -Z
```
