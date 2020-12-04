# TOU Communication (TCP over UDP comminication)
## How Hardware shuffling applies
Because every packet has to come from a different hardware address
It would be considerably more difficult to implement sliding window protocol when the address needs to change.

There are some considerations I must make in regards to not changing the mac until the packet has been acked. As this is a secure message process, the packages definitely need to be acked. I think that is what I am going to do (11/15/20).


Initially A send  -> B ack -> C send -> ack D ...

Now, A -> B ack -> A transform -> C send -> D ack
