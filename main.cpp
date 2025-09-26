#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <memory>
#include "matplotlibcpp.h"
#include "Sensores.h"

namespace plt = matplotlibcpp;

/**
 * FUNCIÓN: encontrarMinMax
 * PROPÓSITO: Encuentra los valores mínimos y máximos en un conjunto de datos
 * COMPLEJIDAD: O(n) donde n = tamaño del vector datos
 * 
 * std::min_element y std::max_element tienen complejidad O(n) cada uno
 */
void encontrarMinMax(const std::vector<double>& datos, 
                    const std::vector<std::string>& horas,
                    double& minVal, double& maxVal,
                    std::string& minHora, std::string& maxHora) {
    if (datos.empty()) return;
    
    auto minIt = std::min_element(datos.begin(), datos.end());  // O(n)
    auto maxIt = std::max_element(datos.begin(), datos.end());  // O(n)
    
    size_t minIndex = std::distance(datos.begin(), minIt);  // O(1)
    size_t maxIndex = std::distance(datos.begin(), maxIt);  // O(1)
    
    minVal = *minIt;
    maxVal = *maxIt;
    minHora = horas[minIndex];  // O(1)
    maxHora = horas[maxIndex];  // O(1)
}

/**
 * FUNCIÓN: graficarPorHora
 * PROPÓSITO: Genera gráficas de temperatura y humedad en orden temporal
 * COMPLEJIDAD: O(n) donde n = número de lecturas de sensores
 * 
 * Todas las operaciones son lineales: procesamiento de datos y plotting
 */
void graficarPorHora(SistemaSensores& sistema) {
    // O(1) - Acceso directo a sensores por ID
    SensorTemperatura* sensorTemp = dynamic_cast<SensorTemperatura*>(sistema.buscarSensor("TEMP_001"));
    SensorHumedad* sensorHum = dynamic_cast<SensorHumedad*>(sistema.buscarSensor("HUM_001"));
    
    if (!sensorTemp || !sensorHum) {
        std::cerr << "Error: Sensores no encontrados" << std::endl;
        return;
    }
    
    const auto& temps = sensorTemp->getLecturas();  // O(1)
    const auto& hums = sensorHum->getLecturas();    // O(1)
    const auto& timestamps = sensorTemp->getTimestamps();  // O(1)
    
    // O(n) - Crear vector de índices
    std::vector<int> x(temps.size());
    for (size_t i = 0; i < x.size(); i++) x[i] = i+1;  // O(n)

    // O(n) - Extraer horas de los timestamps
    std::vector<std::string> horas;
    for (const auto& timestamp : timestamps) {  // O(n)
        horas.push_back(timestamp.substr(11,5));  // O(1) por elemento
    }

    // O(n) - Preparar etiquetas para eje X (con muestreo)
    std::vector<int> xticks;
    std::vector<std::string> xtick_labels;
    int step = std::max(1, (int)horas.size() / 8);  // O(1)
    for (size_t i = 0; i < horas.size(); i += step) {  // O(n/step) = O(n)
        xticks.push_back(i+1);
        xtick_labels.push_back(horas[i]);
    }

    // O(n) - Operaciones de plotting (dependen de la biblioteca)
    plt::figure();
    plt::subplot(2,1,1);
    plt::title("Temperatura por hora (" + sensorTemp->getUnidad() + ")");
    plt::plot(x, temps, "r-o");  // O(n)
    plt::xticks(xticks, xtick_labels);

    plt::subplot(2,1,2);
    plt::title("Humedad por hora (%)");
    plt::plot(x, hums, "b-o");  // O(n)
    plt::xticks(xticks, xtick_labels);

    plt::tight_layout();
    plt::show();
}

/**
 * FUNCIÓN: graficarOrdenadas
 * PROPÓSITO: Genera gráficas de temperatura y humedad ordenadas por valor
 * COMPLEJIDAD: O(n log n) - Dominada por std::sort
 * 
 * La operación más costosa es el ordenamiento de los vectores
 */
void graficarOrdenadas(SistemaSensores& sistema) {
    // O(1) - Acceso directo a sensores
    SensorTemperatura* sensorTemp = dynamic_cast<SensorTemperatura*>(sistema.buscarSensor("TEMP_001"));
    SensorHumedad* sensorHum = dynamic_cast<SensorHumedad*>(sistema.buscarSensor("HUM_001"));
    
    if (!sensorTemp || !sensorHum) {
        std::cerr << "Error: Sensores no encontrados" << std::endl;
        return;
    }
    
    const auto& temps = sensorTemp->getLecturas();  // O(1)
    const auto& hums = sensorHum->getLecturas();    // O(1)
    const auto& timestamps = sensorTemp->getTimestamps();  // O(1)
    
    // O(n) - Extraer horas
    std::vector<std::string> horas;
    for (const auto& timestamp : timestamps) {  // O(n)
        horas.push_back(timestamp.substr(11,5));  // O(1)
    }

    // O(n) - Crear vectores de pares (valor, hora)
    std::vector<std::pair<double, std::string>> tempsOrd;
    std::vector<std::pair<double, std::string>> humsOrd;
    
    for (size_t i = 0; i < temps.size(); i++) {  // O(n)
        tempsOrd.push_back({temps[i], horas[i]});
    }
    for (size_t i = 0; i < hums.size(); i++) {  // O(n)
        humsOrd.push_back({hums[i], horas[i]});
    }

    // O(n log n) - OPERACIÓN MÁS COSTOSA: ordenamiento
    std::sort(tempsOrd.begin(), tempsOrd.end());   // O(n log n)
    std::sort(humsOrd.begin(), humsOrd.end());     // O(n log n)

    // O(1) - Los métodos getMinimo/getMaximo son O(1) si se cachean
    double tempMin = sensorTemp->getMinimo();      // O(1) o O(n) dependiendo de implementación
    double tempMax = sensorTemp->getMaximo();      // O(1) o O(n)
    double humMin = sensorHum->getMinimo();        // O(1) o O(n)
    double humMax = sensorHum->getMaximo();        // O(1) o O(n)
    
    std::string tempMinHora = sensorTemp->getTimestampMinimo();  // O(1) o O(n)
    std::string tempMaxHora = sensorTemp->getTimestampMaximo();  // O(1) o O(n)
    std::string humMinHora = sensorHum->getTimestampMinimo();    // O(1) o O(n)
    std::string humMaxHora = sensorHum->getTimestampMaximo();    // O(1) o O(n)

    // O(1) - Cálculos simples
    double tempRango = tempMax - tempMin;
    double humRango = humMax - humMin;

    // --- Graficar temperaturas ordenadas ---
    plt::subplot(2,1,1);
    plt::title("Temperaturas ordenadas (" + sensorTemp->getUnidad() + ")");
    
    // O(n) - Preparar datos para gráfica
    std::vector<double> yT, xT;
    std::vector<std::string> xtick_labels_T;
    for (size_t i = 0; i < tempsOrd.size(); i++) {  // O(n)
        xT.push_back(i+1);
        yT.push_back(tempsOrd[i].first);
        xtick_labels_T.push_back(tempsOrd[i].second);
    }
    plt::plot(xT, yT, "r-");  // O(n)

    // O(n) - Buscar posiciones de mínimos/máximos en vector ordenado
    auto minTempIt = std::find(yT.begin(), yT.end(), tempMin);  // O(n)
    auto maxTempIt = std::find(yT.begin(), yT.end(), tempMax);  // O(n)
    
    if (minTempIt != yT.end()) {
        size_t minIndex = std::distance(yT.begin(), minTempIt);  // O(1)
        plt::plot(std::vector<double>{xT[minIndex]}, std::vector<double>{tempMin}, "go");  // O(1)
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << tempMin << "°C\n" << tempMinHora;
        plt::text(xT[minIndex], tempMin + tempRango * 0.08, ss.str());  // O(1)
    }
    
    if (maxTempIt != yT.end()) {
        size_t maxIndex = std::distance(yT.begin(), maxTempIt);  // O(1)
        plt::plot(std::vector<double>{xT[maxIndex]}, std::vector<double>{tempMax}, "ro");  // O(1)
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << tempMax << "°C\n" << tempMaxHora;
        plt::text(xT[maxIndex] - 0.8, tempMax - tempRango * 0.30, ss.str());  // O(1)
    }

    // O(n) - Configurar ejes con muestreo
    std::vector<int> xticksT;
    std::vector<std::string> xticksLabelT;
    int stepT = std::max(1, (int)xT.size()/8);  // O(1)
    for (size_t i = 0; i < xT.size(); i += stepT) {  // O(n/step) = O(n)
        xticksT.push_back(xT[i]);
        xticksLabelT.push_back(xtick_labels_T[i]);
    }
    plt::xticks(xticksT, xticksLabelT);

    // O(1) - Ajustar límites del eje Y
    double tempMargin = tempRango * 0.15;
    plt::ylim(tempMin - tempMargin, tempMax + tempMargin);

    // --- Graficar humedades ordenadas (complejidad similar: O(n log n + n)) ---
    plt::subplot(2,1,2);
    plt::title("Humedades ordenadas (%)");
    
    std::vector<double> yH, xH;
    std::vector<std::string> xtick_labels_H;
    for (size_t i = 0; i < humsOrd.size(); i++) {  // O(n)
        xH.push_back(i+1);
        yH.push_back(humsOrd[i].first);
        xtick_labels_H.push_back(humsOrd[i].second);
    }
    plt::plot(xH, yH, "b-");  // O(n)

    auto minHumIt = std::find(yH.begin(), yH.end(), humMin);  // O(n)
    auto maxHumIt = std::find(yH.begin(), yH.end(), humMax);  // O(n)
    
    if (minHumIt != yH.end()) {
        size_t minIndex = std::distance(yH.begin(), minHumIt);  // O(1)
        plt::plot(std::vector<double>{xH[minIndex]}, std::vector<double>{humMin}, "go");  // O(1)
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << humMin << "%\n" << humMinHora;
        plt::text(xH[minIndex], humMin + humRango * 0.08, ss.str());  // O(1)
    }
    
    if (maxHumIt != yH.end()) {
        size_t maxIndex = std::distance(yH.begin(), maxHumIt);  // O(1)
        plt::plot(std::vector<double>{xH[maxIndex]}, std::vector<double>{humMax}, "ro");  // O(1)
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << humMax << "%\n" << humMaxHora;
        plt::text(xH[maxIndex] - 0.8, humMax - humRango * 0.30, ss.str());  // O(1)
    }

    // O(n) - Configurar ejes
    std::vector<int> xticksH;
    std::vector<std::string> xticksLabelH;
    int stepH = std::max(1, (int)xH.size()/8);  // O(1)
    for (size_t i = 0; i < xH.size(); i += stepH) {  // O(n/step) = O(n)
        xticksH.push_back(xH[i]);
        xticksLabelH.push_back(xtick_labels_H[i]);
    }
    plt::xticks(xticksH, xticksLabelH);

    // O(1) - Ajustar límites
    double humMargin = humRango * 0.15;
    plt::ylim(humMin - humMargin, humMax + humMargin);

    plt::tight_layout();
    plt::show();

    // O(1) - Salida a consola (mostrarResumen es O(1) si los valores están cacheados)
    std::cout << "\n=== RESUMEN DE VALORES EXTREMOS ===" << std::endl;
    sensorTemp->mostrarResumen();  // O(1) o O(n)
    std::cout << std::endl;
    sensorHum->mostrarResumen();   // O(1) o O(n)
    
    std::cout << "\n=== INFORMACIÓN ADICIONAL ===" << std::endl;
    if (sensorTemp->tieneFiebre()) {  // O(1) o O(n)
        std::cout << "Alerta: Se detectaron temperaturas de fiebre (>38°C)" << std::endl;
    } else {
        std::cout << "Temperaturas dentro del rango normal" << std::endl;
    }
    
    std::cout << "Nivel de confort por humedad: " << sensorHum->getNivelConfort() << std::endl;  // O(1) o O(n)
}

/**
 * FUNCIÓN: buscarTemperaturaPorHora
 * PROPÓSITO: Permite buscar la temperatura para una hora específica usando búsqueda binaria
 * COMPLEJIDAD: O(n log n) - Dominada por el ordenamiento inicial
 * 
 * La búsqueda binaria es O(log n) pero requiere datos ordenados O(n log n)
 */
void buscarTemperaturaPorHora(SistemaSensores& sistema) {
    // O(1) - Acceso directo al sensor
    SensorTemperatura* sensorTemp = dynamic_cast<SensorTemperatura*>(sistema.buscarSensor("TEMP_001"));
    if (!sensorTemp) {
        std::cerr << "Error: Sensor de temperatura no encontrado" << std::endl;
        return;
    }
    
    const auto& lecturas = sensorTemp->getLecturas();      // O(1)
    const auto& timestamps = sensorTemp->getTimestamps();  // O(1)
    
    // O(n) - Preparar vector de pares (hora, temperatura)
    std::vector<std::pair<std::string, double>> horaTemp;
    for (size_t i = 0; i < timestamps.size(); i++) {  // O(n)
        std::string hora = timestamps[i].substr(11, 5);  // O(1)
        horaTemp.push_back({hora, lecturas[i]});         // O(1)
    }
    
    // O(n log n) - Ordenar por hora para habilitar búsqueda binaria
    std::sort(horaTemp.begin(), horaTemp.end(),  // O(n log n)
              [](const auto& a, const auto& b) {
                  return a.first < b.first;      // O(1) por comparación
              });
    
    std::cout << "\n=== BUSCAR TEMPERATURA POR HORA ===" << std::endl;
    std::cout << "Sensor: " << sensorTemp->getId() << " - " << sensorTemp->getTipo() << std::endl;
    std::cout << "Horas disponibles (formato HH:MM):" << std::endl;
    
    // O(n) - Mostrar horas disponibles (con muestreo implícito de 3 en 3)
    for (size_t i = 0; i < horaTemp.size(); i += 3) {  // O(n/3) = O(n)
        std::cout << horaTemp[i].first;
        if (i + 1 < horaTemp.size()) std::cout << ", " << horaTemp[i+1].first;  // O(1)
        if (i + 2 < horaTemp.size()) std::cout << ", " << horaTemp[i+2].first;  // O(1)
        std::cout << std::endl;
    }
    
    std::string horaBuscada;
    std::cout << "\nIngrese la hora a buscar (HH:MM): ";
    std::cin >> horaBuscada;
    
    // O(1) - Validación de formato
    if (horaBuscada.length() != 5 || horaBuscada[2] != ':') {
        std::cout << "Formato de hora inválido. Use HH:MM (ej: 14:30)" << std::endl;
        return;
    }
    
    // O(log n) - BÚSQUEDA BINARIA (operación eficiente)
    bool encontrada = std::binary_search(horaTemp.begin(), horaTemp.end(),  // O(log n)
                                        std::make_pair(horaBuscada, 0.0),
                                        [](const auto& a, const auto& b) {
                                            return a.first < b.first;  // O(1)
                                        });
    
    if (encontrada) {
        // O(log n) - Encontrar posición exacta
        auto it = std::lower_bound(horaTemp.begin(), horaTemp.end(),  // O(log n)
                                  std::make_pair(horaBuscada, 0.0),
                                  [](const auto& a, const auto& b) {
                                      return a.first < b.first;  // O(1)
                                  });
        
        if (it != horaTemp.end() && it->first == horaBuscada) {  // O(1)
            std::cout << "✓ Temperatura a las " << horaBuscada << ": " 
                      << std::fixed << std::setprecision(1) << it->second 
                      << sensorTemp->getUnidad() << std::endl;
        }
    } else {
        std::cout << "✗ No se encontraron datos para la hora " << horaBuscada << std::endl;
    }
}

/**
 * FUNCIÓN: main
 * PROPÓSITO: Función principal que coordina todo el sistema de sensores
 * COMPLEJIDAD GENERAL: O(n log n) - Dominada por graficarOrdenadas
 * n = número total de lecturas en el archivo CSV
 */
int main() {
    // O(1) - Inicialización del sistema
    SistemaSensores sistema;
    
    // O(1) - Creación de sensores (tiempo constante)
    sistema.agregarSensor(std::make_unique<SensorTemperatura>("TEMP_001", "°C"));
    sistema.agregarSensor(std::make_unique<SensorHumedad>("HUM_001"));
    
    // O(n) - Cargar datos desde CSV (n = número de líneas)
    if (!sistema.cargarDesdeCSV("datos.csv")) {
        std::cerr << "Error: no se pudo abrir datos.csv\n";
        return 1;
    }
    
    // O(1) - Mostrar resumen inicial (si los valores están cacheados)
    sistema.mostrarTodosLosSensores();
    
    // O(n) - Gráfica por hora (operación lineal)
    graficarPorHora(sistema);
    
    // O(n log n) - Gráfica ordenada (OPERACIÓN MÁS COSTOSA - domina la complejidad)
    graficarOrdenadas(sistema);
    
    // O(n log n) - Búsqueda por hora (similar complejidad por el ordenamiento)
    buscarTemperaturaPorHora(sistema);
    
    // O(1) - Resumen final
    sistema.mostrarTodosLosSensores();

    return 0;
}