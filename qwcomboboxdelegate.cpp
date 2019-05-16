#include "qwcomboboxdelegate.h"

#include <QComboBox>

QWComboBoxDelegate::QWComboBoxDelegate(QObject *parent):
    QItemDelegate(parent)
{

}

QWidget *QWComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    QComboBox *editor = new QComboBox(parent);
    QStringList BoxItem;
    BoxItem << "uchar" << "char" << "ushort" << "short" << "uint" << "int"
            << "float" << "double" << "ulong" << "long";
//            << "halfword" << "fullword";
    editor->setFrame(false);
    editor->setEditable(false);
    editor->addItems(BoxItem);
    editor->setCurrentIndex(0);

    return editor;
}

void QWComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QItemDelegate::setEditorData(editor, index);
}

void QWComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                  const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    QString str = comboBox->currentText();
    model->setData(index, str, Qt::EditRole);
}

void QWComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
