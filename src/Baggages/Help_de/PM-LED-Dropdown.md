### LED Präsenz / LED Bewegung

Die beiden Auswahlfelder **LED Präsenz** und **LED Bewegung** steuern vorhandene Hardware-LED. Für beide stehen folgende Auswahlmöglichkeiten zur Verfügung.

Unabhängig von der Einstellung kann für jede Tagesphase eines jeden Kanals eine Sperre für die LEDs eingestellt werden, die ein aufleuchten verhindert.

##### nicht aktiv

Die zugehörige LED wird nicht von der Firmware gesteuert.

##### aktiv bei Bewegung

Die zugehörige LED geht an, sobald von der Hardware eine Bewegung festgestellt wird und geht erst aus, nachdem die Hardware keine Bewegung mehr meldet.

##### aktiv bei Präsenz

Die zugehörige LED geht an, sobald von der Hardware eine Präsenz festgestellt wird und geht erst aus, nachdem die Hardware keine Präsenz mehr meldet.

Ein Präsenzsignal wird derzeit nur vom HF-Sensor erzeugt. Bei einem PIR-Sensor ist kein Präsenzsignal verfügbar.

##### aktiv über externes Objekt

Wird diese Einstellung gewählt, erscheint ein zusätzliches Kommunikationsobjekt, das die Steuerung dieser LED über den KNX-Bus erlaubt.

