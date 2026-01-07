#include <QApplication>
#include <QThread>
#include <iostream>
#include <filesystem>

// Include Python BEFORE other headers to avoid macro conflicts
#undef slots
#include <Python.h>
#define slots Q_SLOTS

#include "gui.h"
#include "worker.h"

int main(int argc, char *argv[]) {
    // Initialize Python with threading support
    Py_Initialize();
    PyEval_InitThreads();
    
    // Get the executable's directory
    std::filesystem::path exePath = std::filesystem::absolute(argv[0]);
    std::filesystem::path exeDir = exePath.parent_path();
    std::filesystem::path projectDir = exeDir.parent_path();

    PyRun_SimpleString("import sys");
    std::string addProjectPath = "sys.path.insert(0, r'" + projectDir.string() + "')";
    PyRun_SimpleString(addProjectPath.c_str());
    
    // For production: add the executable's directory too
    std::string addExePath = "sys.path.insert(0, r'" + exeDir.string() + "')";
    PyRun_SimpleString(addExePath.c_str());
    
    PyRun_SimpleString("sys.path.insert(0, r'C:\\Users\\kerrigan\\Documents\\Projects\\discord-rpc\\.venv\\Lib\\site-packages')");

    PyObject* pModule = PyImport_ImportModule("module");
    if (pModule == nullptr) {
        std::cerr << "Error: Could not load Discord RPC module" << std::endl;
        PyErr_Print();
        Py_Finalize();
        return -1;
    }
    
    // Release GIL before starting threads
    PyThreadState* mainThreadState = PyEval_SaveThread();
    
    // Start Python monitoring threads
    PyEval_RestoreThread(mainThreadState);
    PyObject* pStartFunc = PyObject_GetAttrString(pModule, "start_monitoring");
    if (pStartFunc && PyCallable_Check(pStartFunc)) {
        PyObject* pValue = PyObject_CallObject(pStartFunc, NULL);
        Py_XDECREF(pValue);
    }
    Py_XDECREF(pStartFunc);
    mainThreadState = PyEval_SaveThread();

    // Create Qt application
    QApplication app(argc, argv);
    app.setApplicationName("Discord RPC Controller");
    app.setOrganizationName("YourName");

    // Create GUI
    gui mainWindow;
    mainWindow.show();

    // Create worker thread
    QThread* workerThread = new QThread();
    worker* gameWorker = new worker(pModule);
    gameWorker->moveToThread(workerThread);

    // Connect signals
    QObject::connect(workerThread, &QThread::started, gameWorker, &worker::process);
    QObject::connect(&mainWindow, &gui::Update, gameWorker, &worker::updateManual, Qt::QueuedConnection);
    QObject::connect(&mainWindow, &gui::UpdateCID, gameWorker, &worker::updateCID, Qt::QueuedConnection);
    QObject::connect(&mainWindow, &gui::UpdateGames, gameWorker, &worker::updateGameList, Qt::QueuedConnection);
    
    // Start worker thread
    workerThread->start();

    int result = app.exec();

    // Cleanup
    gameWorker->stop();
    workerThread->quit();
    workerThread->wait();
    delete gameWorker;
    delete workerThread;

    PyEval_RestoreThread(mainThreadState);
    Py_DECREF(pModule);
    Py_Finalize();

    return result;
}
