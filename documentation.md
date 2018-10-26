# Dokumentace

## Zadání

Obarvením grafu rozumíme přiřazení barev uzlům grafu, přičemž žádné dva sousední uzly nesmí být obarveny stejně. Minimální počet použitých barev se nazývá chromatické číslo.

Vytvořte program pro hledání minimálního obarvení neorientovaných grafů.

Pokud existuje více řešení, stačí nalézt pouze jedno. Výsledky prezentujte vhodným způsobem. Součástí projektu bude načítání grafů ze souboru a vhodné testovací grafy. V dokumentaci uveďte teoretickou složitost úlohy a porovnejte ji s experimentálními výsledky. 

## Struktury

V našem řešení jsme 

## Algoritmus

Daným problémem tedy je obarvit v grafu uzly tak, aby jejich sousedi, se kterými jsou spojeni hranou, měli jinou barvu.

Pro hledání řešení obarvení neorientovaného grafu s nejmenším chromatickým číslem jsme vybrali metodu prohledávání stavového prostoru zvanou "backtracking" (AKA metoda zpětného navracení).

Tato metoda funguje tak, že se vytváří strom, přičemž každý level představuje pořadí uzlu v poli uzlů a každá větev vycházející dolů z uzlu představuje výběr barvy pro spodní uzel.

[ZDE SUPERDUPER OBRÁZEK STROMU]