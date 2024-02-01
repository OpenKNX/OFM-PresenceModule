### Spalte Neues KO erzeugen oder ein bestehendes KO nutzen

In dieser Spalte kann man angeben, ob man eine interne Verknüpfung mit einem anderen KO haben möchte. 

Die Auswahl "Neues KO" bedeutet, dass dieser Eingang ein eigenes (neues) KO bekommt und nicht intern verknüpft ist. 


Die Auswahl "Bestehendes KO" erlaubt die Eingabe einer KO-Nummer. Der Eingang ist dann intern mit diesem KO verknüpft. Das ist vergleichbar mit einer externen Verknüpfung mittels einer GA. 

> Wichtig: Es kann jedes aktive KO der gesamten ETS-Applikation ausgewählt werden, egal ob es sich um das Präsenzmodul oder das Logikmodul handelt. Bei nicht aktiven KO *kann* die Verknüpfung funktionieren, muss es aber nicht. Es wird ganz klar empfohlen, nur mit aktiven (in der ETS-KO-Liste sichtbaren) KO zu verknüpfen.

> Achtung: Der DPT vom verknüpften KO muss unbedingt dem DPT des Eingangs entsprechen, der verknüpft wird. Weder die ETS noch die Firmware im Gerät hat eine Möglichkeit, hier etwas zu überprüfen. Bei nicht übereinstimmenden DPT wird es im einfachsten Falle "nur" nicht funktionieren, im schlechtesten Fall wird sich das Gerät aufhängen.

