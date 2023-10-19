### **Sperrobjekt**

Man kann bei diesem Melder verschiedene Arten von Sperren definieren. Solange der Melder im Sperrmodus ist, sendet er keine Telegramme am Ausgang.

#### **nicht aktiv**

Die Sperre ist nicht aktiviert. Das entsprechende Kommunikationsobjekt ist deaktiviert.

#### **Zwangsführung**

Es wird ein Zwangsführungsobjekt DPT 2 sichtbar. Mit den entsprechenden Zwangsführungssignalen kann folgendes Verhalten bewirkt werden:

* normal, AUS (00) - Der Melder geht in den Automatik-Modus und schaltet aus
* normal, EIN (01) - Der Melder geht in den Automatik-Modus und schaltet ein
* priorität, AUS (10) - Der Melder geht in den Sperrmodus und schaltet aus
* priorität, EIN (11) - Der Melder geht in den Sperrmodus und schaltet ein

#### **Sperre**

Es wird ein Sperrobjekt DPT 1 sichtbar. Beim Empfang des entsprechenden Sperrsignals wird die Sperre aktiviert bzw. deaktiviert.

