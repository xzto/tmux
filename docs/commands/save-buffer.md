# save-buffer

Save a paste buffer to a file.

Alias: `saveb`

## USAGE

```text
tmux save-buffer [OPTIONS] path
```

## OPTIONS

- `-a` — Append to the file instead of overwriting it.
- `-b buffer-name` — Save the named buffer.

## NOTES

If `-b` is omitted, tmux saves the most recently added automatically named buffer.

Use `-` as `path` to write the buffer to standard output. By default, the destination file is overwritten.

## EXAMPLES

Save a named buffer to a file:

```sh
tmux save-buffer -b note /tmp/note.txt
```
