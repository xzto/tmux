# new-pane

Create a floating pane in the current window.

Alias: `newp`

## USAGE

```text
tmux new-pane [OPTIONS] [shell-command [argument ...]]
```

## OPTIONS

- `-A` — Keep the floating pane above a zoomed pane.
- `-b` — Place the pane before `target-pane` in split mode. With `-h`, it goes left; with `-v`, above. Use `-L` to enable split mode.
- `-B border-lines` — Set border lines for floating panes.
- `-C` — Close a modal pane when clicked outside. Used with `-O`.
- `-c start-directory` — Set the command's working directory.
- `-D` — Close a modal pane with Escape or Ctrl-C. Used with `-O`.
- `-d` — Leave the new pane unselected.
- `-e VARIABLE=value` — Set a pane environment variable. May be repeated.
- `-E` — Create an empty pane with no command. Cannot be combined with a non-empty `shell-command`.
- `-f` — Make a split span the full window dimension. With `-h`, it spans the height; with `-v`, the width. Use `-L` to enable split mode.
- `-F format` — Set the format used by `-P`.
- `-h` — Split panes horizontally. Requires `-L`.
- `-v` — Split panes vertically. Requires `-L`; this is the default split direction.
- `-I` — Create an empty pane and forward standard input. Cannot be combined with a non-empty `shell-command`.
- `-k` — Keep the pane open after its command exits. Wait for a key to close it.
- `-K` — Pass all keys directly to the modal pane. Requires `-O`.
- `-L` — Use `split-window` behavior. The new pane is tiled instead of floating.
- `-l size` — Set the new pane's size in split mode. Use lines with `-v`, columns with `-h`; `%` specifies available space. Requires `-L`.
- `-M` — Resize the floating pane with a mouse drag. Use from a mouse-drag binding.
- `-m message` — Keep the pane open and show a message. Equivalent to `-k`; sets `remain-on-exit-format`.
- `-O` — Make the floating pane modal. A window can have only one modal pane.
- `-p percentage` — Set split size as a percentage. Shorthand for `-l`; requires `-L`.
- `-P` — Print information about the new pane. Use `-F` to customize the format.
- `-R inactive-border-style` — Set the inactive border style.
- `-s style` — Set the pane content style.
- `-S active-border-style` — Set the active border style.
- `-t target-pane` — Choose the pane for the new pane. Defaults to the active pane.
- `-T title` — Set the new pane's title.
- `-W` — Wait for the command to exit. Return its exit status.
- `-x width` — Set the floating pane's width. `%` specifies window width.
- `-y height` — Set the floating pane's height. `%` specifies window height.
- `-X x-position` — Set the pane's horizontal position. `%` specifies window width.
- `-Y y-position` — Set the pane's vertical position. `%` specifies window height.
- `-Z` — Zoom the window if it is not zoomed. Leave it zoomed if already zoomed.

## NOTES

By default, `new-pane` creates a floating pane. Its size defaults to half the window width and one quarter of its height; floating panes are cascaded from the top-left. `-x` and `-y` set its size, while `-X` and `-Y` set the upper-left position. Each can take a percentage of the corresponding window dimension. `-L` creates a tiled split instead; `-h` and `-v` select its direction, with vertical as the default.

Without `shell-command`, tmux uses `default-command`. `-E` or an empty command (`''`) creates a pane with no running command. `-I` also creates an empty pane and forwards standard input; `display-message -I` can write to an empty pane.

`-k` leaves the pane open after its command exits and waits for a key. `-m` does the same and sets the pane's `remain-on-exit-format` to the supplied message. `-O` creates a modal pane, which prevents interaction with other panes while active; it requires a floating pane and cannot be used with `-L`. `-K`, `-C`, and `-D` modify modal-pane behavior.

`-P` prints `#{session_name}:#{window_index}.#{pane_index}` by default; use `-F` to supply another format. `-B` sets the `pane-border-lines` option for floating panes.

## EXAMPLES

Create a floating pane at 60% width and 50% height:

```sh
tmux new-pane -x 60% -y 50%
```

Create a horizontal tiled split and run a command in it:

```sh
tmux new-pane -L -h -l 40% 'htop'
```
