### Informationen zum Sensor

Die Seite versucht, die möglichen Einstellungen verfügbar zu machen. Um sinnvoll Einstellungen vornehmen und den Sensor an die eigenen Bedürfnisse anpassen zu können, muss man aber das Messprinzip verstehen.

Der HLK misst in 16 verschiedenen Entfernungsbereichen (vom Hersteller Ranges genannt) um seinen Einbauort "herum". Eine Range ist immer 70 cm. Die folgende Abbildung zeigt die ersten Ranges um den installierten Sensor (in der Bildmitte). Da der innere Range im Durchmesser 70 cm ist (blaue Linen), wir aber immer die Entfernungen vom Sensor zur Person betrachten, wirkt sich der erste Range nur in einer Entfernung von 35 cm aus (grüne Linie), alle folgenden Ranges dann mit 70 cm (braune Linien).


Für jeden dieser Ranges kann man 2 Werte Festlegen, bei denen der Sensor was machen soll: 
 
1. Einen oberen Trigger-Wert, der bei Überschreitung zum Einschalten führt. 
2. Einen unteren Halten-Wert, der bei Unterschreitung zum Ausschalten führt.

Das ist die aus KNX bekannte Hysterese-Funktion, nur wird die hier für bis zu 16 Ranges definiert.

Die Grundidee für das Messprinzip ist ganz einfach. Der Sensor misst einen Reflexionswert in einer bestimmten Entfernung, wie auch immer dieser ermittelt wird. Jetzt schaut er bei der passenden Hysterese-Einstellung in der entsprechenden Entfernung nach den 2 Werten:

1. Wird der Trigger-Wert überschritten, liefert der Sensor ein EIN-Signal
2. Wird der Halten-Wert unterschritten, liefert der Sensor ein AUS-Signal

Ein Mensch, der sich im Raum befindet, erhöht den Reflexionswert. Eine Bewegung im Raum erhöht den Reflexionswert sogar sehr stark. Dummerweise reflektieren aber auch alle anderen Dinge im Raum, so dass es immer einen Reflexionswert gibt (wir sprechen vom Grundrauschen). Man muss also immer passende Hysterese-Werte finden, die über dem Grundrauschen liegen und trotzdem die erhöhten Werte von Personen im Raum erkennen.

Eine weitere Schwierigkeit liegt in der Tatsache, dass nicht immer der gleiche Reflexionswert gemessen wird. Die Werte schwanken. Deswegen muss man auch mit einer Hysterese arbeiten.

Ferner will man nicht, dass ein kurzer Abfall des Reflexionswertes unter den Haltewert sofort zum AUS führt, wenn die Werte danach wieder über dem Haltewert liegen. Um so etwas zu vermeiden, benötigt man noch eine Haltezeit, die der Sensor im EIN-Zustand bleibt. Erst wenn alle Reflexionen unter dem Halten-Wert für die Haltezeit bleiben, liefert der Sensor ein AUS.

Als letzten Punkt muss es noch die Möglichkeit geben, die Ranges abzuschalten, aus den man keine Reflexionen messen will, weil diese Bereiche einen nicht interessieren. In einem Raum mit 5 m Durchmesser, bei dem der Sensor in der Mitte hängt, will man sicher nicht Entfernungen von mehr als 3 m berücksichtigen.

Mit den obigen Erklärungen sind die Begriffe auf dem oberen Teil der Einstellungen für den HLK eingeführt. Bei einer neuen Applikation sind die Werte für den Sensor mit den Standardeinstellungen des Herstellers ausgefüllt und können direkt so programmiert werden. Diese Einstellungen können funktionieren, müssen aber nicht.



