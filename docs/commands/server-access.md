# server-access

Change or list server access and write permissions.

## USAGE

```text
tmux server-access [OPTIONS] [user|group]
```

## OPTIONS

- `-a` — Allow access for the user or group.
- `-d` — Deny access for the user or group. Matching clients may be detached.
- `-g` — Treat the argument as a group instead of a user.
- `-l` — List current access permissions.
- `-r` — Make matching clients read-only.
- `-w` — Allow matching clients to write.

## NOTES

A `user` or `group` is required unless `-l` is used. `-a` and `-d` cannot be combined, nor can `-r` and `-w`. Using `-r` or `-w` for an entry that does not exist adds it. The server-owner and root-user entries cannot be changed; user permissions take precedence over group permissions when both match. Only a client's effective group ID is considered, not supplementary groups.

The default access list is empty, and the server socket is normally protected by filesystem permissions. Take care not to allow access to untrusted users, even as read-only users.

## EXAMPLES

List current server access permissions:

```sh
tmux server-access -l
```
