#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <memory>
#include <list>
#include "matplotlibcpp.h"
#include "Sensores.h"

namespace plt = matplotlibcpp;

/**
 * FUNCIÓN: encontrarMinMax
 * PROPÓSITO: Encuentra los valores mínimos y máximos en un conjunto de datos usando listas
 * COMPLEJIDAD: O(n) donde n = tamaño de la lista
 */
void encontrarMinMax(const std::list<double>& datos, 
                    const std::list<std::string>& horas,
                    double& minVal, double& maxVal,
                    std::string& minHora, std::string& maxHora) {
    if (datos.empty()) return;
    
    auto minIt = std::min_element(datos.begin(), datos.end());  // O(n)
    auto maxIt = std::max_element(datos.begin(), datos.end());  // O(n)
    
    // Encontrar las horas correspondientes
    auto dataIt = datos.begin();
    auto hourIt = horas.begin();
    
    // Encontrar posición del mínimo
    while (dataIt != minIt) {
        ++dataIt;
        ++hourIt;
    }
    minVal = *minIt;
    minHora = *hourIt;
    
    // Reiniciar iteradores para encontrar máximo
    dataIt = datos.begin();
    hourIt = horas.begin();
    
    while (dataIt != maxIt) {
        ++dataIt;
        ++hourIt;
    }
    maxVal = *maxIt;
    maxHora = *hourIt;
}

/**
 * FUNCIÓN: graficarPorHora
 * PROPÓSITO: Genera gráficas de temperatura y humedad en orden temporal usando listas
 * COMPLEJIDAD: O(n) donde n = número de lecturas de sensores
 */
void graficarPorHora(SistemaSensores& sistema) {
    SensorTemperatura* sensorTemp = dynamic_cast<SensorTemperatura*>(sistema.buscarSensor("TEMP_001"));
    SensorHumedad* sensorHum = dynamic_cast<SensorHumedad*>(sistema.buscarSensor("HUM_001"));
    
    if (!sensorTemp || !sensorHum) {
        std::cerr << "Error: Sensores no encontrados" << std::endl;
        return;
    }
    
    // Obtener datos como vectores para compatibilidad con matplotlibcpp
    std::vector<double> temps = sensorTemp->getLecturas();
    std::vector<double> hums = sensorHum->getLecturas();
    std::vector<std::string> timestamps = sensorTemp->getTimestamps();
    
    // Crear vector de índices
    std::vector<int> x(temps.size());
    for (size_t i = 0; i < x.size(); i++) x[i] = i+1;

    // Extraer horas de los timestamps
    std::vector<std::string> horas;
    for (const auto& timestamp : timestamps) {
        horas.push_back(timestamp.substr(11,5));
    }

    // Preparar etiquetas para eje X (con muestreo)
    std::vector<int> xticks;
    std::vector<std::string> xtick_labels;
    int step = std::max(1, (int)horas.size() / 8);
    for (size_t i = 0; i < horas.size(); i += step) {
        xticks.push_back(i+1);
        xtick_labels.push_back(horas[i]);
    }

    // Operaciones de plotting
    plt::figure();
    plt::subplot(2,1,1);
    plt::title("Temperatura por hora (" + sensorTemp->getUnidad() + ")");
    plt::plot(x, temps, "r-o");
    plt::xticks(xticks, xtick_labels);

    plt::subplot(2,1,2);
    plt::title("Humedad por hora (%)");
    plt::plot(x, hums, "b-o");
    plt::xticks(xticks, xtick_labels);

    plt::tight_layout();
    plt::show();
}

/**
 * FUNCIÓN: graficarOrdenadas
 * PROPÓSITO: Genera gráficas de temperatura y humedad ordenadas por valor usando listas
 * COMPLEJIDAD: O(n log n) - Dominada por std::sort
 */
void graficarOrdenadas(SistemaSensores& sistema) {
    SensorTemperatura* sensorTemp = dynamic_cast<SensorTemperatura*>(sistema.buscarSensor("TEMP_001"));
    SensorHumedad* sensorHum = dynamic_cast<SensorHumedad*>(sistema.buscarSensor("HUM_001"));
    
    if (!sensorTemp || !sensorHum) {
        std::cerr << "Error: Sensores no encontrados" << std::endl;
        return;
    }
    
    // Obtener datos como vectores para procesamiento
    std::vector<double> temps = sensorTemp->getLecturas();
    std::vector<double> hums = sensorHum->getLecturas();
    std::vector<std::string> timestamps = sensorTemp->getTimestamps();
    
    // Extraer horas
    std::vector<std::string> horas;
    for (const auto& timestamp : timestamps) {
        horas.push_back(timestamp.substr(11,5));
    }

    // Crear vectores de pares (valor, hora)
    std::vector<std::pair<double, std::string>> tempsOrd;
    std::vector<std::pair<double, std::string>> humsOrd;
    
    for (size_t i = 0; i < temps.size(); i++) {
        tempsOrd.push_back({temps[i], horas[i]});
    }
    for (size_t i = 0; i < hums.size(); i++) {
        humsOrd.push_back({hums[i], horas[i]});
    }

    // Ordenamiento
    std::sort(tempsOrd.begin(), tempsOrd.end());
    std::sort(humsOrd.begin(), humsOrd.end());

    // Obtener valores extremos
    double tempMin = sensorTemp->getMinimo();
    double tempMax = sensorTemp->getMaximo();
    double humMin = sensorHum->getMinimo();
    double humMax = sensorHum->getMaximo();
    
    std::string tempMinHora = sensorTemp->getTimestampMinimo();
    std::string tempMaxHora = sensorTemp->getTimestampMaximo();
    std::string humMinHora = sensorHum->getTimestampMinimo();
    std::string humMaxHora = sensorHum->getTimestampMaximo();

    // Cálculos simples
    double tempRango = tempMax - tempMin;
    double humRango = humMax - humMin;

    // --- Graficar temperaturas ordenadas ---
    plt::subplot(2,1,1);
    plt::title("Temperaturas ordenadas (" + sensorTemp->getUnidad() + ")");
    
    std::vector<double> yT, xT;
    std::vector<std::string> xtick_labels_T;
    for (size_t i = 0; i < tempsOrd.size(); i++) {
        xT.push_back(i+1);
        yT.push_back(tempsOrd[i].first);
        xtick_labels_T.push_back(tempsOrd[i].second);
    }
    plt::plot(xT, yT, "r-");

    // Buscar posiciones de mínimos/máximos en vector ordenado
    auto minTempIt = std::find(yT.begin(), yT.end(), tempMin);
    auto maxTempIt = std::find(yT.begin(), yT.end(), tempMax);
    
    if (minTempIt != yT.end()) {
        size_t minIndex = std::distance(yT.begin(), minTempIt);
        plt::plot(std::vector<double>{xT[minIndex]}, std::vector<double>{tempMin}, "go");
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << tempMin << "°C\n" << tempMinHora;
        plt::text(xT[minIndex], tempMin + tempRango * 0.08, ss.str());
    }
    
    if (maxTempIt != yT.end()) {
        size_t maxIndex = std::distance(yT.begin(), maxTempIt);
        plt::plot(std::vector<double>{xT[maxIndex]}, std::vector<double>{tempMax}, "ro");
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << tempMax << "°C\n" << tempMaxHora;
        plt::text(xT[maxIndex] - 0.8, tempMax - tempRango * 0.30, ss.str());
    }

    // Configurar ejes con muestreo
    std::vector<int> xticksT;
    std::vector<std::string> xticksLabelT;
    int stepT = std::max(1, (int)xT.size()/8);
    for (size_t i = 0; i < xT.size(); i += stepT) {
        xticksT.push_back(xT[i]);
        xticksLabelT.push_back(xtick_labels_T[i]);
    }
    plt::xticks(xticksT, xticksLabelT);

    // Ajustar límites del eje Y
    double tempMargin = tempRango * 0.15;
    plt::ylim(tempMin - tempMargin, tempMax + tempMargin);

    // --- Graficar humedades ordenadas ---
    plt::subplot(2,1,2);
    plt::title("Humedades ordenadas (%)");
    
    std::vector<double> yH, xH;
    std::vector<std::string> xtick_labels_H;
    for (size_t i = 0; i < humsOrd.size(); i++) {
        xH.push_back(i+1);
        yH.push_back(humsOrd[i].first);
        xtick_labels_H.push_back(humsOrd[i].second);
    }
    plt::plot(xH, yH, "b-");

    auto minHumIt = std::find(yH.begin(), yH.end(), humMin);
    auto maxHumIt = std::find(yH.begin(), yH.end(), humMax);
    
    if (minHumIt != yH.end()) {
        size_t minIndex = std::distance(yH.begin(), minHumIt);
        plt::plot(std::vector<double>{xH[minIndex]}, std::vector<double>{humMin}, "go");
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << humMin << "%\n" << humMinHora;
        plt::text(xH[minIndex], humMin + humRango * 0.08, ss.str());
    }
    
    if (maxHumIt != yH.end()) {
        size_t maxIndex = std::distance(yH.begin(), maxHumIt);
        plt::plot(std::vector<double>{xH[maxIndex]}, std::vector<double>{humMax}, "ro");
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << humMax << "%\n" << humMaxHora;
        plt::text(xH[maxIndex] - 0.8, humMax - humRango * 0.30, ss.str());
    }

    // Configurar ejes
    std::vector<int> xticksH;
    std::vector<std::string> xticksLabelH;
    int stepH = std::max(1, (int)xH.size()/8);
    for (size_t i = 0; i < xH.size(); i += stepH) {
        xticksH.push_back(xH[i]);
        xticksLabelH.push_back(xtick_labels_H[i]);
    }
    plt::xticks(xticksH, xticksLabelH);

    // Ajustar límites
    double humMargin = humRango * 0.15;
    plt::ylim(humMin - humMargin, humMax + humMargin);

    plt::tight_layout();
    plt::show();

    // Salida a consola
    std::cout << "\n=== RESUMEN DE VALORES EXTREMOS ===" << std::endl;
    sensorTemp->mostrarResumen();
    std::cout << std::endl;
    sensorHum->mostrarResumen();
    
    std::cout << "\n=== INFORMACIÓN ADICIONAL ===" << std::endl;
    if (sensorTemp->tieneFiebre()) {
        std::cout << "Alerta: Se detectaron temperaturas de fiebre (>38°C)" << std::endl;
    } else {
        std::cout << "Temperaturas dentro del rango normal" << std::endl;
    }
    
    std::cout << "Nivel de confort por humedad: " << sensorHum->getNivelConfort() << std::endl;
}

/**
 * FUNCIÓN: buscarTemperaturaPorHora
 * PROPÓSITO: Permite buscar la temperatura para una hora específica
 * COMPLEJIDAD: O(n log n) - Dominada por el ordenamiento inicial
 */
void buscarTemperaturaPorHora(SistemaSensores& sistema) {
    SensorTemperatura* sensorTemp = dynamic_cast<SensorTemperatura*>(sistema.buscarSensor("TEMP_001"));
    if (!sensorTemp) {
        std::cerr << "Error: Sensor de temperatura no encontrado" << std::endl;
        return;
    }
    
    // Obtener datos como vectores para procesamiento
    std::vector<double> lecturas = sensorTemp->getLecturas();
    std::vector<std::string> timestamps = sensorTemp->getTimestamps();
    
    // Preparar vector de pares (hora, temperatura)
    std::vector<std::pair<std::string, double>> horaTemp;
    for (size_t i = 0; i < timestamps.size(); i++) {
        std::string hora = timestamps[i].substr(11, 5);
        horaTemp.push_back({hora, lecturas[i]});
    }
    
    // Ordenar por hora para habilitar búsqueda binaria
    std::sort(horaTemp.begin(), horaTemp.end(),
              [](const auto& a, const auto& b) {
                  return a.first < b.first;
              });
    
    std::cout << "\n=== BUSCAR TEMPERATURA POR HORA ===" << std::endl;
    std::cout << "Sensor: " << sensorTemp->getId() << " - " << sensorTemp->getTipo() << std::endl;
    std::cout << "Horas disponibles (formato HH:MM):" << std::endl;
    
    // Mostrar horas disponibles (con muestreo implícito de 3 en 3)
    for (size_t i = 0; i < horaTemp.size(); i += 3) {
        std::cout << horaTemp[i].first;
        if (i + 1 < horaTemp.size()) std::cout << ", " << horaTemp[i+1].first;
        if (i + 2 < horaTemp.size()) std::cout << ", " << horaTemp[i+2].first;
        std::cout << std::endl;
    }
    
    std::string horaBuscada;
    std::cout << "\nIngrese la hora a buscar (HH:MM): ";
    std::cin >> horaBuscada;
    
    // Validación de formato
    if (horaBuscada.length() != 5 || horaBuscada[2] != ':') {
        std::cout << "Formato de hora inválido. Use HH:MM (ej: 14:30)" << std::endl;
        return;
    }
    
    // Búsqueda binaria
    bool encontrada = std::binary_search(horaTemp.begin(), horaTemp.end(),
                                        std::make_pair(horaBuscada, 0.0),
                                        [](const auto& a, const auto& b) {
                                            return a.first < b.first;
                                        });
    
    if (encontrada) {
        // Encontrar posición exacta
        auto it = std::lower_bound(horaTemp.begin(), horaTemp.end(),
                                  std::make_pair(horaBuscada, 0.0),
                                  [](const auto& a, const auto& b) {
                                      return a.first < b.first;
                                  });
        
        if (it != horaTemp.end() && it->first == horaBuscada) {
            std::cout << "Temperatura a las " << horaBuscada << ": " 
                      << std::fixed << std::setprecision(1) << it->second 
                      << sensorTemp->getUnidad() << std::endl;
        }
    } else {
        std::cout << "No se encontraron datos para la hora " << horaBuscada << std::endl;
    }
}

/**
 * FUNCIÓN: main
 * PROPÓSITO: Función principal que coordina todo el sistema de sensores
 */
int main() {
    SistemaSensores sistema;
    
    sistema.agregarSensor(std::make_unique<SensorTemperatura>("TEMP_001", "°C"));
    sistema.agregarSensor(std::make_unique<SensorHumedad>("HUM_001"));
    
    if (!sistema.cargarDesdeCSV("datos.csv")) {
        std::cerr << "Error: no se pudo abrir datos.csv\n";
        return 1;
    }
    
    sistema.mostrarTodosLosSensores();
    graficarPorHora(sistema);
    graficarOrdenadas(sistema);
    buscarTemperaturaPorHora(sistema);
    sistema.mostrarTodosLosSensores();

    return 0;
}