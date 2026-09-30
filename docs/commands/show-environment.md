# show-environment

Display variables in a session or global environment.

Alias: `showenv`

## USAGE

```text
tmux show-environment [OPTIONS] [variable]
```

## OPTIONS

- `-g` — Show the global environment. By default, show the target session's environment.
- `-h` — Show hidden variables only. Normal output omits hidden variables.
- `-s` — Format output as Bourne shell commands.
- `-t target-session` — Select the target session. Defaults to the current session.

## NOTES

If `variable` is omitted, all variables in the selected environment are shown. Removed variables are prefixed with `-`; with `-s`, they are printed as `unset` commands. Shell output quotes and escapes variable values.

## EXAMPLES

Show the current session environment:

```sh
tmux show-environment
```

Print a global variable as shell commands:

```sh
tmux show-environment -g -s PATH
```
