# IPK
projekt do předmětu Počítačové komunikace a sítě 2018
doporučuju si přečíst dokumentaci, byl jsem celkem vtipálek.

# Zadání projektu

### Vašim úkolem je:
[1] Nastudovat problematiku měření přenosové rychlosti a relevantní informace uvést v projektové dokumentaci. (až 6 bodů)
[2] Naprogramovat aplikaci realizující měření přenosové rychlosti mezi dvěma body v síti (až 12 bodů)
[3] Provést sadu experimentů pro různé prostředí a toto uvést jakou součást dokumentace (až 2 body)

### KONVENCE ODEVZDÁVANÉHO ZIP ARCHIVU xlogin00.zip
dokumentace.pdf - výstupy zadání [1] a [3]
readme - základní informace, případná omezení projektu
Makefile
*.c, *.cpp, *.cc, *.h - zdrojové a hlavičkové soubory výstupů zadání [2]

### DOPORUČENÍ/OMEZENÍ:

K ad hoc testování použijte vaši domácí síť či si neváhejte vytvořit pomocí virtualizačních prostředí jako VirtualBox či VMWare Workstation jednoduchou laboratoř. Pokud jste ještě nikdy nevirtualizovali, třeba vám pomůže následující článek http://www.brianlinkletter.com/how-to-use-virtualbox-to-emulate-a-network/.
Implementované konzolové aplikace budou povinně v jazyce C/C++.
Všechny implementované programy by měly být použitelné a řádně komentované. Pokud už přejímáte zdrojové kódy z různých tutoriálů či příkladů z Internetu (ne mezi sebou pod hrozbou potrestání před disciplinární komisí), tak je POVINNÉ správně vyznačit tyto sekce a jejich autory dle licenčních podmínek, kterými se distribuce daných zdrojových kódů řídí. V případě nedodržení bude na projekt nahlíženo jako na plagiát!
Pro snadné parsování vstupních argumentů se doporučuje použít funkci getopt().
Aplikace reagují korektním ukončením na SIGINT signál (tedy Ctrl+C).
Výsledky vaší implementace by měly být co možná nejvíce multiplatformní mezi OS založenými na unixu.
Projekt bude opravován ručně. Počítejte tedy s nejzazším možným termínem oprav a reklamací určených garantem předmětu.

## UPŘESNĚNÍ ZADÁNÍ:

### Ad [1]

V dobré dokumentaci se očekává následující: titulní strana, obsah, logické strukturování textu, výcuc relevantních informací z nastudované literatury, popis zajímavějších pasáží implementace, demonstrace činnosti implementovaných aplikací, normovaná bibliografie.

Přepisovat Wikipedii do teoretické části dokumentace není ideální. Pokuste se především vlastními slovy vysokoškoláka doplněnými vhodnými obrázky vysvětlit, jaký způsobem lze měření přenosové cesty provádět. Než na kvantitu bude se přihlížet hlavně ke kvalitě textu.

### Ad [2]

Cílem aplikace je pomocí vhodné komunikace změřit dostupnou přenosovou rychlost mezi dvěma stanicemi v Internetu. Pro měření se bude používat generovaná komunikace protokolu UDP. Tato aplikace se bude skládat ze dvou částí - Reflektoru a Měřáku. Měření bude probíha pomocí zasílání UDP paketů tkz. sond z měřáku na reflektor. Reflektor pouze odpovídá na příchozí sondy. Měřák musí implementovat vhodný algoritmus pro zjištění maximální přenosové rychlosti - tedy zjistit jak rychle je možné sondy posílat aniž by docházelo ke ztrátě paketů. Návrh tohoto algoritmu je hlavním výstupem teoretické části projektu. 

Konvence jména aplikace a jejích povinných vstupních parametrů jsou následující. 

### Reflektor:

./ipk-mtrip reflect -p port 

port - číslo portu, na kterém bude reflektor spuštěn.

### Měřák:

./ipk-mtrip meter -h vzdáleny_host -p vzdálený_port -s velikost_sondy -t doba_mereni

vzdáleny_host - doménové jméno nebo IP adresa stanice, na které je reflektor spuštěn.
vzdálený_port - číslo portu, na kterém je reflektor spuštěn.
velikost_sondy - velikost dat použitých v "probe" paketu
doba_mereni - celový čas měření 

Výsledkem měření budou hodnoty:
průměrná přenosová rychlost
maximální naměřená rychlost
minimální naměřená rychlost 
standardní odchylka 
průměrný RTT paketů komunikace

## Ad [3]

Demonstraci činnosti uveďte jako samostatnou kapitolu v dokumentaci. Nezapomeňte uvést nezbytné informace o měření a dosažené výsledky měření.
