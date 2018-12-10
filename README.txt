Něco o programu
===============
Tento program slouží pro hledání minimálního chromatického čísla grafu,
tedy pro nalezení minimálního počtu barev, kterým lze graf obarvit tak, aby uzly spojené hranou neměly stejnou barvu.

Jak program používat
====================
1. Po rozbalení archivu je třeba použít příkaz "make" pro kompilaci
2. Pro spuštění programu napište do příkazového řádku "./main -f FILENAME"
  - FILENAME je zde textový soubor s uzly, několik příkladů k vyzkoušení je ve složce "examples"
3. Formát souborů s uzly je následovný:
  - na prvním řádku se nachází počet uzlů v grafu
  - pod počtem uzlů je pak graf reprezentovaný maticí sousednosti, kde 1 znamená hranu mezi uzly, 0 znamená absenci hrany, čísla jsou odděleny mezerami a řádky matice jsou odděleny znakem konce řádku
4. Pro více informací o spuštění programu je možné zobrazit nápovědu pomocí příkazu "./main -h"
5. Pro ještě více informací si prosím přečtěte naši dokumentaci v souboru "documentation.pdf"

Příkladové grafy
================
Některé z našich testovacích grafů:
1. Malý vůz
2. Krychle
3. Žirafa
4. Graf s 10 uzly
5. Graf s 50 uzly bez spojení

Plně spojené:
6. 20 uzlů
7. 40 uzlů
8. 60 uzlů
9. 80 uzlů
10. 100 uzlů

Náhodně generované:
11. 10 uzlů
12. 20 uzlů
13. 30 uzlů
14. 35 uzlů
15. 40 uzlů (může už trvat 1-2 minuty)

Skript na generování grafů
==========================
- pokud vám nebudou stačit naše příklady grafů, přiložili jsme k 
programu skript na generování grafů "generate_graph.py", pro rychlou
nápovědu jej spusťte s přepínačem "-h"
- skript lze spustit následovně:

	python3 generate_graph.py -MOD -o VYSTUP -n POCET_UZLU
	
- MOD označuje mód tvoření grafu a je na výběr ze 3 možností
  - plně spojený graf (-f) [VÝCHOZÍ MÓD]
  - náhodně spojený graf (-r)
  - graf bez hran (-e)
- VYSTUP označuje jméno souboru, do kterého se graf vygeneruje,
soubor může mít koncovky .txt, .in nebo žádnou [VÝCHOZÍ SOUBOR JE nodes.txt]
- POCET_UZLU označuje počet uzlů v grafu



