Ethernet ENC28J60
==================

[Microchip ENC28J60](http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en022889) is an Ethernet controller with SPI interface that is used in some Arduino Ethernet Shields (these are called "etherShield"s). Unfortunatelly, the standard Arduino Ethernet Shield (called "Ethernet Shield") have the [WIZnet W5100](http://www.wiznet.co.kr/Sub_Modules/en/product/Product_Detail.asp?cate1=5&cate2=7&cate3=26&pid=1011) controller - and only the version with W5100 is oficially supported by the Arduino standard library.

There is a library called [etherShield](http://www.nuelectronics.com/download/projects/etherShield.zip) but it is very hard to use (you need to manage ARP and TCP packets in the "application layer"). So, I decided to write a new socket.c (based on etherShield's code) that exposes the same API the standard Ethernet Shield library exposes.

The goal of the project will be achieved as an Arduino user can use the same code for both Ethernet controllers (W5100 and ENC28J60). With this project I can also identify architectural problems in the standard Ethernet library implementation and make it better.


<div style="height: 350px; line-height: 350px; text-align: center">
    <img src="http://www.CursoDeArduino.com.br/media/ArduinoEthernetShield.jpg" style="vertical-alignt: middle; max-height: 100%" />
</div>
<br />
<div style="text-align: center">
    Arduino with etherShield (based on Microchip ENC28J60 Ethernet controller)
</div>


Usage
=====

[Download the tarball at GitHub](https://github.com/turicas/Ethernet_ENC28J60/tarball/master) and put the library inside `sketchbook/libraries/` folder.
If you just want to test without debugging, use the examples `WebServerSimple` and `WebServerSimpleLED`.

**WARNING:** This is a work-in-progress project and need more tests to be used in production environments!

Using with Arduino Mega
-----------------------

This library is compatible with Arduino Mega, but **you need a hardware-hack** so ENC28J60 can communicate (using SPI) with Arduino. Please refer to <http://mcukits.com/2009/04/06/arduino-ethernet-shield-mega-hack/> for more information.


Limitations
===========

For now that are some limitations that will be removed in a near future:

- Just one socket per time;
- Only support for TCP server -- `connect()`, `sendto()` and `recvfrom()` don't work yet;
- Can only "answer" to the last packet received.

Architecture
============

WIZnet W5100 Library
--------------------

In the standard Ethernet library, the Ethernet, Server and Client classes use the socket API to send and received data over the network. The socket library uses the "driver", (w5100 library) to communicate with the controller. Something like this:


`{Ethernet.cpp, Server.cpp, Client.cpp}` &harr; `socket.c` &harr; `w5100.c`


Microchip ENC28J60 Library
--------------------------

In my implementation I have another layer: ip_arp_udp_tcp. This is a kind-of socket layer (it is really not a socket layer since it doesn't provide a kind of socket API -- merely we have here a lot of helper functions to read, identify, create and send packets using the "driver" enc28j60). For now, we have this:


`{Ethernet.cpp, Server.cpp, Client.cpp}` &harr; `socket.c` &harr; `ip_arp_udp_tcp.c` &harr; `enc28j60.c`

In a near future I want to replace all ip_arp_udp_tcp layer in the socket layer, so the architecture will be more like the standard Ethernet library:


`{Ethernet.cpp, Server.cpp, Client.cpp}` &harr; `socket.c` &harr; `enc28j60.c`

When this goal is reached we can create a single socket.c that communicate with one or another controller (W5100 or ENC28J60).

Next Goals
==========

- Finish implementation of `connect()`
- Modify `send()` to acomplish TCP client sockets
- Implement `sendto()` and `recvfrom()`
- Support for multiple sockets
- Put all the features of `ip_arp_udp_tcp.c` in `socket.c` (and then removes `ip_arp_udp_tcp.*`)


How To Debug
============

If do you want to test and modify this library, probably you will want also to enable debugging.
You can see what is happening using the Serial Monitor on Arduino IDE if you uncomment the line below in the file `utility/socket.h`:

    //#define ETHERSHIELD_DEBUG

Please use the examples `WebServerDEBUG` and `WebClientDEBUG` for debugging (and to learn how to do it).
