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

// Función auxiliar para encontrar mínimos y máximos
void encontrarMinMax(const std::vector<double>& datos, 
                    const std::vector<std::string>& horas,
                    double& minVal, double& maxVal,
                    std::string& minHora, std::string& maxHora) {
    if (datos.empty()) return;
    
    auto minIt = std::min_element(datos.begin(), datos.end());
    auto maxIt = std::max_element(datos.begin(), datos.end());
    
    size_t minIndex = std::distance(datos.begin(), minIt);
    size_t maxIndex = std::distance(datos.begin(), maxIt);
    
    minVal = *minIt;
    maxVal = *maxIt;
    minHora = horas[minIndex];
    maxHora = horas[maxIndex];
}

void graficarPorHora(SistemaSensores& sistema) {
    // Obtener sensores
    SensorTemperatura* sensorTemp = dynamic_cast<SensorTemperatura*>(sistema.buscarSensor("TEMP_001"));
    SensorHumedad* sensorHum = dynamic_cast<SensorHumedad*>(sistema.buscarSensor("HUM_001"));
    
    if (!sensorTemp || !sensorHum) {
        std::cerr << "Error: Sensores no encontrados" << std::endl;
        return;
    }
    
    const auto& temps = sensorTemp->getLecturas();
    const auto& hums = sensorHum->getLecturas();
    const auto& timestamps = sensorTemp->getTimestamps();
    
    std::vector<int> x(temps.size());
    for (size_t i = 0; i < x.size(); i++) x[i] = i+1;

    std::vector<std::string> horas;
    for (const auto& timestamp : timestamps) {
        horas.push_back(timestamp.substr(11,5));
    }

    std::vector<int> xticks;
    std::vector<std::string> xtick_labels;
    int step = std::max(1, (int)horas.size() / 8);
    for (size_t i = 0; i < horas.size(); i += step) {
        xticks.push_back(i+1);
        xtick_labels.push_back(horas[i]);
    }

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

void graficarOrdenadas(SistemaSensores& sistema) {
    // Obtener sensores
    SensorTemperatura* sensorTemp = dynamic_cast<SensorTemperatura*>(sistema.buscarSensor("TEMP_001"));
    SensorHumedad* sensorHum = dynamic_cast<SensorHumedad*>(sistema.buscarSensor("HUM_001"));
    
    if (!sensorTemp || !sensorHum) {
        std::cerr << "Error: Sensores no encontrados" << std::endl;
        return;
    }
    
    const auto& temps = sensorTemp->getLecturas();
    const auto& hums = sensorHum->getLecturas();
    const auto& timestamps = sensorTemp->getTimestamps();
    
    std::vector<std::string> horas;
    for (const auto& timestamp : timestamps) {
        horas.push_back(timestamp.substr(11,5));
    }

    // Crear vectores ordenados
    std::vector<std::pair<double, std::string>> tempsOrd;
    std::vector<std::pair<double, std::string>> humsOrd;
    
    for (size_t i = 0; i < temps.size(); i++) {
        tempsOrd.push_back({temps[i], horas[i]});
    }
    for (size_t i = 0; i < hums.size(); i++) {
        humsOrd.push_back({hums[i], horas[i]});
    }

    std::sort(tempsOrd.begin(), tempsOrd.end());
    std::sort(humsOrd.begin(), humsOrd.end());

    // Encontrar mínimos y máximos
    double tempMin = sensorTemp->getMinimo();
    double tempMax = sensorTemp->getMaximo();
    double humMin = sensorHum->getMinimo();
    double humMax = sensorHum->getMaximo();
    
    std::string tempMinHora = sensorTemp->getTimestampMinimo();
    std::string tempMaxHora = sensorTemp->getTimestampMaximo();
    std::string humMinHora = sensorHum->getTimestampMinimo();
    std::string humMaxHora = sensorHum->getTimestampMaximo();

    // Calcular rangos para un mejor posicionamiento
    double tempRango = tempMax - tempMin;
    double humRango = humMax - humMin;

    // --- Graficar ---
    plt::figure();

    // --- Temperaturas ordenadas ---
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

    // Marcar mínimo y máximo con posiciones invertidas
    auto minTempIt = std::find(yT.begin(), yT.end(), tempMin);
    auto maxTempIt = std::find(yT.begin(), yT.end(), tempMax);
    
    if (minTempIt != yT.end()) {
        size_t minIndex = std::distance(yT.begin(), minTempIt);
        plt::plot(std::vector<double>{xT[minIndex]}, std::vector<double>{tempMin}, "go");
        
        // MÍNIMO: Etiqueta ARRIBA del punto
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << tempMin << "°C\n" << tempMinHora;
        plt::text(xT[minIndex], tempMin + tempRango * 0.08, ss.str()); // +8% del rango
    }
    
    if (maxTempIt != yT.end()) {
        size_t maxIndex = std::distance(yT.begin(), maxTempIt);
        plt::plot(std::vector<double>{xT[maxIndex]}, std::vector<double>{tempMax}, "ro");
        
        // MÁXIMO: Etiqueta ABAJO del punto
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << tempMax << "°C\n" << tempMaxHora;
        plt::text(xT[maxIndex] - 0.8, tempMax - tempRango * 0.30, ss.str()); // -8% del rango
    }

    // Configurar xticks
    std::vector<int> xticksT;
    std::vector<std::string> xticksLabelT;
    int stepT = std::max(1, (int)xT.size()/8);
    for (size_t i = 0; i < xT.size(); i += stepT) {
        xticksT.push_back(xT[i]);
        xticksLabelT.push_back(xtick_labels_T[i]);
    }
    plt::xticks(xticksT, xticksLabelT);

    // Ajustar límites del eje Y para dar espacio a las etiquetas
    double tempMargin = tempRango * 0.15; // 15% de margen
    plt::ylim(tempMin - tempMargin, tempMax + tempMargin);

    // --- Humedades ordenadas ---
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

    // Marcar mínimo y máximo con posiciones invertidas
    auto minHumIt = std::find(yH.begin(), yH.end(), humMin);
    auto maxHumIt = std::find(yH.begin(), yH.end(), humMax);
    
    if (minHumIt != yH.end()) {
        size_t minIndex = std::distance(yH.begin(), minHumIt);
        plt::plot(std::vector<double>{xH[minIndex]}, std::vector<double>{humMin}, "go");
        
        // MÍNIMO: Etiqueta ARRIBA del punto
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << humMin << "%\n" << humMinHora;
        plt::text(xH[minIndex], humMin + humRango * 0.08, ss.str()); // +8% del rango
    }
    
    if (maxHumIt != yH.end()) {
        size_t maxIndex = std::distance(yH.begin(), maxHumIt);
        plt::plot(std::vector<double>{xH[maxIndex]}, std::vector<double>{humMax}, "ro");
        
        // MÁXIMO: Etiqueta ABAJO del punto
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << humMax << "%\n" << humMaxHora;
        plt::text(xH[maxIndex] - 0.8, humMax - humRango * 0.30, ss.str()); // -8% del rango
    }

    // Configurar xticks
    std::vector<int> xticksH;
    std::vector<std::string> xticksLabelH;
    int stepH = std::max(1, (int)xH.size()/8);
    for (size_t i = 0; i < xH.size(); i += stepH) {
        xticksH.push_back(xH[i]);
        xticksLabelH.push_back(xtick_labels_H[i]);
    }
    plt::xticks(xticksH, xticksLabelH);

    // Ajustar límites del eje Y para dar espacio a las etiquetas
    double humMargin = humRango * 0.15; // 15% de margen
    plt::ylim(humMin - humMargin, humMax + humMargin);

    plt::tight_layout();
    plt::show();

    // Mostrar resumen en consola
    std::cout << "\n=== RESUMEN DE VALORES EXTREMOS ===" << std::endl;
    sensorTemp->mostrarResumen();
    std::cout << std::endl;
    sensorHum->mostrarResumen();
    
    // Información adicional
    std::cout << "\n=== INFORMACIÓN ADICIONAL ===" << std::endl;
    if (sensorTemp->tieneFiebre()) {
        std::cout << "Alerta: Se detectaron temperaturas de fiebre (>38°C)" << std::endl;
    } else {
        std::cout << "Temperaturas dentro del rango normal" << std::endl;
    }
    
    std::cout << "Nivel de confort por humedad: " << sensorHum->getNivelConfort() << std::endl;
}

void buscarTemperaturaPorHora(SistemaSensores& sistema) {
    SensorTemperatura* sensorTemp = dynamic_cast<SensorTemperatura*>(sistema.buscarSensor("TEMP_001"));
    if (!sensorTemp) {
        std::cerr << "Error: Sensor de temperatura no encontrado" << std::endl;
        return;
    }
    
    const auto& lecturas = sensorTemp->getLecturas();
    const auto& timestamps = sensorTemp->getTimestamps();
    
    // Crear vector de pares (hora, temperatura)
    std::vector<std::pair<std::string, double>> horaTemp;
    for (size_t i = 0; i < timestamps.size(); i++) {
        std::string hora = timestamps[i].substr(11, 5); // Extraer HH:MM
        horaTemp.push_back({hora, lecturas[i]});
    }
    
    // Ordenar por hora
    std::sort(horaTemp.begin(), horaTemp.end(), 
              [](const auto& a, const auto& b) {
                  return a.first < b.first;
              });
    
    std::cout << "\n=== BUSCAR TEMPERATURA POR HORA ===" << std::endl;
    std::cout << "Sensor: " << sensorTemp->getId() << " - " << sensorTemp->getTipo() << std::endl;
    std::cout << "Horas disponibles (formato HH:MM):" << std::endl;
    
    // Mostrar horas disponibles
    for (size_t i = 0; i < horaTemp.size(); i += 3) {
        std::cout << horaTemp[i].first;
        if (i + 1 < horaTemp.size()) std::cout << ", " << horaTemp[i+1].first;
        if (i + 2 < horaTemp.size()) std::cout << ", " << horaTemp[i+2].first;
        std::cout << std::endl;
    }
    
    std::string horaBuscada;
    std::cout << "\nIngrese la hora a buscar (HH:MM): ";
    std::cin >> horaBuscada;
    
    // Validar formato
    if (horaBuscada.length() != 5 || horaBuscada[2] != ':') {
        std::cout << "Formato de hora inválido. Use HH:MM (ej: 14:30)" << std::endl;
        return;
    }
    
    // Buscar usando binary_search
    bool encontrada = std::binary_search(horaTemp.begin(), horaTemp.end(), 
                                        std::make_pair(horaBuscada, 0.0),
                                        [](const auto& a, const auto& b) {
                                            return a.first < b.first;
                                        });
    
    if (encontrada) {
        auto it = std::lower_bound(horaTemp.begin(), horaTemp.end(),
                                  std::make_pair(horaBuscada, 0.0),
                                  [](const auto& a, const auto& b) {
                                      return a.first < b.first;
                                  });
        
        if (it != horaTemp.end() && it->first == horaBuscada) {
            std::cout << "✓ Temperatura a las " << horaBuscada << ": " 
                      << std::fixed << std::setprecision(1) << it->second 
                      << sensorTemp->getUnidad() << std::endl;
        }
    } else {
        std::cout << "✗ No se encontraron datos para la hora " << horaBuscada << std::endl;
    }
}

int main() {
    // Crear y configurar el sistema de sensores
    SistemaSensores sistema;
    
    // Agregar sensores al sistema
    sistema.agregarSensor(std::make_unique<SensorTemperatura>("TEMP_001", "°C"));
    sistema.agregarSensor(std::make_unique<SensorHumedad>("HUM_001"));
    
    // Cargar datos desde CSV
    if (!sistema.cargarDesdeCSV("datos.csv")) {
        std::cerr << "Error: no se pudo abrir datos.csv\n";
        return 1;
    }
    
    // Mostrar información inicial del sistema
    sistema.mostrarTodosLosSensores();
    
    // Generar gráficas usando los datos de los sensores
    graficarPorHora(sistema);
    graficarOrdenadas(sistema);
    
    // Búsqueda por hora
    buscarTemperaturaPorHora(sistema);
    
    // Mostrar análisis final
    std::cout << "\n=== ANÁLISIS COMPLETO ===" << std::endl;
    sistema.mostrarTodosLosSensores();

    return 0;
}