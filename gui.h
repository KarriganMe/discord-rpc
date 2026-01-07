#ifndef GUIS_GUI_H
#define GUIS_GUI_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <vector>

class gui : public QWidget {
    Q_OBJECT
public:
    explicit gui(QWidget *parent = nullptr);

signals:
    void Update(QString state, QString description, QString imgURL);
    void UpdateCID(long long cid);
    void UpdateGames(QStringList games);

private slots:
    void onUpdate();
    void onGameSelectionChanged();

private:
    QLineEdit *cidInput;
    QLineEdit *stateInput;
    QLineEdit *descInput;
    QLineEdit *imgInput;
    QPushButton *updateBttn;
    
    // Game selection checkboxes
    std::vector<QCheckBox*> gameCheckboxes;
    QGroupBox *gameGroup;
};

#endif //GUIS_GUI_H
