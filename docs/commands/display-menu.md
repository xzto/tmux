# display-menu

Display an interactive menu on a client.

Alias: `menu`

## USAGE

```text
tmux display-menu [OPTIONS] name [key] [command] ...
```

## OPTIONS

- `-b border-lines` — Choose the menu border characters.
- `-c target-client` — Display the menu on this client.
- `-C starting-choice` — Select the initial menu item.
- `-H selected-style` — Set the selected-item style.
- `-M` — Enable mouse handling for the menu.
- `-O` — Keep the menu open when the mouse is released.
- `-s style` — Set the menu style.
- `-S border-style` — Set the menu border style.
- `-t target-pane` — Set the target pane for menu commands.
- `-T title` — Set the menu title format.
- `-x position` — Set the menu's horizontal position.
- `-y position` — Set the menu's vertical position.

## NOTES

Menu items are given as repeated `name key command` groups. An empty `name` adds a separator and takes no `key` or `command`; a name beginning with `-` is disabled. Names and commands are formats. `target-pane` supplies the context for commands run from the menu.

`-b` accepts `single`, `rounded`, `double`, `heavy`, `simple`, `padded`, or `none`. `-C -` leaves no item selected by default; otherwise it gives the starting item number. By default, the menu handles mouse events only when opened by a mouse binding; `-M` enables them in other cases. `-O` prevents the menu from closing when the mouse is released without an item selected.

`-x` and `-y` accept a terminal position, a format, or a placement code. `C` centers the menu; `R` places it at the right (`-x`), `P` at the bottom left of the pane, `M` at the mouse, `L` at the last menu position, and `W` at the window's status-line position. `S` (`-y`) places it beside the status line.

## EXAMPLES

Show a menu item that displays a message:

```sh
tmux display-menu -T 'Quick actions' 'Show message' m 'display-message "Hello"'
```

Place a menu near the mouse position:

```sh
tmux display-menu -x M -y M 'Show message' m 'display-message "Hello"'
```
