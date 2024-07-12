### Dokumentation


Eine vollständige Applikationsbeschreibung ist unter folgendem Link verfügbar: https://github.com/OpenKNX/OFM-PresenceModule/blob/v1/doc/Applikationsbeschreibung-Praesenz.md

Weitere Produktinformationen sind in unserem Wiki verfügbar: https://github.com/OpenKNX/OpenKNX/wiki/Produktinfo-Pr%C3%A4senzmodul

Dieses Präsenzmelder-Modul implementiert folgende Funktionen:

* "Klassischer Präsenzmelder" mit
  * schalten bei Bewegung
  * ausschalten nach einer gewissen Nachlaufzeit
  * Helligkeitsgesteuert oder Helligkeitsunabhängig
  * 2 Ausgängen pro Kanal mit unterschiedlichen DPT
  * Durchgangsraum, einschalten erst nach einiger Zeit

* Virtueller PM: Verwendbar mit einem oder mehreren Slaves
  * PM ist dann Master für alle Slaves
  * Das Präsenz-Signal kann schaltend oder triggernd sein
  * Helligkeit kann vom gleichen Slave kommen oder von irgendeinem anderen Helligkeitssensor
  * Falls die externe Quelle getrennt Bewegungs- und Präsenzinformationen liefern kann, wird das in dafür geeigneten Funktionen berücksichtigt.

* Tagesphasen
  * Es wird nicht nur Tag/Nacht, sondern bis zu 4 Tagesphasen unterstützt (Morgens, Tag, Abend, Nacht)
  * Tagesphasen können beliebig benannt werden
  * Jede Tagesphase unterstützt eigene Nachlaufzeiten, Helligkeiten, Kurzzeitpräsenz
  * Falls mehr als 4 Tagesphasen benötigt werden, kann man das durch zusammenschalten von Kanälen erreichen

* Innovative Adaptive Ausschaltschwelle über Helligkeit
  * Erlaubt absolute oder relative Helligkeitsschwellen
  * Abschaltung bei externen Lichteinflüssen
  * Beachtet mehrere Lichtkanäle pro Raum

* Automatik- und Manuell- und Sperrmodus
  * Der Moduswechsel ist über Ein- und Zweitastenbedienung vorgesehen
  * Der Moduswechsel kann auch über Szenen vorgenommen werden
  * Einstellbare Rückfallzeiten für Manuell- oder Sperrmodus

* Innovative "Raum verlassen"-Funktion
  * Erlaubt dem Melder die Unterscheidung zwischen "Licht aus und im Raum bleiben" und "Licht aus und Raum verlassen".
  * Schaltet damit Licht genau dann erneut ein, wenn man es benötigt

* Viele Parameter über GA modifizierbar
  * aktuelle Helligkeitsschwelle
  * aktuelle Nachlaufzeit
  * aktuelle Tagesphase
  * Sensibilität des HF-Sensors
  * Szenario für HF-Sensor
  * Sensibilität des PIR-Sensors

* Erweiterte Steuerungsmöglichkeiten
  * alle Melderfunktionen sind über Szenen steuerbar
  * Jeder Eingang kann bei Busspannungswiederkehr Werte lesen
  * KO können auch intern verknüpft werden (Interne KO) und so die Buslast vermindern

