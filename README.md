# ipspoof DOS Tool

### Disclaimer
**What you do with this software is your thing and the resulting consequences are your problem!**

**It's illegal to target this software at any computer/server you don't have the permisson to attack!**

### What is this?
This is a simple C program I wrote, which sends spoofed ICMP Echo requests over the network.

Setting the destination address to a broadcast address (e.g. 255.255.255.255) and the "source" address to a target computer, one can use the power of all PCs in a network to DOS a single target. **WARNING: This actually is quite powerful, and will likely crash something, like a router, along the line so do not use this in a real network!**

For further information search for "Smurf DOS". The idea is not mine, this is just an implementation.
