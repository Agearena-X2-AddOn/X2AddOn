/*
--- X2-ADDON :: RELEASE-STUFE I :: PROGRAMM-MODIFIKATIONEN ---
Semikola in Assembler-Bereichen dienen nur der korrekten Funktion der automatischen Einrückung...
Rücksprungadressen (call-Befehl!) werden vom Stack genommen bei der Ausführung, um Zugriff auf den "Original-Stack" zu haben, d.h. es müssen nicht immer erst 4 Byte vom Stackpointer abgezogen werden.
*/


/* INCLUDES */

// Codecave-Funktionen
#include "functions.h"


/* GLOBALE VARIABLEN */

// Die Rücksprungadresse in die EXE.
DWORD _retAddr = 0;


/* CODECAVE-FUNKTIONEN */

// Codecave-Funktion.
// Erweitert die Selbstheilung des Berserkers auf weitere Einheiten.
__declspec(naked) void CC_ExtendSelfHealing()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr;

		// Berserker
		cmp ax, 0x02B4;
		je id_found;

		// Elite-Berserker
		cmp ax, 0x02B6;
		je id_found;

		// Amazone
		cmp ax, 0x0387;
		je id_found;

		// Drachentöter
		cmp ax, 0x03C6;
		je id_found;

		// ID nicht gefunden, al auf 0 setzen
		xor al, al;

		// Zum Ende springen
		jmp end;

	id_found:

		// al auf 1 setzen, d.h. ID gefunden
		mov al, 1;

	end:

		// Ergebnisvergleich von al durchführen (Gleichheit => ID gefunden)
		cmp al, 0x01;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr;

		// Fertig
		ret;
	};
}


/* FUNKTIONEN */

// DLL-Einstiegspunkt.
int WINAPI DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
{
	// DLL-Thread-Messages unterbinden
	if(ulReason == DLL_PROCESS_ATTACH)
		DisableThreadLibraryCalls(hModule);
	
	// Immer laden
	return TRUE;
}

// Initialisierungsfunktion.
// Initialisiert die Codecaves und führt weitere Änderungen im logischen Speicher durch.
extern "C" __declspec(dllexport) void Init()
{
	// Codecaves erstellen
	Codecave(0x004C1795, CC_ExtendSelfHealing, 5);

	// DAT-Name auf "empires2_x2_p1.dat" ändern :: Erstmal nicht gebraucht, da UserPatch das übernimmt...
	/*char *patch = "1";
	WriteBytesASM(0x0067B813, patch, 1);*/

	// Fenster-Titel ändern für Preview
	char *patch1 = "X2-AddOn Alpha Preview\0\0\0\0\0";
	CopyBytesToAddr(0x0067B838, patch1, 27);
}

