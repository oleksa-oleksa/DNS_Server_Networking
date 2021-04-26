Telematics Projects

Project 01:

In  this  project,  we  got  hands-on  experiences  with  core  services  running  on  top  of  theInternet, DNS and HTTP. We implemented own DNS system and a HTTP proxy that uses our DNS.

The goal is to implement basic functionalities of a stub resolver, a recursive resolver, as well asan authoritative DNS server, and to operate namespaces.  Approach should be distributed and scalable.  

Milestones:
(a)  Your stub resolver is able to request an A record from the authorative server.
(b)  Your recursive resolver is able to discover the authoritative server of a name,  andresolve the A record for this name.
(c)  Your  stub  resolver  is  able  to  resolve  any  name  in  the  list  and  profits  from  faster replies in the case of cache hits at the recursive resolver.
(d)  Your DNS implementation is used by an application (see HTTP proxy below).
