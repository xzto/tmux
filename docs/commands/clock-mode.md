# clock-mode

Display a large clock in the target pane.

## USAGE

```text
tmux clock-mode [OPTIONS]
```

## OPTIONS

- `-t target-pane` — Choose the pane in which to display the clock.

## NOTES

If `-t` is omitted, the current pane is used. The `clock-mode-style` and `clock-mode-colour` window options control the clock's appearance.

## EXAMPLES

Display a clock in the current pane:

```sh
tmux clock-mode
```
