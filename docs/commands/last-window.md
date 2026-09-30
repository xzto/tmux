# last-window

Select the previously selected window.

Alias: `last`

## USAGE

```text
tmux last-window [OPTIONS]
```

## OPTIONS

- `-t target-session` — Set the target session.

## NOTES

Without `-t`, the current session is used. The command reports an error if the session has no previously selected window.

## EXAMPLES

Select the previously selected window in the current session:

```sh
tmux last-window
```

Select the previously selected window in the `work` session:

```sh
tmux last-window -t work
```
