The `import` command ingests data. An optional filter expression allows for
restricing the input to matching events. The format of the imported data must
be explicitly specified:

```
vast import [options] <format> [options] [expr]
```

The `import` command is the dual to the `export` command.

This is easiest explained on an example:

```
vast import suricata < path/to/eve.json
```

The above command signals the running node to ingest (i.e., to archive and index
for later export) all Suricata events from the Eve JSON file passed via standard
input.

An optional filter expression allows for importing the relevant subset of
information only. To filter Suricata events by their source address field, this
command can be used:

```
vast import suricata 'src_ip == 147.32.84.165' < path/to/eve.json
```

For more information on the optional filter expression, see the [query language
documentation](https://docs.tenzir.com/vast/query-language/overview).

Some import formats have format-specific options. For example, the `pcap` import
format has an `interface` option that can be used to ingest PCAPs from a network
interface directly. A list of format-specific options can be retrieved using the
`vast import <format> help`, and individual documentation is available using
`vast import <format> documentation`.
