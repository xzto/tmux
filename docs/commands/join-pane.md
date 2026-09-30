# join-pane

Move a pane into another pane's window, splitting the destination pane.

Alias: `joinp`

## USAGE

```text
tmux join-pane [OPTIONS]
```

## OPTIONS

- `-b` — Place the source before the destination. It goes left or above the destination.
- `-d` — Do not select the moved pane. The destination window is not made current.
- `-f` — Use the full window dimension for the new pane. With `-h`, it spans the height; otherwise it spans the width.
- `-h` — Split horizontally. The panes are placed side by side.
- `-v` — Split vertically. This is the default orientation.
- `-l size` — Set the new pane size. Use lines for a vertical split, columns for a horizontal split; `%` means a percentage.
- `-p percentage` — Set the new pane size as a percentage. Equivalent to `-l` with a `%` value.
- `-s src-pane` — Choose the pane to move.
- `-t dst-pane` — Choose the pane to split.

## NOTES

If `-s` is omitted, tmux uses the marked pane if one is set, otherwise the current pane. The destination defaults to the active pane. This reverses `break-pane` by moving the source pane into the destination window.

If the source is floating and the destination is omitted or is the source itself, the pane is returned to its previous tiled position. The `-l` option accepts a percentage as well as a size; `-p` directly specifies a percentage.

## EXAMPLES

Join a pane from window 2 into the active pane's window:

```sh
tmux join-pane -s 2.1
```
