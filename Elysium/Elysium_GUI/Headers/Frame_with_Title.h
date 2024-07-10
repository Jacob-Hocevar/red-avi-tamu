#ifndef Frame_with_Title_H
#define Frame_with_Title_H

#include <QVBoxLayout>
#include <QWidget>
#include <QColor>
#include <QFrame>


class Frame_with_Title : public QVBoxLayout {
    Q_OBJECT
public:
    // Inherits the constructors (default, paramterized, copy, and move) from the base class
    using QVBoxLayout::QVBoxLayout;
    Frame_with_Title(const char* title, QFrame* bottom_widget);
private:
    void set_color(QWidget* widget, const QColor& color);
};

#endif