#include <iostream>
#include <fstream>
#include <cstdio>
using namespace std;

//--------------------- FUNCIONES -------------------------//

// Rotación a la derecha n bits de un byte
unsigned char rotR(unsigned char b, int n) {
    // Paso 1: asegurar que n esté entre 0 y 7 (solo hay 8 bits en un byte)
    n = n & 7;

    // Paso 2: convertir b a unsigned int (32 bits normalmente)
    // Esto evita problemas al hacer corrimientos.
    unsigned int x = b;

    // Paso 3: desplazar a la derecha n posiciones
    unsigned int parteDerecha = x >> n;

    // Paso 4: desplazar a la izquierda (8 - n) posiciones
    // Esto mueve los bits que "se caen" en el corrimiento a la derecha
    // para que vuelvan por el lado izquierdo (rotación circular).
    unsigned int parteIzquierda = x << (8 - n);

    // Paso 5: combinar las dos partes con OR
    unsigned int combinado = parteDerecha | parteIzquierda;

    // Paso 6: quedarnos solo con los 8 bits bajos
    combinado = combinado & 0xFF;

    // Paso 7: devolver como unsigned char
    return static_cast<unsigned char>(combinado);
}
// --- RLE 16-bit BE: [hi][lo][sym] ---
char* rle16_be_descomprimir(const unsigned char* in, size_t nin, size_t &nout){
    nout = 0;
    if (nin < 3) return nullptr;
    for (size_t i = 0; i + 3 <= nin; i += 3)
        nout += ((size_t)in[i] << 8) | (size_t)in[i+1];
    if (nout == 0) return nullptr;

    char* out = new char[nout];
    size_t j = 0;
    for (size_t i = 0; i + 3 <= nin; i += 3) {
        size_t cnt = ((size_t)in[i] << 8) | (size_t)in[i+1];
        unsigned char sym = in[i+2];
        for (size_t k = 0; k < cnt; ++k) out[j++] = (char)sym;
    }
    return out;
}


// Leer archivo binario mínimo
unsigned char* leer_archivo(const char* nombre, size_t &tam) {
    ifstream f(nombre, ios::binary | ios::ate);
    if (!f) { tam = 0; return nullptr; }
    tam = (size_t)f.tellg();
    f.seekg(0, ios::beg);
    unsigned char* datos = new unsigned char[tam];
    f.read(reinterpret_cast<char*>(datos), (std::streamsize)tam);
    return datos;
}

// Búsqueda exacta naive
bool contienePista(const char* texto, size_t lenTexto,
                   const char* pista, size_t lenPista) {
    if (lenPista == 0 || lenPista > lenTexto) return false;
    for (size_t i = 0; i + lenPista <= lenTexto; ++i) {
        size_t j = 0;
        while (j < lenPista && texto[i + j] == pista[j]) ++j;
        if (j == lenPista) return true;
    }
    return false;
}

//------------------------- MAIN -------------------------//
int main() {
    // Rutas: ajusta si es necesario
    const char* rutaEnc   = "C:\\Users\\Jean\\Downloads\\datasetDesarrollo\\datasetDesarrollo\\Encriptado1.txt";
    const char* rutaPista = "C:\\Users\\Jean\\Downloads\\datasetDesarrollo\\datasetDesarrollo\\Pista1.txt";

    size_t tamEnc = 0, tamPista = 0;
    unsigned char* datos = leer_archivo(rutaEnc, tamEnc);
    unsigned char* datos_pista = leer_archivo(rutaPista, tamPista);

    if (!datos || !datos_pista || tamEnc == 0 || tamPista == 0) {
        cout << "No se pudo leer archivos.\n";
        delete[] datos; delete[] datos_pista;
        return 1;
    }

    // Buffer de trabajo
    unsigned char* work = new unsigned char[tamEnc];

    bool encontrado = false;
    int bestK = -1, bestN = -1;

    for (int k = 0; k < 256 && !encontrado; ++k) {
        for (int n = 1; n < 8 && !encontrado; ++n) {
            // Desencriptar: XOR -> rotación derecha n
            for (size_t i = 0; i < tamEnc; ++i) {
                unsigned char x = datos[i] ^ (unsigned char)k;
                work[i] = rotR(x, n);
            }

            size_t outLen = 0;
            // char* mensaje = descomprimirRLE_ASCII(work, tamEnc, outLen);
            char* mensaje = rle16_be_descomprimir(work, tamEnc, outLen);

            if (mensaje && outLen > 0 &&
                contienePista(mensaje, outLen, (const char*)datos_pista, tamPista)) {
                encontrado = true;
                bestK = k; bestN = n;

                cout << "[ENCONTRADO] RLE con K=0x" << hex << bestK
                     << " n=" << dec << bestN << "\n";

                ofstream fout("resultado_encontrado.txt", ios::binary);
                fout.write(mensaje, (std::streamsize)outLen);
                delete[] mensaje;
                break; // salir del for n
            }
            delete[] mensaje; // siempre liberar
        }
    }

    if (!encontrado) {
        cout << "No se encontró la pista.\n";
    }

    // Liberar
    delete[] work;
    delete[] datos;
    delete[] datos_pista;
    return 0;
}
