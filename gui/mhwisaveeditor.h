#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QSignalMapper>

#include "../types/mhw_save.h"

#include "inventory/inventoryeditor.h"
#include "limitedunlocks.h"
#include "generalinfo.h"
#include "hunterinfo.h"

constexpr const char* ALL_SAVE = "All Files (*)";
constexpr const char* ENCRYPTED_SAVE = "Encrypted Save (*.raw)";
constexpr const char* UNENCRYPTED_SAVE = "Unencrypted Save(*.bin)";

struct editor_tab {
  SaveLoader* widget;
  SaveLoader** binding;
  QString name;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MHWISaveEditor; }
QT_END_NAMESPACE

class MHWISaveEditor : public QMainWindow, private SaveLoader
{
  Q_OBJECT

public slots:
  void Open();
  void OpenSAVEDATA1000();
  void Save();
  void SaveAs();
  void Dump(int number);

  void SelectSlot(int slot);
  void SwitchSlot(int slot);
  void CloneSlot(int slot);
  
  void OpenLocation(const QString& location);

  void Backup();
  void Restore();
  
  void EditorTabChange(int editorIndex);

public:
  MHWISaveEditor(QWidget* parent = nullptr);
  ~MHWISaveEditor();

  void closeEvent(QCloseEvent* event);

private:
  bool SaveFileEncrypt(const QString& path, mhw_save_raw* save, bool encrypt = true, bool validate = false);
  bool LoadFile(const QString& path, mhw_save_raw** save);
  void SaveFile(const QString& path);

  void LoadSaveSlot();

  // Inherited via SaveLoader
  virtual void LoadFile(const QString& file) override;

  Ui::MHWISaveEditor* ui;
  QLabel* statusFile = nullptr;

  InventoryEditor* inventoryEditor = nullptr;
  HunterInfo* hunterInfo = nullptr;
  LimitedUnlocks* limitedUnlocks = nullptr;
  GeneralInfo* generalInfo = nullptr;
  QList<SaveLoader*> editors;

  QSignalMapper* slotSignalMapper;
  QSignalMapper* switchSignalMapper;
  QSignalMapper* cloneSignalMapper;
  QSignalMapper* openSignalMapper;
  QSignalMapper* dumpSignalMapper;

  QList<QAction*> selectSlotActions;
  QList<QAction*> switchSlotActions;
  QList<QAction*> cloneSlotActions;

  QMap<QString, QString> ext_map;
  QMap<QString, bool> encrypt_map;
  QStringList filters;
};
