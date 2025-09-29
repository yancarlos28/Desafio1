#include <iostream>
#include <fstream>
using namespace std;

//--------------------- FUNCIONES -------------------------//
unsigned short unirDosBytes(unsigned char hi, unsigned char lo);
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
//construcción de pi[]
unsigned int* construir_tabla_fallo(const char* pat, size_t m) {
    if (m == 0) return nullptr;

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
// Rotación a la derecha n bits de un byte
unsigned char rotR(unsigned char b, int n) {

    unsigned int x = b;
    unsigned int derecha = x >> n;
    unsigned int izquierda = (x << (8 - n)) & 0xFF;
    unsigned int combinado = derecha | izquierda;

    // Devolver byte limpio
    return static_cast<unsigned char>(combinado & 0xFF);
}
//Filtro para descartar combinaciones inválidas del método RLE
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
//--------------funciones para implementar el metodo de compresion LZ78-------------------//
unsigned short unirDosBytes(unsigned char hi,unsigned char lo){
    return ((unsigned char)hi<<8)|(unsigned char)lo;
}
unsigned char** crearDicc(size_t tam){
    //reservaion de memoria para los indices del diccionario.
    /* Eltamaño de la reserva es tam/3, porque cada 3 caracteres del mensaje desencriptado
       se agregara un nuevo caracter en el diccionario y el +1, porque se debe inicializar el primer indice (puntero)
       del diccionario con un caracter vacio ''.
    */
    if (tam == 0) {
        cout << "Error: tamaño inválido para crear el diccionario" << endl;
        return nullptr;
    }
    // tam/3 es la cantidad de tripletas, +1 para el índice 0 (prefijo vacío)
    size_t capacidad = (tam / 3) + 1;
    unsigned char **ptr = new unsigned char*[capacidad];

    for (size_t i = 0; i < capacidad; i++) {
        ptr[i] = new unsigned char[5];
        for (int j = 0; j < 5; j++) {
            ptr[i][j] = '\0';
        }
    }

    return ptr;
}
// Reiniciar diccionario sin liberar memoria
void reiniciarDicc(unsigned char** diccionario, size_t tam) {
    if (!diccionario) return;
    size_t capacidad = (tam / 3) + 1;

    for (size_t i = 0; i < capacidad; i++) {
        for (int j = 0; j < 5; j++) {
            diccionario[i][j] = '\0';
        }
    }
}
void inserEnDicc(unsigned char ** diccionario, unsigned short pr,unsigned char sym, size_t indDicc){
    diccionario[indDicc][0]=(pr >> 8) & 0xFF;
    diccionario[indDicc][1]=pr & 0xFF;
    diccionario[indDicc][2]=sym;
    unsigned short len=0;
    if(pr!=0){
        len=unirDosBytes(diccionario[pr][3],diccionario[pr][4]);
    }
    len++;
    diccionario[indDicc][3]=(len >> 8) & 0xFF;
    diccionario[indDicc][4]=len & 0xFF;
}

void libMem(unsigned char **ptr, size_t filas){
    //para matrices bidimensionales
    for (size_t i=0;i<filas;i++){
        delete[] ptr[i];
        ptr[i]=NULL;
    }
    delete[] ptr;
    ptr=NULL;
}
bool kmp_step(unsigned char c,
              const unsigned char* pat,
              short int m,
              unsigned int& estado,
              const unsigned int* lps) {
    while (estado > 0 && c != pat[estado]) {
        estado = lps[estado - 1];
    }
    if (c == pat[estado]) {
        estado++;
        if (estado == m) {
            estado = lps[estado - 1]; // reiniciar para buscar más ocurrencias
            return true;              // ¡pista encontrada!
        }
    }
    return false;
}

bool emitirPrefijo(unsigned short pr,
                   unsigned char sym,
                   unsigned char** diccionario,
                   const unsigned char* datos_pista,
                   short int tamPista,
                   unsigned int &estado,
                   const unsigned int* pi,
                   size_t indDicc)
{
    // Caso base: prefijo vacío
    if (pr == 0) {
        return kmp_step(sym, datos_pista, tamPista, estado, pi);
    }
    // Validar índice de prefijo
    if (pr >= indDicc) {
        std::cerr << "Error: prefijo fuera de rango (" << pr << ")" << std::endl;
        return false;
    }
    // Recursivamente emitir el prefijo del padre
    unsigned short padre = unirDosBytes(diccionario[pr][0], diccionario[pr][1]);

    // Emitir prefijo del padre
    if (padre != 0) {
        if (emitirPrefijo(padre,
                          diccionario[pr][2],  // símbolo del padre
                          diccionario,
                          datos_pista,
                          tamPista,
                          estado,
                          pi,
                          indDicc)) {
            return true; // pista encontrada en el prefijo
        }
    } else {
        // Si padre == 0, emitir directamente el símbolo del padre
        if (kmp_step(diccionario[pr][2], datos_pista, tamPista, estado, pi)) {
            return true;
        }
    }

    // Finalmente emitir el símbolo actual
    return kmp_step(sym, datos_pista, tamPista, estado, pi);
}

bool LZ78_con_pista(const unsigned char *archivo,
                    const size_t tamEnc,
                    const unsigned char *datos_pista,
                    const short int tamPista,
                    int k,int n,
                    unsigned char** diccionario,
                    unsigned int* pi){

    //progreso en lacoincidencia de pista
    unsigned int estado=0;

    size_t indDicc=1;

    for(size_t i=0;i+2<tamEnc;i+=3){
        // Desencriptar trío
        unsigned short pr = unirDosBytes(rotR(archivo[i] ^ (unsigned char)k, n),rotR(archivo[i+1] ^ (unsigned char)k, n));
        unsigned char sym = rotR(archivo[i+2] ^ (unsigned char)k, n);

        inserEnDicc(diccionario,pr,sym,indDicc);

        // --- Procesar símbolo con KMP ---
        if (emitirPrefijo(pr, sym, diccionario, datos_pista, tamPista, estado, pi,indDicc)) {
            return true; // pista encontrada
        }
        indDicc++;
    }
    return false;
}
bool descomprimir_LZ78(const unsigned char* enc, size_t nin,
                       int K, int nbits, const char* outpath)
{
    if(!enc || nin < 3 || !outpath) return false;
    if(nin % 3 != 0) return false; // deben ser tríos exactos

    size_t max_entries = nin / 3;               // 1 entrada por trío como cota
    int* prefix = new int[max_entries + 1];         // base 1
    unsigned char* lastch = new unsigned char[max_entries + 1];
    char* tmp = new char[max_entries];                         // reconstrucción frase

    std::ofstream out(outpath, std::ios::binary);
    if(!out){ delete[] prefix; delete[] lastch; delete[] tmp; return false; }

    size_t dict_size = 0;

    for(size_t pos = 0; pos + 3 <= nin; pos += 3){
        // Descifrar: XOR K → rotR(n)
        unsigned char hi = rotR((unsigned char)(enc[pos]     ^ (unsigned char)K), nbits);
        unsigned char lo = rotR((unsigned char)(enc[pos + 1] ^ (unsigned char)K), nbits);
        unsigned char ch = rotR((unsigned char)(enc[pos + 2] ^ (unsigned char)K), nbits);

        // Índice 16-bit big-endian
        int idx = ((int)hi << 8) | (int)lo;

        // Validación LZ78: 0 <= idx <= dict_size
        if(idx < 0 || (size_t)idx > dict_size){
            out.close();
            delete[] prefix; delete[] lastch; delete[] tmp;
            return false;
        }

        // Insertar nueva entrada
        dict_size++;
        if(dict_size > max_entries){
            out.close();
            delete[] prefix; delete[] lastch; delete[] tmp;
            return false;
        }
        prefix[dict_size] = idx;
        lastch[dict_size] = ch;

        // Reconstruir S = dict[idx] + ch en 'tmp' (en orden inverso)
        size_t tp = 0;
        int w = (int)dict_size;
        while(w != 0){
            if(tp >= max_entries){               // salvaguarda
                out.close();
                delete[] prefix; delete[] lastch; delete[] tmp;
                return false;
            }
            tmp[tp++] = (char)lastch[w];
            w = prefix[w];
        }

        // Emitir S en orden correcto
        for(size_t i = 0; i < tp; ++i){
            out.put(tmp[tp - 1 - i]);
            if(!out){
                out.close();
                delete[] prefix; delete[] lastch; delete[] tmp;
                return false;
            }
        }
    }

    out.close();
    delete[] prefix; delete[] lastch; delete[] tmp;
    return true;
}
//FUNCIÓN PRINCIPAL
int main() {

    int p;
    cout << "Cantidad de archivos a leer: ";
    cin >> p;

    for (int i = 1; i <= p; i++) {

        char arreglo_archivo[20];
        char arreglo_pista[20];
        char arreglo_salida[20];

        // Construir las rutas dinámicamente
        sprintf(arreglo_archivo, "Encriptado%d.txt", i);
        sprintf(arreglo_pista,   "pista%d.txt", i);
        sprintf(arreglo_salida,  "Salida%d.txt", i);

        // Rutas:
        const char* ruta_archivo   = arreglo_archivo;
        const char* ruta_pista = arreglo_pista;
        const char* ruta_salida   = arreglo_salida;

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
        //Diccionario para el LZ78
        unsigned char **diccionario=crearDicc(tam_archivo);

        //creacion de tabla de fallo(pi)[] para la aplicación del KMP
        unsigned int* pi = construir_tabla_fallo((char*)datos_pista, tam_pista);

        // Variables para guardar parámetros correctos
        bool encontrado = false;
        int valor_k=0, valor_n=0;
        cout<<"-------Encriptado procesado: "<< i<<"-------"<< endl;

        //ciclos para iterar sobre las posibles formas del k y n
        for (int k = 0; k < 256 && !encontrado; k++) {
            for (int n = 1; n < 8 && !encontrado; n++) {
                //filtro rápido para RLE
                if (!encontrado && filtro_RLE(datos_archivo, tam_archivo, k, n)){
                    //filtro para encontrar pista con el texto
                    if (RLE_con_pista(datos_archivo, tam_archivo,(char*)datos_pista, tam_pista, k, n, pi)) {
                        encontrado = true; valor_k = k; valor_n = n;
                        if (encontrado) {
                            cout << "Metodo: RLE" <<endl<<"K=0x"<<hex<< valor_k << ".  n=" <<dec<<valor_n << "\n";
                        }
                        if(descomprimir_RLE(datos_archivo, tam_archivo, valor_k, valor_n, ruta_salida)){
                            cout<<"Archivo creado con exito."<<endl;
                        }
                        else {
                            cout<<"No se pudo escribir correctamente el archivo.";
                        }
                    }
                }
                //Filtro rápido para LZ78
                if (!encontrado && filtro_LZ78(datos_archivo, tam_archivo, k, n)) {
                    //filtro para encontrar pista con el texto
                    if (LZ78_con_pista(datos_archivo, tam_archivo,datos_pista, tam_pista, k, n,diccionario, pi)) {
                        encontrado = true; valor_k = k; valor_n = n;
                        if (encontrado) {
                            cout << "Metodo: LZ78" <<endl<<"K=0x"<<hex<< valor_k << ".  n=" <<dec<<valor_n << "\n";
                            if(descomprimir_LZ78(datos_archivo, tam_archivo, k, n, ruta_salida)){
                                cout<<"Archivo creado con exito."<<endl;
                            }
                            else {
                                cout<<"No se pudo escribir correctamente el archivo.";
                            }
                        }
                    }
                    reiniciarDicc(diccionario, tam_archivo);
                }
            }
        }
        if(!encontrado) {
            cout << "No se encontró la pista en el encriptado #"<< i<< endl;

        }
        // Liberar memoria
        delete[] pi;
        delete[] datos_archivo;
        delete[] datos_pista;
        libMem(diccionario,(tam_archivo/3)+1);
    }
    return 0;
}
