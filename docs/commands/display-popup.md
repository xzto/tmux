# display-popup

Run a command in a floating popup pane.

Alias: `popup`

## USAGE

```text
tmux display-popup [OPTIONS] [shell-command [argument ...]]
```

## OPTIONS

- `-B` — Draw the popup without a border.
- `-b border-lines` — Choose the popup border characters.
- `-C` — Close the active popup.
- `-c target-client` — Display the popup on this client.
- `-d start-directory` — Set the command's working directory.
- `-e VARIABLE=value` — Set an environment variable.
- `-E` — Close the popup when the command exits.
- `-k` — Wait for a key to close after command exit.
- `-h height` — Set the popup height in cells or percent.
- `-w width` — Set the popup width in cells or percent.
- `-s style` — Set the popup content style.
- `-S border-style` — Set the popup border style.
- `-t target-pane` — Choose the pane and window for the popup.
- `-T title` — Set the popup title format.
- `-x position` — Set the popup's horizontal position.
- `-y position` — Set the popup's vertical position.

## NOTES

If `shell-command` is omitted, tmux uses the `default-command` option. `-e` may be repeated. `-b` accepts `single`, `double`, `heavy`, `simple`, `number`, `spaces`, `none`, or `rounded`; `-B` omits the border.

`-E` closes the popup when the command exits. Repeating it (`-EE`) keeps the popup until a key is pressed after the command exits. `-k` also waits for a key after exit; combined with `-EE`, it waits only when the command fails. Without these flags, the popup remains after the command exits and can be closed with Escape or Ctrl-C.

`-x` and `-y` accept a coordinate, a format, or a placement code. `C` centers the popup; `R` places it at the right (`-x`), `P` at the bottom left of the pane, `M` at the mouse, `L` at the last popup position, and `W` at the window's status-line position. `S` (`-y`) places it beside the status line. The default width and height are each half the window.

## EXAMPLES

Open a read-only file in a popup that closes when the command exits:

```sh
tmux display-popup -E 'less README'
```

Show repository status in a borderless popup sized to half the window:

```sh
tmux display-popup -B -w 50% -h 50% -E 'git status --short'
```
