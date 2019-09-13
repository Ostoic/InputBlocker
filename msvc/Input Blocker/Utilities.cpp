#include "Utilities.h"
#include <Windows.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>

void RedirectConsole(bool state)
{
	HANDLE handle_out;
	HANDLE handle_in;
	int hCrt;

	FILE *hf_out = NULL;
	FILE *hf_in	 = NULL;

	if (state)
	{
		AllocConsole();

		// Get stdout handle.
		handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
		hCrt = _open_osfhandle((long) handle_out, _O_TEXT);

		// Open console for writing
		hf_out = _fdopen(hCrt, "w");

		setvbuf(hf_out, NULL, _IONBF, 1);

		// Redirect stdout to console handle.
		*stdout = *hf_out;

		// Get stdin handle.
		handle_in = GetStdHandle(STD_INPUT_HANDLE);

		// Open console for reading.
		hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
		hf_in = _fdopen(hCrt, "r");

		setvbuf(hf_in, NULL, _IONBF, 128);

		// Redirect stdin to console handle.
		*stdin = *hf_in;
	}
	else
	{
		FreeConsole();
	}
}