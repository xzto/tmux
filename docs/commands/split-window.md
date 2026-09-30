# split-window

Split a pane and start a command in the new pane. The new pane is selected unless `-d` is used.

Alias: `splitw`

## USAGE

```text
tmux split-window [OPTIONS] [shell-command [argument ...]]
```

## OPTIONS

- `-b` — Place the new pane before `target-pane`. With `-h`, it goes left; with `-v`, above.
- `-B border-lines` — Set border lines for floating panes.
- `-c start-directory` — Set the command's working directory.
- `-d` — Leave the new pane unselected.
- `-e VARIABLE=value` — Set an environment variable. May be repeated.
- `-E` — Create an empty pane without starting a command. Cannot be used with `shell-command`.
- `-f` — Span the full window instead of the active pane. With `-h`, use its height; with `-v`, its width.
- `-F format` — Set the format used by `-P`.
- `-h` — Split horizontally, placing panes side by side.
- `-v` — Split vertically (the default).
- `-I` — Create an empty pane and forward standard input.
- `-k` — Keep the pane open after its command exits. Wait for a key to close it.
- `-l size` — Set size in lines, columns, or percent. Use lines with `-v`, columns with `-h`; `%` means percent.
- `-m message` — Keep the pane open after exit and show `message`. Equivalent to `-k`.
- `-p percentage` — Set pane size as a percentage. Shorthand for `-l`.
- `-P` — Print information about the new pane. Use `-F` to customize its format.
- `-R inactive-border-style` — Set the inactive border style.
- `-s style` — Set the pane content style.
- `-S active-border-style` — Set the active border style.
- `-t target-pane` — Choose the pane to split.
- `-T title` — Set the new pane's title.
- `-W` — Wait and return the command's exit status.
- `-Z` — Zoom the window if it is not already zoomed.

## NOTES

Without `shell-command`, tmux runs the `default-command`. An empty `shell-command` (`''`) creates an empty pane; the `-E` flag also creates one and cannot be used with a non-empty command. `-I` creates an empty pane and forwards standard input to it; `display-message -I` can write to that pane. `target-pane` defaults to the active pane.

`-f` makes the new pane span the full window height with `-h` or full width with `-v`, instead of splitting the active pane. `-l` sizes the pane in lines (`-v`) or columns (`-h`); append `%` to specify a percentage of available space. `-p` is shorthand for this percentage.

`-k` keeps the pane open after its command exits and waits for a key to close it. `-m` does the same and sets the pane's `remain-on-exit-format` to the supplied message.

`-P` prints the new pane using the format `#{session_name}:#{window_index}.#{pane_index}` by default; use `-F` to supply another format.

`-Z` zooms the window if it is not zoomed, or leaves it zoomed if already zoomed.

If the target is a floating pane, the new pane is floating and adjacent to it. `-h`, `-v`, and `-b` determine its position. If both panes would not fit onscreen, tmux resizes them to fit.

## EXAMPLES

Split the active pane vertically and run a shell:

```sh
tmux split-window
```

Split horizontally, placing the new pane before the active pane:

```sh
tmux split-window -hb
```

Create a pane at 30% of the available space:

```sh
tmux split-window -p 30
```

Run a command and wait for its exit status:

```sh
tmux split-window -W 'make test'
```
