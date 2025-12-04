# Ćwiczenie 3 — RAPORT

## 1. Zaimplementowana polityka przepełnienia (Overflow)

Zaimplementowana polityka przepełnienia bufora cyklicznego (`ring buffer`) polega na odrzucaniu nowych bajtów, gdy bufor jest pełny.

- **Licznik `dropped`**: Każdy odrzucony bajt inkrementuje licznik `dropped` w strukturze bufora. Pozwala to monitorować skalę utraty danych.
- **Flaga `overflow_flag`**: W momencie pierwszego odrzucenia bajtu ustawiana jest flaga `overflow_flag`. Jest ona wykorzystywana przez powłokę (`shell`) do wykrywania "złamanych linii".
- **Licznik `broken_lines`**: Jeśli powłoka wykryje, że nastąpiło przepełnienie (`overflow_flag` jest ustawiona) i w jej wewnętrznym buforze znajduje się niekompletna linia (bez znaku nowej linii), inkrementuje ona licznik `broken_lines` i odrzuca ten fragment. Zapewnia to, że powłoka nie próbuje interpretować częściowych komend i jest gotowa na przyjęcie nowej, kompletnej linii.

## 2. Przebieg testu i analiza logów

Poniżej przedstawiono kluczowe fragmenty logów z wykonania programu testowego, który symuluje masywne przepełnienie bufora wejściowego.

### Logi przed przepełnieniem

Przed symulacją przepełnienia, komendy są przetwarzane poprawnie. Statusy nie zawierają błędów, a komenda `echo` działa zgodnie z oczekiwaniami.

```
READY
set=0.000 ticks=5 drop=0 broken=0
OK set=0.420
rx_free=127 tx_free=127 rx_count=0
OK rate=5
set=0.420 ticks=15 drop=0 broken=0
ECHO hello world
set=0.420 ticks=20 drop=0 broken=0
ECHO BEFORE BURST
```

### Logi w trakcie i po przepełnieniu

Następuje próba zapisu 200 komend `noop\r\n` (łącznie 1200 bajtów) do bufora o rozmiarze 128 bajtów. Powoduje to przepełnienie, utratę danych i wykrycie "złamanej linii".

```
... (poprzednie logi) ...
ECHO BEFORE BURST
ERR
... (wiele linii ERR, aż do opróżnienia bufora z komend 'noop') ...
ERR
set=0.420 ticks=42 drop=1072 broken=1
ECHO AFTER BURST
OK rate=0
set=0.420 ticks=62 drop=1072 broken=1
```

## 3. Wnioski

1.  **Polityka przepełnienia działa poprawnie**: Końcowy status `drop=1072` potwierdza, że nadmiarowe bajty zostały odrzucone, a nie nadpisały istniejących danych.
2.  **Wykrywanie "złamanych linii" jest skuteczne**: Status `broken=1` dowodzi, że powłoka zidentyfikowała i odrzuciła niekompletną komendę, która powstała w wyniku przepełnienia bufora.
3.  **Integralność danych jest zachowana**: Komenda `ECHO AFTER BURST` została poprawnie przetworzona *po* incydencie z przepełnieniem. Dowodzi to, że system jest odporny na błędy tego typu i potrafi odzyskać sprawność, nie mieszając starych, fragmentarycznych danych z nowymi, kompletnymi komendami.