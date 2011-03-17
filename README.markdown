Ethernet ENC28J60
==================

[Microchip ENC28J60](http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en022889) is an Ethernet controller with SPI interface that is used in some Arduino Ethernet Shields (these are called "etherShield"s). Unfortunatelly, the standard Arduino Ethernet Shield (called "Ethernet Shield") have the [WIZnet W5100](http://www.wiznet.co.kr/Sub_Modules/en/product/Product_Detail.asp?cate1=5&cate2=7&cate3=26&pid=1011) controller - and only the version with W5100 is oficially supported by the Arduino standard library.

There is a library called [etherShield](http://www.nuelectronics.com/download/projects/etherShield.zip) but it is very hard to use (you need to manage ARP and TCP packets in the "application layer"). So, I decided to write a new socket.c (based on etherShield's code) that exposes the same API the standard Ethernet Shield library exposes.

The goal of the project will be achieved as an Arduino user can use the same code for both Ethernet controllers (W5100 and ENC28J60). With this project I can also identify architectural problems in the standard Ethernet library implementation and make it better.

Limitations
===========

- Just one socket per time;
- Now only support for TCP server (connect, sendto and recvfrom don't work yet);


Warning
=======
This is a work-in-progress project and need more tests to be used in production environments.
