# unbind-key

Remove a key binding or all bindings in a key table.

Alias: `unbind`

## USAGE

```text
tmux unbind-key [OPTIONS] [key]
```

## OPTIONS

- `-a` — Remove all bindings in a table. Do not give `key`; use `-T` or `-n` to choose the table.
- `-n` — Use the root key table. This is the same as `-T root`.
- `-q` — Suppress errors. This includes errors for a missing key or table.
- `-T key-table` — Choose the key table to search. Defaults to `prefix`, unless `-n` is used.

## NOTES

A key is required unless `-a` is used, and `-a` cannot be combined with a key. Without `-T` or `-n`, the key is removed from the `prefix` table. If both are given, `-T` selects the table.

## EXAMPLES

Quietly remove F12 from the prefix table if it is bound:

```sh
tmux unbind-key -q -T prefix F12
```
