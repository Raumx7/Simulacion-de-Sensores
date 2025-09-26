#ifndef SENSORES_H
#define SENSORES_H

#include <string>
#include <vector>

class Sensor {
protected:
    std::string id;
    std::vector<double> lecturas;  // almacena las lecturas del sensor
public:
    Sensor(const std::string& id) : id(id) {}
    virtual ~Sensor() {}
    virtual void agregarLectura(double valor) {
        lecturas.push_back(valor);
    }
    const std::vector<double>& getLecturas() const { return lecturas; }
    std::string getId() const { return id; }
};

// ===== Sensor de Temperatura =====
class SensorTemperatura : public Sensor {
public:
    SensorTemperatura(const std::string& id) : Sensor(id) {}
};

// ===== Sensor de Humedad =====
class SensorHumedad : public Sensor {
public:
    SensorHumedad(const std::string& id) : Sensor(id) {}
};

#endif // SENSORES_H
