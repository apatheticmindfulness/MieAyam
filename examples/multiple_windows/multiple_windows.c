#include "mieayam_window.h"

int main()
{
	// Inisialisasi Pustaka MieAyam
	MieAyam_Init();

	// Definisikan struktur atribut window sebagai array
	mieayam_window_attributes window[] =
	{
		// Window index 0
		{
			.width = 800,
			.height = 600,
			.title = "First window"
		},

		// Window index 1
		{
			.width = 600,
			.height = 400,
			.title = "Second window"
		}
	};

	// Buat dua window
	MieAyam_CreateWindow(window, 2);

	// Tampilkan window berdasarkan window index-nya
	MieAyam_ShowWindow(0); // Menampilkan window dengan index 0
	MieAyam_ShowWindow(1); // Menampilkan window dengan index 1

	// Jalankan proses per frame
	while (MieAyam_RunProccess())
	{
		// ...
	}
}