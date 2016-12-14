/*
--- X2-ADDON :: PROGRAMM-MODIFIKATIONEN ---
Semikola in Assembler-Bereichen dienen nur der korrekten Funktion der automatischen Einrückung...
Rücksprungadressen (call-Befehl!) werden vom Stack genommen bei der Ausführung, um Zugriff auf den "Original-Stack" zu haben, d.h. es müssen nicht immer erst 4 Byte vom Stackpointer abgezogen werden.
*/


/* INCLUDES */

// Codecave-Funktionen
#include "functions.h"


/* GLOBALE VARIABLEN */

// Hilfsvariable zum Löschen von Stackeinträgen.
DWORD _garbage = 0;

// Spezifiziert den Bekehrungsmodus.
// -> 0: Normal bekehren
// -> 1: Unendliche Bekehrung (Lamaismus-Technologie)
// -> 2: "Belohnung" nach Bekehrung eines Transport-/Handelsschiffs (Prise-Technologie)
// -> 3: "Belohnung" nach Bekehrung eines Kriegsschiffs (Prise-Technologie)
// Auf diese Variable wird nur innerhalb eines Funktionsdurchlaufs zugegriffen, es sind also keine Zugriffskonflikte zu erwarten.
BYTE _conversionModeFlag = 0;

// Gibt an, ob der Bekehrungswiderstandsfaktor für bestimmte Schiffstypen angewandt werden soll (Finkennetze-Technologie).
// -> 0: Nicht anwenden
// -> 1: Zwischenwert, noch nicht anwenden
// -> 2: Anwenden
BYTE _conversionResistForSpecificShips;

// Sprungziele.
DWORD _funcUnknown1 = 0x004C05B0;
DWORD _funcGetString = 0x00562CB0; // Ruft offenbar einen String aus den Language-DLLs ab?
DWORD _funcFormatString = 0x0061442B; // Scheint zwei Strings mithilfe eines Formatstrings zu kombinieren?
DWORD _funcComboBoxCreateEntry1 = 0x00550840; // Erstellt einen Eintrag in einer gegebenen ComboBox?
DWORD _funcComboBoxCreateEntry2 = 0x00550870; // Erstellt einen Eintrag in einer gegebenen ComboBox?
DWORD _funcUnknown2 = 0x00457F90;
DWORD _funcFloatToInt = 0x006139E4; // Konvertiert den obersten Float in der FPU in einen Int-Wert in EAX.
DWORD _funcUnknown3 = 0x00403996;
DWORD _funcUnknown4 = 0x004038ED;
DWORD _funcUnknown5 = 0x00403A2E;
DWORD _funcUnknown6 = 0x0040244D;
DWORD _funcCreateButton = 0x00520620; // Erstellt einen neuen Einheiten-Interface-Button.

// Zeitalter-Konstanten.
float RESEARCH_ID_DUNKEL = 105.0f;
float RESEARCH_ID_FEUDAL = 101.0f;
float RESEARCH_ID_RITTER = 102.0f;
float RESEARCH_ID_IMPERIAL = 103.0f;
float RESEARCH_ID_RENAISSANCE = 104.0f;

// Zeitalter-Konstanten für Zeitalter-Ressource.
float AGE_RESOURCE_DUNKEL = 0.0f;
float AGE_RESOURCE_FEUDAL = 1.0f;
float AGE_RESOURCE_RITTER = 2.0f;
float AGE_RESOURCE_IMPERIAL = 3.0f;
float AGE_RESOURCE_RENAISSANCE = 4.0f;

// Bekehrungszeit-Summand für Finkennetze-Technologie.
float CONVERSION_TIME_SUMMAND_FINK_NETS = 6.0f;

// Hilfskonstanten für Floating-Point-Operationen.
float FLOAT_0 = 0.0f;
float FLOAT_1 = 1.0f;

// Vergrößerter Puffer für Language-DLL-Strings.
static char _newLanguageDllStringBuffer[768];

/* CODECAVE-FUNKTIONEN */

// Codecave-Funktion.
// Erweitert die Selbstheilung des Berserkers auf weitere Einheiten.
DWORD _retAddr_ExtendSelfHealing = 0;
__declspec(naked) void CC_ExtendSelfHealing()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_ExtendSelfHealing;

		// Sicherheitshalber EAX sichern
		push eax;

		// Berserker
		cmp ax, 692;
		je id_found;

		// Elite-Berserker
		cmp ax, 694;
		je id_found;

		// Amazone
		cmp ax, 903;
		je id_found;

		// Drachentöter
		cmp ax, 966;
		je id_found;

		// Shaolin
		cmp ax, 930;
		je id_found;
		cmp ax, 1123;
		je id_found;

		// Elite-Shaolin
		cmp ax, 932;
		je id_found;
		cmp ax, 1124;
		je id_found;

		// Für Upgrade-Einheiten ist ID2 interessant
		mov ax, [edx + 0x12];

		// Selbstheilender Mönch
		cmp ax, 1088;
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

		// EAX wiederherstellen
		pop eax;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_ExtendSelfHealing;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Fügt dem Shaolin den Heil-Button hinzu.
DWORD _retAddr_CreateShaolinHealingButton = 0;
__declspec(naked) void CC_CreateShaolinHealingButton()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_CreateShaolinHealingButton;

		// ID holen
		mov eax, [ecx + 0x08];
		mov ax, [eax + 0x10];

		// Shaolin?
		cmp ax, 930;
		je create_shaolin_healing_button;
		cmp ax, 932;
		je create_shaolin_healing_button;
		cmp ax, 1123;
		je create_shaolin_healing_button;
		cmp ax, 1124;
		jne end;

	create_shaolin_healing_button:
		// Button erstellen
		push 0x00;
		push 0x00;
		push 0x00;
		push 0x00;
		push 0x00;
		push 0xFFFFFFFF;
		push 4924; // Hilfetext
		push 0x00;
		push 28; // Code
		push 15; // Heilen-Icon
		push 14; // Unten rechts in der Ecke platzieren
		mov eax, [esi + 0x00001020];
		push eax;
		mov edi, ecx;
		mov ecx, esi;
		call _funcCreateButton;
		mov ecx, edi;

	end:
		// Überschriebene Befehle ausführen
		mov eax, [esp + 0x20];
		xor edi, edi;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_CreateShaolinHealingButton;

		// Fertig
		ret;
	};
}

// Codecave - Funktion.
// Erlaubt die Ausführung der Heiltätigkeit beim Shaolin.
DWORD _retAddr_EnableShaolinHealingAbility = 0;
__declspec(naked) void CC_EnableShaolinHealingAbility()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_EnableShaolinHealingAbility;

		// Überschriebenen Sprung durchführen
		je end;

		// Shaolin?
		cmp word ptr[ecx + 0x10], 930;
		je end;
		cmp word ptr[ecx + 0x10], 932;
		je end;
		cmp word ptr[ecx + 0x10], 1123;
		je end;
		cmp word ptr[ecx + 0x10], 1124;
		je end;

	no_match:
		// Zu Standard-Behandlung springen
		push 0x00434CDF;
		ret;

	end:
		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_EnableShaolinHealingAbility;

		// Fertig
		ret;
	};
}

// Codecave - Funktion.
// Sorgt dafür, dass der Shaolin trotz Heilfunktion weiterhin Kämpfen zugewiesen kann.
DWORD _retAddr_KeepShaolinFightingAbility = 0;
__declspec(naked) void CC_KeepShaolinFightingAbility()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_KeepShaolinFightingAbility;

		// ID holen
		mov ecx, [esp + 0x30];
		mov cx, [ecx + 0x10];

		// Shaolin?
		cmp cx, 930;
		je end;
		cmp cx, 932;
		je end;
		cmp cx, 1123;
		je end;
		cmp cx, 1124;
		je end;

	no_match:
		// Programmverlauf normal fortsetzen, überschriebene Befehle ausführen
		xor ecx, ecx;
		movsx esi, [eax + 0x08];
		push _retAddr_KeepShaolinFightingAbility;
		ret;

	end:
		// Register in richtigen Zustand bringen
		mov ecx, [esp + 0x30];
		movsx ax, [ecx + 0x16];
		mov ebp, [esp + 0x2C];

		// Zu anderweitiger Behandlung springen
		push 0x00434CDF;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Setzt das Heilflag beim Shaolin und löscht das Angriffsflag bei den Transportkutschen.
DWORD _retAddr_SetShaolinHealingFlagAndRemoveTransportCartAttackFlag = 0;
__declspec(naked) void CC_SetShaolinHealingFlagAndRemoveTransportCartAttackFlag()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_SetShaolinHealingFlagAndRemoveTransportCartAttackFlag;

		// Shaolin?
		cmp word ptr[ebx + 0x10], 930;
		je add_healing_flag;
		cmp word ptr[ebx + 0x10], 932;
		je add_healing_flag;
		cmp word ptr[ebx + 0x10], 1123;
		je add_healing_flag;
		cmp word ptr[ebx + 0x10], 1124;
		je add_healing_flag;

		// Transportkutsche?
		cmp word ptr[ebx + 0x10], 954;
		je remove_attack_flag;
		cmp word ptr[ebx + 0x10], 1076;
		jne end;

	remove_attack_flag:
		// Flag löschen
		and dl, 0xFE;

		// Restliche Abfragen überspringen, treffen eh nicht mehr zu
		push 0x0045AD59;
		ret;

	add_healing_flag:
		// Flag setzen
		or dl, 0x10;

		// Restliche Abfragen überspringen
		push 0x0045AD59;
		ret;

	end:
		// Überschriebenen Befehl ausführen
		cmp word ptr[ebx + 0x10], 0x0D;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_SetShaolinHealingFlagAndRemoveTransportCartAttackFlag;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Führt beim Shaolin bei gescheiteter Auto-Attack-Zielfindung die Auto-Heal-Zielfindung aus.
DWORD _retAddr_EnableShaolinAutoHeal = 0;
__declspec(naked) void CC_EnableShaolinAutoHeal()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_EnableShaolinAutoHeal;

		// Auto-Attack-Rückgabewert -1 (keine Einheit gefunden)?
		cmp edi, -1;
		jne end;

		// ID holen
		mov eax, dword ptr[esi + 0x04]; // this->UnitInstance
		mov eax, dword ptr[eax + 0x08]; // this->UnitInstance->UnitData
		mov ax, word ptr[eax + 0x10]; // this->UnitInstance->UnitData->ID1

		// Shaolin?
		cmp ax, 930;
		je run_auto_heal;
		cmp ax, 932;
		je run_auto_heal;
		cmp ax, 1123;
		je run_auto_heal;
		cmp ax, 1124;
		jne end;

	run_auto_heal:
		// VTable-Adresse austauschen (andere Klasse)
		mov dword ptr[esi], 0x00638DE8;

		// Entsprechende virtuelle Funktion für automatische Heilung aufrufen
		push 1;
		mov ecx, esi;
		mov edx, [ecx];
		call dword ptr[edx + 0xDC];

		// VTable-Adresse zurücksetzen, falls keine Heilung zustandegekommen
		cmp eax, 6; // 5, falls keine Heilung
		je return_function;
		mov dword ptr[esi], 0x0063919C;

	return_function:
		// Rückgabewert liegt bereits in EAX
		pop edi;
		pop esi;
		add esp, 0x0C;
		retn 4;

	end:
		// Überschriebenen Befehl ausführen
		cmp dword ptr[esi + 0x28], 0x02C7;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_EnableShaolinAutoHeal;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Überschreibt einen Patch vom UserPatch. Sorgt für einen korrekten Zielfindungsradius beim Heilen, verhindert jedoch das Weglaufen des Shaolins.
DWORD _retAddr_PatchShaolinAutoHealDistanceCalculation = 0;
__declspec(naked) void CC_PatchShaolinAutoHealDistanceCalculation()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_PatchShaolinAutoHealDistanceCalculation;

		// Einheiteninstanz abrufen
		mov ecx, dword ptr[esi + 0x04]; // this->UnitInstance

		// Grunddistanz laden (Ressource #90 der Spieler-Civ)
		mov eax, dword ptr[ecx + 0x0C]; // this->UnitInstance->PlayerData
		mov eax, dword ptr[eax + 0xA8]; // this->UnitInstance->PlayerData->CivResources
		fld dword ptr[eax + 360]; // this->UnitInstance->PlayerData->CivResources[90] (float-Array)

		// ID holen
		mov eax, dword ptr[ecx + 0x08]; // this->UnitInstance->UnitData
		mov ax, word ptr[eax + 0x10]; // this->UnitInstance->UnitData->ID1

		// Shaolin?
		cmp ax, 930;
		je end;
		cmp ax, 932;
		je end;
		cmp ax, 1123;
		je end;
		cmp ax, 1124;
		je end;

	add_walk_range:
		// Extra-Reichweite zum "Hinlaufen" addieren
		fadd FLOAT_1;

	end:
		// Reichweitenwert speichern
		fstp dword ptr[esp + 0x18];

		// Überschriebenen Befehl ausführen
		mov ecx, esi;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_PatchShaolinAutoHealDistanceCalculation;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Führt beim Shaolin bei gescheiteter Auto-Attack-Zielfindung die Auto-Heal-Zielfindung aus.
// TODO: ID-Test notwendig? Erstmal auskommentiert, bei Fehlern wieder einkommentieren.
DWORD _retAddr_PatchShaolinHealAssignmentBehaviour = 0;
__declspec(naked) void CC_PatchShaolinHealAssignmentBehaviour()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_PatchShaolinHealAssignmentBehaviour;

		// ID holen
		mov ecx, dword ptr[esi + 0x08]; // this->UnitInstance (in ECX, da für VTable-Austausch noch notwendig)
		/*
		mov eax, dword ptr[ecx + 0x08]; // this->UnitInstance->UnitData
		mov ax, word ptr[ecx + 0x10]; // this->UnitInstance->UnitData->ID1

		// Shaolin?
		cmp ax, 930;
		je set_behaviour_vtable;
		cmp ax, 932;
		je set_behaviour_vtable;
		cmp ax, 1123;
		je set_behaviour_vtable;
		cmp ax, 1124;
		jne end;
		*/
	set_behaviour_vtable:
		// VTable-Adresse austauschen (andere Klasse)
		mov eax, dword ptr[ecx + 0x6C];
		mov dword ptr[eax], 0x00638DE8;

	end:
		// Überschriebenene Befehle ausführen
		mov ecx, dword ptr[esp + 0x0C];
		mov eax, esi;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_PatchShaolinHealAssignmentBehaviour;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Sorgt dafür, dass der Shaolin bei abgebrochener Heilung (Einheit hat sich entfernt o.ä.) wieder in den korrekten Ausgangszustand wechselt.
DWORD _retAddr_EnableShaolinAutoHealEnd = 0;
__declspec(naked) void CC_EnableShaolinAutoHealEnd()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_EnableShaolinAutoHealEnd;

		// ID holen
		mov edx, dword ptr[esi + 0x04]; // this->UnitInstance
		mov edx, dword ptr[edx + 0x08]; // this->UnitInstance->UnitData
		mov dx, word ptr[edx + 0x10]; // this->UnitInstance->UnitData->ID1

		// Shaolin?
		cmp dx, 930;
		je end_auto_heal;
		cmp dx, 932;
		je end_auto_heal;
		cmp dx, 1123;
		je end_auto_heal;
		cmp dx, 1124;
		jne default_function_call;

	end_auto_heal:
		// VTable-Adresse austauschen (andere Klasse)
		//mov dword ptr[esi], 0x00638DE8;

		// Überschriebenen Funktionsaufruf durchführen
		call dword ptr[eax + 0xD8];

		// VTable-Adresse zurücksetzen, falls keine Einheit mehr geheilt wird
		mov edx, dword ptr[esi + 0x34]; // this->InteractionUnitInstanceId
		cmp edx, -1;
		jne end;
		mov dword ptr[esi], 0x0063919C;

		// Fertig, zum Ende
		jmp end;

	default_function_call:
		// Überschriebenen Funktionsaufruf durchführen
		call dword ptr[eax + 0xD8];

	end:
		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_EnableShaolinAutoHealEnd;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Ändert die ID-Abfrage zur Reliquienablage im Kloster (EAX-Version).
DWORD _retAddr_UpdateRelicDepositId1 = 0;
__declspec(naked) void CC_UpdateRelicDepositId1()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_UpdateRelicDepositId1;

		// Basiseinheit Steinkreis?
		cmp word ptr[eax + 0x14], 866;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_UpdateRelicDepositId1;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Ändert die ID-Abfrage zur Reliquienablage im Kloster (ECX-Version).
DWORD _retAddr_UpdateRelicDepositId2 = 0;
__declspec(naked) void CC_UpdateRelicDepositId2()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_UpdateRelicDepositId2;

		// Basiseinheit Steinkreis?
		cmp word ptr[ecx + 0x14], 866;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_UpdateRelicDepositId2;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Ändert die ID-Abfrage zur Reliquienablage im Kloster (EDX-Version).
DWORD _retAddr_UpdateRelicDepositId3 = 0;
__declspec(naked) void CC_UpdateRelicDepositId3()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_UpdateRelicDepositId3;

		// Basiseinheit Steinkreis?
		cmp word ptr[edx + 0x14], 866;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_UpdateRelicDepositId3;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Ändert die ID-Abfrage zur Reliquienablage im Kloster (Cursor-Abfrage).
DWORD _retAddr_UpdateRelicDepositId4 = 0;
__declspec(naked) void CC_UpdateRelicDepositId4()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_UpdateRelicDepositId4;

		// Einheit Steinkreis?
		cmp ebx, 866;
		je found;

		// Einheit Kloster?
		cmp ebx, 30;
		je found;
		cmp ebx, 31;
		je found;
		cmp ebx, 32;
		je found;
		cmp ebx, 104;
		jne not_found;

	found:
		// Gefunden, direkt zu Cursorzuweisung hinspringen
		push 0x0045B851;
		ret;

	not_found:
		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_UpdateRelicDepositId4;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Aktiviert die zweite Gebäude-Seite für gegebene Gebäude.
DWORD _retAddr_EnableSecondBuildingPage = 0;
__declspec(naked) void CC_EnableSecondBuildingPage()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_EnableSecondBuildingPage;

		// Hafen
	hafen:
		cmp word ptr[edx + 0x10], 45;
		jne kloster;

		// Zeitalter-ID abrufen
		mov eax, [edi + 0x0C];
		mov ecx, [eax + 0x000000A8];
		fld dword ptr[ecx + 0x18];

		// Zeitalter prüfen
		fcomp AGE_RESOURCE_DUNKEL;
		fnstsw ax;
		test ah, 0x41;
		jne end;

		// Button erstellen!
		jmp create_button;

		// Kloster/Steinkreis
	kloster:
		cmp word ptr[edx + 0x10], 866;
		jne festung;

		// Zeitalter-ID abrufen
		mov eax, [edi + 0x0C];
		mov ecx, [eax + 0x000000A8];
		fld dword ptr[ecx + 0x18];

		// Zeitalter prüfen
		fcomp AGE_RESOURCE_RITTER;
		fnstsw ax;
		test ah, 0x41;
		jne end;

		// Button erstellen!
		jmp create_button;

		// Festung
	festung:
		cmp word ptr[edx + 0x10], 82;
		jne end;

		// Zeitalter-ID abrufen
		mov eax, [edi + 0x0C];
		mov ecx, [eax + 0x000000A8];
		fld dword ptr[ecx + 0x18];

		// Zeitalter prüfen
		fcomp AGE_RESOURCE_IMPERIAL;
		fnstsw ax;
		test ah, 0x41;
		jne end;

		// Button erstellen
	create_button:
		push 0x00;
		push 0x00;
		mov edx, [esi + 0x00001020];
		push 0x00;
		push 0x00;
		push 0x00;
		push 0xFF;
		push 0x00001336; // Hilfetext
		push 0x00;
		push 0x000000AA;
		push 0x20; // Pfeil-Icon
		push 0x0E; // Unten rechts in der Ecke platzieren
		push edx;
		mov ecx, esi;
		call _funcCreateButton;

	end:
		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_EnableSecondBuildingPage;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Aktiviert die Ausladefunktion für die Transportkutsche. Das Verhalten ist dasselbe wie bei Rammen, jedoch mit anderem Icon.
DWORD _retAddr_TransportCartUnloadIcon = 0;
__declspec(naked) void CC_TransportCartUnloadIcon()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_TransportCartUnloadIcon;

		mov edx, [esp + 0x14];
		xor ebx, ebx;
		mov eax, [edx + 0x00000268];
		test eax, eax;
		jle cancel;
		mov eax, edx;
		lea edi, [eax + 0x000001C4];

	begin:
		mov ecx, [edi];
		test ecx, ecx;
		je label1;
		mov edx, [ecx + 0x08];

		// Transportschiff-ID?
		cmp word ptr[edx + 0x10], 545;
		je branch_ship;

		// Transportkutsche-ID?
		cmp word ptr[edx + 0x10], 954;
		je branch_cart;
		cmp word ptr[edx + 0x10], 1076;
		je branch_cart;
		jmp label1;

		// Block für das Transportschiff
	branch_ship:
		mov eax, [ecx];
		call dword ptr[eax + 0x00000218]; // Prüft, ob Schiff Ladung hat
		test eax, eax;
		jg create_button_ship;
		jmp label1;

		// Block für die Transportkutsche
	branch_cart:
		mov eax, [ecx];
		call dword ptr[eax + 0x00000218]; // Prüft, ob Kutsche Ladung hat
		test eax, eax;
		jg create_button_cart;

	label1:
		mov ecx, [esp + 0x14];
		inc ebx;
		add edi, 0x04;
		cmp ebx, [ecx + 0x00000268];
		jnge begin;
		jmp cancel;

		// Transportschiff-Button erstellen
	create_button_ship:
		mov ecx, [esp + 0x18];
		push 0x00;
		push 0x00;
		push 0x00;
		push 0x00;
		push 0x00;
		push 0xFF;
		push 4907;
		mov eax, ecx;
		push 0x00;
		inc ecx;
		push 0x07;
		push 0x11;
		jmp end;

		// Transportkutsche-Button erstellen
	create_button_cart:
		mov ecx, [esp + 0x18];
		push 0x00;
		push 0x00;
		push 0x00;
		push 0x00;
		push 0x00;
		push 0xFF;
		push 4948;
		mov eax, ecx;
		push 0x00;
		inc ecx;
		push 0xAC; // Command-Code
		push 0x45; // Icon-ID
		jmp end;

		// Abbrechen (falsche ID o.ä.)
	cancel:
		mov _retAddr_TransportCartUnloadIcon, 0x00525FDF;

	end:
		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_TransportCartUnloadIcon;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Aktiviert das Steg-Symbol, wenn eine Einheit in die Kutsche geladen werden kann.
DWORD _retAddr_TransportCartLoadCommand = 0;
__declspec(naked) void CC_TransportCartLoadCommand()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_TransportCartLoadCommand;

		// Rammbock?
		cmp ax, 35;
		je end;

		// Sturmbock?
		cmp ax, 422;
		je end;

		// Belagerungsramme?
		cmp eax, 548;
		je end;

		// Kleine Transportkutsche?
		cmp ax, 954;
		je end;

		// Große Transportkutsche?
		cmp ax, 1076;
		je end;

		// Nicht gefunden, woanders hinspringen
		mov _retAddr_TransportCartLoadCommand, 0x0045B29B;

	end:
		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_TransportCartLoadCommand;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Unterdrückt die Auto-Attack bei den Transportkutschen.
__declspec(naked) void CC_DisableTransportCartAutoAttack()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und verwerfen
		pop _garbage;

		// Einheiten-ID abrufen
		mov eax, [esi + 0x04]; // this->UnitInstance
		mov eax, [eax + 0x08]; // this->UnitInstance->UnitData

		// Kleine Transportkutsche
		cmp word ptr[eax + 0x10], 954; // this->UnitInstance->UnitData->ID1
		je disable_auto_attack;

		// Große Transportkutsche
		cmp word ptr[eax + 0x10], 1076;
		jne end;

	disable_auto_attack:
		// Überschriebenen Befehl ausführen
		mov[esp + 0x14], 4;

		// Restliche Anweisungen überspringen
		push 0x005EFA7B;
		ret;

	end:
		// Überschriebene Befehle ausführen
		mov eax, 4;
		cmp edi, eax;
		mov[esp + 0x14], eax;

		// Fertig
		push 0x005EF6F3;
		ret;
	};
}

// Codecave-Funktion.
// 
DWORD _retAddr_Renaissance1 = 0;
__declspec(naked) void CC_Renaissance1()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_Renaissance1;

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
		push _retAddr_Renaissance1;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
DWORD _retAddr_Renaissance2 = 0;
__declspec(naked) void CC_Renaissance2()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_Renaissance2;

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
		push _retAddr_Renaissance2;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
DWORD _retAddr_Renaissance3 = 0;
__declspec(naked) void CC_Renaissance3()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_Renaissance3;

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
		push _retAddr_Renaissance3;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
DWORD _retAddr_Renaissance4 = 0;
__declspec(naked) void CC_Renaissance4()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_Renaissance4;

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
		push _retAddr_Renaissance4;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
DWORD _retAddr_Renaissance5 = 0;
__declspec(naked) void CC_Renaissance5()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_Renaissance5;

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
		push _retAddr_Renaissance5;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
DWORD _retAddr_Renaissance6 = 0;
__declspec(naked) void CC_Renaissance6()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_Renaissance6;

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
		push _retAddr_Renaissance6;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// 
DWORD _retAddr_Renaissance7 = 0;
__declspec(naked) void CC_Renaissance7()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_Renaissance7;

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
		push _retAddr_Renaissance7;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Überschreibt die UserPatch-Startzeitalter-ComboBox-Änderungen.
__declspec(naked) void CC_Renaissance8()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und löschen
		pop _garbage;

		// EDI-Register sichern
		push edi;

		// Einträge für die einzelnen Startzeitalter generieren
		lea edi, [esp + 0x00000148];
		push 2; // Dunkle Zeit
		call build_starting_age_entries;
		push 3; // Feudalzeit
		call build_starting_age_entries;
		push 4; // Ritterzeit
		call build_starting_age_entries;
		push 5; // Imperialzeit
		call build_starting_age_entries;

		// EDI-Register wieder vom Stack nehmen
		pop edi;

		// Renaissance
		mov ecx, [esi + 0x00000B20];
		push 6;
		push 0x0000106D;
		call _funcComboBoxCreateEntry2;

		// Post-Renaissance
		mov ecx, [esi + 0x00000B20];
		push 7;
		push 0x0000106F; // Post-Renaissance-Language-ID ist 4207
		call _funcComboBoxCreateEntry2;

	end:
		// Hinter den schon vom UserPatch übersprungen originalen Startzeitalter-Abschnitt springen
		push 0x005055EB;
		ret;

	build_starting_age_entries:
		// Register sichern
		push ebx;
		push ebp;

		// EBX ist das negierte Endzeitalter: (Maximalzeitalter - EBX) => Die Endzeitalter werden absteigend gelistet
		xor ebx, ebx;

		// Bei höchstem Zeitalter beginnen herunterzuzählen
		mov ebp, 6;

	build_starting_age_entries_loop:
		// Hole Startzeitalter-String
		mov ecx, [esp + 0x0C];
		lea edx, [edi + 0x40];
		lea eax, [ecx + 0x00002C63]; // Neue DLL-IDs für die Abkürzungen
		push 0x20;
		push edx;
		push eax;
		call[_funcGetString];

		// Hole Endzeitalter-String
		mov ecx, [esp + 0x0C];
		lea edx, [edi + 0x60];
		lea eax, [ebp + 0x00002C63]; // Neue DLL-IDs für die Abkürzungen
		push 0x20;
		push edx;
		push eax;
		call[_funcGetString];

		// Setze die beiden Strings zusammen
		lea edx, [edi + 0x60];
		lea ecx, [edi + 0x40];
		push edx;
		push ecx;
		push 0x0066FFC8;
		push edi;
		call[_funcFormatString];
		add esp, 0x10;
		mov edx, [esp + 0x0C];
		mov eax, ebx;

		// Linksshift der Endzeitalternummer um 3 Bits, wird dann mit dem Startzeitalter ver-OR-t
		shl eax, 3;
		or al, dl;

		// ComboBox-Funktion aufrufen und Eintrag einfügen
		push eax;
		push edi;
		mov ecx, [esi + 0x00000B20];
		call[_funcComboBoxCreateEntry1];

		// Startzeitalter in EAX schieben
		mov eax, [esp + 0x0C];

		// Nächsttieferes Endzeitalter
		dec ebp;
		inc ebx;

		// Fertig?
		cmp eax, ebp;

		// Wieder hochspringen für das nächste Endzeitalter
		jle build_starting_age_entries_loop;

		// Funktionsende
		pop ebp;
		pop ebx;
		ret 4;
	};
}

// Codecave-Funktion.
// Überschreibt den rekursiven Startzeitalter-Aufruf, der alle Zeitalter bis zum aktuellen entwickelt.
__declspec(naked) void CC_Renaissance10()
{
	__asm
	{
		// Ressourcen-ID abrufen
		mov eax, [esp + 0x04];
		push esi;

		// Von EAX 23 abziehen
		add eax, -0x17;
		mov esi, ecx;

		//cmp eax, 23;
		//ja end2;

		// Zum jeweiligen Zeitalter-Block springen
		/*xor ecx, ecx;
		mov cl, [eax + 0x00458188];*/
		//jmp dword ptr[ecx * 4 + age2_x1.exe + 58170];
		// Nachfolgende Zahlen +23 ergeben in der DAT teilweise die Zivilisations-Ressourcen-IDs für die Zeitalter-IDs
		cmp eax, 35; // (3A)
		je dunkle;
		cmp eax, 2; // (19)
		je feudal;
		cmp eax, 0; // (17)
		je ritter;
		cmp eax, 1; // (18)
		je imperial;
		cmp eax, 16; // (27)
		je renaissance;
		cmp eax, 200; // (DF) => die neue Post-Renaissance ist auf 217 codiert (sollte keinen Konflikt mit existierenden Ressourcen geben)
		je post_renaissance;
		jmp end2;

		// Dunkle Zeit im Karteneditor?
	dunkle:
		mov edx, [esi + 0x000000A8];
		fld dword ptr[edx + 0x000000E8];
		jmp end1;

		// Feudalzeit
	feudal:
		mov eax, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_FEUDAL];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown3;
		mov ecx, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_DUNKEL];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown4;
		mov edx, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_FEUDAL];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown5;
		mov eax, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_FEUDAL];
		jmp end1;

		// Ritterzeit
	ritter:
		// Erst Feudalzeit aufrufen
		push 0x19;
		mov ecx, esi;
		call CC_Renaissance10;

		// Weiter
		mov ecx, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_RITTER];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown3;
		mov edx, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_FEUDAL];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown4;
		mov eax, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_RITTER];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown5;
		mov ecx, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_RITTER];
		jmp end1;

		// Imperialzeit
	imperial:
		// Erst Ritterzeit aufrufen
		push 0x17;
		mov ecx, esi;
		call CC_Renaissance10;

		// Weiter
		mov edx, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_IMPERIAL];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown3;
		mov eax, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_RITTER];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown4;
		mov ecx, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_IMPERIAL];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown5;
		mov edx, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_IMPERIAL];
		jmp end1;

		// Renaissance
	renaissance:
		// Erst Imperialzeit aufrufen
		push 0x18;
		mov ecx, esi;
		call CC_Renaissance10;

		// Weiter
		mov edx, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_RENAISSANCE];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown3;
		mov eax, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_IMPERIAL];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown4;
		mov ecx, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_RENAISSANCE];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown5;
		mov edx, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_RENAISSANCE];

		// Ende 1
	end1:
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown6;
		mov ecx, [esi + 0x000000A8];
		pop esi;
		mov[ecx + 0x0000018C], 0x00000000;
		ret 4;

		// Post-Renaissance
	post_renaissance:
		// Erst Renaissance aufrufen
		push 0x27;
		mov ecx, esi;
		call CC_Renaissance10;

		// Weiter
		mov eax, [esi + 0x000000A8];
		fld dword ptr[RESEARCH_ID_RENAISSANCE];
		call _funcFloatToInt;
		mov ecx, [esi + 0x000012A0];
		push eax;
		call _funcUnknown4;

		// Ende 2
	end2:
		mov ecx, [esi + 0x000000A8];
		pop esi;
		mov[ecx + 0x0000018C], 0x00000000;
		ret 4;
	};
}

// Codecave-Funktion.
// Überschreibt den Startzeitalter-Aufruf der jeweiligen Spieler.
__declspec(naked) void CC_Renaissance9()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und löschen
		pop _garbage;

		// Zu entsprechendem Zeitalter-Block springen
		cmp eax, 0;
		je dunkel;
		cmp eax, 1;
		je feudal;
		cmp eax, 2;
		je ritter;
		cmp eax, 3;
		je imperial;
		cmp eax, 4;
		je renaissance;
		cmp eax, 5;
		je post_renaissance;
		jmp end;

		// Dunkle Zeit
	dunkel:
		cmp[ebp + 0x48], bx;
		mov esi, ebx;
		jng end;
	dunkel_player:
		mov edx, [ebp + 0x4C];
		mov ecx, [edx + esi * 4];
		call calc_max_age_id;
		movsx eax, word ptr[ebp + 0x48];
		inc esi;
		cmp esi, eax;
		jnge dunkel_player;
		jmp end;

		// Feudalzeit
	feudal:
		cmp[ebp + 0x48], bx;
		mov esi, ebx;
		jle end;
	feudal_player: // Spieler-Schleife
		mov edx, [ebp + 0x4C];
		push 0x19;
		mov ecx, [edx + esi * 4];
		{ // Ehemals UserPatch-Funktion
			push esi;
			mov esi, ecx;
			mov eax, [esp + 0x04];
			push eax;
			call CC_Renaissance10;
			mov ecx, esi;
			call calc_max_age_id;
			pop esi;

			// Ein Objekt mehr vom Stack nehmen, wird verworfen
			pop _garbage;
		}
		movsx eax, word ptr[ebp + 0x48];
		inc esi;
		cmp esi, eax;
		jnge feudal_player;
		jmp end;

		// Ritterzeit
	ritter:
		cmp[ebp + 0x48], bx;
		mov esi, ebx;
		jle end;
	ritter_player: // Spieler-Schleife
		mov ecx, [ebp + 0x4C];
		push 0x17;
		mov ecx, [ecx + esi * 4];
		{ // Ehemals UserPatch-Funktion
			push esi;
			mov esi, ecx;
			mov eax, [esp + 0x04];
			push eax;
			call CC_Renaissance10;
			mov ecx, esi;
			call calc_max_age_id;
			pop esi;

			// Ein Objekt mehr vom Stack nehmen, wird verworfen
			pop _garbage;
		}
		movsx edx, word ptr[ebp + 0x48];
		inc esi;
		cmp esi, edx;
		jnge ritter_player;
		jmp end;

		// Imperialzeit
	imperial:
		cmp[ebp + 0x48], bx;
		mov esi, ebx;
		jle end;
	imperial_player: // Spieler-Schleife
		mov ecx, [ebp + 0x4C];
		push 0x18;
		mov ecx, [ecx + esi * 4];
		{ // Ehemals UserPatch-Funktion
			push esi;
			mov esi, ecx;
			mov eax, [esp + 0x04];
			push eax;
			call CC_Renaissance10;
			mov ecx, esi;
			call calc_max_age_id;
			pop esi;

			// Ein Objekt mehr vom Stack nehmen, wird verworfen
			pop _garbage;
		}
		movsx edx, word ptr[ebp + 0x48];
		inc esi;
		cmp esi, edx;
		jnge imperial_player;
		jmp end;

		// Renaissance
	renaissance:
		cmp[ebp + 0x48], bx;
		mov esi, ebx;
		jle end;
	renaissance_player: // Spieler-Schleife
		mov eax, [ebp + 0x4C];
		push 0x27;
		mov ecx, [eax + esi * 4];
		call CC_Renaissance10;
		movsx ecx, word ptr[ebp + 0x48];
		inc esi;
		cmp esi, ecx;
		jnge renaissance_player;
		jmp end;

		// Post-Renaissance
	post_renaissance:
		cmp[ebp + 0x48], bx;
		mov esi, ebx;
		jle end;
	post_renaissance_player: // Spieler-Schleife
		mov edx, [ebp + 0x4C];
		push 0xDF;
		mov ecx, [edx + esi * 4];
		call CC_Renaissance10;
		movsx eax, word ptr[ebp + 0x48];
		inc esi;
		cmp esi, eax;
		jnge post_renaissance_player;
		jmp end;

		// Berechnet die Technologie-ID des Endzeitalters.
	calc_max_age_id:
		xor edx, edx; // Workaround, da der Compiler irgendwie absolute Adressen nicht mag...
		mov edx, [edx + 0x007912A0];
		mov eax, [edx + 0x00001440];
		shr eax, 3;
		test eax, eax;
		je calc_max_age_id_end;
		mov edx, 105; // Höchstmögliche Zeitalter-ID
		sub edx, eax;
		push 0;
		push edx;
		call _funcUnknown2;
	calc_max_age_id_end:
		ret;

	end:
		// Hinter die überschriebene Funktion springen, spart NOPs und ist schneller
		push 0x0042A7BF;
		ret;
	};
}

// Codecave-Funktion.
// Überschreibt den Zeitalter-Aufruf beim "Test"-Button des Karteneditors.
DWORD _retAddr_Renaissance11 = 0;
__declspec(naked) void CC_Renaissance11()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_Renaissance11;

		// Zeitalter-Zahl abgleichen
		cmp eax, 4;
		ja end;

		// Sprung ausführen
		cmp eax, 0;
		je feudal;
		cmp eax, 1;
		je ritter;
		cmp eax, 2;
		je imperial;
		cmp eax, 3;
		je renaissance;
		jmp post_renaissance;

		// Feudalzeit
	feudal:
		push 0x19;
		jmp age_call;

		// Ritterzeit
	ritter:
		push 0x17;
		jmp age_call;

		// Imperialzeit
	imperial:
		push 0x18;
		jmp age_call;

		// Renaissance
	renaissance:
		push 0x27;
		jmp age_call;

		// Post-Renaissance
	post_renaissance:
		push 0xDF;

	age_call:
		// Aufruf durchführen
		mov ecx, edi;
		call CC_Renaissance10;

	end:
		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_Renaissance11;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Überschreibt den Zeitalter-Aufruf beim Verstellen des Startzeitalters im Karteneditor.
DWORD _retAddr_Renaissance12 = 0;
__declspec(naked) void CC_Renaissance12()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_Renaissance12;

		// Zeitalter-Zahl abgleichen
		cmp esi, 5;
		ja end;

		// Sprung ausführen
		cmp esi, 0;
		je dunkel;
		cmp esi, 1;
		je feudal;
		cmp esi, 2;
		je ritter;
		cmp esi, 3;
		je imperial;
		cmp esi, 4;
		je renaissance;
		jmp post_renaissance;

		// Dunkle Zeit
	dunkel:
		push 0x3A;
		jmp age_call;

		// Feudalzeit
	feudal:
		push 0x19;
		jmp age_call;

		// Ritterzeit
	ritter:
		push 0x17;
		jmp age_call;

		// Imperialzeit
	imperial:
		push 0x18;
		jmp age_call;

		// Renaissance
	renaissance:
		push 0x27;
		jmp age_call;

		// Post-Renaissance
	post_renaissance:
		push 0xDF;

	age_call:
		// Aufruf durchführen
		mov ecx, edi;
		call CC_Renaissance10;

	end:
		mov eax, [edi + 0x78];

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_Renaissance12;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Überschreibt den Aufruf zur Füllung der Startzeitalter-ComboBox im Editor.
DWORD _retAddr_Renaissance13 = 0;
__declspec(naked) void CC_Renaissance13()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und sichern
		pop _retAddr_Renaissance13;

		// Dunkle Zeit
		mov ecx, [esi + 0x00000A34];
		push 2;
		push 0x00001069;
		call _funcComboBoxCreateEntry2;

		// Feudalzeit
		mov ecx, [esi + 0x00000A34];
		push 3;
		push 0x0000106A;
		call _funcComboBoxCreateEntry2;

		// Ritterzeit
		mov ecx, [esi + 0x00000A34];
		push 4;
		push 0x0000106B;
		call _funcComboBoxCreateEntry2;

		// Imperialzeit
		mov ecx, [esi + 0x00000A34];
		push 5;
		push 0x0000106C;
		call _funcComboBoxCreateEntry2;

		// Renaissance
		mov ecx, [esi + 0x00000A34];
		push 6;
		push 0x0000106D;
		call _funcComboBoxCreateEntry2;

		// Post-Renaissance
		mov ecx, [esi + 0x00000A34];
		push 7;
		push 0x0000106F;
		call _funcComboBoxCreateEntry2;

		// Rücksprungadresse wieder auf den Stack legen
		push _retAddr_Renaissance13;

		// Fertig
		ret;
	};
}

// Codecave-Funktion.
// Deaktiviert die Bekehrung des Bundschuh-Bauern.
__declspec(naked) void CC_DisableBundschuhConversion()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und verwerfen
		pop _garbage;

		// Pferd? (Klasse)
		cmp dx, 61;
		je disable;

		// Bundschuh-Bauer? (ID)
		cmp[ebx + 0x10], 1084;
		jne not_matched;

	disable:
		// Einheit ist nicht bekehrbar
		push 0x004B7CC0;
		ret;

	not_matched:
		// Einheit hier nicht betrachtet
		push 0x004B7D05;
		ret;
	};
}

// Codecave-Funktion.
// -> Macht Bekehrungen von bestimten Einheitentypen unendlich (Lamaismus/Buchdruck-Technologien).
//    Hier wird davon ausgegangen, dass bei Entwicklung der Technologie die (ungenutzte) Ressource #68 des Bekehrungsopfers mit dem Hilfswert 0x10 bzw. 0x20 addiert wird.
// -> Verlängert Bekehrungen von bestimmten Einheitentypen um 40% (hardcoded) (Finkennetze-Technologie)
//    Hier wird davon ausgegangen, dass bei Entwicklung der Technologie die (ungenutzte) Ressource #68 des Bekehrungsopfers mit dem Hilfswert 0x80 addiert wird.
// -> Bestimmt die Auszahlung einer Belohnung beim Kapern eines Schiffes (Prise-Technologie).
//    Hier wird davon ausgegangen, dass bei Entwicklung der Technologie die (ungenutzte) Ressource #68 des Bekehrenden mit dem Hilfswert 0x40 addiert wird.
// Diese Funktion bestimmt die zugehörigen Flags, die von der nachfolgenden Funktion dann angewendet werden.
int _resource68ConvertedValue;
int _conversionResistConvertedValue;
__declspec(naked) void CC_ConversionCalcModeFlag()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und verwerfen
		pop _garbage;

		// Finkennetze-Technologie-Flag zurücksetzen
		mov _conversionResistForSpecificShips, 0;

		// Register sichern, werden hier in der Funktion fleißig überschrieben
		// Jaaa, ziemlich verschwenderisch, aber Einfachheit siegt über Geschwindigkeit, und soo oft wird die Funktion nun nicht aufgerufen
		push eax;
		push ebx;
		push ecx;
		push edx;

		// In EBX Klasse der Opfer-Einheit schreiben
		mov bx, word ptr[edx + 0x16];

		// In EDX Klasse der bekehrenden Einheit schreiben
		mov dx, word ptr[edi + 0x16];

		// In CL kommt das ermittelte Blockier-Flag, Standardwert 0
		xor ecx, ecx;

		// Ressource #68 des Bekehrungsopfers abrufen
		fld dword ptr[ebp + 0x00000110]; // = 4 * 68

		// Ressource #68 in Ganzzahl konvertieren (und vom FPU-Stack nehmen)
		// Falls es Rundungsprobleme gibt, die Ressource auf einen Startwert 1 setzen, da dieses Bit ohnehin nie abgefragt wird
		fistp _resource68ConvertedValue;
		mov eax, _resource68ConvertedValue;

		// Bit 4 des Opfer-Spielers pruefen: Militaereinheiten unbekehrbar
		test ax, 0x0010;
		je check_bit5;

		// Bit 4 gesetzt -> Opfer-Einheit-Klasse pruefen
		cmp bx, 0; // Bogenschützen
		je set_mode_flag1;
		cmp bx, 6; // Infanterie
		je set_mode_flag1;
		cmp bx, 13; // B-Waffen
		je set_mode_flag1;
		cmp bx, 23; // Konquistadoren
		je set_mode_flag1;
		cmp bx, 24; // Kriegselefanten
		je set_mode_flag1;
		cmp bx, 26; // Schützenelefanten
		je set_mode_flag1;
		cmp bx, 35; // Petarden
		je set_mode_flag1;
		cmp bx, 36; // Ber. Bogenschützen
		je set_mode_flag1;
		cmp bx, 45; // Zw.-Hd. Schwertkämpfer
		je set_mode_flag1;
		cmp bx, 46; // Pikeniere
		je set_mode_flag1;
		cmp bx, 47; // Ber. Späher
		je set_mode_flag1;
		cmp bx, 50; // Speerkämpfer
		je set_mode_flag1;
		cmp bx, 51; // Gepackte B-Waffen
		je set_mode_flag1;
		cmp bx, 54; // Ungepackte B-Waffen
		je set_mode_flag1;
		cmp bx, 55; // Skorpione
		je set_mode_flag1;
		cmp bx, 56; // Raider?
		je set_mode_flag1;
		cmp bx, 57; // Berittener Raider?
		je set_mode_flag1;

		// Bit 5 des Opfer-Spielers pruefen: Zivileinheiten unbekehrbar
	check_bit5:
		test ax, 0x0020;
		je check_bit7;

		// Bit 5 gesetzt -> Opfer-Einheit-Klasse pruefen
		cmp bx, 2; // Handelsschiff
		je set_mode_flag1;
		cmp bx, 4; // Zivilist
		je set_mode_flag1;
		cmp bx, 19; // Marktkarren
		je set_mode_flag1;
		cmp bx, 21; // Fischkutter
		je set_mode_flag1;

		// Bit 7 des Opfer-Spielers pruefen: Galeeren- und Karackenlinien schwerer bekehrbar
	check_bit7:
		test ax, 0x0080;
		je check_bit6;

		// Merken, dass Bit gesetzt => wird später angewandt
		mov _conversionResistForSpecificShips, 1;

		// Bit 6 des bekehrenden Spielers pruefen: Prise-Technologie ("Belohnung" nach Bekehrung durch Kaperschiffe)
	check_bit6:
		// Ressource #68 des Bekehrenden abrufen
		mov eax, dword ptr[esi + 0x08];
		mov eax, dword ptr[eax + 0x0C];
		mov eax, dword ptr[eax + 0xA8];
		fld dword ptr[eax + 0x00000110]; // = 4 * 68
		fistp _resource68ConvertedValue;
		mov eax, _resource68ConvertedValue;
		test ax, 0x0040;
		je check_end;

		// Bit 6 gesetzt -> Bekehrungs-Einheit-Klasse pruefen
		cmp dx, 53; // Kaperschiff
		jne check_end;

		// Opfer-Einheit-Klasse pruefen, um Prisenwert bestimmen zu können
		cmp bx, 2; // Handelsschiff
		je set_mode_flag2;
		cmp bx, 20; // Transportschiff
		je set_mode_flag2;
		cmp bx, 22; // Kriegsschiff
		je set_mode_flag3;
		jmp check_end;

	set_mode_flag1:
		// Bekehrungs-Blockierungs-Flag setzen
		mov cl, 1;
		jmp check_end;

	set_mode_flag2:
		// Prisen-Flag setzen
		mov cl, 2;
		jmp check_end;

	set_mode_flag3:
		// Prisen-Flag setzen
		mov cl, 3;
		jmp check_end;

	set_mode_flag4:
		// Bekehrung erschweren-Flag setzen
		mov cl, 4;

	check_end:
		// Flagwert zuweisen
		mov _conversionModeFlag, cl;

		// Zuerst EDX-Register wiederherstellen, wird eventuell benötigt
		pop edx;

		// Bekehrwiderstand abrufen
		fld dword ptr[ebp + 0x00000134]; // = 4 * 77

		// Auf bereits berechneten Widerstand addieren
		fadd dword ptr[esp + 0x20];

		// Ggf. Spezialwiderstand addieren? (Finkennetze)
		cmp _conversionResistForSpecificShips, 1;
		jne calc_resistance_end;

		// ID der Opfer-Einheit bestimmen
		mov bx, word ptr[edx + 0x10];
		cmp bx, 1121; // Karacke
		je set_fink_net_resistance_flag;
		cmp bx, 1122; // Gepanzerte Karacke
		je set_fink_net_resistance_flag;
		cmp bx, 442; // Galeone
		je set_fink_net_resistance_flag;
		cmp bx, 1116; // Galeere
		je set_fink_net_resistance_flag;
		cmp bx, 1117; // Kriegsgaleere
		je set_fink_net_resistance_flag;
		cmp bx, 1118; // Große Kriegsgaleere
		je set_fink_net_resistance_flag;
		cmp bx, 1112; // Galeasse
		jne calc_resistance_end;

	set_fink_net_resistance_flag:
		// Flag aktualisieren
		mov _conversionResistForSpecificShips, 2;

	calc_resistance_end:
		// Geänderten Widerstand in lokale Variable legen
		fstp dword ptr[esp + 0x1C];

		// Restliche Register wiederherstellen
		pop ecx;
		pop ebx;
		pop eax;

		// Fertig
		push 0x004B8676;
		ret;
	};
}

// Codecave-Funktion.
// Verlängert ggf. die Maximalzeit einer Bekehrung (Finkennetze-Technologie).
__declspec(naked) void CC_ConversionModifyMaximumTimeCalculation()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und verwerfen
		pop _garbage;

		// Minimalwerte addieren
		fld dword ptr[ecx + 0x2C0];
		fadd dword ptr[ebp + 0x2C8];
		fadd st, st(1);

		// Flag prüfen
		cmp _conversionResistForSpecificShips, 2;
		jne check_minimum;

		// Minimalwert-Summand anwenden
		fadd dword ptr[CONVERSION_TIME_SUMMAND_FINK_NETS];

	check_minimum:
		// Prüfung durchführen
		fld dword ptr[esi + 0x48];
		fcompp;
		fnstsw ax;
		test ah, 1;
		fstp st;
		je mimimum_reached;

		// Minimalwert noch nicht erreicht, auf keinen Fall bekehren
		mov edx, 0x0FFFFFC18;
		jmp end;

		// Minimalwert erreicht -> Maximalwert überschritten?
	mimimum_reached:
		fld dword ptr[ecx + 0x2C4];
		fadd dword ptr[ebp + 0x2CC];
		fadd dword ptr[esp + 0x10];

		// Flag prüfen
		cmp _conversionResistForSpecificShips, 2;
		jne check_maximum;

		// Maximalwert-Summand anwenden
		fadd dword ptr[CONVERSION_TIME_SUMMAND_FINK_NETS];

	check_maximum:
		// Prüfung durchführen
		fcomp dword ptr[esi + 0x48];
		fnstsw ax;
		test ah, 0x41;
		je end; // Maximalwert noch nicht überschritten

		// Maximalwert überschritten, auf jeden Fall bekehren
		mov edx, 0x3E8;

	end:
		// Direkt zurückspringen
		push 0x004B872F;
		ret;
	};
}

// Codecave-Funktion.
// Macht Bekehrungen von bestimmten Einheitentypen unendlich (Lamaismus/Buchdruck-Technologien).
// Dies Funktion wendet das von der vorhergehenden Funktion gesetzte Blockier-Flag an.
__declspec(naked) void CC_ConversionExecBlockFlag()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und verwerfen
		pop _garbage;

		// Blockier-Flag gesetzt?
		cmp _conversionModeFlag, 1;
		jne compare;

		// Flag ist gesetzt => keine Bekehrung durchführen
		mov edx, -1000;

	compare:
		// Vergleich durchführen
		cmp dword ptr[esp + 0x14], edx;
		jg end_jump;

		// Fertig, keinen "echten" Sprung machen
		push 0x004B8739;
		ret;

	end_jump:
		// Fertig, springen wie im Original vorgesehen
		push 0x004B887F;
		ret;
	};
}

// Codecave-Funktion.
// Überweist die Bekehrungs-"Belohnung" im Fall der "Prise"-Technologie.
// Dies Funktion wendet das von der vorhergehenden Funktion gesetzte Blockier-Flag an.
DWORD _retAddr_ConversionApplyCaptureResources = 0;
__declspec(naked) void CC_ConversionApplyCaptureResources()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen und verwerfen
		pop _retAddr_ConversionApplyCaptureResources;

		// Überschriebene Befehle ausführen, um Adresse der Virtual Function Table des Spielerobjekts zu bekommen (-> EDX-Register)
		mov ecx, [eax + 0x0C];
		mov edx, [ecx];

		// Belohnungs-Flag für Transport-/Handelsschiffe gesetzt?
		cmp _conversionModeFlag, 2;
		jne case_warship;

		// Flag ist gesetzt => Belohnung auszahlen
		push 0; // Unbekannter Parameter
		push 0x42C80000; // Menge: 100.0f
		push 3; // Ressourcen-ID: Gold
		call dword ptr[edx + 0x78];
		jmp revert_registers;

	case_warship:
		// Belohnungs-Flag für Kriegsschiffe gesetzt?
		cmp _conversionModeFlag, 3;
		jne end;

		// Flag ist gesetzt => Belohnung auszahlen
		push 0; // Unbekannter Parameter
		push 0x42480000; // Menge: 50.0f
		push 3; // Ressourcen-ID: Gold
		call dword ptr[edx + 0x78];

	revert_registers:
		// Überschriebene Befehle erneut ausführen, da Registerwerte ggf. verlorengegangen
		mov eax, [esi + 0x08];
		mov ecx, [eax + 0x0C];
		mov edx, [ecx];

	end:
		// Fertig
		push _retAddr_ConversionApplyCaptureResources;
		ret;
	};
}

// Codecave-Funktion.
// Verändert die Maximalhöhe der Hilfetext-Box.
__declspec(naked) void CC_ExtendHelpTextBox()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen
		pop eax;

		// Überschriebenen Befehl ausführen
		push 0;

		// Neue Höhe auf den Stack legen
		push 140;
		push 140;

		// Fertig
		push eax;
		ret;
	};
}

// Codecave-Funktion.
// Sorgt dafür, dass der Mineur nach Selbstsprengung nicht wiederbelebt wird.
DWORD _retAddr_EnableMineurSelfDestroy1 = 0;
__declspec(naked) void CC_EnableMineurSelfDestroy1()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen
		pop _retAddr_EnableMineurSelfDestroy1;

		// Überschriebenen Befehl ausführen
		cmp ax, 440; // Petarde
		je self_destroy_handler;

		// Mineur
		cmp ax, 1064;
		jne end;

	self_destroy_handler:
		// Zum Selbstzerstörungshandler springen
		push _retAddr_EnableMineurSelfDestroy1;
		ret;

	end:
		// Handler überspringen
		push 0x004C16B6;
		ret;
	};
}

// Codecave-Funktion.
// Sorgt dafür, dass der Mineur nach Selbstsprengung nicht wiederbelebt wird.
DWORD _retAddr_EnableMineurSelfDestroy2 = 0;
__declspec(naked) void CC_EnableMineurSelfDestroy2()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen
		pop _retAddr_EnableMineurSelfDestroy2;

		// Überschriebenen Befehl ausführen
		cmp ax, 440; // Petarde
		je self_destroy_handler;

		// Mineur
		cmp ax, 1064;
		jne end;

	self_destroy_handler:
		// Zum Selbstzerstörungshandler springen
		push _retAddr_EnableMineurSelfDestroy2;
		ret;

	end:
		// Handler überspringen
		push 0x004C2D1F;
		ret;
	};
}

// Codecave-Funktion.
// Sorgt dafür, dass die "Ketzerei"-Technologie nicht mehr auf Schiffe wirkt.
DWORD _retAddr_DisableShipDestructionOnConversion = 0;
__declspec(naked) void CC_DisableShipDestructionOnConversion()
{
	__asm
	{
		// Rücksprungadresse vom Stack holen
		pop _retAddr_DisableShipDestructionOnConversion;

		// Klasse abrufen
		mov eax, dword ptr[ecx + 0x08]; // this->UnitData
		mov ax, word ptr[eax + 0x16]; // this->UnitData->Class

		// Klasse prüfen
		cmp ax, 2; // Handelsschiff
		je convert_unit;
		cmp ax, 20; // Transportschiff
		je convert_unit;
		cmp ax, 21; // Fischkutter
		je convert_unit;
		cmp ax, 22; // Kriegsschiff
		je convert_unit;
		cmp ax, 53; // Kaperschiff
		je convert_unit;

		// Aufruf ausführen: Einheit töten
		call dword ptr[edx + 0x74];

		// Fertig, überschriebenen JMP-Befehl ausführen
		push 0x004B8839;
		ret;

	convert_unit:
		// Einheit doch bekehren
		push _retAddr_DisableShipDestructionOnConversion;
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
	// TODO: Karteneditor-Zeitalterfunktionen bei 369AA und DFE60

	// Codecaves erstellen
	CreateCodecave(0x004C1795, CC_ExtendSelfHealing, 5);
	CreateCodecave(0x00525B2E, CC_CreateShaolinHealingButton, 1);
	CreateCodecave(0x00434AC6, CC_EnableShaolinHealingAbility, 1);
	CreateCodecave(0x00434B2E, CC_KeepShaolinFightingAbility, 1);
	CreateCodecave(0x0045AD35, CC_SetShaolinHealingFlagAndRemoveTransportCartAttackFlag, 0);
	CreateCodecave(0x0046FF43, CC_EnableShaolinAutoHeal, 2);
	CreateCodecave(0x0046E051, CC_PatchShaolinAutoHealDistanceCalculation, 5);
	CreateCodecave(0x004B4BB3, CC_PatchShaolinHealAssignmentBehaviour, 1);
	CreateCodecave(0x005EEADE, CC_EnableShaolinAutoHealEnd, 1);
	CreateCodecave(0x004B165E, CC_UpdateRelicDepositId1, 0);
	CreateCodecave(0x004B17BA, CC_UpdateRelicDepositId3, 0);
	CreateCodecave(0x004B198E, CC_UpdateRelicDepositId2, 0);
	CreateCodecave(0x0045B70D, CC_UpdateRelicDepositId4, 4);
	CreateCodecave(0x00528307, CC_EnableSecondBuildingPage, 69);
	CreateCodecave(0x00525F58, CC_TransportCartUnloadIcon, 103);
	CreateCodecave(0x0045B7D1, CC_TransportCartLoadCommand, 18);
	CreateCodecave(0x005EF6E8, CC_DisableTransportCartAutoAttack, 6);
	CreateCodecave(0x0040259A, CC_Renaissance1, 141);
	CreateCodecave(0x005AAD61, CC_Renaissance2, 48);
	CreateCodecave(0x0043FF59, CC_Renaissance3, 16);
	CreateCodecave(0x0043FFF8, CC_Renaissance4, 26);
	CreateCodecave(0x0051FF37, CC_Renaissance5, 72);
	CreateCodecave(0x0043D051, CC_Renaissance6, 87);
	CreateCodecave(0x004ACF51, CC_Renaissance7, 43);
	CreateCodecave(0x00505591, CC_Renaissance8, 0);
	CreateCodecave(0x0042A73A, CC_Renaissance9, 0);
	CreateCodecave(0x0043698E, CC_Renaissance11, 28);
	CreateCodecave(0x004DFE40, CC_Renaissance12, 35);
	CreateCodecave(0x004ED2CE, CC_Renaissance13, 85);
	CreateCodecave(0x004B7CBA, CC_DisableBundschuhConversion, 1);
	CreateCodecave(0x004B8655, CC_ConversionCalcModeFlag, 1);
	CreateCodecave(0x004B86ED, CC_ConversionModifyMaximumTimeCalculation, 0);
	CreateCodecave(0x004B872F, CC_ConversionExecBlockFlag, 5);
	CreateCodecave(0x004B8845, CC_ConversionApplyCaptureResources, 0);
	CreateCodecave(0x0051AF03, CC_ExtendHelpTextBox, 1);
	CreateCodecave(0x004C1695, CC_EnableMineurSelfDestroy1, 1);
	CreateCodecave(0x004C2CE6, CC_EnableMineurSelfDestroy2, 1);
	CreateCodecave(0x004B8828, CC_DisableShipDestructionOnConversion, 0);

	// 457FC0 ist in die Renaissance-Codecave 10 gewandert
	CreateCodecave(0x00427291, CC_Renaissance10, 0);
	CreateCodecave(0x004272A4, CC_Renaissance10, 0);
	CreateCodecave(0x004272B7, CC_Renaissance10, 0);
	CreateCodecave(0x004272CA, CC_Renaissance10, 0);
	CreateCodecave(0x00429A41, CC_Renaissance10, 0);
	CreateCodecave(0x00429ACA, CC_Renaissance10, 0);
	CreateCodecave(0x00429C15, CC_Renaissance10, 0);
	CreateCodecave(0x007D8E8A, CC_Renaissance10, 0);

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
		CopyBytesToAddr(0x005280D9, patch1, 1);

		// Zeitalter-Anzahl ändern
		patch1[0] = 0x05;
		CopyBytesToAddr(0x0042A733, patch1, 1);

		// Zeitalter-Civ-Ressourcen-IDs ändern
		patch1[0] = 0xDF; // Post-Renaissance
		CopyBytesToAddr(0x00429A3D, patch1, 1);
		CopyBytesToAddr(0x00429AC6, patch1, 1);
		CopyBytesToAddr(0x00429C11, patch1, 1);

		// TODO: Endzeitalter-KI-Konstante hinzufügen bei 0x007AF140
	}

	// Neue ID für die Einsaat-Warteschleife-Meldung (Kollision mit Renaissance-Meldung)
	BYTE patch[2] = { 0x0A, 0x0C };
	CopyBytesToAddr(0x00529A76, patch, 2);

	// TODO: Einzelne Memory-Patches bei 2667+ (90er-Werte)?

	// Cheat-Ressourcenmengen vergrößern
	union
	{
		float val;
		BYTE bytes[4];
	} cheatResourceAmount;
	cheatResourceAmount.val = 10000;
	CopyBytesToAddr(0x0042C2AE, cheatResourceAmount.bytes, 4);
	CopyBytesToAddr(0x0042C2CE, cheatResourceAmount.bytes, 4);
	CopyBytesToAddr(0x0042C2EE, cheatResourceAmount.bytes, 4);
	CopyBytesToAddr(0x0042C30E, cheatResourceAmount.bytes, 4);

	// Haus-Sonderbehandlung entfernen, damit auch dort Technologien entwickelt werden können
	BYTE nopPatch[11] = { 0x66, 0x90, 0x66, 0x90, 0x90, 0x66, 0x90, 0x66, 0x90, 0x66, 0x90 }; // 2-Byte NOPs benutzen
	CopyBytesToAddr(0x004C8FBC, nopPatch, 11);
	patch[0] = 0;
	CopyBytesToAddr(0x004C8C7E, patch, 1);
	CopyBytesToAddr(0x004C7FE9, patch, 1);

	// Neuen Language-DLL-String-Puffer installieren
	DWORD newStringBufferAddress = reinterpret_cast<DWORD>(_newLanguageDllStringBuffer);
	BYTE *newStringBufferAddressBytes = reinterpret_cast<BYTE *>(&newStringBufferAddress);
	CopyBytesToAddr(0x00562CEA, newStringBufferAddressBytes, 4);
	CopyBytesToAddr(0x00562CF5, newStringBufferAddressBytes, 4);
	BYTE newStringBufferSize_SecondByte = 0x03; // Größe ist eigentlich 0x0300, es muss aber nur Byte 1 (vorher 0x02) durch 0x03 ausgetauscht werden, Byte 0 bleibt gleich
	CopyBytesToAddr(0x00562CE6, &newStringBufferSize_SecondByte, 1);

	// Fenster-Titel ändern für Preview
	char *patchTitle = "Agearena AddOn Dev Preview\0\0";
	CopyBytesToAddr(0x0067B838, patchTitle, 27);
}

