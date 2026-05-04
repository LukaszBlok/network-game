#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define BUF_SIZE  1024

char* getHostIPAddress() { // ta funkcja zwraca moje ip w postaci kropkowo dziesietnej
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("Błąd podczas pobierania nazwy hosta");
        return NULL;
    }

    struct hostent* host_info = gethostbyname(hostname);
    if (host_info == NULL) {
        perror("Błąd podczas pobierania informacji o hoście");
        return NULL;
    }

    struct in_addr** addr_list = (struct in_addr**)host_info->h_addr_list;
    if (addr_list[0] != NULL) {
        char* ip_address = malloc(INET_ADDRSTRLEN);
        if (ip_address == NULL) {
            perror("Błąd podczas alokacji pamięci na adres IP");
            return NULL;
        }

        if (inet_ntop(AF_INET, addr_list[0], ip_address, INET_ADDRSTRLEN) != NULL) {
            return ip_address;
        }
        else {
            perror("Błąd podczas konwersji adresu IP na ciąg znaków");
            free(ip_address);
            return NULL;
        }
    }
    else {
        printf("Brak dostępnych adresów IP dla tego hosta\n");
        return NULL;
    }
}

// poniższe trzy funkcje sluza do sprawdzenia czy pierwszy argument wywolania funkcji byl poprawny i zwrocenie ip w postaci kropkowo dziesietnej
char* getIPAddress(const char* hostname) { // ip hosta w postaci kropkowo dziesietnej
    struct addrinfo hints, * res, * p;
    char ipstr[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return NULL;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
        void* addr = &(ipv4->sin_addr);
        inet_ntop(AF_INET, addr, ipstr, sizeof ipstr);
    }

    freeaddrinfo(res);

    if (ipstr[0] == '\0') {
        fprintf(stderr, "Nie można uzyskać adresu IP dla hosta: %s\n", hostname);
        return NULL;
    }

    char* ip_string = strdup(ipstr);
    if (ip_string == NULL) {
        perror("Błąd alokacji pamięci");
        return NULL;
    }

    return ip_string;
}

int isDotAndDigits(const char* str) { // czy zostalo podane ip w wersji krokpokowo dziesietnej
    if (str == NULL) {
        return 0;
    }

    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]) && str[i] != '.') {
            return 0;
        }
    }

    return 1;
}

char* getValidIPAddress(const char* str) { // uzywamy dwoch poprzednich funkcji do sprawdzenia czy jest taki host i zrocenia jego ip
    int czyOK = isDotAndDigits(str);
    if (czyOK == 1) {
        return strdup(str);
    }
    else {
        char* ipAddress = getIPAddress(str);
        if (ipAddress != NULL) {
            return ipAddress;
        }
        else {
            fprintf(stderr, "Błąd: Niepoprawny adres IP lub błąd podczas pobierania adresu\n");
            exit(EXIT_FAILURE);
        }
    }
}


// Ta funkcja sluzy nam do czyszczenia bufora ( tablicy charow )
void cleanArray(char array[], size_t len) {
    memset(array, 0, len);
}



// zappytanie uytkownika o podanie akcji , poprawna obsluga komunikatu ''wynik'' i bledngeo napisu, zwrocenie ''koniec'' lub liczby w postaci napisu
char* getUserInput(int parameter, int wygrane, int przegrane) {
    char input[1024];
    char* result = NULL;

    do {
        printf("Podaj liczbe, albo wpisz \"koniec\" lub \"wynik\": ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "Błąd podczas wczytywania wejścia.\n");
            exit(EXIT_FAILURE);
        }

        size_t length = strlen(input);
        if (length > 0 && input[length - 1] == '\n') {
            input[length - 1] = '\0';
        }
        else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        if (strcmp(input, "koniec") == 0) {
            result = strdup(input);
        }
        else if (strcmp(input, "wynik") == 0) {
            printf("Twoj wynik %d : %d \n", wygrane, przegrane);

        }
        else {
            int isValidNumber = 1;
            size_t i;
            for (i = 0; i < length - 1; ++i) {
                if (!isdigit(input[i])) {
                    isValidNumber = 0;
                    break;
                }
            }

            if (isValidNumber) {
                int number = atoi(input);
                if (number >= 1 && number <= 50 && number > parameter && number <= parameter + 10) {
                    result = strdup(input);
                }
            }
        }

        if (result == NULL && strcmp(input, "wynik") != 0) {
            printf("Podano niepoprawny napis. Proszę spróbować ponownie.\n");
            cleanArray(input, sizeof(input));
        }

    } while (result == NULL);

    cleanArray(input, sizeof(input));

    return result;
}


// ta funkcja czy podalismy dozwolony port
int isNumberGreaterThan1000(const char* str) {
    if (str == NULL || *str == '\0') {
        return 0;  // Empty string or NULL is not a valid number
    }

    const char* originalStr = str;

    while (*str) {
        if (!isdigit(*str)) {
            return 0;  // Non-digit character found
        }
        str++;
    }

    int number = atoi(originalStr);// Convert the string to an integer

    if (number <= 1000) {
        return 0;
    }
    else {
        return 1;
    }
}


// ta funkcja sprawdza, czy uzytkownik podal napis ''koniec'' lub ''tak''
char* czyDalejGrac() {
    char input[1024];
    char* result = NULL;

    do {

        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "Błąd podczas wczytywania wejścia.\n");
            exit(EXIT_FAILURE);
        }

        size_t length = strlen(input);
        if (length > 0 && input[length - 1] == '\n') {
            input[length - 1] = '\0';
        }
        else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        if (strcmp(input, "koniec") == 0 || strcmp(input, "tak") == 0) {
            result = strdup(input);
        }
        else {

            printf("Podano niepoprawny napis. Proszę spróbować ponownie : ");
            cleanArray(input, sizeof(input));
        }

    } while (result == NULL);

    cleanArray(input, sizeof(input));

    return result;
}



// Ta fukcja sprawia, ze jestesmy serwerem - inicjalizujemy gre

void udpServer(const char* port, const char* message, int wygrane, int przegrane, char* ip) {

    // printf("jestem serwerem\n");

    int sfd, s;
    char buf[BUF_SIZE];
    ssize_t nread;
    socklen_t peer_addrlen;
    struct addrinfo hints;
    struct addrinfo* result, * rp;
    struct sockaddr_storage peer_addr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(NULL, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break; /* Success */

        close(sfd);
    }

    freeaddrinfo(result); /* No longer needed */

    if (rp == NULL) { /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }



    srand(time(NULL));
    int losowaLiczba = rand() % 10 + 1;
    printf("Losowa wartosc poczatkowa: %d\n", losowaLiczba);

    char nick[1024];

    int j = 1;
    for (;;) {

        //      printf("wejsciowa wartosc j : %d\n", j);

        if (j > 2) {
            cleanArray(buf, sizeof(buf));
            printf("Teraz nie jest twoja kolej. Poczekaj\n");
        }

        char host[NI_MAXHOST], service[NI_MAXSERV];

        peer_addrlen = sizeof(peer_addr);
        nread = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr*)&peer_addr, &peer_addrlen);
        if (nread == -1)
            continue; /* Ignore failed request */



        if (j == 1) {
            //printf("mam odebrany nick\n");
            strcpy(nick, buf);

            size_t ij;
            for (ij = 0; ij < strlen(nick); ++ij) {
                if (nick[ij] < 0) {
                    nick[ij] = '\0';
                    break;
                }
            }


            cleanArray(buf, sizeof(buf));
            printf("%s dolaczyl do gry\n", nick);
            //cleanArray(buf, sizeof(buf));
            strcpy(buf, message);
        }
        else if (j > 2) {

            if (strcmp(buf, "koniec") == 0) {
                printf("Przeciwnik zokonczyl gre\n"); // tu moozesz znowu wejsc w funkcje klienta, lub zakonczyc gre definitywnie
                printf("Czy chcesz rozpoczac nowa gre ( napis ,,tak'' ) ? Jezeli chcesz zakonczyc napisz ''koniec''.\n");
                char* userInput = czyDalejGrac();
                if (strcmp(userInput, "koniec") == 0) { //
                    free(userInput);
                    cleanArray(buf, sizeof(buf));
                    cleanArray(nick, sizeof(nick));
                    close(sfd); // zamykan gniazdo i wszytsko czyszcze
                    exit(EXIT_SUCCESS); // wychodze z programu

                }
                else if (strcmp(userInput, "tak") == 0) {
                    free(userInput);
                    cleanArray(buf, sizeof(buf));
                    cleanArray(nick, sizeof(nick));
                    close(sfd);
                    printf("\n\n");
                    klientUDP(port, message, 0, 0, ip);
                    exit(EXIT_FAILURE); // w razie gdyby z jakis przyczyn funkcja powyzej by sie nie wykonala
                }

                free(userInput);
                exit(EXIT_FAILURE); // w przypadku gdyby z jakis powodow program wczytal cos innego niz tak lub koniec
            }
            else if (strcmp(buf, "50") == 0) {
                printf("%s podal wartosc 50. Przegrales te runde\n", nick); // czyli z serwera stajesz sie klientem
                przegrane = przegrane + 1;
                cleanArray(buf, sizeof(buf));
                cleanArray(nick, sizeof(nick));
                close(sfd);
                // KLIENTEM SIE STALES, A TWOJ PRZECIWNIK CHWILE WCZESNIEJ STAL SIE SERWEREM !!!!!!!!!!!!!!!!!!!!!!!!!!
                printf("NOWA GRA \n\n");
                sleep(2); // w razie czego male okno czasowe
                klientUDP(port, message, wygrane, przegrane, ip);
                exit(EXIT_FAILURE); // w razie gdyby z jakis przyczyn funkcja powyzej by sie nie wykonala


            }
            else {
                printf("%s podal wartosc %s\n", nick, buf);
                losowaLiczba = atoi(buf);
            }

        }


        s = getnameinfo((struct sockaddr*)&peer_addr, peer_addrlen, host, NI_MAXHOST, service, NI_MAXSERV,
            NI_NUMERICSERV);
        if (s != 0)
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));



        if (j > 1) {
            // printf("Enter message (or press Ctrl+C to exit): ");
            char* userInput = getUserInput(losowaLiczba, wygrane, przegrane);
            cleanArray(buf, sizeof(buf));
            strcpy(buf, userInput);
            free(userInput);
        }

        size_t len = strlen(buf);


        if (j > 1) {
            if (strcmp(buf, "koniec") == 0) { // zakonczylismy gre jako serwer
                printf("Dziekuje za gre - przekaze informacje przeciwnikowi\n");

                if (sendto(sfd, buf, len, 0, (struct sockaddr*)&peer_addr, peer_addrlen) != len) {
                    fprintf(stderr, "Error sending response\n");
                }

                cleanArray(buf, sizeof(buf));
                cleanArray(nick, sizeof(nick));
                close(sfd);
                printf("\n\n");
                exit(EXIT_SUCCESS);

            }
            else if (strcmp(buf, "50") == 0) {
                wygrane = wygrane + 1;
                printf("WYGRALES TE RUNDE - ZACZYNAM NOWA GRE \n\n"); // - z serwera zostale klientem - wysylasz wynik i czekasz chwile, az tamten stanie sie serwerem

                if (sendto(sfd, buf, len, 0, (struct sockaddr*)&peer_addr, peer_addrlen) != len) { // wysylasz drugiemu informacje o twojej wygranej
                    fprintf(stderr, "Error sending response\n");
                }

                close(sfd);
                cleanArray(buf, sizeof(buf));
                cleanArray(nick, sizeof(nick));
                sleep(2); // Dajesz temu drugiemu chwilke, zeby stal sie serwere
                klientUDP(port, message, wygrane, przegrane, ip);
                exit(EXIT_SUCCESS);


            }
            else { // wynik naszego dzialania to jakas liczba
                losowaLiczba = atoi(buf);
            }
        }



        if (sendto(sfd, buf, len, 0, (struct sockaddr*)&peer_addr, peer_addrlen) != len) {
            fprintf(stderr, "Error sending response\n");
        }

        j = j + 1;
    }
}



// ta funckja srawia, ze jestemy klientem - czyli osobą drugą w kolejnosci gry nie losujaca randomowej liczby
void klientUDP(char* port, char* message, int wygrane, int przegrane, char* ip) {

    printf("Gra w 50, wersja B.\n");
    printf("Rozpoczynam gre z %s Napisz %s by zakonczyc lub %s by wyswietlic aktualny wynik.\n", ip, "koniec", "wynik");
    printf("Propozycja wyslana\n");


    int sfd, s;
    char buf[BUF_SIZE];
    ssize_t nread;
    struct addrinfo hints;
    struct addrinfo* result, * rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    s = getaddrinfo(ip, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(sfd);
    }

    freeaddrinfo(result);

    if (rp == NULL) {
        fprintf(stderr, "Could not connect\n");
        close(sfd);
        exit(EXIT_FAILURE);
    }


    size_t len2 = strlen(message);
    if (write(sfd, message, len2) != len2) {
        fprintf(stderr, "partial/failed write\n");
        close(sfd);
        exit(EXIT_FAILURE);
    }


    int j = 1;
    char nick[1024];

    int losowaLiczba = 0;

    while (1) {

        if (j > 1) {
            printf("Teraz nie jest twoja kolej. Poczekaj\n");
        }



        cleanArray(buf, sizeof(buf)); // !!!!!!!!

        nread = read(sfd, buf, BUF_SIZE);

        if (nread == -1) {
            // perror("read");
            close(sfd); // zamykamy smieci

            udpServer(port, message, przegrane, wygrane, ip); // jezeli jestem pierwszy w grze to wchodze w te funkcje i jestem serwerem

            exit(EXIT_FAILURE);
        }

        //      printf("jestem klientem\n");


        if (j == 1) {
            //printf("mam odebrany nick\n");
            strcpy(nick, buf);
        }
        else if (j > 1) {

            if (strcmp(buf, "koniec") == 0) {
                printf("Przeciwnik zokonczyl gre\n"); // tu moozesz znowu wejsc w funkcje klienta, lub zakonczyc gre definitywnie
                printf("Czy chcesz rozpoczac nowa gre ( napis ,,tak'' ) ? Jezeli chcesz zakonczyc napisz ''koniec''.\n");
                char* userInput = czyDalejGrac();
                if (strcmp(userInput, "koniec") == 0) { //
                    free(userInput);
                    cleanArray(buf, sizeof(buf));
                    cleanArray(nick, sizeof(nick));
                    close(sfd); // zamykan gniazdo i wszytsko czyszcze
                    exit(EXIT_SUCCESS); // wychodze z programu

                }
                else if (strcmp(userInput, "tak") == 0) {
                    free(userInput);
                    cleanArray(buf, sizeof(buf));
                    cleanArray(nick, sizeof(nick));
                    close(sfd);
                    printf("\n\n");
                    klientUDP(port, message, 0, 0, ip);
                    exit(EXIT_FAILURE); // w razie gdyby z jakis przyczyn funkcja powyzej by sie nie wykonala
                }

                exit(EXIT_FAILURE); // w przypadku gdyby z jakis powodow program wczytal cos innego niz tak lub koniec
            }
            else if (strcmp(buf, "50") == 0) {
                printf("%s podal wartosc 50. Przegrales te runde\n", nick); // czyli z klienta stajesz sie serwerem
                przegrane = przegrane + 1;
                cleanArray(buf, sizeof(buf));
                cleanArray(nick, sizeof(nick));
                close(sfd);
                // ZOSTALES SERWEREM !!!!!!!!!!!!!!!!!!!!!!!!!! - z kolei twoj przeciwnik zosatl klientem
                printf("TERAZ TY NACZYNAS GRE \n\n");
                udpServer(port, message, wygrane, przegrane, ip);
                exit(EXIT_FAILURE); // w razie gdyby z jakis przyczyn funkcja powyzej by sie nie wykonala


            }
            else {

                printf("%s podal wartosc %s\n", nick, buf);
                losowaLiczba = atoi(buf);
            }

        }





        if (j > 1) { // dopiero teraz pisze
            char* userInput = getUserInput(losowaLiczba, wygrane, przegrane);
            cleanArray(buf, sizeof(buf));
            strcpy(buf, userInput);
            free(userInput);
        }


        size_t len = strlen(buf);


        if (j > 1) {
            if (strcmp(buf, "koniec") == 0) { // zakonczylismy gre jako serwer
                printf("Dziekuje za gre - przekaze informacje przeciwnikowi\n");
                if (write(sfd, buf, len) != len) {
                    fprintf(stderr, "partial/failed write\n");
                    close(sfd);
                    exit(EXIT_FAILURE);
                }


                cleanArray(buf, sizeof(buf));
                cleanArray(nick, sizeof(nick));
                close(sfd);
                printf("\n\n");
                exit(EXIT_SUCCESS);

            }
            else if (strcmp(buf, "50") == 0) {
                wygrane = wygrane + 1;
                printf("WYGRALES TE RUNDE - ZACZYNAM NOWA GRE\n\n"); // - z serwera zostale klientem - wysylasz wynik i czekasz chwile, az tamten stanie sie serwerem

                if (write(sfd, buf, len) != len) {
                    fprintf(stderr, "partial/failed write\n");
                    close(sfd);
                    exit(EXIT_FAILURE);
                }


                close(sfd);
                cleanArray(buf, sizeof(buf));
                cleanArray(nick, sizeof(nick));
                udpServer(port, message, wygrane, przegrane, ip);
                exit(EXIT_SUCCESS);


            }
            else { // wynik naszego dzialania to jakas liczba
                losowaLiczba = atoi(buf);
            }
        }




        if (write(sfd, buf, len) != len) {
            fprintf(stderr, "partial/failed write\n");
            close(sfd);
            exit(EXIT_FAILURE);
        }

        j = j + 1;
    }

}




int main(int argc, char* argv[]) {

    if (argc < 3 || argc > 4) {
        perror("Nieprawidlowa ilosc parametrow wywolania programu\n");
        exit(EXIT_FAILURE);
    }

    int czyportdobry = isNumberGreaterThan1000(argv[2]);
    if (czyportdobry == 0) {
        printf("Podano niewlasciwa wartosc dla numeru portu\n");
        exit(EXIT_FAILURE);
    }


    char* ip = getValidIPAddress(argv[1]);


    char* message; // piewrsza wiaodmosc do przeciwnika to nasz nick

    if (argc == 4) {
        message = argv[3];
    }
    else {
        message = getHostIPAddress();
    }

    //   printf("%s\n", message);

    char* port = argv[2];

    klientUDP(port, message, 0, 0, ip);

    free(message);
    free(ip);

    exit(EXIT_SUCCESS);
}
