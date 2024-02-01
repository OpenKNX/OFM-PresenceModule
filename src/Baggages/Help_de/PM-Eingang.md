### Eingang Präsenz / Bewegung / weitere Präsenz

Es erscheint je ein Kommunikationsobjekt "Eingang Präsenz", "Eingang Bewegung", "Eingang weitere Präsenz", der das entsprechende Signal eines Slaves empfängt. 

Für jeden dieser Eingänge ist folgende Einstellung ist möglich: 

#### schaltend (EIN und AUS wird ausgewertet)

Der externe Sensor liefert das Signal als schaltendes Objekt. Solange ein EIN-Signal anliegt, ist Präsenz vorhanden, sobald ein AUS empfangen wird, ist die Präsenz weg und die Nachlaufzeit läuft an. Falls während der Nachlaufzeit ein erneutes EIN-Signal anliegt, wird die Nachlaufzeit zurückgesetzt und gestoppt. Beim darauffolgendem AUS läuft die Nachlaufzeit erneut an.

#### triggernd (nur EIN wird ausgewertet)

Der externe Sensor liefert das Signal als Trigger. Sobald ein EIN-Signal anliegt, ist Präsenz vorhanden und die Nachlaufzeit läuft an. Ein erneutes EIN setzt die Nachlaufzeit zurück, d.h. sie läuft erneut an. Ein AUS-Signal wird ignoriert.

Der externe Sensor muss sicherstellen, dass er valide Signale häufiger sendet als die minimale Nachlaufzeit, die verwendet wird. Empfohlen wird ein zyklisches senden, mindestens doppelt so häufig wie die geringste verwendete Nachlaufzeit (Da die Nachlaufzeit tagesphasenabhängig ist und die Kurzzeitpräsenz auch eine Nachlaufzeit hat, muss die kürzeste Nachlaufzeit berücksichtigt werden).

> Achtung: Aus technischen Gründen wird bei triggerndem Betrieb das Eingangs-KO nach dem Empfangen einer 1 sofort wieder auf 0 gesetzt. Da dies ein Eingang ist, hat das üblicherweise keinerlei Auswirkungen. 

