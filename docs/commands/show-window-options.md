# show-window-options

Show window options.

Alias: `showw`

## USAGE

```text
tmux show-window-options [OPTIONS] [option]
```

## OPTIONS

- `-F format` — Format each option's output. The format is expanded for each option.
- `-g` — Show global window options. Windows and panes inherit from this scope.
- `-t target-window` — Choose the target window. Its session provides the context for options.
- `-v` — Show only option values. Option names are omitted.

## NOTES

With no `option` argument, lists options for the target window. Use `-g` to list global window options. Pane options can be shown here at window scope; use `show-options -p` for the options attached to a pane.

With the default output, `-v` prints only values; otherwise output includes option names and values. With `-F`, the supplied format controls the output and can use `#{option_value_only}` to test whether `-v` was given.

## EXAMPLES

Show the global key mode used in copy mode:

```sh
tmux show-window-options -g mode-keys
```

Show only the current window's value for automatic renaming:

```sh
tmux show-window-options -v automatic-rename
```
