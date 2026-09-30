#!/usr/bin/env python3
"""Generate tmux command help and man pages from Markdown command references."""

import argparse
import glob
import json
import os
import re
import sys
import textwrap


def parse(path):
    text = open(path, encoding="utf-8").read()
    lines = text.splitlines()
    if not lines or not re.fullmatch(r"# [a-z][a-z0-9-]*", lines[0]):
        raise ValueError(f"{path}: first line must be '# command-name'")
    name = lines[0][2:]
    alias = None
    sections = {}
    current = "DESCRIPTION"
    sections[current] = []
    for line in lines[1:]:
        if line.startswith("Alias: "):
            alias = line[7:].strip().strip("`")
        elif line.startswith("## "):
            current = line[3:]
            if current not in {"USAGE", "OPTIONS", "NOTES", "EXAMPLES"}:
                raise ValueError(f"{path}: unsupported section: {current}")
            if current in sections:
                raise ValueError(f"{path}: duplicate section {current}")
            sections[current] = []
        else:
            sections[current].append(line)
    if os.path.basename(path) != name + ".md":
        raise ValueError(f"{path}: filename does not match command name")
    if "USAGE" not in sections or "OPTIONS" not in sections:
        raise ValueError(f"{path}: USAGE and OPTIONS sections are required")
    usage_lines = [line for line in sections["USAGE"] if line.startswith("tmux ")]
    if len(usage_lines) != 1 or not (
        usage_lines[0] == f"tmux {name}" or
        usage_lines[0].startswith(f"tmux {name} [OPTIONS]")
    ):
        raise ValueError(f"{path}: expected one 'tmux {name} [OPTIONS] ...' usage line")
    options = []
    for line in sections["OPTIONS"]:
        if not line.strip() or line == "None.":
            continue
        match = re.fullmatch(r"- `(-[A-Za-z0-9])(?: ([^`]+))?` — (.+)", line)
        if not match:
            raise ValueError(f"{path}: invalid option line: {line}")
        options.append((match.group(1), match.group(2) or "", match.group(3)))
    if len({option[0] for option in options}) != len(options):
        raise ValueError(f"{path}: duplicate option")
    if bool(options) != ("[OPTIONS]" in usage_lines[0]):
        raise ValueError(f"{path}: usage [OPTIONS] must match the option list")
    description = " ".join(line.strip() for line in sections["DESCRIPTION"] if line.strip())
    if not description:
        raise ValueError(f"{path}: missing description")
    return {
        "name": name,
        "alias": alias,
        "description": description,
        "sections": sections,
        "options": options,
        "path": path,
    }


def clean_inline(text):
    return re.sub(r"`([^`]+)`", r"\1", text.strip())


def plain_help(doc):
    title = textwrap.wrap(f"{doc['name']} — {clean_inline(doc['description'])}",
                          width=78, subsequent_indent="  ",
                          break_long_words=False, break_on_hyphens=False)
    out = title + ["", "USAGE"]
    usage = [line for line in doc["sections"]["USAGE"]
             if not line.startswith("```")]
    usage = "\n".join(usage).strip().splitlines()
    out.extend("  " + line.rstrip() for line in usage)
    out.extend(["", "OPTIONS"])
    if not doc["options"]:
        out.append("  None.")
    label_width = max((len(flag + (" " + arg if arg else ""))
                       for flag, arg, _ in doc["options"]), default=0)
    for flag, arg, description in doc["options"]:
        label = flag + (" " + arg if arg else "")
        prefix = "  " + label.ljust(label_width + 2)
        summary = clean_inline(description).split(". ", 1)[0].rstrip(".") + "."
        if len(prefix) + len(summary) > 78:
            raise ValueError(f"{doc['path']}: shorten help summary for {flag}")
        out.append(prefix + summary)
    for heading in ("EXAMPLES", "NOTES"):
        if heading not in doc["sections"]:
            continue
        content = doc["sections"][heading]
        out.extend(["", heading])
        out.extend(render_help_body(content))
    out.extend(["", f"See also: man tmux-{doc['name']}(1)"])
    return "\n".join(out) + "\n"


def render_help_body(lines):
    out = []
    code = False
    for line in lines:
        if line.startswith("```"):
            code = not code
            continue
        if code:
            out.append("  " + line)
        elif line.strip():
            out.extend(textwrap.wrap(clean_inline(line), width=78,
                                     initial_indent="  ", subsequent_indent="  "))
        else:
            out.append("")
    return out


def roff_escape(text):
    text = text.replace("\\", r"\e").replace("-", r"\-")
    if text.startswith(".") or text.startswith("'"):
        text = r"\&" + text
    return text


def man_page(doc):
    out = [
        f".Dd $Mdocdate$",
        f".Dt TMUX-{doc['name'].upper()} 1",
        ".Os",
        ".Sh NAME",
        f".Nm tmux-{doc['name']}",
        f".Nd {roff_escape(clean_inline(doc['description']))}",
        ".Sh SYNOPSIS",
        ".Bd -literal -offset indent",
    ]
    out.extend(roff_escape(line) for line in doc["sections"]["USAGE"]
               if line.strip() and not line.startswith("```"))
    out.extend([".Ed", ".Sh DESCRIPTION"])
    out.extend(render_man_body(doc["sections"]["DESCRIPTION"]))
    if doc["alias"]:
        out.extend([".Pp", f"Alias: {roff_escape(doc['alias'])}."])
    out.append(".Sh OPTIONS")
    if doc["options"]:
        out.append(".Bl -tag -width Ds")
    for flag, arg, description in doc["options"]:
        label = f".It Cm {roff_escape(flag)}"
        if arg:
            label += f" Ar {roff_escape(arg)}"
        out.extend([label, roff_escape(clean_inline(description))])
    if doc["options"]:
        out.append(".El")
    else:
        out.append("None.")
    for heading in ("EXAMPLES", "NOTES"):
        if heading not in doc["sections"]:
            continue
        content = doc["sections"][heading]
        if not any(line.strip() for line in content):
            continue
        out.append(f".Sh {heading}")
        out.extend(render_man_body(content))
    out.extend([".Sh SEE ALSO", ".Xr tmux 1"])
    return "\n".join(out) + "\n"


def render_man_body(lines):
    out = []
    code = False
    para = []

    def flush():
        if para:
            out.extend([".Pp", " ".join(roff_escape(clean_inline(x)) for x in para)])
            para.clear()

    for line in lines:
        if line.startswith("```"):
            flush()
            if code:
                out.append(".Ed")
            else:
                out.append(".Bd -literal -offset indent")
            code = not code
        elif code:
            out.append(roff_escape(line))
        elif line.strip():
            para.append(line.strip())
        else:
            flush()
    flush()
    if code:
        raise ValueError("unclosed code fence")
    return out


def generate_c(docs):
    rows = []
    for doc in docs:
        help_text = plain_help(doc)
        rows.append("\t{ %s, %s, %s }," % (
            json.dumps(doc["name"]),
            json.dumps(doc["alias"]) if doc["alias"] else "NULL",
            json.dumps(help_text)))
    return '''/* Generated by tools/gen-command-docs.py; do not edit. */
#include <string.h>

#include "tmux.h"

struct command_help {
\tconst char *name;
\tconst char *alias;
\tconst char *text;
};

static const struct command_help command_help[] = {
%s
\t{ NULL, NULL, NULL }
};

int
cmd_help(const char *name)
{
\tconst struct cmd_entry *entry;
\tchar *cause = NULL;
\tsize_t i;

\tentry = cmd_find(name, &cause);
\tif (entry == NULL) {
\t\tfprintf(stderr, "%%s\\n", cause);
\t\tfree(cause);
\t\treturn (1);
\t}
\tfor (i = 0; command_help[i].name != NULL; i++) {
\t\tif (strcmp(entry->name, command_help[i].name) == 0) {
\t\t\tcmd_help_print(command_help[i].text);
\t\t\treturn (0);
\t\t}
\t}
\tfprintf(stdout, "%%s\\n\\nUSAGE\\n  tmux %%s %%s\\n\\nSee also: man tmux(1)\\n",
\t    entry->name, entry->name, entry->usage);
\treturn (0);
}
''' % "\n".join(rows)


def source_commands(root):
    entries = {}
    pattern = re.compile(r"const struct cmd_entry\s+\w+\s*=\s*\{(.*?)\n\};", re.S)
    for path in glob.glob(os.path.join(root, "cmd-*.c")):
        with open(path, encoding="utf-8") as source:
            text = source.read()
        for match in pattern.finditer(text):
            block = match.group(1)
            name = re.search(r'\.name\s*=\s*"([^"]+)"', block)
            alias = re.search(r'\.alias\s*=\s*"([^"]+)"', block)
            options = re.search(r'\.args\s*=\s*\{\s*"([^"]*)"', block)
            if name and options:
                spec = options.group(1)
                arguments = {}
                for index, character in enumerate(spec):
                    if character == ":":
                        continue
                    if index + 1 < len(spec) and spec[index + 1] == ":":
                        arguments[character] = (
                            "optional" if index + 2 < len(spec) and
                            spec[index + 2] == ":" else "required")
                    else:
                        arguments[character] = "none"
                entries[name.group(1)] = (alias.group(1) if alias else None,
                                          arguments)
    if not entries:
        raise ValueError(f"no command definitions found under {root}")
    return entries


def check_docs(docs, entries, complete):
    errors = []
    names = {doc["name"] for doc in docs}
    if complete:
        errors.extend(f"missing page: {name}" for name in sorted(entries.keys() - names))
    for doc in docs:
        name = doc["name"]
        if name not in entries:
            errors.append(f"{doc['path']}: command not present in source")
            continue
        alias, arguments = entries[name]
        flags = set(arguments)
        if doc["alias"] != alias:
            errors.append(f"{doc['path']}: alias {doc['alias']!r} != source {alias!r}")
        documented = {option[0][1] for option in doc["options"]}
        if documented != flags:
            errors.append(f"{doc['path']}: flags missing {''.join(sorted(flags - documented))!r}, extra {''.join(sorted(documented - flags))!r}")
        for flag, argument, _ in doc["options"]:
            documented_argument = ("none" if not argument else
                                   "optional" if argument.startswith("[") and
                                   argument.endswith("]") else "required")
            if documented_argument != arguments[flag[1]]:
                errors.append(f"{doc['path']}: {flag} argument arity differs from source")
    if errors:
        raise ValueError("\n".join(errors))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--docs", default="docs/commands")
    parser.add_argument("--c-output")
    parser.add_argument("--man-dir")
    parser.add_argument("--check-all", action="store_true")
    args = parser.parse_args()
    docs = [parse(path) for path in sorted(glob.glob(os.path.join(args.docs, "*.md")))]
    if not docs:
        raise ValueError(f"no command docs found in {args.docs}")
    if len({doc['name'] for doc in docs}) != len(docs):
        raise ValueError("duplicate command page")
    root = os.path.abspath(os.path.join(args.docs, "..", ".."))
    check_docs(docs, source_commands(root), args.check_all)
    if args.c_output:
        with open(args.c_output, "w", encoding="utf-8") as output:
            output.write(generate_c(docs))
    if args.man_dir:
        os.makedirs(args.man_dir, exist_ok=True)
        for doc in docs:
            path = os.path.join(args.man_dir, f"tmux-{doc['name']}.1")
            with open(path, "w", encoding="utf-8") as output:
                output.write(man_page(doc))


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError) as error:
        print(error, file=sys.stderr)
        sys.exit(1)
