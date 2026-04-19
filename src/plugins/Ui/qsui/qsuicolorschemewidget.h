#ifndef QSUICOLORSCHEMEWIDGET_H
#define QSUICOLORSCHEMEWIDGET_H

#include <QWidget>
#include "qsuicolorscheme.h"

class QGroupBox;
class QLabel;
class QCheckBox;
class ColorWidget;

class QSUiColorSchemeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QSUiColorSchemeWidget(QWidget *parent = nullptr);

    void loadDefaults(bool darkMode);
    void load(const QSettings *settings, bool darkMode);
    void write(QSettings *settings, bool darkMode);

private:
    void setColorWidgetsEnabled(QSUiColorScheme::QSUiColorRole from, QSUiColorScheme::QSUiColorRole to, bool enabled);

    QGroupBox *m_visualizatioColorsGroupBox;
    QGroupBox *m_playlistColorsGroupBox;
    QGroupBox *m_waveformSeekBarGroupBox;
    QSUiColorScheme m_scheme;
    QHash<QSUiColorScheme::QSUiColorRole, QLabel *> m_colorLabels;
    QHash<QSUiColorScheme::QSUiColorRole, ColorWidget *> m_colorWidges;
    QCheckBox *m_plSystemColorsCheckBox;
    QCheckBox *m_plOverrideGroupColorsCheckBox;
    QCheckBox *m_plOverrideCurrentTrackColorsCheckBox;
};

#endif // QSUICOLORSCHEMEWIDGET_H
