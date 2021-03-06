#ifndef SIMULATIONTHREADPOOL_H
#define SIMULATIONTHREADPOOL_H

#include <QObject>
#include <QVector>

class EquipmentDb;
class Random;
class SimSettings;
class NumberCruncher;
class QThread;

class SimulationThreadPool: public QObject {
    Q_OBJECT
public:
    SimulationThreadPool(EquipmentDb* equipment_db, SimSettings* sim_settings, NumberCruncher* scaler, QObject* parent = nullptr);
    ~SimulationThreadPool();

    void run_sim(const QString& setup_string, bool full_sim, int iterations);

    bool sim_running() const;
    void scale_number_of_threads();

public slots:
    void error_string(const QString&, const QString&);
    void thread_finished();

signals:
    void threads_finished();
    void start_simulation(const unsigned thread_id, QString setup_string, bool full_sim, int iterations);

protected:
private:
    EquipmentDb* equipment_db;
    Random* random;
    SimSettings* sim_settings;
    NumberCruncher* scaler;
    int running_threads;

    QVector<QPair<unsigned, QThread*>> thread_pool;
    QVector<unsigned> active_thread_ids;
    QVector<unsigned> inactive_thread_ids;

    void setup_thread(const unsigned thread_id);

    void add_threads(const int);
    void remove_threads(const int);
};

#endif // SIMULATIONTHREADPOOL_H
