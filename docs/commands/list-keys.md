# list-keys

List key bindings.

Alias: `lsk`

## USAGE

```text
tmux list-keys [OPTIONS] [key]
```

## OPTIONS

- `-1` — List only the first matching binding. The selected binding follows the chosen sort order.
- `-a` — Include keys without notes when using `-N`. These entries show their commands instead.
- `-F format` — Set the output format. The default prints bindings as `bind-key` commands.
- `-N` — List bindings with notes. By default, only the `root` and `prefix` tables are checked.
- `-O sort-order` — Choose the sort order. Values are `key`, `modifier`, or `name` (table name).
- `-P prefix-string` — Print this string before each key. By default, the configured prefix is used.
- `-r` — Reverse the sort order.
- `-T key-table` — List bindings from this table. With `-N`, this replaces the default `root` and `prefix` tables.

## NOTES

Without a key argument, all matching bindings are listed. A key argument limits output to that key. Unless `-T` is used, the default output includes all key tables; with `-N`, only `root` and `prefix` are checked unless `-T` selects one table. Use `-F` to customize each output line.

## EXAMPLES

List bindings in the prefix table:

```sh
tmux list-keys -T prefix
```

List bindings with notes, including bindings without notes:

```sh
tmux list-keys -N -a
```
