#include <iostream>
#include <fstream>
#include <cstdio>
#include <stdint.h>
using namespace std;

//--------------------- FUNCIONES -------------------------//

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

// Rotación a la derecha n bits de un byte
unsigned char rotR(unsigned char b, int n) {
    n &= 7;
    return static_cast<unsigned char>(((b >> n) | (b << (8 - n))) & 0xFF);
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

<<<<<<< HEAD
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
=======
//construcción de pi[]
unsigned int* construir_tabla_fallo(const char* pat, size_t m) {
    if (m == 0) return nullptr;
>>>>>>> ea38a19db01e3630b240d40fc2c32a688d6c9f5f

    unsigned int* pi = new unsigned int[m];
    pi[0] = 0;  // el primer prefijo siempre es 0

    unsigned int k = 0; // longitud del prefijo actual
    for (size_t q = 1; q < m; q++) {
        // mientras haya desajuste, retrocedemos con pi
        while (k > 0 && pat[k] != pat[q]) {
            k = pi[k - 1];
        }
        // si coincide, extendemos el prefijo
        if (pat[k] == pat[q]) {
            k++;
        }
        pi[q] = k;
    }

    return pi;
}

//Filtro para descartas combinaciones inválidas del método RLE
bool filtro_RLE(const unsigned char* datos, size_t tam, int K, int n)
{
    if (tam < 9) return false; // necesitamos mínimo 3 tripletas = 9 bytes

    int validas = 0;

    for (int t = 0; t < 3; ++t) {
        size_t pos = t * 3;

        // desencriptar y rotar
        unsigned char hi  = rotR(datos[pos]   ^ (unsigned char)K, n);
        unsigned char lo  = rotR(datos[pos+1] ^ (unsigned char)K, n);
        unsigned char sym = rotR(datos[pos+2] ^ (unsigned char)K, n);

        unsigned int cnt = ((unsigned int)hi << 8) | (unsigned int)lo;

        // validación: cnt en rango razonable
        if (cnt >= 1 && cnt <= 65000) {
            // validación del símbolo: que sea una letra
            if ((sym >= 65 && sym <= 90) || (sym >= 97 && sym <= 122)) {
                validas++;
            }
<<<<<<< HEAD

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
=======
        }
    }

    // si al menos 2 de las 3 tripletas son válidas es muy probable el RLE
    return (validas >= 2);
}

// metodo RLE y busqueda de pista
bool RLE_con_pista(const unsigned char* archivo, size_t tam_arc,
                   const char* pista, size_t tam_pis,
                   int k, int n,
                   const unsigned int* pi)
{
    if (tam_arc < 3 || tam_pis == 0) {
        return false;
    }

    unsigned int estadoKMP = 0;

    for (size_t i=0; i+3 <= tam_arc; i+=3)
    {
        // Desencriptar y rotar bytes
        unsigned char hi  = rotR(archivo[i]   ^ (unsigned char)k, n);
        unsigned char lo  = rotR(archivo[i+1] ^ (unsigned char)k, n);
        unsigned char sym = rotR(archivo[i+2] ^ (unsigned char)k, n);

        unsigned int cnt = ((unsigned int)hi << 8) | (unsigned int)lo;
        if (cnt == 0 || cnt > 65000)
        {
            return false; // inválido
        }

        // Verificar si la pista es homogénea (ej. "AAAA")
        bool pista_homogenea = true;
        for (size_t j = 0; j < tam_pis; j++)
        {
            if (pista[j] != (char)sym)
            {
                pista_homogenea = false;
                break;
            }
        }

        if (pista_homogenea)
        {
            // Si todo el patrón es el mismo caracter
            if (cnt >= tam_pis)
            {
                return true; // la pista aparece dentro de este run
            }
        } else {
            // Caso general: expandir como máximo #tamaño de la pista de caracteres
            unsigned int limite = (cnt > tam_pis) ? tam_pis : cnt;
            for (unsigned int k = 0; k < limite; k++) {
                char outch = (char)sym;

                // Avanzar KMP
                while (estadoKMP > 0 && pista[estadoKMP] != outch) {
                    estadoKMP = pi[estadoKMP - 1];
                }
                if (pista[estadoKMP] == outch) {
                    estadoKMP++;
                }
                if (estadoKMP == tam_pis) {
                    return true; // pista encontrada
                }
            }
        }
    }

    return false;
}

//filtro LZ78
bool filtro_LZ78(const unsigned char* datos, size_t tam, int K, int n)
{
    if (tam < 9) return false; // al menos 3 tripletas

    size_t dict_size = 0;

    for (int t = 0; t < 3; ++t) {
        size_t pos = t * 3;

        unsigned char hi = rotR(datos[pos]   ^ (unsigned char)K, n);
        unsigned char lo = rotR(datos[pos+1] ^ (unsigned char)K, n);
        unsigned char ch = rotR(datos[pos+2] ^ (unsigned char)K, n);

        unsigned int idx = ((unsigned int)hi << 8) | (unsigned int)lo;

        // Regla 1: índice válido
        if (idx > dict_size) return false;

        // Regla 2: carácter razonable (A–Z, a–z, tab, LF, CR)
        bool printable = (ch >= 'A' && ch <= 'Z') ||
                         (ch >= 'a' && ch <= 'z') ||
                         (ch == '\t' || ch == '\n' || ch == '\r');
        if (!printable) return false;

        dict_size++; // nueva entrada
    }

    return true;
}


// RLE: tríos [hi][lo][sym] con contador 16-bit big-endian
bool descomprimir_RLE(const unsigned char* archivo, size_t tam_arc,
                      int k, int n, const char* salida)
{
    if (!archivo || !salida || tam_arc < 3) return false;

    ofstream out(salida);  // modo texto (escribe como caracteres)
    if (!out) return false;

    for (size_t i = 0; i + 2 < tam_arc; i += 3) {
        // Desencriptar trío
        unsigned char hi  = rotR(archivo[i]   ^ (unsigned char)k, n);
        unsigned char lo  = rotR(archivo[i+1] ^ (unsigned char)k, n);
        unsigned char sym = rotR(archivo[i+2] ^ (unsigned char)k, n);

        // Contador 16-bit (big-endian)
        unsigned int cnt = ((unsigned int)hi << 8) | lo;
        if (cnt == 0 || cnt > 65000) { out.close(); return false; }

        // Escribir directamente como caracteres
        for (unsigned int j = 0; j < cnt; j++) {
            out.put((char)sym);
            if (!out) { out.close(); return false; }
        }
    }

    out.close();
    return true;
}


//FUNCIÓN PRINCIPAL
int main() {

    // Rutas: ajusta si es necesario
    const char* ruta_archivo   = "C:\\Users\\Jean\\Downloads\\datasetDesarrollo\\datasetDesarrollo\\Encriptado1.txt";
    const char* ruta_pista = "C:\\Users\\Jean\\Downloads\\datasetDesarrollo\\datasetDesarrollo\\Pista1.txt";
    const char* ruta_salida   = "C:\\Users\\Jean\\Downloads\\datasetDesarrollo\\datasetDesarrollo\\Salida1.txt";

    size_t tam_archivo = 0, tam_pista = 0;

    //leemos los archivos
    unsigned char* datos_archivo = leer_archivo(ruta_archivo, tam_archivo);
    unsigned char* datos_pista = leer_archivo(ruta_pista, tam_pista);

    //pequeñas validaciones
    if (!datos_archivo || !datos_pista || tam_archivo == 0 || tam_pista == 0) {
        cout << "No se pudo leer archivos.\n";
        delete[] datos_archivo; delete[] datos_pista;
        return 1;
    }

    //creacion de pi[] para la aplicación del KMP
    unsigned int* pi = construir_tabla_fallo((char*)datos_pista, tam_pista);
    //imprimir la tabla del fallo(pi)
    //for (size_t i=0; i<tam_pista; i++){
    // cout<< pi[i]<<";";
    //}

    // Variables para guardar parámetros correctos
    bool encontrado = false;
    int valor_k, valor_n;

    //ciclos para iterar sobre las posibles formas del k y n
    for (int k = 0; k < 256 && !encontrado; k++) {
        for (int n = 1; n < 8 && !encontrado; n++) {
            //Descartar posibles combinaciones
            unsigned char primer_byte = rotR(datos_archivo[0] ^ (unsigned char)k, n);
            unsigned char segundo_byte = rotR(datos_archivo[1] ^ (unsigned char)k, n);
            unsigned int combinacion_bytes = ((unsigned int)primer_byte << 8) | (unsigned int)segundo_byte;

            //Primer filtro rápido para método RLE
            bool posible_RLE = (combinacion_bytes != 0);
            if (posible_RLE && filtro_RLE(datos_archivo, tam_archivo, k, n)){
                if (RLE_con_pista(datos_archivo, tam_archivo,(char*)datos_pista, tam_pista, k, n, pi)) {
                    encontrado = true; valor_k = k; valor_n = n;
                    if (encontrado) {
                        cout << "Metodo: RLE"
                             << "  K=0x" << hex << valor_k   // hex para ver K en hexadecimal
                             << "  n="   << dec << valor_n   // dec para imprimir n en decimal
                             << "\n";
                    }
                    if(descomprimir_RLE(datos_archivo, tam_archivo, valor_k, valor_n, ruta_salida)){
                        cout<<"Archivo creado con exito."<<endl;
                    }
                    else {
                        cout<<"No se pudo escribir correctamente el archivo.";
                    }

                }
            }
            //Primer filtro rápido para LZ78
            //bool posible_LZ  = (combinacion_bytes == 0);

            //if (posible_LZ){
                //if (filtro_LZ78(datos_archivo, tam_archivo, k, n)) {

                    //}
                //}
            //}

        }
    }

    //No se encontró la pista en el texto.
    if (!encontrado) {
        cout << "No se encontró la pista";
    }
    // Liberar memoria
    delete []pi;
    delete[] datos_archivo;
>>>>>>> ea38a19db01e3630b240d40fc2c32a688d6c9f5f
    delete[] datos_pista;
    return 0;
}
