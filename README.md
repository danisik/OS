# OS

## Zadání

- Vytvořte virtuální stroj, který bude simulovat OS
- Součástí bude shell s gramatikou cmd, tj. včetně exit
- Vytvoříte ekvivalenty standardních příkazů a programů
  - echo, cd, dir, md, rd, type, find /v /c"" (tj. co dělá wc v unix-like prostředí), sort, tasklist, shutdown
    - cd musí umět relativní cesty
    - echo musí umět @echo on a off
    - type musí umět vypsat jak stdin, tak musí umět vypsat soubor
  - Dále vytvoříte programy rgen a freq
  - rgen bude vypisovat náhodně vygenerovaná čísla v plovoucí čárce na stdout, dokud mu nepřijde znak Ctrl+Z //EOF
  - freq bude číst z stdin a sestaví frekvenční tabulku bytů, kterou pak vypíše pro všechny byty s frekvencí větší než 0 ve formátu: “0x%hhx : %d”
- Implementujte roury a přesměrování
- Nebudete přistupovat na souborový systém, ale použijete simulovaný disk

  - Za 5 bonusových bodů můžete k realizaci souborového systému použít semestrální práci z KIV/ZOS - tj. implementace FAT.
Při zpracování tohoto zadání použijte a dále pracujte s kostrou tohoto řešení, kterou najdete v archívu os_simulator.zip. Součástí archívu, ve složce compiled, je soubory checker.exe a test.exe. Soubor checker.exe je validátor semestrálních prací. Soubor test.exe generuje možný testovací vstup pro vaši semestrální práci.

Vaše vypracování si před odevzdáním zkontrolujte programem checker.exe. V souboru checker.ini si upravte položku Setup_Environment_Command, v sekci General, tak, aby obsahovala cestu dle vaší instalace Visual Studia. Např. vzorové odevzdání otestujete příkazem ''compiled\vzorove odevzdani.zip", spuštěného v kořenovém adresáři rozbaleného archívu. Odevzdávaný archív nemá obsahovat žádné soubory navíc a program musí úspěšně proběhnout.

### IDE
MS Visual Studio 2019 (MS Visual C++17)

### Hotové:
  
  ✔ Commandy.
  
  ✔ Pipe.
  
  ✔ Redirecting.
  
  ✔ VFS (pseudoNTFS).
  
  ✔ Dokumentace.
  
