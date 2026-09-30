# list-commands

List the syntax of one or all tmux commands.

Alias: `lscm`

## USAGE

```text
tmux list-commands [OPTIONS] [command]
```

## OPTIONS

- `-F format` — Set the output format. Available fields are `command_list_name`, `command_list_alias`, and `command_list_usage`.

## NOTES

Without `command`, lists the name, alias (if any), and usage of every command. With `command`, lists only that command; aliases may be used.

## EXAMPLES

List all commands:

```sh
tmux list-commands
```

Show the syntax for one command:

```sh
tmux list-commands split-window
```
