/*
--- X2-ADDON :: PROGRAMM-MODIFIKATIONEN ---
Semikola in Assembler-Bereichen dienen nur der korrekten Funktion der automatischen Einrückung...
Rücksprungadressen (call-Befehl!) werden vom Stack genommen bei der Ausführung, um Zugriff auf den "Original-Stack" zu haben, d.h. es müssen nicht immer erst 4 Byte vom Stackpointer abgezogen werden.
*/


/* INCLUDES */

// Codecave-Funktionen
#include "functions.h"


/* GLOBALE VARIABLEN */

// Die Rücksprungadresse in die EXE.
DWORD _retAddr = 0;

// Sprungziele
DWORD _funcUnknown1 = 0x004C05B0;


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

// Codecave-Funktion.
// 
__declspec(naked) void CC_Renaissance1()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr;

		// Von Codecave überschriebene Befehle
		movsx eax, word ptr[ebp + 0x08];
		cmp eax, 0x65;

		// Feudalzeit-Teil
		jne ritter;
		push 0;
		push 0;
		push 0;
		push 0;
		mov ecx, [ebp - 0x20];
		mov edx, [ecx + 0x0C];
		mov ecx, [edx + 0x000018E8];
		call[_funcUnknown1];
		jmp block1;

		// Ritterzeit-Teil
	ritter:
		movsx eax, word ptr[ebp + 0x08];
		cmp eax, 0x66;
		jne imperial;
		push 0;
		push 0;
		push 0;
		push 1;
		mov ecx, [ebp - 0x20];
		mov edx, [ecx + 0x0C];
		mov ecx, [edx + 0x000018E8];
		call[_funcUnknown1];
		jmp block1;

		// Imperialzeit-Teil
	imperial:
		movsx eax, word ptr[ebp + 0x08];
		cmp eax, 0x67;
		jne renaissance;
		push 0;
		push 0;
		push 0;
		push 2;
		mov ecx, [ebp - 0x20];
		mov edx, [ecx + 0x0C];
		mov ecx, [edx + 0x000018E8];
		call[_funcUnknown1];
		jmp block1;

		// Renaissance-Teil
	renaissance:
		movsx eax, word ptr[ebp + 0x08];
		cmp eax, 0x68;
		jne block1;
		push 0;
		push 0;
		push 0;
		push 3;
		mov ecx, [ebp - 0x20];
		mov edx, [ecx + 0x0C];
		mov ecx, [edx + 0x000018E8];
		call[_funcUnknown1];

		// Block 1
	block1:
		movsx eax, word ptr[ebp + 0x08];
		cmp eax, 0x69;
		je end;
		movsx ecx, word ptr[ebp + 0x08];
		cmp ecx, 0x65;
		je end;
		movsx edx, word ptr[ebp + 0x08];
		cmp edx, 0x66;
		je end;
		movsx edx, word ptr[ebp + 0x08];
		cmp edx, 0x67;
		je end;
		movsx eax, word ptr[ebp + 0x08];
		cmp eax, 0x68;

		// Rausspringen aus Codecave, dafür Zieladresse auf den Stack legen
		je end; // Umgekehrtes JNE (an 0x00402626)
		push 0x004026F7;
		ret;

	end:
		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
__declspec(naked) void CC_Renaissance2()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr;

		// Block 1
		mov ecx, [ebp + 0x00];
		cmp ecx, 0x69;
		je block2;
		cmp ecx, 0x65;
		je block2;
		cmp ecx, 0x66;
		je block2;
		cmp ecx, 0x67;
		je block2;
		cmp ecx, 0x68;
		jne end;

		// Block 2
	block2:
		mov ecx, [esp + 0x24];
		cmp ecx, 0x69;
		je end;
		cmp ecx, 0x65;
		je end;
		cmp ecx, 0x66;
		je end;
		cmp ecx, 0x67;
		je end;
		cmp ecx, 0x68;

		// Rausspringen aus Codecave, dafür Zieladresse auf den Stack legen
		je end; // Umgekehrtes JNE (an 0x005AAD8E)
		push 0x005AAEE8;
		ret;

	end:
		mov edx, [eax];

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
__declspec(naked) void CC_Renaissance3()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr;

		// Zeitalter-Block
		cmp ebx, 0x65;
		je end;
		cmp ebx, 0x66;
		je end;
		cmp ebx, 0x67;
		je end;
		cmp ebx, 0x68;

		// Rausspringen aus Codecave, dafür Zieladresse auf den Stack legen
		je end; // Umgekehrtes JNE (an 0x0043FF68)
		push 0x0043FFD5;
		ret;

	end:
		mov eax, [esi + 0x00001820];

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
__declspec(naked) void CC_Renaissance4()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr;

		// Zeitalter-Block
		cmp ebx, 0x65;
		je block2;
		cmp ebx, 0x66;
		je block2;
		cmp ebx, 0x67;
		je block2;
		cmp ebx, 0x68;
		je block2;
		push 0x00;
		push 0x00;
		push 0x1B;
		jmp end;

		// Block 2
	block2:
		push 0x00;
		push 0x00;
		push 0x05;

	end:
		mov ecx, esi;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
__declspec(naked) void CC_Renaissance5()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr;

		// Dunkle Zeit
		cmp eax, 0;
		jne feudal;
		mov ecx, [esi + 0x000011BC];
		push 0x00001069;
		mov eax, [ecx];
		jmp end;

		// Feudalzeit
	feudal:
		cmp eax, 1;
		jne ritter;
		mov ecx, [esi + 0x000011BC];
		push 0x0000106A;
		mov eax, [ecx];
		jmp end;

		// Ritterzeit
	ritter:
		cmp eax, 2;
		jne imperial;
		mov ecx, [esi + 0x000011BC];
		push 0x0000106B;
		mov eax, [ecx];
		jmp end;

		// Imperialzeit
	imperial:
		cmp eax, 3;
		jne renaissance;
		mov ecx, [esi + 0x000011BC];
		push 0x0000106C;
		mov eax, [ecx];
		jmp end;

		// Renaissance
	renaissance:
		mov ecx, [esi + 0x000011BC];
		push 0x0000106D;
		mov eax, [ecx];

	end:
		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
__declspec(naked) void CC_Renaissance6()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr;

		// Sprung ausführen
		cmp eax, 0;
		je dunkel;
		cmp eax, 1;
		je feudal;
		cmp eax, 2;
		je ritter;
		cmp eax, 3;
		je imperial;
		jmp renaissance;

		// Dunkle Zeit
	dunkel:
		mov eax, [esp + 0x20];
		mov edx, [ecx];
		push eax;
		push esi;
		push 0x00001069;
		jmp end;

		// Feudalzeit
	feudal:
		mov eax, [esp + 0x20];
		mov edx, [ecx];
		push eax;
		push esi;
		push 0x0000106A;
		jmp end;

		// Ritterzeit
	ritter:
		mov eax, [esp + 0x20];
		mov edx, [ecx];
		push eax;
		push esi;
		push 0x0000106B;
		jmp end;

		// Imperialzeit
	imperial:
		mov eax, [esp + 0x20];
		mov edx, [ecx];
		push eax;
		push esi;
		push 0x0000106C;
		jmp end;

		// Renaissance
	renaissance:
		mov eax, [esp + 0x20];
		mov edx, [ecx];
		push eax;
		push esi;
		push 0x0000106D;

	end:
		call dword ptr[edx + 0x28];
		pop edi;
		pop esi;
		pop ebp;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
__declspec(naked) void CC_Renaissance7()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr;

		// Sprung ausführen
		cmp eax, 0;
		je feudal;
		cmp eax, 1;
		je ritter;
		cmp eax, 2;
		je imperial;
		jmp renaissance;

		// Feudalzeit:
	feudal:
		mov edx, [esi];
		push 0x0000106A;
		mov ecx, esi;
		call dword ptr[edx + 0x000000FC];
		jmp end;

		// Ritterzeit:
	ritter:
		mov eax, [esi];
		push 0x0000106B;
		mov ecx, esi;
		call dword ptr[eax + 0x000000FC];
		jmp end;

		// Imperialzeit:
	imperial:
		mov edx, [esi];
		push 0x0000106C;
		mov ecx, esi;
		call dword ptr[edx + 0x000000FC];
		jmp end;

		// Renaissance:
	renaissance:
		mov edx, [esi];
		push 0x0000106D;
		mov ecx, esi;
		call dword ptr[edx + 0x000000FC];

	end:
		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr;

		// Fertig
		ret;
	};
}

/* DLL-FUNKTIONEN */

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
	CreateCodecave(0x004C1795, CC_ExtendSelfHealing, 5);
	CreateCodecave(0x0040259A, CC_Renaissance1, 141);
	CreateCodecave(0x005AAD61, CC_Renaissance2, 48);
	CreateCodecave(0x0043FF59, CC_Renaissance3, 16);
	CreateCodecave(0x0043FFF8, CC_Renaissance4, 26);
	CreateCodecave(0x0051FF37, CC_Renaissance5, 72);
	CreateCodecave(0x0043D051, CC_Renaissance6, 87);
	CreateCodecave(0x004ACF51, CC_Renaissance7, 43);

	// Renaissance-Mini-Patches
	{
		// Dunkle Zeit-ID ändern
		BYTE patch1[1] = { 0x69 };
		CopyBytesToAddr(0x0040265F, patch1, 1);
		CopyBytesToAddr(0x00402689, patch1, 1);
		CopyBytesToAddr(0x004026B2, patch1, 1);
		CopyBytesToAddr(0x004026DB, patch1, 1);

		// Renaissance-ID einbeziehen
		patch1[0] = 0x68;
		CopyBytesToAddr(0x0040282C, patch1, 1);
		CopyBytesToAddr(0x00402B07, patch1, 1);
		CopyBytesToAddr(0x00402ECB, patch1, 1);
		CopyBytesToAddr(0x0040313E, patch1, 1);
		CopyBytesToAddr(0x004864F5, patch1, 1);

		// Zeitalter-Anzahl ändern
		patch1[0] = 0x04;
		CopyBytesToAddr(0x0051FF2E, patch1, 1);
		CopyBytesToAddr(0x0043D04A, patch1, 1);
		//CopyBytesToAddr(0x004ACF4E, patch1, 1); Siehe Renaissance7, scheint Post-Imperialzeit zu überschreiben
	}

	// Neue ID für die Einsaat-Warteschleife-Meldung (Kollision mit Renaissance-Meldung)
	BYTE patch[2] = { 0x0A, 0x0C };
	CopyBytesToAddr(0x00529A76, patch, 2);


	// TODO: Einzelne Memory-Patches bei 2667+ (90er-Werte)?


	// Fenster-Titel ändern für Preview
	char *patchTitle = "X2-AddOn Alpha Preview\0\0\0\0\0";
	CopyBytesToAddr(0x0067B838, patchTitle, 27);
}

