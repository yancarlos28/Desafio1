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
//--------------funciones para implementar el metodo de compresion LZ78-------------------//
unsigned char** crearDicc(size_t tam){
    //reservaion de memoria para los indices del diccionario.
    /* Eltamaño de la reserva es tam/3, porque cada 3 caracteres del mensaje desencriptado
       se agregara un nuevo caracter en el diccionario y el +1, porque se debe inicializar el primer indice (puntero)
       del diccionario con un caracter vacio ''.
    */
    unsigned char **ptr = new unsigned char *[(tam/3)+1];
    return ptr;
}
void iniDicc(unsigned char **ptr, size_t tam ){
    //se reserva memoria para los arreglos que almacenarán el diccionario
    for (int i=0;i<(tam/3)+1;i++){
        ptr[i]=nullptr;
    }
    ptr[0]=new unsigned char [1];
    ptr[0][0]='\0';
}
unsigned char *buscarCadena(unsigned char **ptr,short int a){
    return ptr[a];
}
unsigned char* reconstruirCadena(unsigned char* ptr, unsigned char c) {
    //La longitud se calcula manualmente
    int len =longitud(ptr);
    // Reservar memoria para la nueva cadena (+1 para c, +1 para '\0')
    unsigned char* s = new unsigned char[len + 2];

    for (int i = 0; i < len; ++i) {
        s[i] = ptr[i];
    }
    s[len] = c;
    s[len + 1] = '\0';

    return s;
}
void cadDescomprimida(unsigned char *ptr1 , unsigned char *ptr2,int &nRef){
    //formato de parametros para la funcion cadDescomprimida (cadRecons, arrDesCom, nRef)

    for (int i = 0; ptr1[i]!='\0';i++){
        ptr2[nRef]=ptr1[i];
        nRef++;
    }

}
void inserEnDicc(unsigned char ** ptr1, unsigned char *ptr2,short int n){
    ///formato de parámetros para la funcon inserEnDicc(diccionario,cadRecons,indDicc)
    int len = longitud(ptr2);
    ptr1[n] = new unsigned char[len + 1];
    for (int i = 0; i <= len; i++) {
        ptr1[n][i] = ptr2[i];
    }
}
unsigned char* descompresor (unsigned char * arreglo,size_t tam,size_t &lenMen){
    //formato de parametros para la funcion descompresor(work,tam)
    short int indDicc=1;
    unsigned char **diccionario;
    unsigned char *arrDesCom;

    diccionario=crearDicc(tam);
    iniDicc(diccionario,tam);
    arrDesCom = new unsigned char [tam*10];

    for (size_t i=0; i<tam;i+=3){
        size_t prefijo = ((size_t)arreglo[i]<<8)|(size_t)arreglo[i+1];
        unsigned char cArreglo = arreglo[i+2];
        unsigned char *arrEnDicc=buscarCadena(diccionario,prefijo);
        unsigned char *cadRecons=reconstruirCadena(arrEnDicc,cArreglo);
        cadDescomprimida(cadRecons,arrDesCom,lenMen);
        inserEnDicc(diccionario,cadRecons,indDicc);
        indDicc+=1;
        delete[] cadRecons;
    }
    arrDesCom[lenMen] = '\0';
    libMem(diccionario,(tam/3)+1);
    return arrDesCom;
}
void libMem(unsigned char **ptr, size_t filas){
    //para matrices bidimensionales
    for (int i=0;i<filas;i++){
        delete[] ptr[i];
        ptr[i]=NULL;
    }
    delete[] ptr;
    ptr=NULL;
}
int longitud(unsigned char* cadena) {
    //esta funcion calcula manualmente la longitud de un arreglo
    int len = 0;
    while (cadena[len] != '\0') {
        len++;
    }
    return len;
}
//------------------------- MAIN -------------------------//
int main() {
    // Rutas: ajusta si es necesario
    const char* rutaEnc   = "C:\\2025_2\\informatica_II\\datasetDesarrollo\\datasetDesarrollo\\Encriptado2.txt";
    const char* rutaPista = "C:\\2025_2\\informatica_II\\datasetDesarrollo\\datasetDesarrollo\\pista2.txt";

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
            else if(){
                delete[] mensaje;
                mensaje=nullptr;
                size_t tam = sizeof(work);
                size_t lenMen=0;
                unsigned char *mensaje=descompresor(work,tam,&lenMen);
                if(contienePista(mensaje,lenMen,(const char*)datos_pista, tamPista)==true){
                    encontrado = true;
                    bestK = k; bestN = n;

                    cout << "[ENCONTRADO] RLE con K=0x" << hex << bestK
                         << " n=" << dec << bestN << "\n";

                    ofstream fout("resultado_encontrado.txt", ios::binary);
                    fout.write(mensaje, (std::streamsize)outLen);
                    delete[] mensaje;
                    mensaje= NULL;
                    break;
                }
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
