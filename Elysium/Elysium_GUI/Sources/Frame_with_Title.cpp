#include "Frame_with_Title.h"
#include <QLabel>
#include <QPalette>
#include <Qt>

// For testing only
#include<iostream>
using std::cout;
using std::endl;

Frame_with_Title::Frame_with_Title(const char* title, QFrame* bottom_widget): QVBoxLayout() {
    // Formatting
    this->setContentsMargins(0, 10, 0, 0);
    this->setSpacing(0);

    // Adds the title to a widget aligned to the left side of the top box
    QLabel* title_w = new QLabel();
    title_w->setText(title);
    title_w->setAlignment(Qt::AlignLeft);
    title_w->setMaximumHeight(15);
    this->addWidget(title_w);

    // Adds a frame around the bottom widget and inserts into the bottom box
    bottom_widget->setFrameShape(QFrame::Box);
    this->addWidget(bottom_widget);


    // Testing color for boundaries
    this->set_color(title_w, Qt::darkMagenta);
    this->set_color(bottom_widget, Qt::green);
}

void Frame_with_Title::set_color(QWidget* widget, const QColor& color) {
    QPalette palette = QPalette();
    palette.setColor(QPalette::Window, color);

    widget->setAutoFillBackground(true);
    widget->setPalette(palette);
}
