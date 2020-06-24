#ifndef OSCILLOSCOPE_WIDGET_H
#define OSCILLOSCOPE_WIDGET_H

#include <QWidget>
#include <QColor>
#include <QTimer>

#define MAX_PLANES 2
#define MAX_X_POINTS 4096

enum TRIGGER_TYP{TRIGGER_OFF, RISING_EDGE, FALLING_EDGE};

namespace Ui {
class OscilloscopeWidget;
}

class OscilloscopeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OscilloscopeWidget(QWidget *parent = nullptr);
    ~OscilloscopeWidget();

    void SetSamplerate(float samplerate);
    void SetBackgroundColor(QColor color);
    void SetLineColor(QColor color);
    void SetTriggerLineColor(QColor color);
    void SetRasterColor(QColor color);
    void SetVerticalPosition(float value);      // 0.0f is top 1.0 is bottom
    void SetAmplifire(float value);
    void SetTriggerTyp(int value);
    void SetTriggerLevel(float value);

    void paintEvent(QPaintEvent *);
    void DrawRaster(QPainter &painter, int width, int height);
    void DrawTriggerLevel(QPainter &painter, int width, int height);
    void DrawData(QPainter &painter, int width);
    void DrawInfos(QPainter &painter, int width, int height);
    void NextAudioData(float *data, int length);

public slots:

    void OnRefresh();


private:
    Ui::OscilloscopeWidget *ui;
    QColor CalcFadeColor(QColor src_color, QColor dst_color, float fade);   // fade 0.0 = 100% scr_color | 1.0 = 100% dst_color

    float samplerate;

    float vertical_position;
    float horizontal_position;
    float amplifire;

    QColor background_color;
    QColor line_color;
    QColor raster_color;
    QColor trigger_line_color;

    QPen *output_line_pen;
    int output_plane_xpos;

    int ringbuffer[0x10000];
    int ringbuffer_pos_w;
    int ringbuffer_pos_r;
    int tmp_output[0x10000];    // 64Kb

    bool trigger;
    int trigger_typ;
    float trigger_level;

    float old_value;
    float current_value;

    uint32_t trigger_sample_counter;    // Zum ermitteln der Frequenz
    uint32_t sample_counter_period;     // Anzahl der Sample pro Periode (zwischen 2 Trigger!!)


    QTimer *timer1;
    bool refresh;
};

#endif // OSCILLOSCOPE_WIDGET_H
