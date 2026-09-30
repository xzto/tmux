# clear-prompt-history

Clear status prompt history.

Alias: `clearphist`

## USAGE

```text
tmux clear-prompt-history [OPTIONS]
```

## OPTIONS

- `-T prompt-type` — Select one prompt type to clear. Omit it to clear all types.

## NOTES

The supported prompt types are `command` and `search`. An invalid type is reported as an error.

## EXAMPLES

Clear history for all prompt types:

```sh
tmux clear-prompt-history
```

Clear search prompt history only:

```sh
tmux clear-prompt-history -T search
```
