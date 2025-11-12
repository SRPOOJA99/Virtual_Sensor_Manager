#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <fstream>
#include <chrono>
#include <thread>
#include <iomanip>

// ---------------- Base Sensor Class ----------------
class Sensor {
public:
    virtual ~Sensor() = default;
    virtual double readValue() const = 0;
    virtual std::string getType() const = 0;
};

// ---------------- Derived Sensor Classes ----------------
class TemperatureSensor : public Sensor {
public:
    double readValue() const override {
        static std::default_random_engine gen;
        static std::uniform_real_distribution<double> dist(20.0, 30.0);
        return dist(gen);
    }
    std::string getType() const override { return "Temperature"; }
};

class PressureSensor : public Sensor {
public:
    double readValue() const override {
        static std::default_random_engine gen;
        static std::uniform_real_distribution<double> dist(0.9, 1.1);
        return dist(gen);
    }
    std::string getType() const override { return "Pressure"; }
};

// ---------------- SensorManager ----------------
class SensorManager {
    std::vector<std::unique_ptr<Sensor>> sensors;
public:
    void addSensor(std::unique_ptr<Sensor> sensor) {
        sensors.push_back(std::move(sensor));
    }

    std::vector<double> readAll() const {
        std::vector<double> values;
        for (const auto& s : sensors)
            values.push_back(s->readValue());
        return values;
    }

    std::vector<std::string> getSensorTypes() const {
        std::vector<std::string> types;
        for (const auto& s : sensors)
            types.push_back(s->getType());
        return types;
    }
};

// ---------------- Helper: Timestamp ----------------
std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_t = std::chrono::system_clock::to_time_t(now);
    std::tm* ptm = std::localtime(&now_t);
    char buffer[32];
    std::strftime(buffer, 32, "%H:%M:%S", ptm);
    return std::string(buffer);
}

// ---------------- Main ----------------
int main() {
    SensorManager manager;
    manager.addSensor(std::make_unique<TemperatureSensor>());
    manager.addSensor(std::make_unique<PressureSensor>());

    std::ofstream logfile("sensor_data.csv");
    logfile << "Time(s),Timestamp,Temperature(C),Pressure(bar)\n";

    double time = 0.0;
    double dt = 1.0; // seconds between readings
    int totalSamples = 20;

    std::cout << "Logging sensor data to sensor_data.csv ...\n";

    for (int i = 0; i < totalSamples; ++i) {
        auto values = manager.readAll();
        auto types = manager.getSensorTypes();

        logfile << std::fixed << std::setprecision(2)
                << time << "," << getTimestamp();

        for (double v : values)
            logfile << "," << v;

        logfile << "\n";

        std::cout << "[" << getTimestamp() << "] ";
        for (size_t j = 0; j < types.size(); ++j)
            std::cout << types[j] << ": " << std::fixed << std::setprecision(2) << values[j] << "  ";
        std::cout << "\n";

        std::this_thread::sleep_for(std::chrono::seconds(1));
        time += dt;
    }

    logfile.close();
    std::cout << "Data logging complete. File saved as sensor_data.csv ✅\n";
    return 0;
}

