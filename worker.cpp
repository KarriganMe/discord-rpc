#include "worker.h"

// Include Python AFTER Qt headers to avoid slots macro conflict
#undef slots
#include <Python.h>
#define slots Q_SLOTS

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

worker::worker(PyObject* pModule, QObject *parent) :
    QObject(parent), m_pModule(pModule), m_lastGame(L"") {
    // Default games - will be updated by GUI
    m_games = {
        L"League of Legends.exe"
    };
    
    // Create timer for game checking
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &worker::checkForGames);
}

std::wstring worker::GetGame(const std::vector<std::wstring>& games) {
    std::vector<std::wstring> processList;
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return L"";
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            processList.emplace_back(pe32.szExeFile);
        } while (Process32NextW(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);

    for (const std::wstring& game : games) {
        for (const std::wstring& process : processList) {
            if (game == process) {
                return game;
            }
        }
    }
    return L"";
}

void worker::SendGame(const std::wstring& game) {
    PyGILState_STATE gstate = PyGILState_Ensure();
    
    PyObject* pFunc = PyObject_GetAttrString(m_pModule, "send_game");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject* pArgs;
        if (game.empty()) {
            pArgs = PyTuple_Pack(1, Py_None);
        } else {
            std::string gameStr(game.begin(), game.end());
            PyObject* pGame = PyUnicode_FromString(gameStr.c_str());
            pArgs = PyTuple_Pack(1, pGame);
            Py_DECREF(pGame);
        }
        PyObject* pValue = PyObject_CallObject(pFunc, pArgs);
        Py_XDECREF(pValue);
        Py_DECREF(pArgs);
    }
    Py_XDECREF(pFunc);
    
    PyGILState_Release(gstate);
}

void worker::SendManual(const std::string& state, const std::string& details, const std::string& largeImage) {
    std::cout << "[WORKER] SendManual called, acquiring GIL..." << std::endl;
    PyGILState_STATE gstate = PyGILState_Ensure();
    std::cout << "[WORKER] GIL acquired, calling Python man_data..." << std::endl;
    
    PyObject* pFunc = PyObject_GetAttrString(m_pModule, "man_data");
    if (pFunc && PyCallable_Check(pFunc)) {
        std::cout << "[WORKER] man_data function found, preparing arguments..." << std::endl;
        PyObject* pState = PyUnicode_FromString(state.c_str());
        PyObject* pDetails = PyUnicode_FromString(details.c_str());
        PyObject* pLargeImage = PyUnicode_FromString(largeImage.c_str());

        PyObject* pArgs = PyTuple_Pack(3, pState, pDetails, pLargeImage);
        std::cout << "[WORKER] Calling Python function..." << std::endl;
        PyObject* pValue = PyObject_CallObject(pFunc, pArgs);
        
        if (pValue == NULL) {
            std::cout << "[WORKER] ERROR: Python call failed!" << std::endl;
            PyErr_Print();
        } else {
            std::cout << "[WORKER] Python call successful!" << std::endl;
        }
        
        Py_XDECREF(pValue);
        Py_DECREF(pArgs);
        Py_DECREF(pState);
        Py_DECREF(pDetails);
        Py_DECREF(pLargeImage);
    } else {
        std::cout << "[WORKER] ERROR: man_data function not found or not callable!" << std::endl;
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
    }
    Py_XDECREF(pFunc);
    
    std::cout << "[WORKER] Releasing GIL..." << std::endl;
    PyGILState_Release(gstate);
    std::cout << "[WORKER] SendManual complete" << std::endl;
}

void worker::process() {
    std::cout << "[WORKER] Process started, starting timer..." << std::endl;
    m_timer->start(2000); // Check every 2 seconds
}

void worker::checkForGames() {
    std::wstring game = GetGame(m_games);

    if (game != m_lastGame) {
        if (game.empty()) {
            std::cout << "No game running" << std::endl;
        } else {
            std::wcout << L"Game detected: " << game << std::endl;
        }
        SendGame(game);
        m_lastGame = game;
    }
}

void worker::stop() {
    std::cout << "[WORKER] Stopping..." << std::endl;
    m_running = false;
    if (m_timer) {
        m_timer->stop();
    }
}

void worker::updateCID(long long cid) {
    std::cout << "[WORKER] CID update received: " << cid << std::endl;
    
    PyGILState_STATE gstate = PyGILState_Ensure();
    
    PyObject* pFunc = PyObject_GetAttrString(m_pModule, "update_cid");
    if (pFunc && PyCallable_Check(pFunc)) {
        PyObject* pArgs = PyTuple_Pack(1, PyLong_FromLongLong(cid));
        PyObject* pValue = PyObject_CallObject(pFunc, pArgs);
        Py_XDECREF(pValue);
        Py_DECREF(pArgs);
    }
    Py_XDECREF(pFunc);
    
    PyGILState_Release(gstate);
}

void worker::updateManual(QString state, QString description, QString imgURL) {
    std::cout << "[WORKER] Manual update received!" << std::endl;
    std::cout << "[WORKER]   State: " << state.toStdString() << std::endl;
    std::cout << "[WORKER]   Description: " << description.toStdString() << std::endl;
    std::cout << "[WORKER]   Image: " << imgURL.toStdString() << std::endl;
    
    SendManual(
        state.toStdString(),
        description.toStdString(),
        imgURL.toStdString()
    );
}

void worker::updateGameList(QStringList games) {
    m_games.clear();
    
    std::cout << "Updated game list:" << std::endl;
    for (const QString& game : games) {
        std::wstring wGame = game.toStdWString();
        m_games.push_back(wGame);
        std::wcout << L"  - " << wGame << std::endl;
    }
    
    if (m_games.empty()) {
        std::cout << "  (No games selected)" << std::endl;
    }
}
