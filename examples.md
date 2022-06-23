critical features:
- packets
- queues
- packets
- retries

detection options:
- ratio of goodput to throughput
- (queue depth) and (ratio of retry packets to actual packets)

Ben's claim is that (queue depth) and (ratio of retry packets to actual packets) is the definition of congestive collapse.

Bob's claim is that monitoring (ratio of goodput to throughput) as a function of time would be capable of detecting congestive collapse.

model: 
- sender application puts packets in a "to send" queue
- receiver application receives packets from the "from" queue
- send queue is connected to from queue by a link

