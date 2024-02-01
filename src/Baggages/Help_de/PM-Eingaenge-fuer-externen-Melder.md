### Eingänge für externen Melder

Dieser Melder kann Präsenz- bzw. Bewegungssignale von externen Meldern verarbeiten. Die externen Melder sind dann Slaves, der (virtuelle) PM agiert als Master.

Die Auswahlliste erlaubt folgende Auswahl:

* **nicht aktiv** - der Melder hat keine Slave-Melder. Es muss die eingebaute Hardware zur Präsenzerkennung genutzt werden. Bei einem reinen virtuellen PM macht diese Einstellung keinen Sinn.
* **Präsenz** - Es wird nur ein Präsenzsignal vom externen Melder geliefert
* **Bewegung** - Es wird nur ein Bewegungssignal geliefert
* **Präsenz und Bewegung** - Es wird ein Präsenz- und Bewegungssignal vom externen Melder geliefert
* **Präsenz und weitere Präsenz** - Es werden 2 Präsenzmelder mit diesem Master verbunden.

Die Unterscheidung zwischen Präsenz- und Bewegungssignal ist rein theoretisch, da beides Bewegung im Raum anzeigt. Die Applikation geht davon aus, dass ein Präsenzsignal auf jeden Fall länger EIN ist als ein Bewegungssignal und es auch nicht so oft zwischen EIN und AUS wechselt. Ferner repräsentiert ein Präsenzsignal auch Mikrobewegungen (Hände beim Umblättern eines Buches) und nicht nur starke Bewegungen (aufstehen, gehen).
Ein Bewegungssignal repräsentiert eher starke Bewegungen und ist nur so lange EIN, wie diese Bewegung stattfindet.
Für die Applikation ist es somit erwartungskonform, wenn ein Bewegungssignal zuerst EIN wird, gefolgt von einem Präsenzsignal. Es ist aber nicht erwartungskonform, wenn das Präsenzsignal AUS geht, bevor die Bewegung AUS ist (Obwohl auch mit dieser Situation versucht wird, korrekt umzugehen).

Damit mehrere Slaves an den Melder ohne weitere Logik angeschlossen werden können, erlaubt jeder Eingang ein triggerndes Signal (sich wiederholende EIN-Signale). Bevorzugt werden aber schaltende Signale (EIN bei Bewegung/Präsenz, AUS bei deren Ende). Schaltende Signale erlauben kürzere Nachlaufzeiten und exaktere Einstellungen. Hat man mehrere schaltende Slaves, dann ist die Empfehlung, diese über ein ODER der enthaltenen Logik zusammenzuführen und dann als schaltendes Signal zu nutzen.

#### Empfohlene Auswahl

Diese Auswahlbox unterstützt einige klassische Konstellationen:

Ein Melder, dessen Nachlaufzeit sich nicht unter 30 Sekunden stellen lässt, sollte als **Präsenz** eingebunden werden.

Melder, die kurze Nachlaufzeiten kleiner 30 Sekunden erlauben, sei es dass es Präsenzmelder oder Bewegungsmelder sind oder klassische über einen Binäreingang eingebundene Melder, sollten mit **Bewegung** eingebunden werden. Gute Erfahrungen wurden mich einer Nachlaufzeit von 7 Sekunden gemacht.

Melder, die eine an sich gute Präsenzerkennung bieten aber auch kurze Nachlaufzeiten, sollten mit 2 Kanälen also **Präsenz und Bewegung** eingebunden werden, wobei der Präsenzkanal etwa 30 Sekunden und der Bewegungskanal 5 bis 7 Sekunden Nachlaufzeit haben sollte.

Einen Sonderfall nimmt hier der True Presence ein. Dieser sollte als **Präsenz und Bewegung** eingebunden werden und die Eingänge sollten mit den rohdaten des TP zur Präsenz und Bewegung verbunden werden (KO 81 und 82 vom TP)

Will man 2 schaltende Slaves anschließen und kein extra ODER spendieren, kann man dies über **Präsenz und weitere Präsenz** einbinden. Es sind hier auch triggernde Slaves denkbar, aber triggernde Slaves (auch mehr als 2) können bei **Präsenz** als hörende Adressen eingebunden werden.  

Wenn der virtuelle PM nur Präsenzinformationen bekommt, sind die Funktionen Kurzzeitpräsenz und Raum verlassen nur mit langen Nachlaufzeiten und somit mit weniger Komfort nutzbar.

