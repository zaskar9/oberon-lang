(*
  Oberon Stuetzkurs, Uebung 1

  Temperaturumrechnung

  von Pedro Gonnet (pedro@vis.ethz.ch) und Michael Grossniklaus (michael@vis.ethz.ch)

  Die Situation kennt jeder: man will Brownies, einen Cheezecake oder sonst was Amerikanisches oder
  Britisches backen. Nur eines steht dem kulinarischen und der kalorimetrisch Glueck nur eines im Wege:
  alle Temperaturangaben fuer den Ofen sind in Grad Fahrenheit angegeben.

  Da uns die Brownies am Herzen liegen, wollen wir ein kleines Modul schreiben, welches fuer uns eine
  Tabelle von Celsiuswerten mit den zugehoerigen Fahrenheitswerten ausgibt. Die Tabelle kann man
  dann ausdrucken und am Ofen ankleben, damit man nicht jedesmal, wenn man was backen will, das
  hauseigene Oberonsystem auffahren muss.

  Die Formel zur umberechnung von Grad Celsius nach Grad Fahrenheit sieht -- mathematisch formuliert
  -- wie folgt aus:

    CtoF(c) - (5/9) * c + 32

  Dieses mathematische Kleinod koennen wir, bequemlichkeitshalber, in eine PROCEDURE einpacken,
  Zum Beispiel

    PROCEDURE CtoF ( c : REAL) : REAL;

  Dies berechnet fuer uns jedoch einen Wert, und wir wollen eine ganze Tabelle davon. Wir schreiben
  daher eine PROCEDURE namens Tabelle, welche drei Werte einliest -- von, nach und schritt -- und eine
  Tabelle mit den Celsiuswerten von von nach nach, mit Schritt schritt und deren Fahrenheit-Umrechnung
  ausgibt. Zudem soll geprueft werden, ob die eingegebenen Werten ueberhaupt Sinn machen. Sollte dies
  nicht der Fall sein, wird eine Fehlermeldung ausgegeben und nichts wird berechnet.

  Wer beim theoretischen Teil nicht gepennt hat wird sich daran erinnern, dass wir diese Schleife in mehr
  als einer Variante implementieren koennen. Die Musterloesung hat zwei Varianten, eine WHILE- und eine
  FOR-Schleife.

  Die Ein- und Ausgabe erfolgt ueber den importierten Modulen In und Out. Diese stellen folgende, fuer uns
  sehr wichtige PROCEDUREs zur verfuegung:

    In.Open                 Eingabe initialisieren
    In.Int(i)               Ein INTEGER-Wert einlesen und in der Variablen i ablegen
    Out.Open                Das Ausgabefenster oeffnen (immer wieder)
    Out.Int(i,s)            Den INTEGER-Wert i ausgeben mit s Stellen
    Out.RealFix(r,v,n)      Den REAL-Wert r ausgeben mit v Stellen vor und n Stellen nach dem Komma

  Die werte, welche von In eingelesen werden, muessen nach dem Kommando stehen, zum Beispiel

    Temperatur.Tabelle 0 100 10

  sollte eine Tabelle mit den Weiten von 0 bis 100 in 10er-Schritten ausgeben. Zudem sollte Out.Open besser
  im Modul-Koerper stehen, da es bei jedem Aufruf ein neues Ausgabefenster erzeugt.

*)

MODULE Temperatur;

  IMPORT
    In, Out;

  PROCEDURE CtoF ( c: REAL ) : REAL;
    BEGIN
      RETURN (6/9) * c + 32.0;
    END CtoF;

  PROCEDURE Tabelle*;
    VAR
      von, nach, schritt : INTEGER;
    BEGIN
      In.Open;
      In.Int(von); In.Int(nach); In.Int(schritt);
      IF (von > nach) OR (schritt > (nach - von)) THEN
        Out.String("Gib mir doch eine Eingabe, die Sinn macht!"); Out.Ln;
      ELSE
        WHILE von < nach DO
          Out.Int(von,5); Out.String(" -> ");
          Out.RealFix(CtoF(von),3,2); Out.Ln;
          von := von + schritt;
        END;
      END;
    END Tabelle;

  PROCEDURE TabelleFor*;
    VAR
      von, nach : INTEGER;
      i : INTEGER;
    BEGIN
      In.Open;
      In.Int(von); In.Int(nach);
      IF von > nach THEN
        Out.String("Gib mir doch eine Eingabe, die Sinn macht!"); Out.Ln;
      ELSE
        FOR i := von TO nach BY 10 DO
          Out.Int(i,5); Out.String(" -> ");
          Out.RealFix(CtoF(i),3,2); Out.Ln;
        END;
      END;
    END TabelleFor;

  BEGIN
    Out.Open;
    TabelleFor;
  END Temperatur.

  Temperatur.Tabelle 0 100 10
  Temperatur.TabelleFor 0 100