#ifndef SENSORES_H
#define SENSORES_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>
#include <fstream>
#include <list>
#include <iterator>

// Clase base Sensor
class Sensor {
protected:
    std::string id;
    std::list<double> lecturas;        
    std::list<std::string> timestamps; 

public:
    // Constructor: O(1)
    Sensor(const std::string& id) : id(id) {}
    virtual ~Sensor() = default;
    
    // O(1) - push_back en std::list
    virtual void agregarLectura(double valor, const std::string& timestamp) {
        lecturas.push_back(valor);
        timestamps.push_back(timestamp);
    }
    
    // O(n) - Convertir a vector para compatibilidad
    std::vector<double> getLecturas() const { 
        return std::vector<double>(lecturas.begin(), lecturas.end());
    }
    
    std::vector<std::string> getTimestamps() const { 
        return std::vector<std::string>(timestamps.begin(), timestamps.end());
    }
    
    // Métodos adicionales para acceso directo a la lista
    const std::list<double>& getLecturasList() const { return lecturas; }
    const std::list<std::string>& getTimestampsList() const { return timestamps; }
    
    std::string getId() const { return id; } // O(1)
    
    virtual std::string getTipo() const = 0; // O(1) en clases derivadas
    
    // O(n) - Encontrar máximo usando std::max_element con iteradores
    double getMaximo() const {
        if (lecturas.empty()) return 0.0;
        return *std::max_element(lecturas.begin(), lecturas.end());
    }
    
    // O(n) - Encontrar mínimo usando std::min_element con iteradores
    double getMinimo() const {
        if (lecturas.empty()) return 0.0;
        return *std::min_element(lecturas.begin(), lecturas.end());
    }
    
    // O(n) - Calcular promedio
    double getPromedio() const {
        if (lecturas.empty()) return 0.0;
        double suma = 0.0;
        for (double lectura : lecturas) {
            suma += lectura;
        }
        return suma / lecturas.size();
    }
    
    // O(n) - Encontrar timestamp del máximo
    std::string getTimestampMaximo() const {
        if (lecturas.empty()) return "";
        auto maxIt = std::max_element(lecturas.begin(), lecturas.end());
        
        // Encontrar el índice correspondiente en timestamps
        auto tempIt = lecturas.begin();
        auto timeIt = timestamps.begin();
        
        while (tempIt != maxIt) {
            ++tempIt;
            ++timeIt;
        }
        
        return timeIt->substr(11, 5);
    }
    
    // O(n) - Encontrar timestamp del mínimo
    std::string getTimestampMinimo() const {
        if (lecturas.empty()) return "";
        auto minIt = std::min_element(lecturas.begin(), lecturas.end());
        
        // Encontrar el índice correspondiente en timestamps
        auto tempIt = lecturas.begin();
        auto timeIt = timestamps.begin();
        
        while (tempIt != minIt) {
            ++tempIt;
            ++timeIt;
        }
        
        return timeIt->substr(11, 5);
    }
    
    // O(n) - Por llamar a getMinimo, getMaximo, getPromedio
    void mostrarResumen() const {
        std::cout << "=== " << getTipo() << " - " << id << " ===" << std::endl;
        std::cout << "Lecturas: " << lecturas.size() << std::endl;
        std::cout << "Mínimo: " << getMinimo() << " (" << getTimestampMinimo() << ")" << std::endl;
        std::cout << "Máximo: " << getMaximo() << " (" << getTimestampMaximo() << ")" << std::endl;
        std::cout << "Promedio: " << getPromedio() << std::endl;
    }
    
    // Método auxiliar para obtener el tamaño
    size_t getNumLecturas() const {
        return lecturas.size();
    }
};

// SensorTemperatura
class SensorTemperatura : public Sensor {
private:
    std::string unidad;

public:
    SensorTemperatura(const std::string& id, const std::string& unidad = "°C") 
        : Sensor(id), unidad(unidad) {}
    
    std::string getTipo() const override {
        return "Sensor de Temperatura";
    }
    
    std::string getUnidad() const {
        return unidad;
    }
    
    bool tieneFiebre() const {
        return getMaximo() > 38.0;
    }
};

// SensorHumedad - Sin cambios en la interfaz
class SensorHumedad : public Sensor {
public:
    SensorHumedad(const std::string& id) : Sensor(id) {}
    
    std::string getTipo() const override {
        return "Sensor de Humedad";
    }
    
    std::string getNivelConfort() const {
        double promedio = getPromedio();
        if (promedio < 30) return "Muy seco";
        if (promedio < 40) return "Seco";
        if (promedio < 60) return "Confortable";
        if (promedio < 70) return "Húmedo";
        return "Muy húmedo";
    }
};

// SistemaSensores - Se mantiene igual (usa vector para sensores, list para lecturas)
class SistemaSensores {
private:
    std::vector<std::unique_ptr<Sensor>> sensores;

public:
    void agregarSensor(std::unique_ptr<Sensor> sensor) {
        sensores.push_back(std::move(sensor));
    }
    
    Sensor* buscarSensor(const std::string& id) {
        for (auto& sensor : sensores) {
            if (sensor->getId() == id) {
                return sensor.get();
            }
        }
        return nullptr;
    }
    
    void mostrarTodosLosSensores() const {
        std::cout << "\n=== SISTEMA DE SENSORES ===" << std::endl;
        std::cout << "Total de sensores: " << sensores.size() << std::endl;
        
        for (const auto& sensor : sensores) {
            sensor->mostrarResumen();
            std::cout << std::endl;
        }
    }
    
    bool cargarDesdeCSV(const std::string& nombreArchivo) {
        std::ifstream file(nombreArchivo);
        if (!file.is_open()) return false;

        std::string line;
        std::getline(file, line); // encabezado

        if (sensores.empty()) {
            sensores.push_back(std::make_unique<SensorTemperatura>("TEMP_001"));
            sensores.push_back(std::make_unique<SensorHumedad>("HUM_001"));
        }

        while (std::getline(file, line)) {
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1+1);
            size_t pos3 = line.find(',', pos2+1);

            std::string fecha = line.substr(pos1+1, pos2-pos1-1);
            double temperatura = std::stod(line.substr(pos2+1, pos3-pos2-1));
            double humedad = std::stod(line.substr(pos3+1));

            if (auto sensorTemp = buscarSensor("TEMP_001")) {
                sensorTemp->agregarLectura(temperatura, fecha);
            }

            if (auto sensorHum = buscarSensor("HUM_001")) {
                sensorHum->agregarLectura(humedad, fecha);
            }
        }
        file.close();
        return true;
    }
};

#endif // SENSORES_H