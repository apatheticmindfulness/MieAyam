# MieAyam
Pustaka sederhana untuk keperluan pemrograman berbasis Windows dengan Win32 API.

## Cara Penggunaan

#### Pembuatan Window (single window)
```cpp
#include "mieayam_window.h"

int main()
{
	// Inisialisasi Pustaka MieAyam
	MieAyam_Init();
	
	// Definisikan atribut pada window
	mieayam_window_attributes window =
	{
		.width = 800,
		.height = 600,
		.title = "Amazing"
	};
	
	// Buat window
	MieAyam_CreateWindow(&window, 1);
	
	// Tampilkan window, 0 untuk single window
	MieAyam_ShowWindow(0);
	
	// Jalankan proses per frame
	while (MieAyam_RunProccess())
	{
	   // ...
	}
}
```

#### Pembuatan Window (multiple windows)
```cpp
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
```

#### Penggunaan Grafis
```cpp
#include "mieayam_window.h"
#include "mieayam_basic_graphics.h"

int main()
{
	// Inisialisasi Pustaka MieAyam
	MieAyam_Init();
	
	// Definisikan atribut pada window
	mieayam_window_attributes window =
	{
		.width = 800,
		.height = 600,
		.title = "Amazing"
	};
	
	// Definisikan atribut pada grafis
	mieayam_basic_graphics_attributes graphics =
	{
		.width = 800,
		.height = 600,
		.windowIndex = 0,
		.bitCount = 32
	};
	
	// Buat window
	MieAyam_CreateWindow(&window, 1);
	
	// Tampilkan window, 0 untuk single window
	MieAyam_ShowWindow(0);

	// Inisialisasi Grafis
	MieAyam_InitBasicGraphics(&graphics, 1);

	// Tampilkan window index 0
	MieAyam_ShowWindow(0);
	
	// Jalankan proses per frame
	while (MieAyam_RunProccess())
	{
		mieayam_color color = MIEAYAM_COLOR_CORNFLOWERBLUE;
		
		// Menggambar rectangle
		MieAyam_RenderStart(0);
		for (int32_t y = 100; y < 200; y++)
		{
			for (int32_t x = 100; x < 200; x++)
			{
				MieAyam_SetPixel(x, y, color);
			}
		}
		MieAyam_RenderEnd();
	}
}
```

Contoh program bisa dilihat pada folder examples, [di sini](examples).
