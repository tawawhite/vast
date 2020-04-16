The `vast start` command spins up a VAST node. Starting a node is the first step
when deploying VAST as a continuously running server. The process runs in the
foreground and uses standard error for logging. Standard output remains unused.

By default, the `vast start` command creates a `vast.db` directory in the
current working directory. It is recommended to set the options for the node in
the `vast.conf` file, such that they are picked up by all client commands as
well.

The node is the core piece of VAST that is continuously running in the
background, and can be interacted with using the `vast import` and `vast export`
commands (among others). To gracefully stop the node, `vast stop` can be used.

Further information on getting started with using VAST can be found on
[docs.tenzir.com](https://docs.tenzir.com/vast/quick-start/introduction).
