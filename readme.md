# Proyecto: Simulación de Sensores
Este proyecto simula un sistema de medición con sensores de temperatura y humedad. Los datos se cargan desde un archivo CSV y el programa permite:

Visualizar gráficas por hora y gráficas con valores ordenados.
Analizar valores mínimos y máximos registrados.
Realizar búsquedas de valores específicos de temperatura en función de la hora.

El objetivo es demostrar el uso de estructuras de datos en C++ junto con algoritmos eficientes de ordenamiento y búsqueda, fundamentales en la gestión de información de sensores en tiempo real.

## Descripción del avance 1
En este avance se implementó:

Lectura de datos desde un archivo CSV con registros de sensores.
Representación gráfica de los datos con la librería matplotlibcpp.
Ordenamiento de datos (temperatura y humedad) para análisis de mínimos y máximos.
Búsqueda eficiente de temperaturas en un horario específico usando std::binary_search.

Este avance muestra cómo la correcta selección de algoritmos mejora el desempeño en programas que requieren procesar datos de sensores de forma rápida y confiable.

## Instrucciones para compilar el avance de proyecto
Ejecuta el siguiente comando en la terminal:

`g++ main.cpp -std=c++11 -o primer_avance` 

## Instrucciones para ejecutar el avance de proyecto
Ejecuta el siguiente comando en la terminal:

`./primer_avance` 

## Descripción de las entradas del avance de proyecto
Archivo de entrada: datos.csv que contiene los registros de cada sensor con el siguiente formato:

timestamp,temperatura,humedad
2024-09-01 08:00:00,25.3,60.1
2024-09-01 09:00:00,26.7,58.4
2024-09-01 10:00:00,27.5,57.9
...

timestamp: fecha y hora (AAAA-MM-DD HH:MM:SS).
temperatura: valor en °C.
humedad: valor en %.

## Descripción de las salidas del avance de proyecto
Escribe aquí la descripción de los resultados de la ejecución de tu programa.

## Desarrollo de competencias

### SICT0301: Evalúa los componentes
#### Hace un análisis de complejidad correcto y completo para los algoritmos de ordenamiento usados en el programa.
Escribe aquí tus argumentos sobre por qué consideras que has desarrrollado esta competencia y dónde se puede observar el desarrollo que mencionas.

### SICT0302: Toma decisiones
#### Selecciona un algoritmo de ordenamiento adecuado al problema y lo usa correctamente.
Escribe aquí tus argumentos sobre por qué consideras que has desarrrollado esta competencia y dónde se puede observar el desarrollo que mencionas.