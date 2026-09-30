# set-option

Set a server, session, window, or pane option.

Alias: `set`

## USAGE

```text
tmux set-option [OPTIONS] option [value]
```

## OPTIONS

- `-a` — Append to the existing option value. For string and style options, appends the supplied value; for arrays, preserves existing members.
- `-F` — Expand formats in the value. Expansion happens before the option is set.
- `-g` — Set a global option. Selects the global session or window options, according to the option's scope.
- `-o` — Set only if the option is unset. An already-set option is an error unless `-q` is also used.
- `-p` — Select pane-option scope. For options available at both pane and window scope, sets the target pane's value.
- `-q` — Suppress option lookup errors. Unknown or ambiguous options, missing scopes, and `-o` conflicts do not report errors.
- `-s` — Select server-option scope. Use for server options; server options are global.
- `-t target-pane` — Choose the target pane. Its session and window provide the context for scoped options.
- `-u` — Unset an option. A local option then inherits its parent value; a global option returns to its default.
- `-U` — Unset a pane option throughout the window. Also removes that option from panes in the target window.
- `-w` — Select window scope for user options. Named options infer their scope from the option definition.

## NOTES

Built-in pane options use window scope by default, so panes inherit the value unless they have their own setting. Use `-p` to set a pane's value, and `-g` for global session or window options. Named options infer their scope; `-s` selects server scope. User options begin with `@` and can be set to any string; `-w` selects window scope for user options, which otherwise default to session scope.

Array options may include a key in square brackets after the option name, such as `command-alias[zoom]`. Without a key, setting an array replaces its members; `-a` appends instead. If `value` is omitted for a flag or choice option, its value is toggled; choice options toggle between their first two choices.

`-u` removes a local value so it inherits from its parent, or restores a global option to its default. `-U` also removes a pane option from panes in the target window.

## EXAMPLES

Set the global session status update interval to 10 seconds:

```sh
tmux set-option -g status-interval 10
```

Set a user option:

```sh
tmux set-option -g @project 'demo'
```
