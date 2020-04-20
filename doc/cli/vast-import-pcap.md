The PCAP import format uses [libpcap](https://www.tcpdump.org) to read network
packets from a trace or an interface.

VAST automatically calculates the [Community
ID](https://github.com/corelight/community-id-spec) for PCAPs for better
pivoting support. The extra computation induces an overhead of approximately 15%
of the ingestion rate. The option `--disable-community-id` can be used to
disable the computation completely.

The PCAP import format has many additional options that offer a user interface
that should be familiar to users of other tools interacting with PCAPs. A full
list of options can be retrieved using `vast import pcap help`. Here's an
example that reads from the network interface `en0` cuts off packets after 65535
bytes.

```sh
sudo vast import pcap -i en0 -c 65535
```
