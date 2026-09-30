# show-prompt-history

Display status prompt history.

Alias: `showphist`

## USAGE

```text
tmux show-prompt-history [OPTIONS]
```

## OPTIONS

- `-T prompt-type` — Select one prompt type. Omit it to show all types.

## NOTES

The supported prompt types are `command` and `search`. History is displayed by type, with entries numbered in order.

## EXAMPLES

Show history for all prompt types:

```sh
tmux show-prompt-history
```

Show command prompt history only:

```sh
tmux show-prompt-history -T command
```
