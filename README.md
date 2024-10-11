### Structura
Implementarea temei se bazeaza pe scheletul de cod din labolatorul 9. In requests.c, am completat to do urile, si am modificat functiile de GET si POST astfel incat sa poate formata cookies si JWT tokens, atunci cand este nevoie. De asemenea, am adaugat si o functie pentru DELETE, care formateaza cererea la fel ca functia de GET, intrucat niciuna nu necesita body.

Pentru lucrul cu formatul JSON, am ales sa folosesc biblioteca parson, pentru ca imi permite sa transform rapid si usor campurile unei structuri in formatul dorit. Acest lucru este folositor pentru transmiterea informatiilor catre server prin rute de tipul POST.

In helpers.h, am definit structuri pentru user, respectiv carte si am adaugat mai multe functii ajutatoare.

### Client
Clientul are doua siruri de caractere declarate inainte de bucla. Unul pentru a retine cookie-ul unui eventual user logat, si unul pentru a mentine JWT token-ul al unei eventuale biblioteci accesate. Ambele sunt initial initializate cu 0.

Apoi, clientul intra intr-o bucla, asteptand sa primeasca input de la tastatura. Cand input-ul se potriveste cu una dintre comenzi, se trateaza cazul. De fiecare data cand trebuie trimis un mesaj la server, deschidem conexiunea, trimitem si o inchidem, deoarece ea este short lived.

### Mesaje afisate
In general, am incercat ca mesajele afisate sa fie cat mai specifice. Mai intai se afiseaza, "SUCCESS" sau "ERROR" in functie de caz, apoi un mesaj descriptiv si statusul. Statusul 200 iniseamna ca totul este in regula, statusurile de forma 40x inseamna ca utilizatorul a facut ceva gresit, iar statusul 500 inseamna ca este o probelma interna a serverului (caz care nu ar trebui sa fie afisat niciodata in mod normal).

### Comenzi implementate:
1. register - Se citesc in continuare de la tastatura numele si parola dorite, in structura utilizatorului. Se apeleaza o functie ajutatoare din helper.h, care ia utilizatorul si il returneaza in format JSON folosindu-se de biblioteca parson. Se formateaza cererea de POST cu zero cookies si tokens, si se trimite serverului. Apoi, verificam ce intoarce serverul. In cazul in care cuvantul eroare nu se gaseste in raspuns, actiunea a fost realizata cu succes. Altfel, putem verifica mai specific daca eroarea este cauzata de faptul ca exista deja utilizatorul, cautand "taken!" in raspuns.
2. login - Se citesc datele de logare, se formateaza JSON si se trimit la server exact ca la register. In cazul unei erori, putem verifica daca ea este data de faptul ca utilizatorul nu exista sau ca parola a fost introdusa gresit. Altfel, extragem cookie ul din raspuns si in salvam in variabila mentionata anterior din client.
3. enter_library - Se face o cerere GET pe ruta corespunzatoare, adaugand in antetul cererii cookie ul extras la login. In cazul unei erori, putem verifica daca utilizatorul nu este logat. Altfel, trebuie sa extragem si retinem JWT token ul din raspuns si variabila din client cu acest rol.
4. get_books - Antetul cererii GET va avea atat cookie ul de la logare, cat si token ul de la accesul in biblioteca. Astfel eroarea poate aparea daca utilizatorul nu este logat sau nu este in biblioteca. In cazul unui succes, se afiseaza lista de carti extrasa din raspunsul serverului.
5. get_book - Se citeste id-ul dorit. El se concateneaza la ruta corespunzatoare si, apoi, se face cererea cu cookie si token. Erorile pot fi, cele descrise la cazul anterior sau faptul ca id-ul nu exista. In cazul unui succes, se afiseaza informatiile despre cartea corespunzatoare extrase din raspunsul serverului.
6. add_book - Se citesc toate campurile cartii intr-o structura corespunzatoare. Se valideaza datele, prin a verifica daca numarul de pagini este mai mare decat 0 si diferit de un sir de caractere, si prin a ne asigura ca niciun camp nu e gol. Se afiseaza o eroare daca nu sunt indeplinite conditiile. Se apeleaza o functie ajutatoare din helper.h, care ia cartea si o returneaza in format JSON folosindu-se de biblioteca parson. Este formatata cererea POST cu cookie si token. Erorile care pot aparea in raspunsul serverului sunt la fel ca la cazul get_books.
7. delete_book - Se citeste id-ul cartii si se formateaza o cerere de tip DELETE cu cookie si token. Pot aparea erori daca id-ul nu exista sau daca utilizatorul nu este logat sau nu a intrat in biblioteca.
8. logout - Se formateaza cererea GET catre ruta corespunzatoare. Poate aparea eroare data utilizatorul nu era logat. In caz de succes, se seteaza cookie si token pe 0, pentru ca sesiunea s-a terminat si utilizatorul nu mai are acces la anumite informatii.
9. exit - Schimba flag-ul care este pus ca conditie in bucla serverului pentru a o opri.