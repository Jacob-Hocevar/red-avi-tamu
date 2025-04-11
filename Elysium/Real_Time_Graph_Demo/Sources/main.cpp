#include <QtWidgets/QApplication>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QTimer>
#include <QRandomGenerator>

QT_CHARTS_USE_NAMESPACE

class RealTimeChart : public QChartView {
    Q_OBJECT

public:
RealTimeChart(QWidget *parent = nullptr) : QChartView(parent), time(0) {
    // Create the chart
    chart = new QChart();
    chart->setTitle("Pressures v.s. Time");
    setChart(chart);
    setRenderHint(QPainter::Antialiasing);

    // Create and configure the first line series
    series1 = new QLineSeries();
    series1->setName("Sensor 1");
    series1->setColor(Qt::yellow); // Set color after initialization
    chart->addSeries(series1);

    // Create and configure the second line series
    series2 = new QLineSeries();
    series2->setName("Sensor 2");
    series2->setColor(Qt::red); // Set color after initialization
    chart->addSeries(series2);

    // Create and configure the first line series
    series3 = new QLineSeries();
    series3->setName("Sensor 3");
    series3->setColor(Qt::blue); // Set color after initialization
    chart->addSeries(series3);

    // Create and configure the second line series
    series4 = new QLineSeries();
    series4->setName("Sensor 4");
    series4->setColor(Qt::gray); // Set color after initialization
    chart->addSeries(series4);


    // Create and configure the first line series
    series5 = new QLineSeries();
    series5->setName("Sensor 5");
    series5->setColor(Qt::green); // Set color after initialization
    chart->addSeries(series5);

    // Create and configure the second line series
    series6 = new QLineSeries();
    series6->setName("Sensor 6");
    series6->setColor(Qt::black); // Set color after initialization
    chart->addSeries(series6);

    // Create and configure the X-axis
    axisX = new QValueAxis;
    axisX->setRange(0, 100);
    axisX->setLabelFormat("%d");
    axisX->setTitleText("Time (s)");
    chart->addAxis(axisX, Qt::AlignBottom);
    series1->attachAxis(axisX);
    series2->attachAxis(axisX);
    series3->attachAxis(axisX);
    series4->attachAxis(axisX);
    series5->attachAxis(axisX);
    series6->attachAxis(axisX);

    // Create and configure the Y-axis
    axisY = new QValueAxis;
    axisY->setRange(0, 1000);
    axisY->setTitleText("Pressure Values (psi)");
    chart->addAxis(axisY, Qt::AlignLeft);
    series1->attachAxis(axisY);
    series2->attachAxis(axisY);
    series3->attachAxis(axisY);
    series4->attachAxis(axisY);
    series5->attachAxis(axisY);
    series6->attachAxis(axisY);

    // Set up the timer for real-time updates
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RealTimeChart::updateGraph);
    timer->start(100);
}

private slots:
    void updateGraph() {
        // Generate random data for both series
        qreal y1 = QRandomGenerator::global()->bounded(0, 1000);
        qreal y2 = QRandomGenerator::global()->bounded(0, 1000);
        qreal y3 = QRandomGenerator::global()->bounded(0, 1000);
        qreal y4 = QRandomGenerator::global()->bounded(0, 1000);
        qreal y5 = QRandomGenerator::global()->bounded(0, 1000);
        qreal y6 = QRandomGenerator::global()->bounded(0, 1000);

        // Append data to the series
        series1->append(time, y1);
        series2->append(time, y2);
        series3->append(time, y3);
        series4->append(time, y4);
        series5->append(time, y5);
        series6->append(time, y6);
        time++;

        // Scroll the X-axis if necessary
        if (time > 100) {
            axisX->setRange(time - 100, time);
        }
    }

    private:
    QChart *chart;
    QLineSeries *series1;
    QLineSeries *series2;
    QLineSeries *series3;
    QLineSeries *series4; 
    QLineSeries *series5; 
    QLineSeries *series6; 
    QValueAxis *axisX;
    QValueAxis *axisY;
    QTimer *timer;
    int time;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    RealTimeChart chartView;
    chartView.resize(800, 600);
    chartView.show();
    return app.exec();
}

#include "main.moc"




