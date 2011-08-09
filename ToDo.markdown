To Do / Bugs
============

- Put license information (file LICENSE and/or GPLv2 headers)
- SYNACK (when a request comes) -> win size is changed. why?
- ACK number when receive an ACK packet with data is INCORRECT! ACK number should be the last received packet len (of this connection) + 1
- send is not working properly: disconnect that is really sending packets
- recv is not working for length > 1
- What if we receive/send a packet size greater than BUFFER\_SIZE?
- Implement connect()
  - SYN+ACK is not being received/read (maybe the SYN we are sending is not ok)
- Implement send()
