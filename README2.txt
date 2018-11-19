Něco o programu
===============
Tento program slouží pro hledání minimálního chromatického čísla grafu,
tedy pro naleznutí minimálního počtu barev, kterým lze graf obarvit tak, aby uzly spojené hranou neměly stejnou barvu.

Jak program používat
====================
1. Po rozbalení archivu je třeba použít příkaz make pro kompilaci
2. Pro spuštění programu napište do příkazového řádku "./main -f FILENAME"
  - FILENAME je zde textový soubor s uzly, několik příkladů je ve složce "examples"
3. Formát souborů s uzly je následovný:
  - na prvním řádku se nachází počet uzlů v grafu
  - pod počtem uzlů je pak graf reprezentovaný maticí sousednosti, kde 1 znamená hranu mezi uzly, 0 znamená absenci hrany, čísla jsou odděleny mezerami a řádky matice jsou odděleny znakem konce řádku
4. Pro více informací o spuštění programu je možné zobrazit nápovědu pomocí příkazu "./main -h"
5. Pro ještě více informací si prosím přečtěte naši dokumentaci v souboru "documentation.pdf"