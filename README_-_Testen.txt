Testen
-------
Voranmerkung: Ein Stub kommuniziert eigentlich nur mit dem Rekursiven Resolver auf 127.0.0.10. Zu Testzwecken spricht er jetzt aber 27.0.0.21 - ns.switch.telematik an. Das kann man an einer Stelle in stub.cpp w�hrend der Initialisierungsphase �ndern - ihr werdet die Stelle erkennen.

Beispiel 1:
1. Erste Bash-Session: ./stub www.switch.telematik
2. Zweite Bash-Session: ./authns ns.switch.telematik
3. Im Stub anfordern mail.switch.telematik - man bekommt vom NS die IP
4. Ein zweites Mal anfordern mail.switch.telematik - man bekommt die IP aus dem Cache.
5. Anfordern bla.zeugs.de - Fehlermeldung

Beispiel 2: Erw�hnte Stelle in stub.cpp �ndern auf 127.0.0.1, das ist der Root NS. 
1. Erste Bash-Session: ./stub www.switch.telematik
2. Zweite Bash-Session: ./authns ns
3. Im Stub anfordern mail.switch.telematik - man bekommt vom NS eine Referenz auf ns.telematik
4. Anfordern bla.zeugs.de - Fehlermeldung
