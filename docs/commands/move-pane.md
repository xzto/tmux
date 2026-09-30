# move-pane

Move a pane into another window or reposition a floating pane.

Alias: `movep`

## USAGE

```text
tmux move-pane [OPTIONS]
```

## OPTIONS

- `-b` — Place the source before the destination. It goes left or above the destination.
- `-D [lines]` — Move the floating pane down. The default distance is one line.
- `-U [lines]` — Move the floating pane up. The default distance is one line.
- `-L [columns]` — Move the floating pane left. The default distance is one column.
- `-R [columns]` — Move the floating pane right. The default distance is one column.
- `-d` — Do not select the moved pane. The destination window is not made current.
- `-f` — Use the full window dimension for the new pane. With `-h`, it spans the height; otherwise it spans the width.
- `-h` — Split horizontally. The panes are placed side by side.
- `-v` — Split vertically. This is the default orientation.
- `-l size` — Set the new pane size. Use lines for a vertical split, columns for a horizontal split; `%` means a percentage.
- `-M` — Start a mouse drag to move a floating pane. Use from a mouse key binding.
- `-P position` — Place the floating pane at a named position. See NOTES for accepted positions.
- `-s src-pane` — Choose the pane to move.
- `-t dst-pane` — Choose the destination pane. With `-P`, `-z`, `-X`, `-Y`, or a direction option, it identifies the floating pane to reposition.
- `-X x-position` — Set the floating pane's horizontal position. A percentage of the window width may be used.
- `-Y y-position` — Set the floating pane's vertical position. A percentage of the window height may be used.
- `-z z-index` — Set the floating pane's stack position. Zero places it at the front.

## NOTES

Without floating-pane movement options, `move-pane` works like `join-pane`: it moves the source pane into the destination pane's window. The source defaults to the marked pane if one is set, otherwise the current pane; the destination defaults to the active pane.

`-D`, `-L`, `-P`, `-R`, `-U`, `-X`, `-Y`, and `-z` reposition the floating target pane, which must already be floating. `-D`, `-L`, `-R`, and `-U` move it by the given number of lines or columns, or one if omitted. `-X` and `-Y` set absolute positions and accept a percentage of the window size.

`-P` accepts `top-left`, `top-centre`/`top-center`, `top-right`, `centre-left`/`center-left`, `centre`/`center`, `centre-right`/`center-right`, `bottom-left`, `bottom-centre`/`bottom-center`, `bottom-right`, `top-left-centre`/`top-left-center`, `top-right-centre`/`top-right-center`, `bottom-left-centre`/`bottom-left-center`, `bottom-right-centre`/`bottom-right-center`, `front`, `back`, `forward`, `backward`, `forward-loop`, or `backward-loop`. The `front` and `back` positions set the pane's floating stack order; `forward` and `backward` move it one position, while their `-loop` variants wrap at the ends. `-z` sets a numeric stack index, where zero is the front.

## EXAMPLES

Move a pane from window 2 into the active pane's window:

```sh
tmux move-pane -s 2.1
```
