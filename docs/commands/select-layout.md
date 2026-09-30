# select-layout

Choose a layout for the target window.

Alias: `selectl`

## USAGE

```text
tmux select-layout [OPTIONS] [layout-name]
```

## OPTIONS

- `-E` — Spread the target pane and its neighbors evenly.
- `-n` — Select the next layout.
- `-p` — Select the previous layout.
- `-o` — Restore the last layout when possible.
- `-t target-pane` — Choose the target pane and its window.

## NOTES

If `layout-name` is omitted, tmux reapplies the last preset layout used, if any. The `-n` and `-p` flags are equivalent to `next-layout` and `previous-layout`. The `-o` flag reapplies the last set layout when possible, undoing the most recent layout change. The `-E` flag spreads the target pane and any panes next to it evenly.

Preset layouts include `even-horizontal`, `even-vertical`, `main-horizontal`, `main-horizontal-mirrored`, `main-vertical`, `main-vertical-mirrored`, and `tiled`. A layout string shown by `list-windows` can also be supplied as `layout-name`. Tmux adjusts a layout to fit the window, but it cannot be applied if the target window has more panes than the window from which the layout was defined.

## EXAMPLES

Apply the tiled layout to the current window:

```sh
tmux select-layout tiled
```

Undo the most recent layout change when possible:

```sh
tmux select-layout -o
```
