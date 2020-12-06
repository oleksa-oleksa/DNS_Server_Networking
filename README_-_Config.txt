Konfigurationsdateien
---------------------

In dns_db/config

Enthalten Vorkonfigurationen f�r alle Hosts. Alle verwendeten Namen sind Fully Qualified Domain Names (FQDN), also mit abschlie�endem "."

Die Recursive Resolver-Config enth�lt einen A-Record f�r sich (dns.server.local) und einen A-Record sowie einen NS-Record f�r den Root NS:
    dns.server.local 315360000 A 127.0.0.10
    ns. 315360000 A 127.0.0.1
    . 315360000 NS ns.

Alle Stub-Configs enthalten einen A-Record f�r sich und einen A-Record f�r den vorkonfigurierten Recursive Resolver, z.B. easy.homework.fuberlin:
    easy.homework.fuberlin. 315360000 A 127.0.0.33
    dns.server.local 315360000 A 127.0.0.10

Alle Auth.NS-Configs enthalten einen A-Record f�r sich, sowie f�r alle Kindelemente einen A- sowie ggf. einen weiteren NS-Record (wenn das Kind ein NS ist), z.B. ns.fuberlin:
    ns.fuberlin 315360000 A 127.0.0.30
    ns.homework.fuberlin. 315360000 A 127.0.0.31
    homework.telematik. 315360000 NS ns.homework.fuberlin.
    ns.pcpools.fuberlin. 315360000 A 127.0.0.32
    pcpools.telematik. 315360000 NS ns.pcpools.fuberlin.
