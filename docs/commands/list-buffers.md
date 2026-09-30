# list-buffers

List the server's paste buffers.

Alias: `lsb`

## USAGE

```text
tmux list-buffers [OPTIONS]
```

## OPTIONS

- `-F format` — Set the format for each output line.
- `-f filter` — Filter buffers with a format.
- `-O sort-order` — Set the sort order.
- `-r` — Reverse the sort order.

## NOTES

The default output shows each buffer's name, size, and a sample of its contents. `-f` takes a format; buffers for which it evaluates to zero are omitted. If a filter would hide every buffer, it is ignored. Sort orders are `name`, `size`, and `creation` (time).

## EXAMPLES

List buffers using the default format:

```sh
tmux list-buffers
```

List buffers by size, largest first:

```sh
tmux list-buffers -O size -r
```
