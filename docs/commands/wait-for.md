# wait-for

Wait on or signal a named channel or event.

Alias: `wait`

## USAGE

```text
tmux wait-for [OPTIONS] name
```

## OPTIONS

- `-E` — Wait for an event named `name`.
- `-F format` — Filter events using a format.
- `-L` — Lock the channel, waiting if it is already locked.
- `-U` — Unlock the channel and admit the next waiter.
- `-S` — Signal the channel and wake its waiters.
- `-l` — List clients waiting on the channel or event.
- `-v` — Print event payload keys while waiting.
- `-w waiter` — Wake a specific waiter by client name.

## NOTES

Without options, the client waits until `wait-for -S` signals the same channel. A signal with no current waiters is remembered for the next waiter. `-L` locks a channel; later lockers wait until it is unlocked with `-U`. Unlocking wakes the next locker. `-S` wakes all current waiters.

With `-E`, `name` is an event name such as a hook or notification name, or a user `@` event. `-F` makes the command wait until the expanded format is true for an event payload. `-v` prints payload keys whether or not the format matches. With `-l`, the command lists clients waiting for the channel or event; `-w` wakes a matching waiter by client name.

## EXAMPLES

Signal a channel and wait for it:

```sh
tmux wait-for -S task-finished
tmux wait-for task-finished
```

Wait for a named event:

```sh
tmux wait-for -E window-linked
```
