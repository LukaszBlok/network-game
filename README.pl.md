# Projekt 2 - Sieciowa Gra "Do 50"

Gra sieciowa dla dwoch graczy komunikujaca sie przez gniazda UDP. Gracze na zmiane podaja liczby, a ten kto osiagnie dokladnie 50 wygrywa runde.

## Zasady gry

1. Pierwszy gracz uruchamiajacy program staje sie **klientem** i laczy sie z drugim graczem
2. Serwer losuje poczatkowa liczbe (1-10)
3. Gracze graja na zmiane - kazdy gracz musi podac liczbe, ktora jest:
   - wieksza od aktualnej liczby
   - co najwyzej o 10 wieksza od aktualnej liczby
   - z zakresu 1-50
4. Gracz ktory osiagnie dokladnie **50** wygrywa runde
5. Po kazdej rundzie role sie zamieniaja - zwyciezca staje sie klientem, przegrany serwerem
6. Wynik jest sledzony przez kolejne rundy

## Kompilacja

```bash
gcc game.c -o game
```

## Uzycie

Obaj gracze uruchamiaja ten sam program. **Pierwszy gracz** ktory go uruchomi staje sie klientem i laczy sie z maszyna drugiego gracza.

```bash
./game <ip_lub_hostname> <port> [nick]
```

- `<ip_lub_hostname>` - adres IP lub nazwa hosta maszyny przeciwnika
- `<port>` - numer portu (musi byc wiekszy niz 1000)
- `[nick]` - opcjonalny nick (jezeli nie podano, uzywany jest lokalny adres IP)

### Przyklad

Gracz 1 (laczy sie z Graczem 2 pod adresem 192.168.1.10):
```bash
./game 192.168.1.10 5000 Alicja
```

Gracz 2 (laczy sie z Graczem 1 pod adresem 192.168.1.5):
```bash
./game 192.168.1.5 5000 Bartek
```

> Obaj gracze musza uzyc tego samego numeru portu.

## Komendy w trakcie gry

W swojej turze mozesz wpisac:

| Komenda | Opis |
|---------|------|
| `<liczba>` | Podaj liczbe (musi byc o 1-10 wieksza od aktualnej, max 50) |
| `wynik` | Wyswietl aktualny wynik (wygrane : przegrane) |
| `koniec` | Zakoncz gre i powiadom przeciwnika |

## Przebieg gry

- Obaj gracze lacza sie ze soba uzywajac tego samego portu
- Program automatycznie obsluguje zamiane rol (serwer/klient) po kazdej rundzie
- Po zakonczeniu rundy przegrany czeka i staje sie nowym serwerem w nastepnej rundzie
- Gdy przeciwnik zakonczy gre, zostaniesz zapytany czy chcesz zaczac nowa gre (`tak`) czy wyjsc (`koniec`)

## Wazne informacje

- Obaj gracze musza byc osiagalni w sieci (ta sama siec LAN lub z przekierowaniem portow)
- Port musi byc wiekszy niz 1000
- Jezeli nie podano nicku, jako nick uzywany jest adres IP lokalnej maszyny
- Program uzywa gniazd **UDP** (bezpolaczeniowych)
