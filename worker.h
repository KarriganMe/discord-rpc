#ifndef GUIS_WORKER_H
#define GUIS_WORKER_H

#include <QObject>
#include <QStringList>
#include <QTimer>
#include <string>
#include <vector>
#include <atomic>

// Forward declare to avoid including Python.h in header
struct _object;
typedef _object PyObject;

class worker : public QObject {
    Q_OBJECT
public:
    explicit worker(PyObject* pModule, QObject *parent = nullptr);
    void stop();

public slots:
    void process();
    void updateCID(long long cid);
    void updateManual(QString state, QString description, QString imgURL);
    void updateGameList(QStringList games);
    void checkForGames(); // New slot for timer

private:
    std::wstring GetGame(const std::vector<std::wstring>& games);
    void SendGame(const std::wstring& game);
    void SendManual(const std::string& state, const std::string& details, const std::string& largeImage);
    
    PyObject* m_pModule;
    std::atomic<bool> m_running{true};
    std::vector<std::wstring> m_games;
    std::wstring m_lastGame;
    QTimer* m_timer;
};

#endif //GUIS_WORKER_H
