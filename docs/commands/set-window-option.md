# set-window-option

Set a window option.

Alias: `setw`

## USAGE

```text
tmux set-window-option [OPTIONS] option [value]
```

## OPTIONS

- `-a` — Append to the existing option value. For string and style options, appends the supplied value; for arrays, preserves existing members.
- `-F` — Expand formats in the value. Expansion happens before the option is set.
- `-g` — Set a global window option. Windows and panes inherit options from this scope.
- `-o` — Set only if the option is unset. An already-set option is an error unless `-q` is also used.
- `-q` — Suppress option lookup errors. Unknown or ambiguous options, missing scopes, and `-o` conflicts do not report errors.
- `-t target-window` — Choose the target window. Its session provides the context for the option.
- `-u` — Unset an option. A local option then inherits its parent value; a global option returns to its default.

## NOTES

Window options apply to the target window; pane options can also be set at window scope so panes inherit the value unless they have their own setting. Use `-g` to change the global window options inherited by windows and panes.

Array options may include a key in square brackets after the option name. Without a key, setting an array replaces its members; `-a` appends instead. If `value` is omitted for a flag or choice option, its value is toggled; choice options toggle between their first two choices.

`-u` removes a local value so it inherits from the global window options, or restores a global option to its default.

## EXAMPLES

Set the global key mode used in copy mode:

```sh
tmux set-window-option -g mode-keys vi
```

Disable automatic renaming for the current window:

```sh
tmux set-window-option automatic-rename off
```
