#include "gui.h"
#include <QFormLayout>
#include <QLabel>
#include <iostream>

gui::gui(QWidget *parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // ========== GAME SELECTION SECTION ==========
    gameGroup = new QGroupBox("Games to Monitor", this);
    QVBoxLayout* gameLayout = new QVBoxLayout(gameGroup);
    
    // List of available games
    QStringList availableGames = {
        "League of Legends.exe",
        "Dota2.exe",
        "RocketLeague.exe",
        "valorant.exe",
        "cs2.exe",
        "Overwatch.exe"
    };
    
    // Create checkbox for each game
    for (const QString& game : availableGames) {
        QCheckBox* checkbox = new QCheckBox(game, gameGroup);
        
        // Check League of Legends by default
        if (game == "League of Legends.exe") {
            checkbox->setChecked(true);
        }
        
        connect(checkbox, &QCheckBox::checkStateChanged, this, &gui::onGameSelectionChanged);
        gameCheckboxes.push_back(checkbox);
        gameLayout->addWidget(checkbox);
    }
    
    mainLayout->addWidget(gameGroup);
    
    // ========== DISCORD RPC SECTION ==========
    QGroupBox* rpcGroup = new QGroupBox("Discord RPC Settings", this);
    QFormLayout* fLayout = new QFormLayout(rpcGroup);

    // Initialize inputs
    cidInput = new QLineEdit(this);
    stateInput = new QLineEdit(this);
    descInput = new QLineEdit(this);
    imgInput = new QLineEdit(this);
    updateBttn = new QPushButton("Update Status", this);

    // Set placeholders - NO DEFAULT CID!
    cidInput->setPlaceholderText("Enter your Discord App Client ID");
    stateInput->setPlaceholderText("State (e.g., Playing League)");
    descInput->setPlaceholderText("Details (e.g., In game)");
    imgInput->setPlaceholderText("Image URL (optional)");

    // Add to form
    fLayout->addRow("Client ID:", cidInput);
    fLayout->addRow("State:", stateInput);
    fLayout->addRow("Description:", descInput);
    fLayout->addRow("Image URL:", imgInput);
    
    mainLayout->addWidget(rpcGroup);
    mainLayout->addWidget(updateBttn);
    
    // Add stretch to push everything to top
    mainLayout->addStretch();

    // Connect button
    connect(updateBttn, &QPushButton::clicked, this, &gui::onUpdate);

    setWindowTitle("Discord RPC Controller");
    resize(450, 500);
    
    // Send initial game list
    onGameSelectionChanged();
}

void gui::onUpdate() {
    // Emit CID update
    bool ok;
    long long cid = cidInput->text().toLongLong(&ok);
    if (ok && cid > 0) {
        emit UpdateCID(cid);
    }
    
    // Emit manual data update
    emit Update(
        stateInput->text(),
        descInput->text(),
        imgInput->text()
    );
}

void gui::onGameSelectionChanged() {
    QStringList selectedGames;
    
    // Collect all checked games
    for (QCheckBox* checkbox : gameCheckboxes) {
        if (checkbox->isChecked()) {
            selectedGames.append(checkbox->text());
        }
    }
    
    // Emit the updated game list
    emit UpdateGames(selectedGames);
}
