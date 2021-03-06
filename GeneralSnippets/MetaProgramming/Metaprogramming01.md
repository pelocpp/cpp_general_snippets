# Einf�hrung in die Metaprogrammierung

[Zur�ck](../../Readme.md)

---

[Teil II: Metaprogrammierung und Listenverarbeitung](Metaprogramming02.md)

---

## �berblick

C++ -Metaprogrammierung bezeichnet die Technik der Metaprogrammierung innerhalb der Programmiersprache C++,
also eine Technik, um in C++ Programmcode zur �bersetzungszeit generieren zu lassen.

Bei der Templatemetaprogrammierung macht man sich zunutze, dass Templates w�hrend des Kompilierens ausgewertet werden.
So kann man Code schreiben, der zur �bersetzungszeit ausgef�hrt wird. Obwohl sich so die Dauer des Kompilierens verl�ngert, hat das Verfahren den Vorteil, dass es zu einer Verk�rzung der Laufzeit kommen kann.

## Das klassische Beispiel

(siehe auch [Quellcode](MetaProgramming01.cpp))

```cpp
template <int N>
struct Factorial {

    static const int result = N * Factorial<N - 1>::result;

};

template <>
struct Factorial<1> {
    static const int result = 1;
};
```

In den Code-Snippets wird zum Thema "*Metaprogrammierung*" das wohl bekannteste Beispiel demontriert, die Berechnung
der Fakult�t-Funktion zur �bersetzungszeit. Dabei ist zu beachten, dass es auch sehr wohl zur �bersetzungszeit zu Fehler kommen kann.

Diese werden vom Compiler - in meinem Fall als "*warning*" - gemeldet:

warning C4307: '*': signed integral constant overflow
message : see reference to class template instantiation 'Metaprogramming::Factorial<13>' being compiled

Es tritt also der (hinl�nglich bekannte) �berlauf in der Fakult�tfunktion bei Verwendung des Datentyps `int` auf.

Die Ausgabe des Programms - mit Fehlern - sieht auf meinem Rechner wie folgt aus:

```cpp
1
2
6
24
120
720
5040
40320
362880
3628800
39916800
479001600
1932053504
1278945280
2004310016
2004189184
-288522240
```

Nach diesem einf�hrenden Beispiel gehen wir im Folgenden auf wesentliche Techniken der Metaprogrammierung
n�her ein. In einem zweiten Dokument ([Metaprogrammierung und Listenverarbeitung](Metaprogramming02.md)) vertiefen
wir das Thema.

## Parcours durch g�ngige Techniken der Metaprogrammierung

### `static const`

Templates werden beim �bersetzen ausgewertet.
Das bedeutet, dass der gesamte Quellcode beim �bersetzen der Quelle tats�chlich ausgef�hrt wird,
also nicht zur Laufzeit. Da der �bersetzer Klassen - keine Objekte - �bersetzt, k�nnen Templates
nur auf Klassen angewendet werden, es machen also nur Klassenvariablen (`static const`) Sinn.

### Definition neuer Datentypen mit `using`

Wir betrachten ein weiteres Beispiel: Die klassische *"Bruchrechnung"* aus der Schulmathematik.
Beachten Sie am folgenden Beispiel: In der Metaprogrammierung gibt es *keine* Variablen,
es lassen sich aber sehr wohl *neue* Datentypen aus bestehenden Datentypen bilden.
`Frac` ist ebenfalls ein Typ, wenn er an zwei konkrete Werte gebunden wird.
Binden wir diesen Typ mit `using` an einen Namen, k�nnen wir diesen neuen Typen ansprechen,
siehe dazu die Schablone `ScalarMultiplication`:

```cpp
template <int N, int D>
struct Frac {
    static const long Num = N;
    static const long Den = D;
};

template <int N, typename F>
struct ScalarMultiplication {
    using result = Frac<N * F::Num, N * F::Den>;
};
```

*Test*:

```cpp
using TwoThirds = Frac< 2, 3 >;
using Four_Sixths = ScalarMultiplication< 2, TwoThirds >::result;
std::cout << Four_Sixths::Num << "/" << Four_Sixths::Den << std::endl;
```

*Ausgabe*:

```cpp
4/6
```

Es werden also in Schablonen neue Typen gebildet, auf die �ber einen Namen von au�en zugegriffen werden kann!


### Partielle Spezialisierung von Templates

Die partielle Spezialisierung von Templates ist zentrales Hilfsmittel in der Metaprogrammierung.
Wenn wir dieses Feature noch mit der rekursiven Definition von Templates zusammenf�hren,
lassen sich m�chtige Funktionen definieren! Wir demonstrieren dies am Beispiel der Berechnung
des gr��ten gemeinsamen Teilers zweier nat�rlicher Zahlen (ggT):

```cpp
template <int X, int Y>
struct GGT {
    static const long result = GGT<Y, X % Y>::result;
};

template <int X>
struct GGT<X, 0> {
    static const long result = X;
};

template <class F>
struct FracNormalizedVerbose {
    static const long ggt = GGT<F::Num, F::Den>::result;
    static const long newNum = F::Num / ggt;
    static const long newDen = F::Den / ggt;
    using result = Frac<newNum, newDen>;
};

template <class F>
struct FracNormalized {
    static const long ggt = GGT<F::Num, F::Den>::result;
    using result = Frac<F::Num / ggt, F::Den / ggt>;
};
```

Betrachten Sie die Definition von `GGT<X, 0>` an: Dies ist eine partielle Template Spezialisierung!

*Test*:

```cpp
using Four = Frac<16, 4>;
using FourNormalized = FracNormalizedVerbose<Four>::result;
std::cout << FourNormalized::Num << "/" << FourNormalized::Den << std::endl;
using Eight = Frac<32, 4>;
using EightNormalized = FracNormalized<Eight>::result;
std::cout << EightNormalized::Num << "/" << EightNormalized::Den << std::endl;
```

*Ausgabe*:

```cpp
4/1
8/1
```

### Erste Zusammenfassung: Wiederholungsanweisung, bedingte Anweisungen und R�ckgabewerte

In der Metaprogrammierung gelten andere Regeln als in der gew�hnlichen Programmierung. F�r die klassischen
programmiersprachlichen Konstrukte Wiederholungsanweisung, bedingte Anweisung und R�ckgabewert von Funktionen
lauten die �quivalente in der Metaprogrammierung:
 
  * **Wiederholungsanweisung**: Rekursive Template-Definition 
  * **bedingte Anweisung**: Partielle Template Spezialisierung
  * **R�ckgabewert von Funktionen**: `using`-Anweisung (oder `typedef` Anweisung)

Wir betrachten hierzu ein "komplexeres" Beispiel, in dem alle drei Mechanismen angewendet werden:

```cpp
template <typename X1, typename Y1>
struct SameBase {
    using X = typename ScalarMultiplication<Y1::Den, X1>::result;
    using Y = typename ScalarMultiplication<X1::Den, Y1>::result;
};

template <typename X, typename Y>
struct Sum {
    using BASE = SameBase<X, Y>;
    static const long Num = BASE::X::Num + BASE::Y::Num;
    static const long Den = BASE::Y::Den; // same as BASE::X::Den
    using result = typename FracNormalized<Frac<Num, Den>>::result;
};
```

*Hinweis*: Man beachte die Verwendung des Schl�sselworts `typename` in den Schablonen `SameBase` und `Sum`: 
 `typename` wird hier ben�tigt, um den Compiler wissen zu lassen, dass der nachfolgenden Bezeichner ein Typ und kein statischer Member der Klasse (Struktur) ist.

*Test*:

```cpp
using Frac1 = Frac<3, 7>;
using Frac2 = Frac<1, 7>;
using Result = Sum<Frac1, Frac2>::result;
std::cout << Result::Num << "/" << Result::Den << std::endl;
```

*Ausgabe*:

```cpp
4/7
```

### Ein Beispiel: Die Eulersche Zahl *e*

Die Eulersche Zahl *e* l�sst sich mit folgender Formel berechnen:

<img src="euler_number.png" width="300"> 

Abbildung: Reihenentwicklung der Eulerschen Zahl *e*.

Da wir bereits wissen, wie Wiederholungsanweisungen, bedingte Anweisungen und R�ckgabewerte in der Metaprogrammierung
formuliert werden, f�llt uns eine Implementierung nicht schwer:

```cpp
template <int N>
struct E {
    // e = 1/0! + 1/1! + 1/2! + ...
    static const long Den = Factorial<N>::result;
    using Term = Frac<1, Den>;
    using NextTerm = typename E<N - 1>::result;
    using result = typename Sum<Term, NextTerm>::result;
};

template <>
struct E<0> {
    using result = Frac<1, 1>;
};
```

*Test*:

```cpp
using X = E<8>::result;
std::cout << "e = " << (1.0 * X::Num / X::Den) << std::endl;
std::cout << "e = " << X::Num << "/" << X::Den << std::endl;
```

*Ausgabe*:

```cpp
e = 2.71828
e = 109601/40320
```

---

[Teil II: Metaprogrammierung und Listenverarbeitung](Metaprogramming02.md)

---

## Literaturhinweise:

Die Anregungen zu den Beispielen aus diesem Code-Snippet finden sich unter

[Nico Brailovsky's blog](https://monoinfinito.wordpress.com/series/introduction-to-c-template-metaprogramming/)<br>(abgerufen am 20.05.2020).

Eine sehr gute Einf�hrung in diese Thematik findet man bei

[C++ Meta-Programming](http://www.mario-konrad.ch/blog/programming/cpp_metaprogramming.html)<br>(abgerufen am 03.04.2021).

---

[Zur�ck](../../Readme.md)

---
