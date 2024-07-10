#include "Standard_Label.h"
#include <Qt>

Standard_Label::Standard_Label(const char* text, QGridLayout* layout, int row): QLabel() {
    this->setText(text);
    this->setAlignment(Qt::AlignLeft);
    layout->addWidget(this, row, 0);
}
