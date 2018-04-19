#### Wojciech Fica

##### Zad. 1.
| adres IP           | adres sieci     | adres rozgłoszeniowy | adres innego komputera |
| ------------------ | --------------- | -------------------- | ---------------------- |
| 10.1.2.3/8         | 10.0.0.0        | 10.255.255.255       | 10.1.2.4               |
| 156.17.0.0/16      | 156.17.0.0      | 156.17.255.255       | 156.17.1.1.            |
| 99.99.99.99/27     | 99.99.99.96     | 99.99.99.127         | 99.99.99.100           |
| 156.17.64.4/30     | 156.17.64.4     | 156.17.64.7          | 156.17.64.6            |
| 123.123.123.123/32 | 123.123.123.123 | 123.123.123.123      | -                      |

##### Zad. 2.
Przykładowy podział:

| adres sieci    | liczba adresów w podsieci |
| -------------- | ------------------------- |
| 10.10.0.0/17   | 2^15                      |
| 10.10.128.0/19 | 2^13                      |
| 10.10.160.0/19 | 2^13                      |
| 10.10.192.0/19 | 2^13                      |
| 10.10.224.0/19 | 2^13                      |

Najmniejsza podsieć może mieć 2^12 adresów. Każda z pięciu podsieci jest jednoznacznie wyznaczona przez pierwszy zapalony bit w 3 bajcie, tj. 

| adres sieci    | liczba adresów w podsieci |
| -------------- | ------------------------- |
| 10.10.128.0/17 | 2^15                      |
| 10.10.64.0/18  | 2^14                      |
| 10.10.32.0/19  | 2^13                      |
| 10.10.16.0/20  | 2^12                      |
| 10.10.0.0/20   | 2^12                      |

Mniej się nie da. Dowód:
Załóżmy, że istnieją 4 liczby naturalne $a \leq b \leq c \leq d \leq e$, że a < 12. 
Wtedy
$$
    2^{16} = 2^a + 2^b + 2^c + 2^d + 2^e = 2^a(1+2^{b-a} + 2^{c-a} + 2^{d-a} + 2^{e-a})
$$
Zatem
$$
    2^{16-a} = 1+2^{b-a} + 2^{c-a} + 2^{d-a} + 2^{e-a}
$$
Skoro lewa strona jest parzysta to $b-a = 0$. Podstawiając $b=a$ do równania wyżej i dzieląc przez $2$ otrzymamy
$$
    2^{15-a} = 1+ 2^{c-a-1} + 2^{d-a-1} + 2^{e-a-1}
$$
Zatem $c-a-1 = 0$. Podobnie wnioskujemy $d-a-2 = 0$ i $e-a-3 = 0$. Zatem mamy $2^{16} = 2^a + 2^a + 2^{a+1} + 2^{a+2} +2^{a+3}$, skąd otrzymujemy $a = 12$. Sprzeczonść. 



##### Zad. 3.

| Lp. | podsieć binarnie                        | podsieć       | dokąd wysyłać | komentarz          |
| --- | --------------------------------------- | ------------- | ------------- | ------------------ |
| 0   | 00000000.00000000.00000000.00000000     | 0.0.0.0/0     | A             |                    |
| 1   | **00001010.00000000.0000000**0.00000000 | 10.0.0.0/23   | B             | merge(1)           |
| 2   | **00001010.00000000.00000010**.00000000 | 10.0.2.0/24   | B             | merge(0), merge(1) |
| 3   | **00001010.00000000.00000011**.00000000 | 10.0.3.0/24   | B             | merge(0)           |
| 4   | **00001010.00000000.00000001**.00000000 | 10.0.1.0/24   | C             |                    |
| 5   | **00001010.00000000.00000000.1**0000000 | 10.0.0.128/25 | B             | redundantny        |
| 6   | **00001010.00000000.00000001.00001**000 | 10.0.1.8/29   | B             |                    |
| 7   | **00001010.00000000.00000001.00010**000 | 10.0.1.16/29  | B             | merge(2)           |
| 8   | **00001010.00000000.00000001.00011**000 | 10.0.1.24/29  | B             | merge(2)           |

Powyższą tablicę redukujemy do poniższej. 

| podsieć binarnie                        | podsieć      | dokąd wysyłać |
| --------------------------------------- | ------------ | ------------- |
| 00000000.00000000.00000000.00000000     | 0.0.0.0/0    | A             |
| **00001010.00000000.000000**00.00000000 | 10.0.0.0/22  | B             |
| **00001010.00000000.00000001**.00000000 | 10.0.1.0/24  | C             |
| **00001010.00000000.00000001.00001**000 | 10.0.1.8/29  | B             |
| **00001010.00000000.00000001.0001**0000 | 10.0.1.16/28 | B             |


##### Zad. 4.
Postępujemy podobnie jak w zadaniu wyżej.

| Lp. | podsieć binarnie                        | podsieć      | dokąd wysyłać | komentarz |
| --- | --------------------------------------- | ------------ | ------------- | --------- |
| 0   | 00000000.00000000.00000000.00000000     | 0.0.0.0/0    | A             |           |
| 1   | **00001010**.00000000.00000000.00000000 | 10.0.0.0/8   | B             |           |
| 2   | **00001010.00000011.00000000**.00000000 | 10.3.0.0/24  | C             |           |
| 3   | **00001010.00000011.00000000.001**00000 | 10.3.0.32/27 | B             |           |
| 4   | **00001010.00000011.00000000.010**00000 | 10.3.0.64/27 | B             | merge(0)  |
| 5   | **00001010.00000011.00000000.011**00000 | 10.3.0.96/27 | B             | merge(0)  |


| podsieć binarnie                        | podsieć      | dokąd wysyłać |
| --------------------------------------- | ------------ | ------------- |
| 00000000.00000000.00000000.00000000     | 0.0.0.0/0    | A             |
| **00001010**.00000000.00000000.00000000 | 10.0.0.0/8   | B             |
| **00001010.00000011.00000000**.00000000 | 10.3.0.0/24  | C             |
| **00001010.00000011.00000000.001**00000 | 10.3.0.32/27 | B             |
| **00001010.00000011.00000000.01**000000 | 10.3.0.64/26 | B             |


##### Zad. 5.

Wystarczy posortować wpisy po długości maski - od najdłuższych masek do najkrótszych. Najdłuższy wpis w tablicy, jaki się dopasuje do danej sieci, dopasuje się wtedy pierwszy.

##### Zad. 6.

Krok 0.

|            | A   | B   | C   | D   | E   | F   |
| ---------- | --- | --- | --- | --- | --- | --- |
| trasa do A | -   | 1   |     |     |     |     |
| trasa do B | 1   | -   | 1   |     |     |     |
| trasa do C |     | 1   | -   |     | 1   | 1   |
| trasa do D |     |     |     | -   | 1   |     |
| trasa do E |     |     | 1   | 1   | -   | 1   |
| trasa do F |     |     | 1   |     | 1   | -   |
| trasa do S | 1   | 1   |     |     |     |     |


Krok 1.

|            | A   | B   | C   | D   | E   | F   |
| ---------- | --- | --- | --- | --- | --- | --- |
| trasa do A | -   | 1   | 2   |     |     |     |
| trasa do B | 1   | -   | 1   |     | 2   | 2   |
| trasa do C | 2   | 1   | -   | 2   | 1   | 1   |
| trasa do D |     |     | 2   | -   | 1   | 2   |
| trasa do E |     | 2   | 1   | 1   | -   | 1   |
| trasa do F |     | 2   | 1   | 2   | 1   | -   |
| trasa do S | 1   | 1   | 2   |     |     |     |


Krok 2.

|            | A   | B   | C   | D   | E   | F   |
| ---------- | --- | --- | --- | --- | --- | --- |
| trasa do A | -   | 1   | 2   |     | 3   | 3   |
| trasa do B | 1   | -   | 1   | 3   | 2   | 2   |
| trasa do C | 2   | 1   | -   | 2   | 1   | 1   |
| trasa do D |     | 3   | 2   | -   | 1   | 2   |
| trasa do E | 3   | 2   | 1   | 1   | -   | 1   |
| trasa do F | 3   | 2   | 1   | 2   | 1   | -   |
| trasa do S | 1   | 1   | 2   |     | 3   | 3   |



Krok 3.

|            | A   | B   | C   | D   | E   | F   |
| ---------- | --- | --- | --- | --- | --- | --- |
| trasa do A | -   | 1   | 2   | 4   | 3   | 3   |
| trasa do B | 1   | -   | 1   | 3   | 2   | 2   |
| trasa do C | 2   | 1   | -   | 2   | 1   | 1   |
| trasa do D | 4   | 3   | 2   | -   | 1   | 2   |
| trasa do E | 3   | 2   | 1   | 1   | -   | 1   |
| trasa do F | 3   | 2   | 1   | 2   | 1   | -   |
| trasa do S | 1   | 1   | 2   | 4   | 3   | 3   |

##### Zad. 7.

Krok 0.

|            | A     | B   | C   | D     | E   | F   |
| ---------- | ----- | --- | --- | ----- | --- | --- |
| trasa do A | -     | 1   | 2   | **1** | 3   | 3   |
| trasa do B | 1     | -   | 1   | 3     | 2   | 2   |
| trasa do C | 2     | 1   | -   | 2     | 1   | 1   |
| trasa do D | **1** | 3   | 2   | -     | 1   | 2   |
| trasa do E | 3     | 2   | 1   | 1     | -   | 1   |
| trasa do F | 3     | 2   | 1   | 2     | 1   | -   |
| trasa do S | 1     | 1   | 2   | 4     | 3   | 3   |



Krok 1.

|            | A   | B   | C   | D   | E   | F   |
| ---------- | --- | --- | --- | --- | --- | --- |
| trasa do A | -   | 1   | 2   | 1   | 2   | 3   |
| trasa do B | 1   | -   | 1   | 2   | 2   | 2   |
| trasa do C | 2   | 1   | -   | 2   | 1   | 1   |
| trasa do D | 1   | 2   | 2   | -   | 1   | 2   |
| trasa do E | 2   | 2   | 1   | 1   | -   | 1   |
| trasa do F | 3   | 2   | 1   | 2   | 1   | -   |
| trasa do S | 1   | 1   | 2   | 2   | 3   | 3   |

##### Zad. 8.
Przedstawiamy poniżej jak rosyłana jest informacja o doległościach do E po sieci. 

- D zauważa, że zespuło się połączenie do E
- D wysyła do B i C informację "mam do E odległość nieskończoną"
- B i C uaktualniają sobie odlegość do E na nieskończoną
- A wysła do C informację "mam do E odległość 3"
- A wysła do B informację "mam do E odległość nieskończoną" (zatruwanie ścieżek)
- C uaktualnia sobie odlegość do E na 4, B tego nie robi, bo stosujemy zatruwanie ścieżek
- C wysyła do D informację "mam do E odległość 4"
- B wysyła do A informację "mam do E odległość nieskończoną"
- A uaktualnia sobie odlegość do E na nieskończoną
- C wysyła do D informację "mam do E odległość 4"
- D uaktualnia sobie odlegość do E na 5
- A wysyła do C informację "mam do E odległość nieskończoną"
- C uaktualnia sobie odlegość do E na nieskończoną
- D wysyła do A informację "mam do E odległość 5"
- ...

##### Zad. 9.
Algorytm stanu łączy: 

- Powiadom wszystkich o swoim bezpośrednim sąsiedztwie. 
- Na podstawie sąsiedztw zbuduj graf sieci i oblicz lokalnie najkrótsze ścieżki

Informacja jest wysyłana na początku + przy zmianie + co jakiś czas (30 min.)

Rozważmy sieć reprezentowaną przez graf składający się z następujących krawędzi:

- A --- B o "odległości" 1
- B --- C o "odległości" 10
- C --- A o "odległości" 1

Następuje przerwanie sieci A --- B. A i B dowiadują się jako pierwsze i uaktualniają sobie odległości do wszystkich wierzchołków w sieci. W momencie, gdy C nie dowiedział się jeszcze o awarii, ma wpisaną odległość do B równą 2 przez A. Ale w tym momencie A ma wpisaną odległość do B równą 11 przez C. Zatem mamy cykl.

##### Zad. 10.

Rozważmy graf z wierzchołkami: $s$ - startowy, $p$ - przedostatni, $k$ - końcowy, $a_i$ i $b_i$ dla $i = 1, ..., \lfloor\frac{n}{2}\rfloor$ i krawędziami:

- s &rarr; $a_1$, s &rarr; $b_1$
- $a_i$ &rarr; $a_{i+1}$, $a_i$ &rarr; $b_{i+1}$
- $b_i$ &rarr; $a_{i+1}$, $b_i$ &rarr; $b_{i+1}$
- $b_{\lfloor\frac{n}{2}\rfloor}$ &rarr; $p$
- $a_{\lfloor\frac{n}{2}\rfloor}$ &rarr; $p$
- $p$ &rarr; $k$

Wtedy przesyłanie informacji od routera $s$ do $k$ zajmie  $2^{\lfloor\frac{n}{2}\rfloor - 1}$. Aby to udowodnić wystarczy zauważyć, że do wierzchołków $a_i$ i $b_i$ zostanie dostarczonych po $2^{i-1}$ pakietów. Zatem do wierzchołka $p$ dotrze $2^{\lfloor\frac{n}{2}\rfloor}$ pakietów więc co najmniej tyle samo czasu zajmie mu przesłanie ich do wierzchołka $k$. 


