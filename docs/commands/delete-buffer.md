# delete-buffer

Delete a paste buffer.

Alias: `deleteb`

## USAGE

```text
tmux delete-buffer [OPTIONS]
```

## OPTIONS

- `-b buffer-name` — Delete the named buffer.

## NOTES

Without `-b`, tmux deletes the most recently added automatically named buffer. The command reports an error if the named buffer does not exist or there is no automatic buffer to delete.

## EXAMPLES

Delete a buffer by name:

```sh
tmux delete-buffer -b scratch
```
