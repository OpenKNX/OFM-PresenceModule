### **Eingänge**

Dies ist eine Funktion für erfahrene Benutzer.

Bitte details in der Applikationsbeschreibung nachlesen! 
-->
<!-- ENDDOC -->
<kbd>![Eingänge](pics/InterneEingaenge.png)</kbd>

### **Lesetelegramme beim Neustart**

Bei einem Geräteneustart, sei es durch einen Ausfall der Busspannung, einen Reset auf dem Bus, eine Neuprogrammierung durch die ETS oder dem Drücken der Reset-Taste am Gerät, sind zuerst einmal alle Ein- und Ausgänge initial. Bei Ausgängen ist das nicht kritisch, denn diese werden vom Gerät selbst initialisiert, sobald das Gerät das erste Telegramm sendet. 

Eingänge sind hier kritischer, da die Funktion eines Gerätes gravierend von den Werten an seinen Eingängen abhängen kann. So ist es prima, wenn das Gerät in der Tagesphase "Nacht" als Halbautomat konfiguriert ist und kein Licht einschaltet, sondern nur ausschaltet. Dummerweise gibt es einen kurzen Stromausfall, das Gerät startet neu und ist nach dem Neustart auf der Tagesphase "Abend", die bei der ersten Bewegung die volle Beleuchtung bietet. Oder man hat "einfach mal schnell" wenn alle schlafen mal einen Parameter am PM im Kinderzimmer geändert und das Gerät neuprogrammiert! Das Ergebnis ist das gleiche: Das Gerät ist nach der Neuprogrammierung im Initialzustand und macht bei nächster Gelegenheit das Licht an.

Dieses Gerät bietet die Möglichkeit der dezidierten Einstellung, welcher Eingang nach einem Neustart ein Lesetelegramm auf den Bus senden soll. Dadurch kann das Gerät den Bus abfragen und sich so in einen korrekten Zustand versetzen, wodurch die oben beschriebenen Effekte vermieden werden.

Triggernde Eingänge bieten keine Möglichkeit des Lesens bei Neustart. Das ist auch logisch, wenn man sich klar macht, dass ein Trigger nur in dem Augenblick gültig ist, in dem er empfangen wird. Ein Trigger (EIN-Signal) wird ja nicht zu einem AUS-Signal, sondern er sendet irgendwann einfach kein EIN-Signal mehr. Man weiß also, wenn der Trigger 2 Minuten lang nicht gesendet hat, ist jetzt AUS. Würde man aber das Trigger-KO per Read abfragen, auch nach 5 Minuten, würde es mit einem EIN-Signal antworten.

Eingänge für Szenen können beim Neustart gelesen werden, aber man sollte sich Gedanken dazu machen, ob das Sinn macht. Eine GA für Szenen transportiert eine Szenen-Nummer, auf die potentiell viele Geräte reagieren, andere aber auch nicht (obwohl sie mit der Szenen-GA verbunden sind). Es kann somit sein, dass der PM auf die "Licht AUS"-Szene reagiert, aber nicht auf die "Fernsehen"-Szene, die auf der gleichen GA transportiert wird. Als Mensch würde man nach einem Neustart des PM erwarten, dass er den letzten Zustand wieder herstellt, also den "Licht AUS"-Zustand. Wenn der PM aber die Szenen-GA liest, bekommt er "Fernsehen" als Antwort (weil das einfach die letzte Szene war) und kann damit nichts anfangen. Somit sollten Szenen-GA, die nach einem Neustart gelesen werden, nur Szenen-Telegramme senden, die der PM auch verstehen kann. Dies gilt insbesondere für die Szenen, die die Tagesphasen abbilden.

Auch wenn es trivial ist, soll es hier erwähnt werden: Inaktive Eingänge können natürlich auch keine Lesetelegramme verschicken. 

Bei mehreren mit einem Eingang verbundenen GA wird das Lesetelegramm immer nur an die erste GA (diejenige, die das Sendend-Flag trägt) gesendet. Somit sollte diese GA mit dem Statusobjekt verbunden sein, welches eine qualifizierte Antwort senden kann. 

> Wichtig: Der Neustart eines Gerätes nach dem Neuprogrammieren verhält sich womöglich anders als der Neustart aller KNX-Geräte nach einem Stromausfall. Lesetelegramme, die zu früh abgeschickt werden, erhalten womöglich keine Antwort, weil das angefragte Gerät selbst noch nicht gestartet ist oder noch nicht im korrekten Zustand zum Beantworten von Telegrammen ist. Deswegen kann sowohl der gesamte Präsenzmelder wie auch jeder Präsenzkanal des Melders eine Startverzögerung erhalten, die es erlaubt, erst dann zu starten, wenn alle abhängigen Geräte bereits gestartet sind.

### **Interne Verknüpfungen mit anderen KO**

Normalerweise werden in KNX alle Kommunikationsobjekte über Gruppenadressen verbunden. Zwischen verschiedenen Geräten ist es anders gar nicht möglich.

Diese Applikation enthält aber neben dem Präsenzmodul auch noch ein Logikmodul. Natürlich können Logiken des Logikmoduls mit den Eingängen des Präsenzmoduls über Gruppenadressen kommunizieren. Das führt aber zu zusätzlichen Telegrammen auf dem Bus. Und da Logiken kompliziert sein können, erfordern sie möglicherweise viele GA und damit viele Telegramme.

Interne Eingänge ermöglichen es, einen Eingang direkt mit irgendeinem Kommunikationsobjekt des Gerätes zu verbinden. Da sowohl das Präsenzmodul wie auch das Logikmodul interne Eingänge unterstützen, können beide direkt miteinander kommunizieren, ohne extern über GA verbunden zu werden.

Um zu testen, ob eine beabsichtigte Verknüpfung (z.B. mit Logik) erwartungsgemäß funktioniert, sollte man die Verknüpfung erst einmal immer über GA machen. Dann kann man die GA auf dem Bus über den Gruppenmonitor beobachten und so feststellen, ob die erwarteten Werte im Telegramm stehen. Nachdem man dan festgestellt hat, dass alles funktioniert, kann man die internen Verknüpfungen aufbauen und die GA entfernen, um die Telegrammlast auf dem Bus zu vermindern.

> Wichtig: Es ist immer nur möglich, einen Eingang mit einem anderen KO zu verbinden, egal ob dieses ein Ein- oder Ausgang ist. Es ist allerdings nicht möglich (und auch nicht sinnvoll), Ausgänge miteinander zu verbinden. Somit verbinden man immer einen Eingang vom Präsenzmodul mit einem Ein- oder Ausgang vom Logik- oder Präsenzmodul. Ebenso verbindet man einen Eingang vom Logikmodul mit einem Ein- oder Ausgang vom Logik- bzw. Präsenzmodul.

Folgendes ist noch zu beachten:

> Achtung: Von direkt intern verbundene Kommunikationsobjekten weiß die ETS nichts. Somit können da auch nicht die üblichen Prüfungen der ETS greifen. Es ist die Aufgabe des Benutzers, darauf zu achten, dass die DPT der verbundenen Objekte zusammen passen. Weder die ETS-Applikation noch die Firmware des Gerätes können technisch überprüfen, ob die DPT der verbundenen KO stimmen. Somit kann auch nicht entsprechend gewarnt werden. Aus diesem Grunde werden interne Eingänge nur für erfahrene Benutzer empfohlen.

#### **Lesetelegramme und interne Verknüpfungen**

Wird ein Eingang intern verknüpft, ist dies so, als ob der verknüpfte Eingang am anderen "lauscht", der Eingang bekommt alle Telegramme mit, die bei dem verknüpften KO eingehen. Nichtsdestotrotz ist der verknüpfte Eingang nur eine Art "Gast", das KO "gehört" eine anderen Funktion, z.B. ist es der Eingang einer Logik im Logikmodul. Deswegen hat der verknüpfte Eingang auch keinerlei "Rechte" am verknüpften KO und dann dieses KO nicht irgendwie beeinflussen. 

Deswegen ist es auch logisch, dass ein solcher verknüpfter Eingang auch keine Lesetelegramme beim Neustart absetzen kann. Somit bieten verknüpfte Eingänge auch keine Option zum Senden von Lesetelegrammen an. Man kann aber - sofern es für die Funktion des verknüpften KO zulässt - das fremde KO ein Lesetelegramm senden lassen. Die Antwort würde auch der verknüpfte Eingang erhalten.

### **Mögliche Einstellungen für einen Eingang**

Die Einstellungen für einen Eingang des Präsenzmoduls sind in einer Tabelle angeordnet. Da jede Zeile der Tabelle die gleichen Einstellungsmöglichkeiten bietet, wird hier nur eine Zeile im Detail beschrieben. Für die anderen Zeilen gilt analog das Gleiche.

<kbd>![Eingang mit Standardeinstellungen](pics/EingangDefault.png)</kbd>

Ein Eingang mit seinen Standardeinstellungen sieht wie im Bild oben aus: Der Eingang wird nicht bei Neustart gelesen und ist nicht intern verknüpft.

#### **Spalte Eingang**

Die Spalte Eingang gibt an, um welchen Eingang es sich handelt. Der Name entspricht dem Text des KO, das für diesen Eingang freigeschaltet wird, wenn dieser nicht intern mit einem anderen KO verknüpft ist.

