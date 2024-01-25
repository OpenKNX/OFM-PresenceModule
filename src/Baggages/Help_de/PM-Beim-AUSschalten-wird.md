### Beim AUSschalten wird

Erscheint nur, wenn das Sperrobjekt "Sperre" eingestellt ist.

Der Melder kann beim deaktivieren einer Sperre etwas senden lassen, hier wird bestimmt, was passiert.

> Achtung: Ein EIN- bzw. AUS-Signal ist nicht zu verwechseln mit einem EIN- oder AUS-Telegramm. Das entsprechende Signal wird am Ausgang in das passende Telegramm verwandelt, das in der aktuellen Tagesphase für  EIN- bzw. AUS-Signal gesendet werden soll. Ein EIN-Signal kann somit durchaus zu einer Szene 5 und ein AUS-Signal zu einer Szene 17 führen.

#### nichts gesendet

Beim Deaktivieren der Sperre passiert nichts am Ausgang.

#### AUS gesendet

Beim Deaktivieren der Sperre wird ein AUS-Signal gesendet.

#### EIN gesendet

Beim Deaktivieren der Sperre wird ein EIN-Signal gesendet.

#### Aktueller Zustand gesendet

Beim deaktivieren der Sperre wird das Signal gesendet, dass gerade am Ausgang anliegen würde, wenn der Melder nicht gesperrt gewesen wäre.

Somit funktioniert ein gesperrter Melder intern normal weiter und sendet einfach nur keine Telegramme. Bei dieser Einstellung wird beim Entsperren einfach das letzte unterdrückte Signal gesendet.

