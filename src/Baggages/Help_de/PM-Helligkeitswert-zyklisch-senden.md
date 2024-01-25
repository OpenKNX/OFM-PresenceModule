### Helligkeitswert zyklisch senden

Die gemessene Helligkeit kann auf den Bus gesendet werden. Mit dieser Einstellung wird das Zeitintervall zum erneuten Senden festgelegt.

Wird das Zeitintervall von 2 Minuten eingestellt, wird alle 2 Minuten die aktuell gemessene Helligkeit auf den Bus gesendet.

Wird während eines Zeitintervalls die Helligkeit wegen Änderung gesendet, so beeinflusst das nicht das Zeitintervall. Es kann somit passieren, dass kurz vor oder nach dem zyklischen Senden erneut wegen einer Änderung der Helligkeit gesendet wird.

> Wenn weder zyklisch noch aufgrund von einer Änderung gesendet werden soll (beide Werte auf 0), wird die Helligkeit gar nicht aktiv auf dem Bus gesendet. Sie kann immer noch über ein Lesetelegramm gelesen werden.

