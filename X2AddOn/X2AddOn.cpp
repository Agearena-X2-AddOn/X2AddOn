/*
--- X2-ADDON :: LOADER ---
*/

/* INCLUDES */

// Windows-Funktionen
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// C-Standard-input/Output-Funktionen
#include <cstdio>

// Injection-Funktionen
#include "injection.h"


/* FUNKTIONEN */

// Programm-Einstiegsfunktion.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int cmdShow)
{
	// Pfad zum Arbeitsverzeichnis ist der aktuelle Ordner
	// Der String hat MAX_STRING_LENGTH-L�nge, um bei den Ladefunktionen einfacher kopiert werden zu k�nnen
	char workdir[MAX_STRING_LENGTH + 1] = { 0 };
	GetCurrentDirectory(MAX_STRING_LENGTH, workdir);

	// Pfad zur EXE
	char exepath[MAX_STRING_LENGTH + 1] = { 0 };

	// EXE-Parameter mit Pfad verkn�pfen
	// nostartup: Keine Videos am Anfang
	_snprintf_s(exepath, MAX_STRING_LENGTH, MAX_STRING_LENGTH, "\"%s\\age2_x1.exe\" game=AgearenaAddOn nostartup", workdir);

	// Prozessstart-Variablen
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };

	// Gr��e der STARTUPINFO-Struktur �bergeben
	si.cb = sizeof(STARTUPINFO);

	// EXE starten, aber noch nicht ausf�hren (suspend)
	int res = CreateProcess(NULL, exepath, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, workdir, &si, &pi);
	if(res == 0)
	{
		// Mist
		MessageBoxA(0, "Konnte age2_x1.exe nicht starten!", "Fehler", MB_ICONERROR);

		// Abbruch
		return -1;
	}

	// Haupt-DLL-Injection durchf�hren
	char dllpath[MAX_STRING_LENGTH + 1] = { 0 };
	_snprintf_s(dllpath, MAX_STRING_LENGTH, MAX_STRING_LENGTH, "AgearenaAddOn.dll");
	InjectDLL(pi.hProcess, dllpath, "Init");

	// TechTree-DLL-Injection durchf�hren
	_snprintf_s(dllpath, MAX_STRING_LENGTH, MAX_STRING_LENGTH, "TechTree.dll");
	InjectDLL(pi.hProcess, dllpath, "Init");
	
	// Prozess-Ausf�hrung beginnen
	ResumeThread(pi.hThread);

	// Alles gut
	return 0;
}


