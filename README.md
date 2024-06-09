# UDP Sliding Window File Transfer

Este repositorio contiene una implementación en C de un mecanismo de control de flujo "Ventana deslizante" con la técnica de control de error "Retroceder-N" (Go-Back-N) mediante sockets de datagrama (UDP) para el envío de archivos.

## Descripción

El proyecto consiste en dos programas:
1. `sender`: Programa que envía un archivo.
2. `receiver`: Programa que recibe un archivo y lo guarda con el mismo nombre que el archivo enviado.

El `sender` envía el archivo especificado por el usuario en paquetes utilizando una ventana deslizante. El `receiver` recibe los paquetes, envía ACKs para confirmar la recepción de cada paquete y guarda los datos en un archivo con el mismo nombre que el archivo original.

## Estructura del Proyecto

- `sender.c`: Código fuente del programa emisor.
- `receiver.c`: Código fuente del programa receptor.

## Compilación

Para compilar ambos programas, utiliza los siguientes comandos:

```sh
gcc -o sender sender.c
gcc -o receiver receiver.c
```

## Ejecución

### Iniciar el Receptor

Primero, ejecuta el programa receptor en una terminal:

```sh
./receiver
```

El receptor se quedará esperando a recibir el nombre del archivo y los datos.

### Iniciar el Emisor

En otra terminal, ejecuta el programa emisor pasando el nombre del archivo que deseas enviar como argumento:

```sh
./sender <nombre_del_archivo>
```

Por ejemplo, si deseas enviar un archivo llamado `mensaje.txt`:

```sh
./sender mensaje.txt
```

El emisor enviará el nombre del archivo como el primer paquete, seguido por los datos del archivo en paquetes de tamaño fijo.

## Ejemplo Completo

Supongamos que tienes un archivo llamado `mensaje.txt` que contiene el texto "Hola, este es un mensaje de prueba".

1. Compilar los programas:

    ```sh
    gcc -o sender sender.c
    gcc -o receiver receiver.c
    ```

2. Iniciar el Receptor:

    ```sh
    ./receiver
    ```

    La salida será algo como:

    ```
    Esperando a recibir el nombre del archivo...
    ```

3. Iniciar el Emisor:

    ```sh
    ./sender mensaje.txt
    ```

    La salida será algo como:

    ```
    Nombre del archivo enviado: mensaje.txt
    Paquete enviado con número de secuencia: 1, tamaño de datos: 34
    ...
    ```

4. Verificar el archivo recibido:

    ```sh
    cat mensaje.txt
    ```

    Deberías ver:

    ```
    Hola, este es un mensaje de prueba
    ```

## Contribuciones

Las contribuciones son bienvenidas. Si tienes alguna mejora o corrección, por favor abre un issue o un pull request.
