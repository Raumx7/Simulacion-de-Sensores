#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

bool leerCSV(const std::string& nombreArchivo,
             std::vector<double>& temps,
             std::vector<double>& hums,
             std::vector<std::string>& fechas) {
    std::ifstream file(nombreArchivo);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // encabezado

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

void graficarPorHora(const std::vector<double>& temps,
                     const std::vector<double>& hums,
                     const std::vector<std::string>& fechas) {
    std::vector<int> x(temps.size());
    for (size_t i = 0; i < x.size(); i++) x[i] = i+1;

    std::vector<std::string> horas;
    for (const auto& f : fechas) {
        horas.push_back(f.substr(11,5));
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
    plt::title("Temperatura por hora (°C)");
    plt::plot(x, temps, "r-o");
    plt::xticks(xticks, xtick_labels);

    plt::subplot(2,1,2);
    plt::title("Humedad por hora (%)");
    plt::plot(x, hums, "b-o");
    plt::xticks(xticks, xtick_labels);

    plt::tight_layout();
    plt::show();
}

void graficarOrdenadas(const std::vector<double>& temps,
                       const std::vector<double>& hums,
                       const std::vector<std::string>& fechas) {
    std::vector<int> x(temps.size());
    for (size_t i = 0; i < x.size(); i++) x[i] = i+1;

    std::vector<std::string> horas;
    for (const auto& f : fechas) {
        horas.push_back(f.substr(11,5));
    }

    std::vector<std::pair<double,std::string>> tempsOrd;
    std::vector<std::pair<double,std::string>> humsOrd;
    for (size_t i = 0; i < temps.size(); i++) tempsOrd.push_back({temps[i], horas[i]});
    for (size_t i = 0; i < hums.size(); i++) humsOrd.push_back({hums[i], horas[i]});

    std::sort(tempsOrd.begin(), tempsOrd.end());
    std::sort(humsOrd.begin(), humsOrd.end());

    // --- Graficar ---
    plt::figure();

    // --- Temperaturas ordenadas ---
    plt::subplot(2,1,1);
    plt::title("Temperaturas ordenadas");
    std::vector<double> yT, xT;
    std::vector<std::string> xtick_labels_T;
    for (size_t i = 0; i < tempsOrd.size(); i++) {
        xT.push_back(i+1);
        yT.push_back(tempsOrd[i].first);
        xtick_labels_T.push_back(tempsOrd[i].second);
    }
    plt::plot(xT, yT, "r-");

    // xticks espaciados para horas
    std::vector<int> xticksT;
    std::vector<std::string> xticksLabelT;
    int stepT = std::max(1, (int)xT.size()/8);
    for (size_t i = 0; i < xT.size(); i+=stepT) {
        xticksT.push_back(xT[i]);
        xticksLabelT.push_back(xtick_labels_T[i]);
    }
    plt::xticks(xticksT, xticksLabelT);

    // --- Humedades ordenadas ---
    plt::subplot(2,1,2);
    plt::title("Humedades ordenadas");
    std::vector<double> yH, xH;
    std::vector<std::string> xtick_labels_H;
    for (size_t i = 0; i < humsOrd.size(); i++) {
        xH.push_back(i+1);
        yH.push_back(humsOrd[i].first);
        xtick_labels_H.push_back(humsOrd[i].second);
    }
    plt::plot(xH, yH, "b-");

    // xticks espaciados para horas
    std::vector<int> xticksH;
    std::vector<std::string> xticksLabelH;
    int stepH = std::max(1, (int)xH.size()/8);
    for (size_t i = 0; i < xH.size(); i+=stepH) {
        xticksH.push_back(xH[i]);
        xticksLabelH.push_back(xtick_labels_H[i]);
    }
    plt::xticks(xticksH, xticksLabelH);

    plt::tight_layout();
    plt::show();
}

int main() {
    std::vector<double> temperaturas;
    std::vector<double> humedades;
    std::vector<std::string> fechas;

    if (!leerCSV("datos.csv", temperaturas, humedades, fechas)) {
        std::cerr << "Error: no se pudo abrir datos.csv\n";
        return 1;
    }

    graficarPorHora(temperaturas, humedades, fechas);
    graficarOrdenadas(temperaturas, humedades, fechas);

    return 0;
}