#include "mhwisaveeditor.h"
#include "ui_mhwisaveeditor.h"

#include <QFileDialog>
#include <QByteArray>

#include "crypto/iceborne_crypt.h"
#include "utility/paths.h"

MHWISaveEditor::MHWISaveEditor(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MHWISaveEditor)
{
  ui->setupUi(this);

  slotSignalMapper = new QSignalMapper(this);
  for (int i = 0; i < 3; ++i) {
    QAction* slotAction = new QAction("Slot " + QString::number(i + 1));
    connect(slotAction, SIGNAL(triggered()), slotSignalMapper, SLOT(map()));
    slotSignalMapper->setMapping(slotAction, i);
    ui->menuSlot->addAction(slotAction);
  }

  connect(slotSignalMapper, SIGNAL(mappedInt(int)),
    this, SLOT(Slot(int)));

  mhwRaw = nullptr;
}

MHWISaveEditor::~MHWISaveEditor()
{
  delete ui;

  free(mhwRaw);
  mhwRaw = nullptr;
}

void MHWISaveEditor::Open()
{
  qInfo("Hello from Open");

  QString path = GetDefaultSavePath();
  QString filepath = QString();

  QFileDialog dialog(this);
  dialog.setDirectory(path);

  if (dialog.exec()) {
    QStringList files = dialog.selectedFiles();
    filepath = files[0];
    qInfo("%s", qUtf8Printable(filepath));

    QFile file(filepath, this);
    if (!file.open(QIODevice::ReadOnly)) {
      qWarning("File: %s, cannot be read.", qUtf8Printable(filepath));
      return;
    }

    QByteArray saveBlob = file.readAll();
    if (saveBlob.length() != sizeof(MHWSaveRaw)) {
      qWarning("File: %s, cannot be read.", qUtf8Printable(filepath));
      return;
    }
    file.close();

    if (mhwRaw) free(mhwRaw);
    mhwRaw = (MHWSaveRaw*)malloc(sizeof(MHWSaveRaw));
    if (!mhwRaw) {
      qWarning("Error allocating memory.");
      return;
    };

    memcpy(mhwRaw->data, saveBlob.constData(), saveBlob.length());
    if (!IsBlowfishDecrypted(&mhwRaw->save)) {
      DecryptSave(mhwRaw->data, sizeof(MHWSaveRaw));
    }
  }
}

void MHWISaveEditor::Save()
{
  qInfo("Hello from Save");
}

void MHWISaveEditor::Slot(int slot)
{
  qInfo("Selected slot index %d.", slot);
}
