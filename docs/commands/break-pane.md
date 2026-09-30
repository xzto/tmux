# break-pane

Move a pane out of its window into a new window, or make it floating.

Alias: `breakp`

## USAGE

```text
tmux break-pane [OPTIONS]
```

## OPTIONS

- `-a` — Place the new window after the target. Existing windows are shifted as needed.
- `-b` — Place the new window before the target. Existing windows are shifted as needed.
- `-d` — Leave the new window unselected.
- `-F format` — Set the printed window format. Used with `-P`.
- `-n window-name` — Name the new window.
- `-P` — Print information about the new window. The default format is `#{session_name}:#{window_index}.#{pane_index}`; use `-F` to change it.
- `-s src-pane` — Choose the pane to move.
- `-t dst-window` — Choose the destination window.
- `-W` — Make the pane floating. This lifts it out of the tiled layout instead of moving it to a new window.
- `-x width` — Set the floating pane width. Use columns or a percentage of the window width.
- `-y height` — Set the floating pane height. Use lines or a percentage of the window height.
- `-X x-position` — Set the floating pane's horizontal position. A percentage of the window width may be used.
- `-Y y-position` — Set the floating pane's vertical position. A percentage of the window height may be used.

## NOTES

With `-a` or `-b`, the new window is inserted after or before the target window, shifting other windows if necessary. If `-t` is omitted, tmux chooses the destination window index. The source pane defaults to the current pane.

`-W` makes the source pane floating rather than creating a window. It cannot be used on an already floating or hidden pane, or while the window is zoomed. The default floating size is half the window width and one quarter of its height; if the pane was previously floating, unspecified size and position values are restored. If positions are omitted for a newly floating pane, it is cascaded from the top-left. The `-x`, `-y`, `-X`, and `-Y` values may end in `%` to specify a percentage of the window size.

## EXAMPLES

Move the active pane into a new window without selecting it:

```sh
tmux break-pane -d
```
