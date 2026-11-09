#include "gui.h"
#include <QStyle>

// ---------- pixel mascot ----------
// Left half of the face (8 columns); the right half is mirrored when drawing.
static const char* const MASCOT_ROWS[13] = {
    "..##....",
    ".#..#...",
    ".#..####",
    "#.......",
    "#..##...",
    "#..##...",
    "#.......",
    "#.....##",
    "#.##...#",
    "#.......",
    ".#......",
    "..#.....",
    "...#####"
};

PixelMascot::PixelMascot(QWidget* parent) : QWidget(parent) {
    setMinimumSize(80, 65);
}

QSize PixelMascot::sizeHint() const {
    return QSize(96, 78);
}

void PixelMascot::paintEvent(QPaintEvent*) {
    QPainter p(this);
    const int cols = 16, rows = 13;
    int cell = std::min(width() / cols, height() / rows);
    if (cell < 1) return;
    int ox = (width() - cell * cols) / 2;
    int oy = (height() - cell * rows) / 2;
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(60, 255, 126));
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < 8; c++) {
            if (MASCOT_ROWS[r][c] == '#') {
                p.drawRect(ox + c * cell, oy + r * cell, cell, cell);
                p.drawRect(ox + (15 - c) * cell, oy + r * cell, cell, cell);
            }
        }
    }
}

// ---------- small helpers ----------

static const int CELL_W = 32;
static const int CELL_H = 19;
static const int ZERO_CELL = 3;

static QString cellStyle(int state) {
    switch (state) {
        case 1:  return "color:#031008; background:#3CFF7E; border:1px solid #3CFF7E;";
        case 2:  return "color:#FFC247; background:#2A1E05; border:1px solid #FFB000;";
        case 3:  return "color:#14683A; background:transparent; border:1px solid transparent;";
        default: return "color:#3CFF7E; background:transparent; border:1px solid transparent;";
    }
}

static QFrame* makePanel(const QString& title, QLayout* inner, bool stretch) {
    QFrame* frame = new QFrame;
    frame->setObjectName("panel");
    QVBoxLayout* lay = new QVBoxLayout(frame);
    QLabel* t = new QLabel(title);
    t->setObjectName("panelTitle");
    lay->addWidget(t);
    lay->addLayout(inner);
    if (stretch) lay->addStretch(1);
    return frame;
}

static void addGlow(QWidget* w, int blur) {
    QGraphicsDropShadowEffect* glow = new QGraphicsDropShadowEffect(w);
    glow->setColor(QColor(60, 255, 126));
    glow->setBlurRadius(blur);
    glow->setOffset(0, 0);
    w->setGraphicsEffect(glow);
}

QString Gui::hx(int value, int digits) {
    return QString("%1").arg(value, digits, 16, QChar('0')).toUpper();
}

// ---------- construction ----------

Gui::Gui(QWidget* parent)
    : QMainWindow(parent), machine(), ready(false), halted(false), finishedAsked(false), cursorOn(true) {
    prevMem.fill(0);
    prevReg.fill(0);
    memState.fill(-1);
    regState.fill(-1);

    buildUi();
    applyStyle();
    machine.setScreenFormat(ScreenFormat::ASCII);
    setReady(false);
    refresh(false);
    setStatus("Welcome to Vole Machine!", false);
    setStatus("Press LOAD to choose a program...", false);

    // Enter = next step
    QShortcut* s1 = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(s1, &QShortcut::activated, this, &Gui::onStep);
    QShortcut* s2 = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    connect(s2, &QShortcut::activated, this, &Gui::onStep);

    QTimer* blink = new QTimer(this);
    connect(blink, &QTimer::timeout, this, &Gui::blinkCursor);
    blink->start(500);

    centralWidget()->layout()->activate();
    QSize natural = sizeHint();
    setMinimumSize(natural);
    resize(natural);
}

void Gui::buildUi() {
    setWindowTitle("Vole Machine Simulator v1.0");

    QWidget* content = new QWidget;
    setCentralWidget(content);
    QVBoxLayout* root = new QVBoxLayout(content);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(8);

    // --- header: mascot, title, PC / IR / HALT ---
    QFrame* header = new QFrame;
    header->setObjectName("panel");
    QHBoxLayout* hl = new QHBoxLayout(header);
    hl->setContentsMargins(16, 10, 16, 10);
    hl->setSpacing(22);
    hl->addWidget(new PixelMascot);

    QVBoxLayout* hv = new QVBoxLayout;
    titleLabel = new QLabel("Vole Machine Simulator v1.0");
    titleLabel->setObjectName("title");
    pcLabel = new QLabel;
    irLabel = new QLabel;
    haltLabel = new QLabel;
    pcLabel->setObjectName("readout");
    irLabel->setObjectName("readout");
    haltLabel->setObjectName("readout");
    hv->addWidget(titleLabel);
    hv->addSpacing(6);
    hv->addWidget(pcLabel);
    hv->addWidget(irLabel);
    hv->addWidget(haltLabel);
    hl->addLayout(hv, 1);

    // --- LCD screen ---
    screenView = new QTextEdit;
    screenView->setObjectName("screen");
    screenView->setReadOnly(true);
    screenView->setFrameShape(QFrame::NoFrame);
    screenView->setFocusPolicy(Qt::NoFocus);
    screenView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    screenView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    screenView->setMinimumWidth(300);
    screenView->setFixedHeight(76);

    QFrame* bezel = new QFrame;
    bezel->setObjectName("lcdBezel");

    QVBoxLayout* bz = new QVBoxLayout(bezel);
    bz->setContentsMargins(8, 8, 8, 8);
    bz->setSpacing(2);

    // Small SCREEN label above the LCD
    QLabel* screenTitle = new QLabel("SCREEN");
    screenTitle->setStyleSheet(
        "color:#1FCC5C; font-size:9px; font-weight:bold;"
    );
    screenTitle->setAlignment(Qt::AlignCenter);
    screenTitle->setFixedHeight(14);
    screenTitle->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    bz->addWidget(screenTitle);
    bz->addWidget(screenView, 1);

    hl->addWidget(bezel, 1);

    addGlow(header, 26);
    root->addWidget(header);

    // --- console messages ---
    consoleView = new QTextEdit;
    consoleView->setObjectName("console");
    consoleView->setReadOnly(true);
    consoleView->setFrameShape(QFrame::NoFrame);
    consoleView->setFocusPolicy(Qt::NoFocus);
    consoleView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    consoleView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    consoleView->setFixedWidth(280);
    consoleView->setMinimumHeight(110);
    consoleView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);

    QVBoxLayout* conBox = new QVBoxLayout;
    conBox->addWidget(consoleView);
    QFrame* consolePanel = makePanel("CONSOLE", conBox, false);

    // --- body: registers | memory ---
    QHBoxLayout* body = new QHBoxLayout;
    body->setSpacing(8);
    root->addLayout(body, 1);

    QGridLayout* regGrid = new QGridLayout;
    regGrid->setSpacing(4);

    for (int i = 0; i < 16; i++) {
        regLabels[i] = new QLabel;
        regLabels[i]->setMinimumSize(72, CELL_H);
        regGrid->addWidget(regLabels[i], i % 8, i / 8);
    }

    QVBoxLayout* left = new QVBoxLayout;
    left->setSpacing(8);
    left->addWidget(consolePanel, 1);
    left->addWidget(makePanel("REGISTERS", regGrid, false), 0);
    body->addLayout(left, 0);

    QGridLayout* memGrid = new QGridLayout;
    memGrid->setSpacing(1);

    for (int c = 0; c < 16; c++) {
        QLabel* h = new QLabel(hx(c, 1));
        h->setObjectName("hdr");
        h->setAlignment(Qt::AlignCenter);
        h->setMinimumSize(CELL_W, CELL_H);
        memGrid->addWidget(h, 0, c + 1);
    }

    for (int r = 0; r < 16; r++) {
        QLabel* h = new QLabel(hx(r * 16, 2));
        h->setObjectName("hdr");
        h->setMinimumSize(28, CELL_H);
        memGrid->addWidget(h, r + 1, 0);

        for (int c = 0; c < 16; c++) {
            QLabel* cell = new QLabel;
            cell->setAlignment(Qt::AlignCenter);
            cell->setMinimumSize(CELL_W, CELL_H);
            cell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            memCells[r * 16 + c] = cell;
            memGrid->addWidget(cell, r + 1, c + 1);
        }
    }

    body->addWidget(makePanel("MEMORY", memGrid, true), 1);

    // --- controls ---
    QHBoxLayout* controls = new QHBoxLayout;
    loadBtn = new QPushButton("LOAD");
    editBtn = new QPushButton("EDIT");
    stepBtn = new QPushButton("STEP");
    runBtn = new QPushButton("RUN ALL");
    QPushButton* helpBtn = new QPushButton("HELP");
    QPushButton* exitBtn = new QPushButton("EXIT");
    exitBtn->setProperty("exitButton", true);
    stepBtn->setToolTip("Run one instruction (Enter)");

    QFrame* keypad = new QFrame;
    keypad->setObjectName("keypad");
    QHBoxLayout* keys = new QHBoxLayout(keypad);
    keys->setContentsMargins(8, 6, 8, 6);
    keys->setSpacing(10);

    for (QPushButton* b : {loadBtn, editBtn, stepBtn, runBtn, helpBtn, exitBtn}) {
        b->setObjectName("key");
        b->setFocusPolicy(Qt::NoFocus);
        b->setCursor(Qt::PointingHandCursor);
        keys->addWidget(b);
    }

    controls->addWidget(keypad);
    controls->addStretch(1);

    QLabel* fmtLabel = new QLabel("SCREEN FORMAT:");
    formatBox = new QComboBox;
    formatBox->addItems(QStringList() << "ASCII" << "HEX" << "DECIMAL");
    formatBox->setFocusPolicy(Qt::NoFocus);

    controls->addWidget(fmtLabel);
    controls->addWidget(formatBox);
    root->addLayout(controls);

    connect(loadBtn, &QPushButton::clicked, this, &Gui::onLoad);
    connect(editBtn, &QPushButton::clicked, this, &Gui::onEdit);
    connect(stepBtn, &QPushButton::clicked, this, &Gui::onStep);
    connect(runBtn, &QPushButton::clicked, this, &Gui::onRunAll);

    connect(helpBtn, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this, "Vole Machine - Help",
            "<h2>Welcome to Vole Machine Simulator!</h2>"
            "<h3>Getting Started</h3>"
            "<p><b>LOAD:</b> Load a Vole program from a file. "
            "You will be asked to enter its starting memory address in hexadecimal.</p>"
            "<h3>Execution Controls</h3>"
            "<p><b>STEP:</b> Execute one instruction at a time.</p>"
            "<p><b>RUN ALL:</b> Execute the program automatically.</p>"
            "<p><b>ENTER:</b> Keyboard shortcut for executing the next instruction.</p>"
            "<h3>Editing</h3>"
            "<p><b>EDIT:</b> Manually overwrite a register or memory cell "
            "using a hexadecimal value.</p>"
            "<h3>Understanding the Interface</h3>"
            "<p><b>PC:</b> Program Counter, indicating the address of the next instruction.</p>"
            "<p><b>IR:</b> Instruction Register, displaying the current instruction.</p>"
            "<p><b>HALT:</b> Indicates whether the program has stopped.</p>"
            "<p><b>REGISTERS:</b> Displays the 16 CPU registers (R0-RF).</p>"
            "<p><b>MEMORY:</b> Displays all 256 memory cells (00-FF).</p>"
            "<p><b>SCREEN:</b> Displays program output.</p>"
            "<p><b>CONSOLE:</b> Displays execution messages and errors.</p>"
            "<h3>Screen Format</h3>"
            "<ul>"
            "<li><b>ASCII:</b> Characters.</li>"
            "<li><b>HEX:</b> Hexadecimal values.</li>"
            "<li><b>DECIMAL:</b> Decimal values.</li>"
            "</ul>"
            "<p>After a program finishes, choose LOAD to load another program "
            "or EXIT to close the simulator.</p>"
        );
    });

    connect(exitBtn, &QPushButton::clicked, this, &QWidget::close);

    connect(formatBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Gui::onFormatChanged);
}

void Gui::applyStyle() {
    setStyleSheet(R"(
        QMainWindow, QDialog, QWidget { background:#020B06; color:#3CFF7E;
            font-family:'Consolas','Courier New',monospace; font-size:13px; }
        QLabel { background:transparent; border:none; }
        QFrame#panel { border:2px solid #1FCC5C; border-radius:12px; background:#03100A; }
        QLabel#panelTitle { color:#1FCC5C; font-weight:bold; padding-bottom:4px; }
        QLabel#title { font-size:22px; font-weight:bold; color:#3CFF7E; }
        QLabel#readout { font-size:18px; color:#3CFF7E; }
        QLabel#hdr { color:#1E9C4A; }

        /* --- CUSTOM CONSOLE STYLE (Matches the Terminal Image) --- */
        QTextEdit#console {
            font-size: 14px;
            color: #E6E6E6;
            background: #1E1E1E;
            border: 1px solid #333333;
            border-radius: 6px;
            padding: 6px;
        }
        QTextEdit#console QScrollBar:vertical {
            background: #1E1E1E;
            width: 12px;
        }
        QTextEdit#console QScrollBar::handle:vertical {
            background: #444444;
            border-radius: 5px;
            min-height: 24px;
        }
        QTextEdit#console QScrollBar::add-line:vertical, QTextEdit#console QScrollBar::sub-line:vertical {
            height: 0;
        }
        /* ---------------------------------------------------------- */

        QFrame#lcdBezel { background:#0B100C; border:3px solid #2E352C; border-radius:12px; }
        QTextEdit#screen { font-size:20px; font-weight:bold; color:#1B2316;
            background:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #D3D9BE, stop:1 #BEC6A4);
            border:2px solid #5C634F; border-radius:4px; padding:4px; }
        QTextEdit#screen QScrollBar:vertical { background:#AEB694; width:10px; }
        QTextEdit#screen QScrollBar::handle:vertical { background:#5C634F; border-radius:4px; min-height:20px; }
        QTextEdit#screen QScrollBar::add-line:vertical, QTextEdit#screen QScrollBar::sub-line:vertical { height:0; }
        QPushButton { background:#03100A; color:#3CFF7E; border:2px solid #1FCC5C;
            border-radius:8px; padding:7px 18px; font-weight:bold; }
        QPushButton:hover { background:#3CFF7E; color:#031008; }
        QPushButton:disabled { color:#14583A; border:2px solid #14583A; }
        QComboBox { background:#03100A; color:#3CFF7E; border:2px solid #1FCC5C;
            border-radius:8px; padding:4px 10px; }
        QComboBox QAbstractItemView { background:#03100A; color:#3CFF7E;
            selection-background-color:#3CFF7E; selection-color:#031008; }
        QLineEdit { background:#020804; color:#3CFF7E; border:1px solid #1FCC5C; }
        QFrame#keypad { background:#121412; border:2px solid #2B2E2B; border-radius:12px; }
        QPushButton#key { color:#EDEDED; font-size:15px; font-weight:bold;
            min-width:92px; min-height:20px; padding:4px 14px;
            background:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #5E615E, stop:0.5 #3C3F3C, stop:1 #2A2C2A);
            border:1px solid #0A0A0A; border-bottom:5px solid #050605; border-radius:8px; }
        QPushButton#key:hover { color:#FFFFFF;
            background:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6E716E, stop:0.5 #484B48, stop:1 #323532); }
        QPushButton#key[choiceHighlight="true"] {
            color:#031008; background:#3CFF7E;
            border:2px solid #B6FFD0; border-bottom:5px solid #1FCC5C;
        }
        QPushButton#key:pressed { background:#262826; border-bottom:2px solid #050605; padding-top:7px; }
        QPushButton#key:disabled { color:#6B6E6B; background:#262826; border-bottom:5px solid #101210; }
        QScrollArea { border:none; background:#020B06; }
        QScrollBar:vertical { background:#03100A; width:12px; }
        QScrollBar::handle:vertical { background:#1FCC5C; border-radius:5px; min-height:24px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }
        QScrollBar:horizontal { background:#03100A; height:12px; }
        QScrollBar::handle:horizontal { background:#1FCC5C; border-radius:5px; min-width:24px; }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width:0; }
    )");
}

// ---------- display ----------

void Gui::setCellState(QLabel* label, int& stored, int state) {
    if (stored == state) return;
    stored = state;
    label->setStyleSheet(cellStyle(state));
}

void Gui::refresh(bool markChanges) {
    int pc = machine.getPC();

    for (int i = 0; i < 16; i++) {
        uint8_t cur = machine.getRegister(i);
        regLabels[i]->setText(QString("R%1: %2").arg(hx(i, 1), hx(cur, 2)));
        bool changed = markChanges && cur != prevReg[i];
        setCellState(regLabels[i], regState[i], changed ? CHANGED : (cur == 0 ? ZERO_CELL : NORMAL));
        prevReg[i] = cur;
    }

    bool loaded = machine.isProgramLoaded();

    for (int i = 0; i < 256; i++) {
        uint8_t cur = machine.getMemoryCell(i);
        memCells[i]->setText(hx(cur, 2));
        bool changed = markChanges && cur != prevMem[i];
        bool atPc = loaded && (i == pc || i == pc + 1);
        int state = changed ? CHANGED : (atPc ? PC_CELL : (cur == 0 ? ZERO_CELL : NORMAL));
        setCellState(memCells[i], memState[i], state);
        prevMem[i] = cur;
    }

    pcLabel->setText("PC   : 0x" + hx(pc, 2));
    irLabel->setText("IR   : 0x" + QString::fromStdString(machine.getIRString()));
    haltLabel->setText(QString("HALT : ") + (halted ? "Yes" : "No"));

    screenText = QString::fromStdString(machine.getScreenString());
    updateScreen();
}

void Gui::updateScreen() {
    QScrollBar* sb = screenView->verticalScrollBar();
    bool atBottom = sb->value() >= sb->maximum() - 2;
    int oldValue = sb->value();
    screenView->setPlainText(screenText + (cursorOn ? QString(QChar(0x2588)) : QString(" ")));
    sb->setValue(atBottom ? sb->maximum() : oldValue);
}

void Gui::renderConsole() {
    QString html = logLines.join("<br>");
    html += cursorOn ? "&nbsp;&#9608;" : "&nbsp;&nbsp;";
    QScrollBar* sb = consoleView->verticalScrollBar();
    bool atBottom = sb->value() >= sb->maximum() - 2;
    int oldValue = sb->value();
    consoleView->setHtml(html);
    sb->setValue(atBottom ? sb->maximum() : oldValue);
}

void Gui::blinkCursor() {
    cursorOn = !cursorOn;
    updateScreen();
    renderConsole();
}

void Gui::setStatus(const QString& text, bool isError) {
    QString line = text.toHtmlEscaped();
    if (isError) {
        // macOS red for errors (matches the 'Cancel X' in the image)
        line = "<span style='color:#FF5F56;'>" + line + "</span>";
    } else {
        // White/light gray for standard text (matches the prompt text)
        line = "<span style='color:#E6E6E6;'>" + line + "</span>";
    }
    logLines.append(line);
    while (logLines.size() > 200) logLines.removeFirst();
    renderConsole();
}

void Gui::setReady(bool value) {
    ready = value;
}

// ---------- messages ----------

QString Gui::loadErrorText(LoadStatus s, int badLine) const {
    switch (s) {
        case LoadStatus::FILE_NOT_FOUND:       return "Error: File not found.";
        case LoadStatus::ODD_ADDRESS:          return "Error: Odd address encountered.";
        case LoadStatus::ADDRESS_OUT_OF_RANGE: return "Error: Address out of range.";
        case LoadStatus::PROGRAM_TOO_LARGE:    return "Error: Program too large to fit in memory.";
        case LoadStatus::INVALID_INSTRUCTION_IN_FILE:
            return QString("Error: Invalid instruction in file at line %1.").arg(badLine);
        default: return "Unknown error occurred while loading program.";
    }
}

void Gui::showStepResult(StepResult r) {
    halted = (r == StepResult::STEP_HALTED);
    haltLabel->setText(QString("HALT : ") + (halted ? "Yes" : "No"));

    switch (r) {
        case StepResult::STEP_OK:
            setStatus("Step executed. Press Enter for the next step.", false);
            break;
        case StepResult::STEP_HALTED:
            setStatus("Program halted.", false);
            break;
        case StepResult::STEP_INVALID_INSTRUCTION:
            setStatus("Error: Invalid instruction encountered.", true);
            break;
        case StepResult::STEP_PC_OUT_OF_RANGE:
            setStatus("Error: Program counter out of range.", true);
            break;
        case StepResult::STEP_NOT_LOADED:
            setStatus("Error: No program loaded.", true);
            break;
        case StepResult::STEP_LIMIT_REACHED:
            setStatus("Error: Step limit reached.", true);
            break;
    }

    if (r != StepResult::STEP_OK && r != StepResult::STEP_NOT_LOADED && !finishedAsked) {
        finishedAsked = true;
        QTimer::singleShot(0, this, [this]() { askExitOrLoad(); });
    }
}

// ---------- actions ----------

void Gui::onLoad() {
    QString file = QFileDialog::getOpenFileName(this, "Load Vole program", QString(),
                                                "Vole programs (*.vole *.txt);;All files (*)");
    if (file.isEmpty()) return;

    bool ok = false;
    QString addrText = QInputDialog::getText(this, "Start address",
                                             "Start address (hex, even, e.g. 00):",
                                             QLineEdit::Normal, "00", &ok);
    if (!ok) return;

    int addr = addrText.trimmed().toInt(&ok, 16);
    if (!ok) {
        setReady(false);
        setStatus("Error: invalid hex address.", true);
        return;
    }

    int badLine = 0;
    LoadStatus st = machine.loadProgram(file.toStdString(), addr, badLine);

    if (st != LoadStatus::LOAD_OK) {
        setReady(false);
        setStatus(loadErrorText(st, badLine), true);
        return;
    }

    halted = false;
    finishedAsked = false;

    loadBtn->setProperty("choiceHighlight", false);
    loadBtn->style()->unpolish(loadBtn);
    loadBtn->style()->polish(loadBtn);
    loadBtn->update();

    for (QPushButton* b : findChildren<QPushButton*>()) {
        if (b->property("exitButton").toBool()) {
            b->setProperty("choiceHighlight", false);
            b->style()->unpolish(b);
            b->style()->polish(b);
            b->update();
            break;
        }
    }

    setReady(true);
    refresh(false);
    setStatus("Program loaded at " + hx(addr, 2) + ".", false);
    setStatus("Press Enter or STEP to step, or RUN ALL...", false);
}

void Gui::onEdit() {
    bool ok = false;
    QString kind = QInputDialog::getItem(this, "Edit", "Overwrite:",
                                         QStringList() << "Register" << "Memory cell", 0, false, &ok);
    if (!ok) return;

    bool isReg = (kind == "Register");

    QString idxText = QInputDialog::getText(this, "Edit",
        isReg ? "Register index (hex 0-F):" : "Memory address (hex 00-FF):",
        QLineEdit::Normal, "", &ok);
    if (!ok) return;

    int idx = idxText.trimmed().toInt(&ok, 16);
    if (!ok || idx < 0) {
        setStatus("Error: invalid hex index.", true);
        return;
    }

    QString valText = QInputDialog::getText(this, "Edit", "New value (hex 00-FF):",
                                            QLineEdit::Normal, "", &ok);
    if (!ok) return;

    int val = valText.trimmed().toInt(&ok, 16);
    if (!ok || val < 0) {
        setStatus("Error: invalid hex value.", true);
        return;
    }

    bool done = isReg ? machine.setRegister(idx, val) : machine.setMemoryCell(idx, val);

    if (!done) {
        setStatus(isReg ? "Error: Register index out of range." : "Error: Address out of range.", true);
        return;
    }

    refresh(true);
    setStatus(QString(isReg ? "Register R%1" : "Memory at %1").arg(hx(idx, isReg ? 1 : 2))
              + " overwritten with " + hx(val & 0xFF, 2) + ".", false);
}

void Gui::askExitOrLoad() {
    setStatus("Program finished. Choose LOAD to load another program or EXIT to close the simulator.", false);

    // Highlight LOAD and EXIT without disabling any controls.
    loadBtn->setProperty("choiceHighlight", true);
    loadBtn->style()->unpolish(loadBtn);
    loadBtn->style()->polish(loadBtn);
    loadBtn->update();

    for (QPushButton* b : findChildren<QPushButton*>()) {
        if (b->property("exitButton").toBool()) {
            b->setProperty("choiceHighlight", true);
            b->style()->unpolish(b);
            b->style()->polish(b);
            b->update();
            break;
        }
    }
}

void Gui::onStep() {
    if (!ready) return;
    StepResult r = machine.step();
    refresh(true);
    showStepResult(r);
}

void Gui::onRunAll() {
    if (!ready) return;
    StepResult r = machine.runAll(Constants::MAX_STEPS);
    refresh(true);
    showStepResult(r);
}

void Gui::onFormatChanged(int index) {
    ScreenFormat f = index == 1 ? ScreenFormat::HEX
                   : index == 2 ? ScreenFormat::DECIMAL
                                : ScreenFormat::ASCII;

    machine.setScreenFormat(f);
    screenText = QString::fromStdString(machine.getScreenString());
    updateScreen();
}

// ---------- entry point ----------

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    Gui window;
    window.show();
    return app.exec();
}
