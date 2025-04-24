/***************************************************************************
 *   Copyright (C) 2011-2025 by Ilya Kotov                                 *
 *   forkotov02@ya.ru                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QSettings>
#include <QDir>
#include <QFontDialog>
#include <QTabBar>
#include <qmmp/qmmp.h>
#include <qmmpui/filedialog.h>
#include <qmmpui/uihelper.h>
#include <qmmpui/metadataformattermenu.h>
#include <qmmpui/shortcutdialog.h>
#include "qsuiactionmanager.h"
#include "qsuishortcutitem.h"
#include "qsuipopupsettings.h"
#include "qsuitoolbareditor.h"
#include "ui_qsuisettings.h"
#include "qsuisettings.h"

QSUiSettings::QSUiSettings(QWidget *parent) : QWidget(parent), m_ui(new Ui::QSUISettings)
{
    m_ui->setupUi(this);
    //icon sizes
    m_ui->toolBarIconSizeComboBox->addItem(tr("Default"), -1);
    m_ui->toolBarIconSizeComboBox->addItem(tr("16x16"), 16);
    m_ui->toolBarIconSizeComboBox->addItem(tr("22x22"), 22);
    m_ui->toolBarIconSizeComboBox->addItem(tr("32x32"), 32);
    m_ui->toolBarIconSizeComboBox->addItem(tr("48x48"), 48);
    m_ui->toolBarIconSizeComboBox->addItem(tr("64x64"), 64);
    //other settings
    m_ui->tabPositionComboBox->addItem(tr("Top"), QTabBar::RoundedNorth);
    m_ui->tabPositionComboBox->addItem(tr("Bottom"), QTabBar::RoundedSouth);
    m_ui->tabPositionComboBox->addItem(tr("Left"), QTabBar::RoundedWest);
    m_ui->tabPositionComboBox->addItem(tr("Right"),  QTabBar::RoundedEast);
    m_ui->wfsbProgressBarColor->setOptions(QColorDialog::ShowAlphaChannel);
    //connections
    connect(m_ui->plFontButton, &QToolButton::clicked, this, [this] { selectFont(m_ui->plFontLabel); });
    connect(m_ui->groupFontButton, &QToolButton::clicked, this, [this] { selectFont(m_ui->groupFontLabel); });
    connect(m_ui->extraRowFontButton, &QToolButton::clicked, this, [this] { selectFont(m_ui->extraRowFontLabel); });
    connect(m_ui->columnFontButton, &QToolButton::clicked, this, [this] { selectFont(m_ui->columnFontLabel); });
    connect(m_ui->tabsFontButton, &QToolButton::clicked, this, [this] { selectFont(m_ui->tabsFontLabel); });
    //load settings
    readSettings();
    loadFonts();
    createActions();
}

QSUiSettings::~QSUiSettings()
{
    delete m_ui;
}

void QSUiSettings::showEvent(QShowEvent *)
{
    m_ui->hiddenCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
    m_ui->hideOnCloseCheckBox->setEnabled(UiHelper::instance()->visibilityControl());
}

void QSUiSettings::selectFont(QLabel *label)
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, label->font(), this);
    if(ok)
    {
        label->setText(font.family() + QChar::Space + QString::number(font.pointSize()));
        label->setFont(font);
    }
}

void QSUiSettings::setFont(QLabel *label, const QString &fontName)
{
    QFont font;
    font.fromString(fontName);
    label->setText(font.family() + QChar::Space + QString::number(font.pointSize()));
    label->setFont(font);
}

void QSUiSettings::loadFonts()
{
    QFont extraRowDefaultFont = qApp->font("QAbstractItemView");
    extraRowDefaultFont.setPointSize(extraRowDefaultFont.pointSize() - 1);
    extraRowDefaultFont.setStyle(QFont::StyleItalic);

    QSettings settings;
    settings.beginGroup(u"Simple"_s);
    m_ui->systemFontsCheckBox->setChecked(settings.value(u"use_system_fonts"_s, true).toBool());
    setFont(m_ui->plFontLabel, settings.value(u"pl_font"_s, qApp->font("QAbstractItemView").toString()).toString());
    setFont(m_ui->groupFontLabel, settings.value(u"pl_group_font"_s, qApp->font("QAbstractItemView").toString()).toString());
    setFont(m_ui->extraRowFontLabel, settings.value(u"pl_extra_row_font"_s, extraRowDefaultFont.toString()).toString());
    setFont(m_ui->tabsFontLabel, settings.value(u"pl_tabs_font"_s, qApp->font("QTabWidget").toString()).toString());
    setFont(m_ui->columnFontLabel, settings.value(u"pl_header_font"_s, qApp->font("QAbstractItemView").toString()).toString());
    settings.endGroup();
}

void QSUiSettings::createActions()
{
    MetaDataFormatterMenu *menu = new MetaDataFormatterMenu(MetaDataFormatterMenu::TITLE_MENU, this);
    m_ui->windowTitleButton->setMenu(menu);
    m_ui->windowTitleButton->setPopupMode(QToolButton::InstantPopup);
    connect(menu, &MetaDataFormatterMenu::patternSelected, this, &QSUiSettings::addWindowTitleString);
}

void QSUiSettings::on_popupTemplateButton_clicked()
{
    QSUiPopupSettings *p = new QSUiPopupSettings(this);
    p->exec();
    p->deleteLater();
}

void QSUiSettings::on_customizeToolBarButton_clicked()
{
    QSUiToolBarEditor editor(this);
    editor.exec();
}

void QSUiSettings::on_resetFontsButton_clicked()
{
    QSettings settings;
    settings.remove(u"Simple/pl_font"_s);
    settings.remove(u"Simple/pl_group_font"_s);
    settings.remove(u"Simple/pl_extra_row_font"_s);
    settings.remove(u"Simple/pl_tabs_font"_s);
    settings.remove(u"Simple/pl_header_font"_s);
    loadFonts();
}

void QSUiSettings::on_resetColorsButton_clicked()
{
    m_ui->vColor1->setColor(u"#BECBFF"_s);
    m_ui->vColor2->setColor(u"#BECBFF"_s);
    m_ui->vColor3->setColor(u"#BECBFF"_s);
    m_ui->peaksColor->setColor(u"#DDDDDD"_s);
    m_ui->bgColor->setColor(u"Black"_s);
}

void QSUiSettings::readSettings()
{
    QSettings settings;
    settings.beginGroup(u"Simple"_s);
    //playlist
    m_ui->smoothScrollingCheckBox->setChecked(settings.value(u"pl_smooth_scrolling"_s, true).toBool());
    m_ui->protocolCheckBox->setChecked(settings.value(u"pl_show_protocol"_s, false).toBool());
    m_ui->numbersCheckBox->setChecked(settings.value(u"pl_show_numbers"_s, true).toBool());
    m_ui->lengthsCheckBox->setChecked(settings.value(u"pl_show_lengths"_s, true).toBool());
    m_ui->alignCheckBox->setChecked(settings.value(u"pl_align_numbers"_s, false).toBool());
    m_ui->anchorCheckBox->setChecked(settings.value(u"pl_show_anchor"_s, false).toBool());
    m_ui->showSplittersCheckBox->setChecked(settings.value(u"pl_show_splitters"_s, true).toBool());
    m_ui->popupCheckBox->setChecked(settings.value(u"pl_show_popup"_s, false).toBool());
    //tabs
    m_ui->tabsClosableCheckBox->setChecked(settings.value(u"pl_tabs_closable"_s, false).toBool());
    m_ui->showNewPLCheckBox->setChecked(settings.value(u"pl_show_new_pl_button"_s, false).toBool());
    m_ui->showTabListMenuCheckBox->setChecked(settings.value(u"pl_show_tab_list_menu"_s, false).toBool());
    int index = m_ui->tabPositionComboBox->findData(settings.value(u"pl_tab_position"_s, QTabBar::RoundedNorth).toInt());
    m_ui->tabPositionComboBox->setCurrentIndex(index);
    //view
    m_ui->hiddenCheckBox->setChecked(settings.value(u"start_hidden"_s, false).toBool());
    m_ui->hideOnCloseCheckBox->setChecked(settings.value(u"hide_on_close"_s, false).toBool());
    m_ui->windowTitleLineEdit->setText(settings.value(u"window_title_format"_s, u"%if(%p,%p - %t,%t)"_s).toString());
    //visualization colors
    m_ui->vColor1->setColor(settings.value(u"vis_color1"_s, u"#BECBFF"_s).toString());
    m_ui->vColor2->setColor(settings.value(u"vis_color2"_s, u"#BECBFF"_s).toString());
    m_ui->vColor3->setColor(settings.value(u"vis_color3"_s, u"#BECBFF"_s).toString());
    m_ui->peaksColor->setColor(settings.value(u"vis_peak_color"_s, u"#DDDDDD"_s).toString());
    m_ui->bgColor->setColor(settings.value(u"vis_bg_color"_s, u"Black"_s).toString());
    //playlist colors
    QString normal_bg = palette().color(QPalette::Base).name();
    QString alternate = palette().color(QPalette::AlternateBase).name();
    QString selected_bg = palette().color(QPalette::Highlight).name();
    QString normal = palette().color(QPalette::Text).name();
    QString current = palette().color(QPalette::Text).name();
    QString highlighted = palette().color(QPalette::HighlightedText).name();
    QString group_text = palette().color(QPalette::Text).name();
    m_ui->plSystemColorsCheckBox->setChecked(settings.value(u"pl_system_colors"_s, true).toBool());
    m_ui->plBg1Color->setColor(settings.value(u"pl_bg1_color"_s, normal_bg).toString());
    m_ui->plBg2Color->setColor(settings.value(u"pl_bg2_color"_s, alternate).toString());
    m_ui->plHlColor->setColor(settings.value(u"pl_highlight_color"_s, selected_bg).toString());
    m_ui->plTextNormalColor->setColor(settings.value(u"pl_normal_text_color"_s, normal).toString());
    m_ui->plTextCurrentColor->setColor(settings.value(u"pl_current_text_color"_s, current).toString());
    m_ui->plTextHlCurrentColor->setColor(settings.value(u"pl_hl_text_color"_s, highlighted).toString());
    m_ui->plGrBgColor->setColor(settings.value(u"pl_group_bg"_s, normal_bg).toString());
    m_ui->plSplitterColor->setColor(settings.value(u"pl_splitter_color"_s, normal).toString());
    m_ui->plGrTextColor->setColor(settings.value(u"pl_group_text"_s, group_text).toString());
    m_ui->plCurrentTrackBgColor->setColor(settings.value(u"pl_current_bg_color"_s, normal_bg).toString());
    m_ui->plOverrideGroupColorsCheckBox->setChecked(settings.value(u"pl_override_group_colors"_s, false).toBool());
    m_ui->plOverrideCurrentTrackColorsCheckBox->setChecked(settings.value(u"pl_override_current_track_colors"_s, false).toBool());
    //waveform seekbar colors
    m_ui->wfsbBgColor->setColor(settings.value(u"wfsb_bg_color"_s, u"Black"_s).toString());
    m_ui->wfsbRmsColor->setColor(settings.value(u"wfsb_rms_color"_s, u"#DDDDDD"_s).toString());
    m_ui->wfsbWaveFormColor->setColor(settings.value(u"wfsb_waveform_color"_s, u"#BECBFF"_s).toString());
    m_ui->wfsbProgressBarColor->setColor(settings.value(u"wfsb_progressbar_color"_s, u"#9633CA10"_s).toString());
    //toolbar
    index = m_ui->toolBarIconSizeComboBox->findData(settings.value(u"toolbar_icon_size"_s, -1).toInt());
    m_ui->toolBarIconSizeComboBox->setCurrentIndex(index > 0 ? index : 0);
    settings.endGroup();
}

void QSUiSettings::writeSettings()
{
    QSettings settings;
    settings.beginGroup(u"Simple"_s);
    settings.setValue(u"pl_smooth_scrolling"_s, m_ui->smoothScrollingCheckBox->isChecked());
    settings.setValue(u"pl_show_protocol"_s, m_ui->protocolCheckBox->isChecked());
    settings.setValue(u"pl_show_numbers"_s, m_ui->numbersCheckBox->isChecked());
    settings.setValue(u"pl_show_lengths"_s, m_ui->lengthsCheckBox->isChecked());
    settings.setValue(u"pl_align_numbers"_s, m_ui->alignCheckBox->isChecked());
    settings.setValue(u"pl_show_anchor"_s, m_ui->anchorCheckBox->isChecked());
    settings.setValue(u"pl_show_splitters"_s, m_ui->showSplittersCheckBox->isChecked());
    settings.setValue(u"pl_show_popup"_s, m_ui->popupCheckBox->isChecked());
    settings.setValue(u"pl_tabs_closable"_s, m_ui->tabsClosableCheckBox->isChecked());
    settings.setValue(u"pl_show_new_pl_button"_s, m_ui->showNewPLCheckBox->isChecked());
    settings.setValue(u"pl_show_tab_list_menu"_s, m_ui->showTabListMenuCheckBox->isChecked());
    settings.setValue(u"pl_tab_position"_s, m_ui->tabPositionComboBox->currentData().toInt());
    settings.setValue(u"start_hidden"_s, m_ui->hiddenCheckBox->isChecked());
    settings.setValue(u"hide_on_close"_s, m_ui->hideOnCloseCheckBox->isChecked());
    settings.setValue(u"window_title_format"_s, m_ui->windowTitleLineEdit->text());
    settings.setValue(u"vis_color1"_s, m_ui->vColor1->colorName());
    settings.setValue(u"vis_color2"_s, m_ui->vColor2->colorName());
    settings.setValue(u"vis_color3"_s, m_ui->vColor3->colorName());
    settings.setValue(u"vis_peak_color"_s, m_ui->peaksColor->colorName());
    settings.setValue(u"vis_bg_color"_s, m_ui->bgColor->colorName());
    settings.setValue(u"pl_system_colors"_s, m_ui->plSystemColorsCheckBox->isChecked());
    settings.setValue(u"pl_bg1_color"_s, m_ui->plBg1Color->colorName());
    settings.setValue(u"pl_bg2_color"_s, m_ui->plBg2Color->colorName());
    settings.setValue(u"pl_highlight_color"_s, m_ui->plHlColor->colorName());
    settings.setValue(u"pl_normal_text_color"_s, m_ui->plTextNormalColor->colorName());
    settings.setValue(u"pl_current_text_color"_s, m_ui->plTextCurrentColor->colorName());
    settings.setValue(u"pl_hl_text_color"_s, m_ui->plTextHlCurrentColor->colorName());
    settings.setValue(u"pl_group_bg"_s, m_ui->plGrBgColor->colorName());
    settings.setValue(u"pl_splitter_color"_s, m_ui->plSplitterColor->colorName());
    settings.setValue(u"pl_group_text"_s, m_ui->plGrTextColor->colorName());
    settings.setValue(u"pl_current_bg_color"_s, m_ui->plCurrentTrackBgColor->colorName());
    settings.setValue(u"pl_override_group_colors"_s, m_ui->plOverrideGroupColorsCheckBox->isChecked());
    settings.setValue(u"pl_override_current_track_colors"_s, m_ui->plOverrideCurrentTrackColorsCheckBox->isChecked());
    settings.setValue(u"pl_font"_s, m_ui->plFontLabel->font().toString());
    settings.setValue(u"pl_group_font"_s, m_ui->groupFontLabel->font().toString());
    settings.setValue(u"pl_extra_row_font"_s, m_ui->extraRowFontLabel->font().toString());
    settings.setValue(u"pl_tabs_font"_s, m_ui->tabsFontLabel->font().toString());
    settings.setValue(u"pl_header_font"_s, m_ui->columnFontLabel->font().toString());
    settings.setValue(u"use_system_fonts"_s, m_ui->systemFontsCheckBox->isChecked());
    int index = m_ui->toolBarIconSizeComboBox->currentIndex();
    settings.setValue(u"toolbar_icon_size"_s, m_ui->toolBarIconSizeComboBox->itemData(index));
    settings.setValue(u"wfsb_bg_color"_s, m_ui->wfsbBgColor->colorName());
    settings.setValue(u"wfsb_rms_color"_s, m_ui->wfsbRmsColor->colorName());
    settings.setValue(u"wfsb_waveform_color"_s, m_ui->wfsbWaveFormColor->colorName());
    settings.setValue(u"wfsb_progressbar_color"_s, m_ui->wfsbProgressBarColor->colorName());
    settings.endGroup();
}

void QSUiSettings::addWindowTitleString(const QString &str)
{
    if (m_ui->windowTitleLineEdit->cursorPosition () < 1)
        m_ui->windowTitleLineEdit->insert(str);
    else
        m_ui->windowTitleLineEdit->insert(u" - "_s + str);
}
