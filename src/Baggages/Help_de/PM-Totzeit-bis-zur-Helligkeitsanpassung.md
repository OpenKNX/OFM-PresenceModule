### **Totzeit bis zur Helligkeitsanpassung**

Diese Einstellung ist nur wichtig, wenn die [adaptive Ausschaltschwelle](#adaptive-ausschaltschwelle-über-helligkeit) genutzt werden soll.

Ändert sich die Helligkeit im Raum, indem ein weiterer Lichtkreis ein-, ausgeschaltet oder gedimmt wurde, benötigt der entsprechende Helligkeitssensor eine gewisse Zeit, um die neue Helligkeit zu messen. Erst danach macht es Sinn, eine neue Ausschaltschwelle für das Licht zu berechnen.

Die hier angegebene Zeit ist die Pause, in der auf einen neuen Helligkeitswert gewartet wird.

Die Zeit sollte wohlüberlegt sein. Der interne Sensor benötigt für eine Lichtmessung ca. 5 Sekunden. Da intern keine Telegramme verloren gehen können, kann in diesem Fall 5 Sekunden angegeben werden.

Bei einem externen Helligkeitssensor sollte die Zeitspanne mindestens die Zeit sein, mit der der externe Sensor seine Helligkeit zyklisch sendet. Da Telegramme verloren gehen können, wird eher die doppelte Zykluszeit empfohlen. 
Wenn der externe Helligkeitssensor auf Lesetelegramme antwortet und beim lesen wirklich die aktuell gemessene Helligkeit zurückliefert (machen die wenigsten), kann man die Zeit auch kürzer angeben und für die Neuberechnung den Helligkeitswert lesen lassen.

> WICHTIG: Wenn man die [adaptive Ausschaltschwelle](#adaptive-ausschaltschwelle-über-helligkeit) nutzen will, ist es wichtig, dass nach dem einschalten einer neuen Lichtquelle auch ein neuer Helligkeitswert dem Melder vorliegt und er anhand dieses neuen Helligkeitswertes eine neue Ausschaltschwelle berechnen kann. Falls noch mit dem alten Helligkeitswert gerechnet wird, bleibt es bei der alten Ausschaltschwelle und das Licht wird möglicherweise sofort ausgeschaltet.

