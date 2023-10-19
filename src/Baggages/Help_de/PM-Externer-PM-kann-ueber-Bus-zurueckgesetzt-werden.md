### **Externer PM kann über Bus zurückgesetzt werden**

Manche externe PM können über ein KO zurückgesetzt werden. Auch wenn es in der Applikation als "Reset" bezeichnet wird, ist damit nicht die ETS-Funktion "Gerät zurücksetzen" gemeint, sondern die Möglichkeit, den PM in einen Zustand zu versetzen, der die aktuelle Präsenzerkennung und alle zugehörigen Nachlaufzeiten zurücksetzt und unmittelbar auf die nächste erkannte Bewegung bzw. Präsenz ein Signal schickt. Besitzt der externe Melder einen solchen Eingang, kann dieser für die erweiterten Funkionen wie "Kurzzeitpräsenz" oder "Raum verlassen" genutzt werden. 

In dem Eingabefeld gibt man an, ob der externe PM mit einem EIN-Signal oder einem AUS-Signal zurückgesetzt werden kann. 

Es erscheint ein KO "Externen PM zurücksetzen", das mit dem "Zurücksetzen"-Eingang des externen PM verbunden werden muss.

#### **Nein**

Wird dieser Wert in der Auswahlbox gewählt, kann der externe PM nicht über ein Telegramm zurückgesetzt werden.

#### **Mit einem EIN-Signal**

Wird dieser Wert in der Auswahlbox gewählt, kann der externe PM mit einem EIN-Signal zurückgesetzt werden. Der VPM schickt zu gegebener Zeit einen EIN-Trigger an den externen PM.

#### **Mit einem AUS-Signal**

Wird dieser Wert in der Auswahlbox gewählt, kann der externe PM mit einem 
AUS-Signal zurückgesetzt werden. Der VPM schickt zu gegebener Zeit einen AUS-Trigger an den externen PM.

