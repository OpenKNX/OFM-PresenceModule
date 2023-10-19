### **Ausschalten über Helligkeit**

Hier kann man bestimmen, ob ein Ausschalten über Helligkeit gewünscht wird.

#### **nicht aktiv**

Bei der Einstellung wird nicht über Helligkeit ausgeschaltet, sondern nur über Präsenz (bzw. Nichtpräsenz).

#### **absolute Schwelle**

Dies ist ein klassisches Ausschaltverhalten über eine absolute Schwelle. Es wird angegeben, um wie viel es heller werden darf. Überschreitet der gemessene Helligkeitswert diese Schwelle, wird auch bei Präsenz ein AUS-Signal generiert. Da nicht festgestellt werden kann, ob die Helligkeitsüberschreitung durch Sonnenlicht oder durch weitere Lichtquellen erfolgt ist, kann es passieren, dass das Licht ausgeschaltet wird, nachdem man weitere Lichtquellen eingeschaltet hat. Deswegen sollte hier ein hoher Luxwert stehen, sprich: Es muss viel heller werden, bevor Licht über Helligkeit ausgeschaltet wird.

#### **adaptive Schwelle**

Diese Einstellung wählt die neue und innovative [adaptive Ausschaltschwelle](#adaptive-ausschaltschwelle-über-helligkeit), die in diesem Melder implementiert ist. Damit diese korrekt und erwartungskonform funktionieren kann, ist es unbedingt notwendig, dass der Melder von allen Lichtquellen, die die Helligkeit in dem Raum beeinflussen können, entsprechende Informationen über Helligkeitsänderungen bekommt. Dazu muss

* am Kommunikationsobjekt 'Änderung schalten' der Aktorstatus einer jeden Lichtquelle verbunden sein (als hörende Adresse)
* am Kommunikationsobjekt 'Änderung relativ dimmen' alle Dimmadressen einer jeden Lichtquelle verbunden sein (als hörende Adresse)
* am Kommunikationsobjekt 'Änderung absolut dimmen' alle Dimmstatus-Adressen einer jeden Lichtquelle verbunden sein (als hörende Adresse)
* am Kommunikationsobjekt 'PM über Szene steuern' alle Szenen-Adressen aller Lichtquellen, die die Helligkeit beeinflussen, verbunden sein.
* In der Szenensteuerung alle helligkeitsverändernden Szenen aufgelistet sein mit der Funktion 'ändert Helligkeit im Raum'

Dies ist die Maximalforderung für eine sofortige und unmittelbare Reaktion des Melders. Technisch sollte es reichen, alle Lichtquellen-Aktoren-Status mit 'Änderung schalten' und 'Änderung absolut dimmen' zu verbinden.

