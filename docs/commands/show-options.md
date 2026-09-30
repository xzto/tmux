# show-options

Show session, server, window, or pane options.

Alias: `show`

## USAGE

```text
tmux show-options [OPTIONS] [option]
```

## OPTIONS

- `-A` — Include inherited options. Parent values are marked with `*`.
- `-F format` — Format each option's output. The format is expanded for each option.
- `-g` — Show global options. Selects the global session or window scope.
- `-H` — Include hooks in the output. Hooks are omitted by default.
- `-p` — Show pane options. Uses the target pane's option scope.
- `-q` — Suppress option lookup errors. This includes missing scopes and unknown or unset options.
- `-s` — Show server options. Server options are global.
- `-t target-pane` — Choose the target pane. Its session and window provide the context for options.
- `-v` — Show only option values. Option names are omitted.
- `-w` — Show window options. Uses the target window's option scope.

## NOTES

With no `option` argument, lists options in the selected scope. The default scope is the current session; `-s`, `-w`, or `-p` selects server, window, or pane options, respectively. Use `-g` to show global session or window options. Named options infer their scope from their definitions.

When listing all options, hooks are omitted unless `-H` is used. `-A` includes options inherited from a parent scope and marks them with `*`. With the default output, `-v` prints only values; otherwise output includes option names and values. With `-F`, the supplied format controls the output and can use `#{option_value_only}` to test whether `-v` was given.

## EXAMPLES

Show the global session status interval:

```sh
tmux show-options -g status-interval
```

List global session options as `name=value` lines:

```sh
tmux show-options -g -F '#{option_name}=#{option_value}'
```
