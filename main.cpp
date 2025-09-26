#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <algorithm>
#include <memory>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

// ===== Clase abstracta Sensor =====
class Sensor {
protected:
    std::string id;
public:
    Sensor(const std::string& id) : id(id) {}
    virtual ~Sensor() {}
    virtual double leer() = 0;
    std::string getId() const { return id; }
};

// ===== Sensor de Temperatura =====
class SensorTemperatura : public Sensor {
public:
    SensorTemperatura(const std::string& id) : Sensor(id) {}
    double leer() override {
        static std::default_random_engine gen(std::random_device{}());
        std::uniform_real_distribution<double> dist(18.0, 30.0);
        return dist(gen);
    }
};

// ===== Sensor de Humedad =====
class SensorHumedad : public Sensor {
public:
    SensorHumedad(const std::string& id) : Sensor(id) {}
    double leer() override {
        static std::default_random_engine gen(std::random_device{}());
        std::uniform_real_distribution<double> dist(30.0, 80.0);
        return dist(gen);
    }
};

// ===== Formatear timestamp como string =====
std::string formatTime(const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buffer);
}

// ===== Guardar CSV con fecha =====
void guardarCSV(const std::string& nombreArchivo,
                const std::vector<double>& temps,
                const std::vector<double>& hums,
                const std::vector<std::string>& fechas) {
    std::ofstream file(nombreArchivo);
    file << "Lectura,Fecha,Temperatura,Humedad\n";
    for (size_t i = 0; i < temps.size(); i++) {
        file << i+1 << "," << fechas[i] << "," << temps[i] << "," << hums[i] << "\n";
    }
    file.close();
}

// ===== Leer CSV =====
bool leerCSV(const std::string& nombreArchivo,
             std::vector<double>& temps,
             std::vector<double>& hums,
             std::vector<std::string>& fechas) {
    std::ifstream file(nombreArchivo);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // saltar encabezado

    while (std::getline(file, line)) {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1+1);
        size_t pos3 = line.find(',', pos2+1);

        fechas.push_back(line.substr(pos1+1, pos2-pos1-1));
        temps.push_back(std::stod(line.substr(pos2+1, pos3-pos2-1)));
        hums.push_back(std::stod(line.substr(pos3+1)));
    }
    file.close();
    return true;
}

// ===== Graficar datos =====
void graficar(const std::vector<double>& temps,
              const std::vector<double>& hums,
              const std::vector<std::string>& fechas) {

    std::vector<int> x(temps.size());
    for (size_t i = 0; i < x.size(); i++) x[i] = i+1;

    plt::figure();
    plt::subplot(2,1,1);
    plt::title("Temperatura (°C)");
    plt::plot(x, temps, "r-");

    plt::subplot(2,1,2);
    plt::title("Humedad (%)");
    plt::plot(x, hums, "b-");

    // Mostrar solo algunas fechas en eje X para que no se amontonen
    std::vector<std::string> xticks;
    std::vector<int> xtick_positions;
    int step = std::max(1, static_cast<int>(fechas.size()/10)); // máximo 10 etiquetas
    for (size_t i = 0; i < fechas.size(); i += step) {
        // Mostrar solo HH:MM
        xticks.push_back(fechas[i].substr(11, 5));
        xtick_positions.push_back(i+1);
    }
    plt::xticks(xtick_positions, xticks);

    plt::tight_layout();
    plt::show();
}

int main() {
    std::unique_ptr<Sensor> tempSensor = std::make_unique<SensorTemperatura>("T1");
    std::unique_ptr<Sensor> humSensor  = std::make_unique<SensorHumedad>("H1");

    std::vector<double> temperaturas;
    std::vector<double> humedades;
    std::vector<std::string> fechas;

    const int N = 30; // número de nuevas lecturas

    // ===== Leer CSV previo =====
    if (leerCSV("datos.csv", temperaturas, humedades, fechas)) {
        std::cout << "Se cargaron " << temperaturas.size() << " lecturas previas desde datos.csv" << std::endl;
    }

    // ===== Simular nuevas lecturas cada hora =====
    std::chrono::system_clock::time_point lastTime;
    if (fechas.empty()) {
        lastTime = std::chrono::system_clock::now();
    } else {
        // Tomar la última fecha del CSV y convertir a time_point
        std::tm tm = {};
        std::istringstream ss(fechas.back());
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        lastTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    
    for (int i = 0; i < N; i++) {
        temperaturas.push_back(tempSensor->leer());
        humedades.push_back(humSensor->leer());
        lastTime += std::chrono::hours(1); // incrementar 1 hora
        fechas.push_back(formatTime(lastTime));
    }

    // ===== Ordenar temperaturas (ejemplo) =====
    std::vector<double> tempsOrdenadas = temperaturas;
    std::sort(tempsOrdenadas.begin(), tempsOrdenadas.end());

    // ===== Guardar CSV =====
    guardarCSV("datos.csv", temperaturas, humedades, fechas);

    // ===== Graficar =====
    graficar(temperaturas, humedades, fechas);

    std::cout << "Temperaturas ordenadas: ";
    for (auto t : tempsOrdenadas) std::cout << t << " ";
    std::cout << std::endl;

    return 0;
}
