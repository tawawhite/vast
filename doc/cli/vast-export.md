The `vast export` command retrieves a subset of data according to a given query
expression. The export format must be explicitly specified:

```
vast export [options] <format> [options] <expr>
```

This is easiest explained on an example:

```
vast export --max-events=100 --continuous json '#timestamp < 1 hour ago'
```

The above command signals the running node to export 100 events to the `export`
command, and to do so continuously (i.e., not matching data that was previously
imported). Only events that have a field annotated with the `#timestamp`
attribute will be exported, and only if the timestamp in that field is older
than 1 hour ago from the current time at the node.

The default mode of operation for the `export` command is historical queries,
which exports data that was already archived and indexed by the node. The
`--unified` flag can be used to export both historical and continuous data.

For more information on the query expression, see the [query language
documentation](https://docs.tenzir.com/vast/query-language/overview).

Some export formats have format-specific options. For example, the `pcap` export
format has a `flush-interval` option that determines after how many packets the
output is flushed to disk. A list of format-specific options can be retrieved
using the `vast export <format> help`, and individual documentation is available
using `vast export <format> documentation`.
