#include <iostream>
#include <fstream>
#include <cstdio>
using namespace std;

// Rotación circular a la derecha n bits
unsigned char rotR(unsigned char b, int n) {
    return (b >> n) | (b << (8 - n));
}

int main()
{
    ifstream archivo("C:\\Users\\Jean\\OneDrive\\Documentos\\DesafioI\\Encriptado3.txt", ios::binary);
    if (!archivo) {
        cout << "No se pudo abrir el archivo.\n";
        return 1;
    }

    // Calcular tamaño
    archivo.seekg(0, ios::end);
    streampos tam = archivo.tellg();
    archivo.seekg(0, ios::beg);

    // Reservar memoria dinámica
    unsigned char* datos = new unsigned char[(size_t)tam];

    // Leer el archivo en memoria
    archivo.read(reinterpret_cast<char*>(datos), tam);
    archivo.close();

    // Buffer de trabajo (para no modificar datos originales)
    unsigned char* arreglo_de_trabajo = new unsigned char[(size_t)tam];

    // Probar todas las combinaciones de K y n
    for (int k = 0; k < 256; k++) {
        for (int n = 1; n < 8; n++) {
            for (int i = 0; i < tam; i++) {
                unsigned char x = datos[i] ^ (unsigned char)k; // XOR primero
                arreglo_de_trabajo[i] = rotR(x, n);            // luego rotación derecha
            }

        }
    }

    delete[] datos;
    delete[] arreglo_de_trabajo;
    return 0;
}


