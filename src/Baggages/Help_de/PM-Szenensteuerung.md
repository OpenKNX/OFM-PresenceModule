### **Szenensteuerung**

Viele Funktionen, für die ein Kommunikationsobjekt zur Verfügung steht, können auch über Szenen aufgerufen werden.


In der Tabelle können bis zu 10 Szenen ausgewählt werden, die mit entsprechenden Funktionen verbunden werden. Sobald eine der ausgewählten Szenen am Kommunikationsobjekt "PM über Szenen steuern" empfangen wird, wird die verknüpfte Funktion ausgeführt.

Im Folgenden werden die verknüpfbaren Funktionen beschrieben:

### **ändert Helligkeit im Raum**

Wird diese Funktion ausgeführt, wird die adaptive Ausschaltschwelle neu berechnet. Die Funktion ist identisch mit dem Empfang eines Telegramms auf einem der Kommunikationsobjekte "Änderung schalten", "Änderung relativ dimmen", "Änderung absolut dimmen".

### **Automatik übersteuern mit AUS**

Wird diese Funktion ausgeführt, wird der Automatikmodus gestartet und ein AUS-Signal gesendet. Die Funktion ist identisch mit dem Empfang eines AUS-Telegramms auf dem Kommunikationsobjekt "Automatik übersteuern".

### **Automatik übersteuern mit EIN**

Wird diese Funktion ausgeführt, wird der Automatikmodus gestartet und ein EIN-Signal gesendet. Die Funktion ist identisch mit dem Empfang eines EIN-Telegramms auf dem Kommunikationsobjekt "Automatik übersteuern".

### **Manuell übersteuern mit AUS**

Wird diese Funktion ausgeführt, wird der Manuell-Modus gestartet und ein AUS-Signal gesendet. Die Funktion ist identisch mit dem Empfang eines AUS-Telegramms auf dem Kommunikationsobjekt "Manuell übersteuern", wenn "Manuell übersteuern" für den "Zweitastenmodus" konfiguriert ist. 

> Wichtig: Auch wenn bei "Manuell übersteuern" der "Eintastenmodus" konfiguriert ist, verhält sich diese Szene wie beim "Zweitastenmodus"!

### **Manuell übersteuern mit EIN**

Wird diese Funktion ausgeführt, wird der Manuell-Modus gestartet und ein EIN-Signal gesendet. Die Funktion ist identisch mit dem Empfang eines EIN-Telegramms auf dem Kommunikationsobjekt "Manuell übersteuern", wenn "Manuell übersteuern" für den "Zweitastenmodus" konfiguriert ist. 

> Wichtig: Auch wenn bei "Manuell übersteuern" der "Eintastenmodus" konfiguriert ist, verhält sich diese Szene wie beim "Zweitastenmodus"!

### **Manuell aktivieren**

Wird diese Funktion ausgeführt, wird der Manuell-Modus gestartet und der aktuelle Schaltzustand beigehalten. Die Funktion ist identisch mit dem Empfang eines EIN-Telegramms auf dem Kommunikationsobjekt "Manuell übersteuern", wenn "Manuell übersteuern" für den "Eintastenmodus" konfiguriert ist. 

> Wichtig: Auch wenn bei "Manuell übersteuern" der "Zweitastenmodus" konfiguriert ist, verhält sich diese Szene wie beim "Eintastenmodus"!

### **Manuell deaktivieren**

Wird diese Funktion ausgeführt, wird der Manuell-Modus gestoppt und aktuelle Schaltzustand beibehalten. Es wird der passende Automatik-Modus aktiviert, also Automatik-EIN, falls der Ausgang EIN ist, oder Automatik-AUS, falls der Ausgang AUS ist.

Die Funktion ist identisch mit dem Empfang eines AUS-Telegramms auf dem Kommunikationsobjekt "Manuell übersteuern", wenn "Manuell übersteuern" für den "Eintastenmodus" konfiguriert ist. 

> Wichtig: Auch wenn bei "Manuell übersteuern" der "Zweitastenmodus" konfiguriert ist, verhält sich diese Szene wie beim "Eintastenmodus"!

### **Sperren und AUS senden**

Wird diese Funktion ausgeführt, wird der Melder gesperrt und ein AUS-Signal gesendet. Die Funktion ist identisch mit dem Empfang eines EIN-Telegramms auf dem Kommunikationsobjekt "Sperren", wenn die Sperre parametrisiert ist und diese ein AUS-Signal beim Sperren senden soll. Ist eine Rückfallzeit definiert, wird die Sperre nach der angegebenen Zeit aufgehoben.

Diese Funktion ist auch aufrufbar, wenn keine Sperre parametriert ist.

### **Sperren und EIN senden**

Wird diese Funktion ausgeführt, wird der Melder gesperrt und ein EIN-Signal gesendet. Die Funktion ist identisch mit dem Empfang eines EIN-Telegramms auf dem Kommunikationsobjekt "Sperren", wenn die Sperre parametrisiert ist und diese ein EIN-Signal beim Sperren senden soll. Ist eine Rückfallzeit definiert, wird die Sperre nach der angegebenen Zeit aufgehoben.

Diese Funktion ist auch aufrufbar, wenn keine Sperre parametriert ist.

### **Sperren und nichts senden**

Wird diese Funktion ausgeführt, wird der Melder gesperrt und der aktuelle Zustand beibehalten (es wird nichts gesendet). Die Funktion ist identisch mit dem Empfang eines EIN-Telegramms auf dem Kommunikationsobjekt "Sperren", wenn die Sperre parametrisiert ist und diese nichts beim Sperren senden soll. Ist eine Rückfallzeit definiert, wird die Sperre nach der angegebenen Zeit aufgehoben.

Diese Funktion ist auch aufrufbar, wenn keine Sperre parametriert ist.

### **Sperre aufheben und nichts senden**

Wird diese Funktion ausgeführt, wird der Melder entsperrt und der aktuelle Zustand beibehalten (es wird nichts auf den Bus gesendet). Die Funktion ist identisch mit dem Empfang eines AUS-Telegramms auf dem Kommunikationsobjekt "Sperren", wenn die Sperre parametrisiert ist und diese nichts beim Aufheben der Sperre senden soll.

Diese Funktion ist auch aufrufbar, wenn keine Sperre parametriert ist.

### **Sperre aufheben und EIN senden**

Wird diese Funktion ausgeführt, wird der Melder entsperrt und ein EIN-Telegramm auf den Bus gesendet. Die Funktion ist identisch mit dem Empfang eines AUS-Telegramms auf dem Kommunikationsobjekt "Sperren", wenn die Sperre parametrisiert ist und diese ein EIN-Telegramm beim Aufheben der Sperre senden soll.

Diese Funktion ist auch aufrufbar, wenn keine Sperre parametriert ist.

### **Sperre aufheben und AUS senden**

Wird diese Funktion ausgeführt, wird der Melder entsperrt und ein AUS-Telegramm auf den Bus gesendet. Die Funktion ist identisch mit dem Empfang eines AUS-Telegramms auf dem Kommunikationsobjekt "Sperren", wenn die Sperre parametrisiert ist und diese ein UAS-Telegramm beim Aufheben der Sperre senden soll.

Diese Funktion ist auch aufrufbar, wenn keine Sperre parametriert ist.

### **Sperre aufheben und Zustand senden**

Wird diese Funktion ausgeführt, wird der Melder entsperrt und der aktuelle Zustand auf den Bus gesendet. Die Funktion ist identisch mit dem Empfang eines AUS-Telegramms auf dem Kommunikationsobjekt "Sperren", wenn die Sperre parametrisiert ist und diese den aktuellen Zustand beim Aufheben der Sperre senden soll.

Diese Funktion ist auch aufrufbar, wenn keine Sperre parametriert ist.

### **Raum verlassen**

Es wird die [Raum verlassen](#raum-verlassen) Funktion aufgerufen. Es ist identisch mit dem Empfang eines AUS-Telegramms auf dem Kommunikationsobjekts "Automatik übersteuern", wenn "Raum verlassen" mit "Ja" parametrisiert ist.

### **Reset auslösen**

Es wird ein interner Reset des Melders durchgeführt. Die Funktion ist identisch mit dem Empfang eines EIN-Telegramms auf dem Kommunikationsobjekt "Reset".

### **Zur Tagesphase *n* wechseln**

Für jede der Tagesphasen 1-4 gibt es die entsprechende Tagesphasen-funktion.
Es passiert exakt das gleiche wie beim Senden der Tagesphase an das KO "Tagesphase". 

Wird eine Tagesphase aufgerufen, die nicht definiert ist (z.B. Tagesphase 4, obwohl nur 3 definiert sind), wird die aktuelle Tagesphase beibehalten.

Der Tagesphasenwechsel findet so statt, wie er unter [Neue Tagesphase übernehmen](#neue-tagesphase-übernehmen) definiert wurde.

> Anmerkung: Tagesphasen sollten durch Telegramme an das KO "Tagesphase" gewechselt werden. Allerdings lässt sich dort pro Szene nur eine Tagesphase parametriere. Mithilfe der Szenensteuerung kann man auch einer Tagesphase mehrere Szenen zuordnen.

### **Zur Tagesphase *n* sofort wechseln**

Hier gilt alles für [Zur Tagesphase *n* wechseln](#zur-tagesphase-n-wechseln) gesagte, bis auf den Punkt, dass der Tagesphasenwechsel sofort stattfindet, selbst wenn die Parametrierung einen Wechsel bei Zustand AUS vorsieht.

