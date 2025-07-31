### Ausgang sendet zusätzlich

Ein Ausgang vom PM sendet immer bei einer Änderung des internen Zustands. Wenn also der PM intern vom EIN auf AUS geht oder umgekehrt, wird gesendet. Es gibt Situationen, bei denen dieses Verhalten nicht ausreicht. Bei Szenen will man z.B. für externe Änderungen (Manuell Übersteuern oder Automatik Übersteuern) eventuell ein erneutes Senden des Zustandes bewirken, ohne dass sich dieser geändert hat. Im folgenden können die zusätzlichen Sendebedingungen definiert werden.

#### nichts

Es wird immer nur bei Zustandsänderungen gesendet.

#### bei Tagesphasenänderung

Wird die Tagesphase geändert, sendet der PM nochmal den aktuellen Ausgangszustand auf den Bus.

#### bei externer Änderung

Wird über "Manuell Übersteuern" oder "Automatik Übersteuern" der Melder in seinem Verhalten extern geändert, wird der aktuelle Ausgangszustand auf den Bus gesendet.

#### Bei Tagesphasen- oder externer Änderung

Der aktuelle Ausgangszustand wird auf den Bus gesendet, sobald sich die Tagesphase ändert oder das Verhalten vom Melder über "Manuell Übersteuern" bzw. "Automatik übersteuern" extern geändert wird.

