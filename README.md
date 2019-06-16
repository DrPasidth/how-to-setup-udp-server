# nbIOT-and-udp-Server
Using nbIOT shield to send data to udp server.
This code is for sending data to udp server.
The connection is the same as explained in my first repository.
udp server needs to be configured in your server. you can follow my step below to set up the udp server.
First step:
1. Checking whether the json format has arrived your server:
   - install nc application in your server, in case of centos use
   # yum install nc
2. on your terminal screen, test the incoming string using this command:
   # nc -ulp 2000
3. result amy look like this on your screen:
   {"speed":"0.185","ddate":"160619","position":["13.656687","100.360395"],"temp":"0.00","cnt":"7"}
4. from my experiment, there are some times the data could not be received. Let say you could get the data 3 out of 10 times try.
   probably caused by the quality of shield or may be from the nbIOT bandwidth in my area. Try to relocate it, may be help.
   
