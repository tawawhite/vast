The `vast stop` command gracefully brings down a VAST node, and is the analog of
the `vast start` command.

WHile it is technically possible to shut down a VAST node gracefully by sending
`SIGINT(2)` to the `vast start` process, it is recommended to use `vast stop` to
shut down the node, as it works over the wire as well and guarantees a proper
shutdown. The command blocks execution until the node has quit, and returns a
zero exit code when it succeeded, making it ideal for use in launch system
scripts.
