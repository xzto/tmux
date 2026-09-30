# kill-pane

Destroy a pane.

Alias: `killp`

## USAGE

```text
tmux kill-pane [OPTIONS]
```

## OPTIONS

- `-a` — Kill other panes in the window. The target pane is kept.
- `-f filter` — Filter panes selected by `-a`. The filter is a format evaluated for each pane.
- `-t target-pane` — Choose the pane to kill or keep. Defaults to the active pane.

## NOTES

Without `-a`, tmux destroys the target pane. With `-a`, all other panes in the target window are killed unless `-f` restricts the set to panes for which its format evaluates as true. `-f` is only valid with `-a`. If no panes remain, tmux also destroys the containing window.

## EXAMPLES

Close a specific pane (replace `%3` with the pane ID you intend to close):

```sh
tmux kill-pane -t %3
```
