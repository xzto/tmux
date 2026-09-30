# rename-window

Rename the current window.

Alias: `renamew`

## USAGE

```text
tmux rename-window [OPTIONS] new-name
```

## OPTIONS

- `-t target-window` — Set the window to rename.

## NOTES

Without `-t`, the current window is renamed. The new name is expanded as a format string and must be valid. Renaming a window disables its `automatic-rename` option.

## EXAMPLES

Rename the current window:

```sh
tmux rename-window logs
```

Rename window 2 in the current session:

```sh
tmux rename-window -t :2 logs
```
