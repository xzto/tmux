# resize-pane

Resize a pane by an adjustment or to an absolute size.

Alias: `resizep`

## USAGE

```text
tmux resize-pane [OPTIONS] [adjustment]
```

## OPTIONS

- `-D [lines]` — Resize the pane downward. The adjustment defaults to 1 line.
- `-U [lines]` — Resize the pane upward. The adjustment defaults to 1 line.
- `-L [columns]` — Resize the pane to the left. The adjustment defaults to 1 column.
- `-R [columns]` — Resize the pane to the right. The adjustment defaults to 1 column.
- `-x width` — Set the pane's absolute width. A trailing `%` uses a percentage of the window width.
- `-y height` — Set the pane's absolute height. A trailing `%` uses a percentage of the window height.
- `-M` — Begin resizing with the mouse. Use only from a mouse key binding.
- `-T` — Trim lines below the cursor. Lines removed from the screen are replaced from history.
- `-t target-pane` — Choose the pane to resize. Defaults to the active pane.
- `-Z` — Toggle zoom for the target pane. Zoomed panes occupy the whole window.

## NOTES

Directional adjustments default to 1. They can take an adjustment after the flag or use the optional positional `adjustment`; an adjustment may be negative. For a floating pane, the directional flags act on the corresponding borders. Drag a floating pane's borders or corners to resize it, or its top border to move it.

`-x` and `-y` set an absolute size in columns and lines respectively; append `%` to use a percentage of the corresponding window dimension. `-M` starts mouse resizing and is valid when bound to a mouse key. `-T` trims all lines below the cursor and moves lines out of history to replace them.

## EXAMPLES

Increase the active pane's height by five lines:

```sh
tmux resize-pane -D 5
```

Set the active pane to 80 columns wide:

```sh
tmux resize-pane -x 80
```
