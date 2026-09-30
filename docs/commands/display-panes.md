# display-panes

Show pane indicators and allow a pane to be selected.

Alias: `displayp`

## USAGE

```text
tmux display-panes [OPTIONS] [template]
```

## OPTIONS

- `-d duration` — Set how long indicators remain, in milliseconds.
- `-k` — Kill the target pane when panes mode exits.
- `-N` — Keep indicators open until the duration expires.
- `-s source-window` — Show panes from this window.
- `-t target-pane` — Choose the pane to put into panes mode.
- `-Z` — Start panes mode unzoomed.

## NOTES

The target pane's window is shown unless `-s` specifies another source window. Indicators close when a key is pressed or the duration expires; `-N` ignores key presses. Without `-d`, the `display-panes-time` window option sets the duration. A duration of zero waits for a key press (unless `-N` is used).

Select an indicator with its `0` to `9` key or by clicking it. The optional `template` is run as a tmux command with `%%` replaced by the selected pane ID; the default is `select-pane -t '%%'`. `-Z` starts panes mode unzoomed; otherwise it is zoomed. `-k` kills the pane when panes mode exits.

## EXAMPLES

Show pane indicators for the configured duration:

```sh
tmux display-panes
```

Show pane indicators for two seconds:

```sh
tmux display-panes -d 2000
```
