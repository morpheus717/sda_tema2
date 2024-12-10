Tema 2
    Pentru a implementa aceasta tema, am avut nevoie de a implementa un hashtable, bazat
pe o lista simpla inlantuita si o lista dublu inlantuita care retine atat primul nod in
head cat si ultimul nod in tail. Lista dublu inlantita este folosita mai mult ca o
coada, atat in cache, cat si in task queue. Diferenta este ca, spre deosebire de o coada
clasica, putem parcurge intreaga lista, doar ca nu scoatem noduri decat de la inceput si
adaugam doar la final. La nivelul cacheului, in urma evaluarii optiunilor, am decis ca cel
mai logic si eficient mod de a il implementa este folosindu-ma de aceasta lista dublu inlantuita
care functioneaza pe principiul least recently used. In campul data al listei, stocam o 
structura de tip document care retine numele si continutul unui document. Pentru a face
cautarea in lista sa fie in O(1), avem un hashtable in cache care retine la key numele unui document, 
iar la value adresa nodului din lista, corespunzator documentului respectiv. Pentru a 
usura lucrul am implementat si o functie de remove_by_address in lista, care scoate un nod
in O(1) fara sa parcurga lista. Pentru inserare nu am considerat necesar sa implementez
ceva de genul deoarece, intotdeauna inserez doar la final, si conform structurii, finalul il
retin mereu in tail. Local database este de fapt doar un hashtable. Task queue este tot o lista
dublu inlantuita, dar am implementat in plus niste functii "de coada" pentru a fi mai
intutitiv de scris si citit codul.
    La nivelul serverului, in functia server_edit_document am reusit sa ma folosesc
de valoarea de return a functiei lru_cache_put si a pointerului evicted pentru a prevedea toate
cazurile posibile pentru inserarea unui document in cache atunci cand acesta este plin sau nu, 
cand documentul exista deja sau nu si afisarea mesajelor corepunzatoare. In functia get_document,
updatam cacheul dupa ce afisam continutul, care poate exista sau nu, dupa caz. In functia
handle_request, daca avem un request de tip edit il executam, iar daca avem un request de tip get,
executam intai tot task queue-ul de edit-uri, dupa care intram la executarea comenzii propriu-zise.
