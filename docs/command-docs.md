# Command documentation

## Source of truth

Each registered command has one canonical Markdown page at
`docs/commands/<command>.md`. Generated C help and `tmux-COMMAND(1)` pages are
outputs; edit the Markdown page, not generated files. Aliases use the canonical
command page.

Use tmux source as authoritative. Check command names and aliases against
`cmd_entry`, and flags plus required or optional flag arguments against `.args`.
Do not copy `.usage` or `tmux.1` blindly when they disagree with what the command
parser accepts; document the accepted syntax and report source inconsistencies.

## Page format

Keep pages concise and navigable, not guide chapters. Start with the command
name as the H1 and a short description. An optional alias uses this form:

```markdown
Alias: `alias`
```

Only these uppercase sections are supported; `USAGE` and `OPTIONS` are required:

- `USAGE` — one synopsis beginning `tmux command`; include `[OPTIONS]` exactly
  when the page documents options.
- `OPTIONS` — one bullet per flag: ``- `-x argument` — Description.``. Use
  brackets around an optional flag argument and write `None.` for commands with
  no flags. Keep related flags adjacent. Keep the first sentence concise enough
  for generated help (78 columns including the option label).
- `NOTES` — additional behavior and qualifications that do not fit the concise
  option summaries.
- `EXAMPLES` — short, useful examples in fenced code blocks.

Text before the first section is the command description. Do not add other
headings; the generator rejects unsupported sections.

## Generated output and checks

`make` generates command help and individual man pages from these references.
`make install` installs them alongside `tmux(1)`. The command index in `tmux.1`
should link every canonical page and stay synchronized with page summaries and
aliases.

Run `make check` after editing. It checks complete command coverage, aliases,
flags and argument arity against source, synopsis operands against the built
command list, and command-index coverage. It also verifies global and
per-command help, including TTY styling. Render the main page and all generated
command pages with `nroff -mdoc` when changing man-page output.
