#ifndef GUI_H
#define GUI_H

// Qt headers first, then the Vole headers (VoleMachine.h contains "using namespace std;",
// which must not appear before Qt headers on Windows).
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QSize>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QFrame>
#include <QScrollArea>
#include <QScrollBar>
#include <QTextEdit>
#include <QLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QKeySequence>
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <algorithm>
#include <array>
#include <cstdint>
#include <string>

#include "VoleMachine.h"

// Little pixel-art vole drawn with QPainter (no image files needed).
class PixelMascot : public QWidget {
public:
    explicit PixelMascot(QWidget* parent = nullptr);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
};

class Gui : public QMainWindow {
    Q_OBJECT

public:
    explicit Gui(QWidget* parent = nullptr);

private slots:
    void onLoad();
    void onEdit();
    void onStep();
    void onRunAll();
    void onFormatChanged(int index);
    void blinkCursor();

private:
    enum CellState { NORMAL = 0, CHANGED = 1, PC_CELL = 2 };

    void buildUi();
    void applyStyle();
    void refresh(bool markChanges);
    void updateScreen();
    void renderConsole();
    void setStatus(const QString& text, bool isError);
    void setReady(bool value);
    void showStepResult(StepResult r);
    void askExitOrLoad();
    QString loadErrorText(LoadStatus s, int badLine) const;
    void setCellState(QLabel* label, int& stored, int state);
    static QString hx(int value, int digits);

    // The simulator itself: same VoleMachine that Menu uses, only its public functions are called.
    VoleMachine machine;
    bool ready;
    bool halted;
    bool finishedAsked;       // the "exit or load another?" question is asked once per run
    bool cursorOn;
    QString screenText;
    QStringList logLines;     // console messages (HTML), newest last

    std::array<uint8_t, 256> prevMem;
    std::array<uint8_t, 16>  prevReg;
    std::array<int, 256>     memState;
    std::array<int, 16>      regState;

    QLabel*      titleLabel;
    QLabel*      pcLabel;
    QLabel*      irLabel;
    QLabel*      haltLabel;
    QTextEdit*   consoleView;
    QTextEdit*   screenView;
    QLabel*      regLabels[16];
    QLabel*      memCells[256];
    QPushButton* loadBtn;
    QPushButton* editBtn;
    QPushButton* stepBtn;
    QPushButton* runBtn;
    QComboBox*   formatBox;
};

#endif