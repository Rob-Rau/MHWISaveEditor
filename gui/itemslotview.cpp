#include "itemslotview.h"
#include "ui_itemslotview.h"

#include <QMessageBox>

ItemSlotView::ItemSlotView(const inventory_area* area, int slot, QWidget* parent)
  : QWidget(parent)
{
  ui = new Ui::ItemSlotView();
  ui->setupUi(this);

  ui->toolButton->setIconSize({ 64, 64 });
  ui->toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  bitmapDB = bitmapDB->GetInstance();
  itemDB = itemDB->GetInstance();

  this->area = area;
  this->invslot = slot;
}

ItemSlotView::~ItemSlotView()
{
  delete ui;
}

void ItemSlotView::Load(mhw_save_raw* mhwSave, int saveslot)
{
  loading = true;

  this->mhwSave = mhwSave;
  this->saveslot = saveslot;

  u8* slot = ((u8*)(&mhwSave->save.section3.Saves[saveslot])) + area->localoffset;
  mhw_item_slot* itemSlot = ((mhw_item_slot*)(slot)+invslot);
  itemInfo* info = itemDB->GetItemById(itemSlot->id);
  if (!info) {
    QMessageBox msgBox;
    msgBox.setText("Invalid item. This is probably an Iceborne item not in the database");
    msgBox.setInformativeText("You may still edit your save, you just won't be able to see the invalid item.");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    return;
  }

  UpdateItemDisplay(info);
  UpdateMaxAmount(info, itemSlot);

  loading = false;
}

void ItemSlotView::UpdateItemDisplay(itemInfo* info)
{
  QIcon* icon = bitmapDB->ItemIcon(info);

  ui->toolButton->setIcon(icon ? *icon : QIcon());
  ui->toolButton->setText(QString::fromUtf8(info->name));
}

void ItemSlotView::UpdateMaxAmount(itemInfo* info, mhw_item_slot* item_slot)
{
  Q_ASSERT(info->id == item_slot->id);
  int max = (area->storage) ? 9999 : info->carry_limit;

  ui->spinBox->setMaximum(max);
  ui->spinBox->setValue(item_slot->amount);
}

void ItemSlotView::AmountChanged(int amount)
{
  if (!mhwSave)
  {
    ui->spinBox->setValue(0);
    return;
  }

  if (!loading) {
    u8* slot = ((u8*)(&mhwSave->save.section3.Saves[saveslot])) + area->localoffset;
    mhw_item_slot* itemSlot = ((mhw_item_slot*)(slot)+invslot);
    mhw_item_slot dummy = { itemSlot->id, itemSlot->amount };

    bool update = amount == 0 || (itemSlot->amount == 0 && amount != 0);
    itemSlot->amount = amount;
    if (amount == 0) dummy.id = 0;

    if (update) {
      itemInfo* dummyInfo = itemDB->GetItemById(dummy.id);
      itemInfo* info = itemDB->GetItemById(itemSlot->id);

      UpdateItemDisplay(dummyInfo);
      UpdateMaxAmount(info, itemSlot);
    }
  }
}