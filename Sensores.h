#ifndef SENSORES_H
#define SENSORES_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>
#include <fstream>

// Clase base Sensor - Complejidad de métodos en comentarios
class Sensor {
protected:
    std::string id;
    std::vector<double> lecturas;        // O(1) acceso, O(n) búsqueda
    std::vector<std::string> timestamps; // O(1) acceso, O(n) búsqueda

public:
    // Constructor: O(1)
    Sensor(const std::string& id) : id(id) {}
    virtual ~Sensor() = default;
    
    // O(1) amortizado - push_back en vector
    virtual void agregarLectura(double valor, const std::string& timestamp) {
        lecturas.push_back(valor);
        timestamps.push_back(timestamp);
    }
    
    // O(1) - Retornar referencia constante
    const std::vector<double>& getLecturas() const { return lecturas; }
    const std::vector<std::string>& getTimestamps() const { return timestamps; }
    std::string getId() const { return id; } // O(1)
    
    virtual std::string getTipo() const = 0; // O(1) en clases derivadas
    
    // O(n) - Recorrer todo el vector de lecturas
    double getMaximo() const {
        if (lecturas.empty()) return 0.0;
        return *std::max_element(lecturas.begin(), lecturas.end());
    }
    
    // O(n) - Recorrer todo el vector de lecturas
    double getMinimo() const {
        if (lecturas.empty()) return 0.0;
        return *std::min_element(lecturas.begin(), lecturas.end());
    }
    
    // O(n) - Sumar todos los elementos
    double getPromedio() const {
        if (lecturas.empty()) return 0.0;
        double suma = 0.0;
        for (double lectura : lecturas) { // O(n)
            suma += lectura;
        }
        return suma / lecturas.size();
    }
    
    // O(n) - Encontrar máximo + acceso por índice
    std::string getTimestampMaximo() const {
        if (lecturas.empty()) return "";
        auto it = std::max_element(lecturas.begin(), lecturas.end()); // O(n)
        size_t index = std::distance(lecturas.begin(), it); // O(1)
        return timestamps[index].substr(11, 5); // O(1)
    }
    
    // O(n) - Encontrar mínimo + acceso por índice
    std::string getTimestampMinimo() const {
        if (lecturas.empty()) return "";
        auto it = std::min_element(lecturas.begin(), lecturas.end()); // O(n)
        size_t index = std::distance(lecturas.begin(), it); // O(1)
        return timestamps[index].substr(11, 5); // O(1)
    }
    
    // O(n) - Por llamar a getMinimo, getMaximo, getPromedio
    void mostrarResumen() const {
        std::cout << "=== " << getTipo() << " - " << id << " ===" << std::endl;
        std::cout << "Lecturas: " << lecturas.size() << std::endl; // O(1)
        std::cout << "Mínimo: " << getMinimo() << " (" << getTimestampMinimo() << ")" << std::endl; // O(n)
        std::cout << "Máximo: " << getMaximo() << " (" << getTimestampMaximo() << ")" << std::endl; // O(n)
        std::cout << "Promedio: " << getPromedio() << std::endl; // O(n)
    }
};

// SensorTemperatura - Complejidad igual a clase base + O(1) métodos adicionales
class SensorTemperatura : public Sensor {
private:
    std::string unidad;

public:
    // O(1) - Constructor
    SensorTemperatura(const std::string& id, const std::string& unidad = "°C") 
        : Sensor(id), unidad(unidad) {}
    
    // O(1) - Retorno constante
    std::string getTipo() const override {
        return "Sensor de Temperatura";
    }
    
    // O(1) - Retorno constante
    std::string getUnidad() const {
        return unidad;
    }
    
    // O(n) - Por llamar a getMaximo que es O(n)
    bool tieneFiebre() const {
        return getMaximo() > 38.0;
    }
};

// SensorHumedad - Complejidad similar
class SensorHumedad : public Sensor {
public:
    // O(1) - Constructor
    SensorHumedad(const std::string& id) : Sensor(id) {}
    
    // O(1) - Retorno constante
    std::string getTipo() const override {
        return "Sensor de Humedad";
    }
    
    // O(n) - Por llamar a getPromedio que es O(n)
    std::string getNivelConfort() const {
        double promedio = getPromedio(); // O(n)
        if (promedio < 30) return "Muy seco";
        if (promedio < 40) return "Seco";
        if (promedio < 60) return "Confortable";
        if (promedio < 70) return "Húmedo";
        return "Muy húmedo";
    }
};

// SistemaSensores - Gestión de múltiples sensores
class SistemaSensores {
private:
    std::vector<std::unique_ptr<Sensor>> sensores; // O(m) donde m = número de sensores

public:
    // O(1) amortizado - push_back en vector
    void agregarSensor(std::unique_ptr<Sensor> sensor) {
        sensores.push_back(std::move(sensor));
    }
    
    // O(m) - Búsqueda lineal en vector de sensores
    Sensor* buscarSensor(const std::string& id) {
        for (auto& sensor : sensores) { // O(m)
            if (sensor->getId() == id) {
                return sensor.get();
            }
        }
        return nullptr;
    }
    
    // O(m * n) - Donde m = sensores, n = lecturas por sensor
    void mostrarTodosLosSensores() const {
        std::cout << "\n=== SISTEMA DE SENSORES ===" << std::endl;
        std::cout << "Total de sensores: " << sensores.size() << std::endl;
        
        for (const auto& sensor : sensores) { // O(m)
            sensor->mostrarResumen(); // O(n) por sensor
            std::cout << std::endl;
        }
    }
    
    // O(l) - Donde l = líneas en el archivo CSV
    bool cargarDesdeCSV(const std::string& nombreArchivo) {
        std::ifstream file(nombreArchivo);
        if (!file.is_open()) return false;

        std::string line;
        std::getline(file, line); // O(1) - encabezado

        // O(1) - Crear sensores si no existen
        if (sensores.empty()) {
            sensores.push_back(std::make_unique<SensorTemperatura>("TEMP_001"));
            sensores.push_back(std::make_unique<SensorHumedad>("HUM_001"));
        }

        // O(l) - Procesar cada línea del archivo
        while (std::getline(file, line)) { // O(l)
            size_t pos1 = line.find(','); // O(k) donde k = longitud de línea
            size_t pos2 = line.find(',', pos1+1); // O(k)
            size_t pos3 = line.find(',', pos2+1); // O(k)

            std::string fecha = line.substr(pos1+1, pos2-pos1-1); // O(k)
            double temperatura = std::stod(line.substr(pos2+1, pos3-pos2-1)); // O(k)
            double humedad = std::stod(line.substr(pos3+1)); // O(k)

            // O(1) - Buscar sensores (m es pequeño, normalmente 2)
            if (auto sensorTemp = buscarSensor("TEMP_001")) {
                sensorTemp->agregarLectura(temperatura, fecha); // O(1)
            }

            if (auto sensorHum = buscarSensor("HUM_001")) {
                sensorHum->agregarLectura(humedad, fecha); // O(1)
            }
        }
        file.close();
        return true;
    }
};

#endif // SENSORES_H