/***************************************************************************
 *   Copyright (C) 2007-2025 by Ilya Kotov                                 *
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
#include <QDir>
#include <QSettings>
#include <QFontDialog>
#include <QTreeWidgetItem>
#include <QIntValidator>
#include <qmmp/decoder.h>
#include <qmmp/output.h>
#include <qmmp/decoderfactory.h>
#include <qmmp/outputfactory.h>
#include <qmmp/visualfactory.h>
#include <qmmp/effectfactory.h>
#include <qmmp/effect.h>
#include <qmmp/soundcore.h>
#include <qmmp/enginefactory.h>
#include <qmmp/abstractengine.h>
#include <qmmp/qmmpsettings.h>
#include <qmmp/inputsource.h>
#include <qmmp/inputsourcefactory.h>
#include "ui_configdialog.h"
#include "pluginitem_p.h"
#include "radioitemdelegate_p.h"
#include "generalfactory.h"
#include "general.h"
#include "uihelper.h"
#include "uiloader.h"
#include "filedialog.h"
#include "mediaplayer.h"
#include "qmmpuisettings.h"
#include "playlistmodel.h"
#include "metadataformattermenu.h"
#include "configdialog.h"

ConfigDialog::ConfigDialog (QWidget *parent) : QDialog (parent)
{
    m_ui = new Ui::ConfigDialog;
    m_ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose, false);
    m_ui->preferencesButton->setEnabled(false);
    m_ui->informationButton->setEnabled(false);
    m_ui->treeWidget->setItemDelegate(new RadioItemDelegate(this));
    m_ui->treeWidget->header()->setSectionsMovable(false);
    connect(this, &QDialog::rejected, this, &ConfigDialog::saveSettings);
    m_ui->linesPerGroupComboBox->addItem(tr("1 row"), 1);
    m_ui->linesPerGroupComboBox->addItem(tr("3 rows"), 3);
    m_ui->linesPerGroupComboBox->addItem(tr("4 rows"), 4);
    m_ui->linesPerGroupComboBox->addItem(tr("5 rows"), 5);
    m_ui->replayGainModeComboBox->addItem(tr("Track"), QmmpSettings::REPLAYGAIN_TRACK);
    m_ui->replayGainModeComboBox->addItem(tr("Album"), QmmpSettings::REPLAYGAIN_ALBUM);
    m_ui->replayGainModeComboBox->addItem(tr("Disabled"), QmmpSettings::REPLAYGAIN_DISABLED);
    m_ui->bitDepthComboBox->addItem(u"16"_s, Qmmp::PCM_S16LE);
    m_ui->bitDepthComboBox->addItem(u"24"_s, Qmmp::PCM_S24LE);
    m_ui->bitDepthComboBox->addItem(u"32"_s, Qmmp::PCM_S32LE);
    m_ui->bitDepthComboBox->addItem(u"32 (float)"_s, Qmmp::PCM_FLOAT);
    m_ui->proxyTypeComboBox->addItem(tr("HTTP"), QmmpSettings::HTTP_PROXY);
    m_ui->proxyTypeComboBox->addItem(tr("SOCKS5"), QmmpSettings::SOCKS5_PROXY);
    m_ui->portLineEdit->setValidator(new QIntValidator(0, 65535, this));
    readSettings();
    loadPluginsInfo();
    loadLanguages();
    createMenus();
    updateGroupSettings();
    //connections
    connect(m_ui->linesPerGroupComboBox, &QComboBox::currentIndexChanged, this, &ConfigDialog::updateGroupSettings);
    connect(m_ui->showExtraRowCheckBox, &QCheckBox::clicked, this, &ConfigDialog::updateGroupSettings);
}

ConfigDialog::~ConfigDialog()
{
    delete m_ui;
}

void ConfigDialog::addPage(const QString &name, QWidget *widget, const QIcon &icon)
{
    m_ui->stackedWidget->insertWidget(m_insert_row, widget);
    m_ui->contentsWidget->insertItem (m_insert_row, name);
    m_ui->contentsWidget->item(m_insert_row)->setIcon(icon);
    m_ui->contentsWidget->setCurrentRow(0);
    m_insert_row++;
}

void ConfigDialog::readSettings()
{
    if(MediaPlayer::instance())
    {
        //playlist options
        QmmpUiSettings *guis = QmmpUiSettings::instance();
        m_ui->groupLineEdit->setText(guis->groupFormat());
        m_ui->extraRowFormatLineEdit->setText(guis->groupExtraRowFormat());
        m_ui->linesPerGroupComboBox->setCurrentIndex(m_ui->linesPerGroupComboBox->findData(guis->linesPerGroup()));
        m_ui->groupsShowCoverCheckBox->setChecked(guis->groupCoverVisible());
        m_ui->showDividingLineCheckBox->setChecked(guis->groupDividingLineVisible());
        m_ui->showExtraRowCheckBox->setChecked(guis->groupExtraRowVisible());
        m_ui->metaDataCheckBox->setChecked(guis->useMetaData());
        m_ui->plMetaDataCheckBox->setChecked(guis->readMetaDataForPlayLists());
        m_ui->underscoresCheckBox->setChecked(guis->convertUnderscore());
        m_ui->per20CheckBox->setChecked(guis->convertTwenty());
        m_ui->clearPrevPLCheckBox->setChecked(guis->clearPreviousPlayList());
        m_ui->skipExistingTracksCheckBox->setChecked(guis->skipExistingTracks());
        m_ui->stopAfterRemovingCheckBox->setChecked(guis->stopAfterRemovingOfCurrentTrack());
        //resume playback on startup
        m_ui->continuePlaybackCheckBox->setChecked(guis->resumeOnStartup());
        //directory filters
        m_ui->dirRestrictLineEdit->setText(guis->restrictFilters().join(QLatin1Char(',')).trimmed());
        m_ui->dirExcludeLineEdit->setText(guis->excludeFilters().join(QLatin1Char(',')).trimmed());
        //default playlist
        m_ui->defaultPlayListCheckBox->setChecked(guis->useDefaultPlayList());
        m_ui->defaultPlayListLineEdit->setText(guis->defaultPlayListName());
        //playlist auto-save when modified
        m_ui->autoSavePlayListCheckBox->setChecked(guis->autoSavePlayList());
        //url dialog
        m_ui->clipboardCheckBox->setChecked(guis->useClipboard());
    }
    //proxy settings
    QmmpSettings *gs = QmmpSettings::instance();
    m_ui->enableProxyCheckBox->setChecked(gs->isProxyEnabled());
    m_ui->authProxyCheckBox->setChecked(gs->useProxyAuth());
    m_ui->hostLineEdit->setText(gs->proxy().host());
    m_ui->proxyTypeComboBox->setCurrentIndex(m_ui->proxyTypeComboBox->findData(gs->proxyType()));
    if (gs->proxy().port(0))
        m_ui->portLineEdit->setText(QString::number(gs->proxy().port(0)));
    m_ui->proxyUserLineEdit->setText(gs->proxy().userName());
    m_ui->proxyPasswLineEdit->setText(gs->proxy().password());

    m_ui->hostLineEdit->setEnabled(m_ui->enableProxyCheckBox->isChecked());
    m_ui->portLineEdit->setEnabled(m_ui->enableProxyCheckBox->isChecked());
    m_ui->proxyTypeComboBox->setEnabled(m_ui->enableProxyCheckBox->isChecked());
    m_ui->proxyUserLineEdit->setEnabled(m_ui->authProxyCheckBox->isChecked());
    m_ui->proxyPasswLineEdit->setEnabled(m_ui->authProxyCheckBox->isChecked());
    //file type determination
    m_ui->byContentCheckBox->setChecked(gs->determineFileTypeByContent());
    //cover options
    m_ui->coverIncludeLineEdit->setText(gs->coverNameFilters(true).join(QLatin1Char(',')));
    m_ui->coverExcludeLineEdit->setText(gs->coverNameFilters(false).join(QLatin1Char(',')));
    m_ui->coverDepthSpinBox->setValue(gs->coverSearchDepth());
    m_ui->useCoverFilesCheckBox->setChecked(gs->useCoverFiles());
    //replay gain
    m_ui->clippingCheckBox->setChecked(gs->replayGainPreventClipping());
    m_ui->replayGainModeComboBox->setCurrentIndex(m_ui->replayGainModeComboBox->findData(gs->replayGainMode()));
    m_ui->preampDoubleSpinBox->setValue(gs->replayGainPreamp());
    m_ui->defaultGainDoubleSpinBox->setValue(gs->replayGainDefaultGain());
    //audio
    m_ui->volumeStepSpinBox->setValue(gs->volumeStep());
    m_ui->softVolumeCheckBox->setChecked(gs->useSoftVolume());
    m_ui->bitDepthComboBox->setCurrentIndex(m_ui->bitDepthComboBox->findData(gs->outputFormat()));
    m_ui->ditheringCheckBox->setChecked(gs->useDithering());
    m_ui->bufferSizeSpinBox->setValue(gs->bufferSize());
    m_ui->abrCheckBox->setChecked(gs->averageBitrate());
    //equalizer
    m_ui->twoPassEqCheckBox->setChecked(gs->eqSettings().twoPasses());
    //geometry
    QSettings settings;
    resize(settings.value(u"ConfigDialog/window_size"_s, QSize(700,470)).toSize());
    QList<QVariant> var_sizes = settings.value(u"ConfigDialog/splitter_sizes"_s).toList();
    if(var_sizes.count() != 2)
    {
        var_sizes.clear();
        var_sizes << 180 << width()-180;
    }
    QList<int> sizes = { var_sizes.constFirst().toInt(), var_sizes.constLast().toInt() };
    m_ui->splitter->setSizes(sizes);
    //fonts
    QFont font;
    font.fromString(settings.value(u"CueEditor/font"_s, qApp->font("QPlainTextEdit").toString()).toString());
    m_ui->cueFontLabel->setText(font.family() + QChar::Space + QString::number(font.pointSize()));
    m_ui->cueFontLabel->setFont(font);
    m_ui->cueSystemFontCheckBox->setChecked(settings.value(u"CueEditor/use_system_font"_s, true).toBool());
}

void ConfigDialog::on_contentsWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;
    m_ui->stackedWidget->setCurrentIndex (m_ui->contentsWidget->row (current));
}

void ConfigDialog::loadPluginsInfo()
{
    m_ui->treeWidget->blockSignals(true);
    /*
        load transport plugin information
     */
    QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->treeWidget, { tr("Transports") });
    item->setFirstColumnSpanned(true);
    for(InputSourceFactory *factory : InputSource::factories())
    {
        new PluginItem(item, factory,  InputSource::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load input plugins information
    */
    item = new QTreeWidgetItem(m_ui->treeWidget, { tr("Decoders") });
    item->setFirstColumnSpanned(true);
    for(DecoderFactory *factory : Decoder::factories())
    {
        new PluginItem(item, factory,  Decoder::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load audio engines information
    */
    item = new QTreeWidgetItem(m_ui->treeWidget, { tr("Engines") });
    item->setFirstColumnSpanned(true);
    for(EngineFactory *factory : AbstractEngine::factories())
    {
        new PluginItem(item, factory,  AbstractEngine::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    item->setHidden(!item->childCount());
    /*
        load effect plugin information
    */
    item = new QTreeWidgetItem(m_ui->treeWidget, { tr("Effects") });
    item->setFirstColumnSpanned(true);
    for(EffectFactory *factory : Effect::factories())
    {
        new PluginItem(item, factory, Effect::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load visual plugin information
    */
    item = new QTreeWidgetItem(m_ui->treeWidget, { tr("Visualization") });
    item->setFirstColumnSpanned(true);
    for(VisualFactory *factory : Visual::factories())
    {
        new PluginItem(item, factory, Visual::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    item->setHidden(!item->childCount());
    /*
        load general plugin information
    */
    item = new QTreeWidgetItem(m_ui->treeWidget, { tr("General") });
    item->setFirstColumnSpanned(true);
    for(GeneralFactory *factory : General::factories())
    {
        new PluginItem(item, factory, General::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load output plugins information
    */
    item = new QTreeWidgetItem(m_ui->treeWidget, { tr("Output") });
    item->setFirstColumnSpanned(true);
    for(OutputFactory *factory : Output::factories())
    {
        new PluginItem(item, factory, Output::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    /*
        load file dialogs information
    */
    item = new QTreeWidgetItem(m_ui->treeWidget, { tr("File Dialogs") });
    item->setFirstColumnSpanned(true);
    for(FileDialogFactory *factory : FileDialog::factories())
    {
        new PluginItem(item, factory, FileDialog::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);
    item->setHidden(!item->childCount());
    /*
        load user interfaces information
    */
    item = new QTreeWidgetItem(m_ui->treeWidget, { tr("User Interfaces") });
    item->setFirstColumnSpanned(true);
    for(UiFactory *factory : UiLoader::factories())
    {
        new PluginItem(item, factory, UiLoader::file(factory));
    }
    m_ui->treeWidget->addTopLevelItem(item);
    item->setExpanded(true);

    m_ui->treeWidget->blockSignals(false);
    m_ui->treeWidget->resizeColumnToContents(0);
    m_ui->treeWidget->resizeColumnToContents(1);
}

void ConfigDialog::on_preferencesButton_clicked()
{
    QTreeWidgetItem *item = m_ui->treeWidget->currentItem();
    if(item && item->type() >= PluginItem::TRANSPORT)
        dynamic_cast<PluginItem *>(item)->showSettings(this);

}

void ConfigDialog::on_informationButton_clicked()
{
    QTreeWidgetItem *item = m_ui->treeWidget->currentItem();
    if(item && item->type() >= PluginItem::TRANSPORT)
        dynamic_cast<PluginItem *>(item)->showAbout(this);
}

void ConfigDialog::createMenus()
{
    MetaDataFormatterMenu *groupMenu = new MetaDataFormatterMenu(MetaDataFormatterMenu::GROUP_MENU, this);
    m_ui->groupButton->setMenu(groupMenu);
    m_ui->groupButton->setPopupMode(QToolButton::InstantPopup);
    connect(groupMenu, &MetaDataFormatterMenu::patternSelected, this, [this] (const QString &str) {
        m_ui->groupLineEdit->insert(m_ui->groupLineEdit->cursorPosition() < 1 ? str : (u" - "_s + str));
    });

    MetaDataFormatterMenu *extraRowFormatMenu = new MetaDataFormatterMenu(MetaDataFormatterMenu::GROUP_EXTRA_ROW_MENU, this);
    m_ui->extraRowFormatButton->setMenu(extraRowFormatMenu);
    m_ui->extraRowFormatButton->setPopupMode(QToolButton::InstantPopup);
    connect(extraRowFormatMenu, &MetaDataFormatterMenu::patternSelected, this, [this] (const QString &str) {
        m_ui->extraRowFormatLineEdit->insert(m_ui->extraRowFormatLineEdit->cursorPosition() < 1 ? str : (u" - "_s + str));
    });

    m_ui->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_preferencesAction = new QAction(QIcon::fromTheme(u"configure"_s), tr("Preferences"), m_ui->treeWidget);
    m_preferencesAction->setEnabled(false);
    m_ui->treeWidget->addAction(m_preferencesAction);
    m_informationAction = new QAction(QIcon::fromTheme(u"dialog-information"_s), tr("Information"), m_ui->treeWidget);
    m_informationAction->setEnabled(false);
    m_ui->treeWidget->addAction(m_informationAction);
    connect(m_preferencesAction, &QAction::triggered, this, &ConfigDialog::on_preferencesButton_clicked);
    connect(m_informationAction, &QAction::triggered, this, &ConfigDialog::on_informationButton_clicked);
}

void ConfigDialog::loadLanguages()
{
    const QMap<QString, QString> l = {
        { u"auto"_s, tr("<Autodetect>") },
        { u"pt_BR"_s, tr("Brazilian Portuguese") },
        { u"zh_CN"_s, tr("Chinese Simplified") },
        { u"zh_TW"_s, tr("Chinese Traditional") },
        { u"cs"_s, tr("Czech") },
        { u"nl"_s, tr("Dutch") },
        { u"en_US"_s, tr("English") },
        { u"fr"_s, tr("French") },
        { u"gl_ES"_s, tr("Galician") },
        { u"de"_s, tr("German") },
        { u"el"_s, tr("Greek") },
        { u"he"_s, tr("Hebrew") },
        { u"hu"_s, tr("Hungarian") },
        { u"id"_s, tr("Indonesian") },
        { u"it"_s, tr("Italian") },
        { u"ja"_s, tr("Japanese") },
        { u"kk"_s, tr("Kazakh") },
        { u"ko"_s, tr("Korean") },
        { u"lt"_s, tr("Lithuanian") },
        { u"pl_PL"_s, tr("Polish") },
        { u"pt"_s, tr("Portuguese") },
        { u"ru_RU"_s, tr("Russian") },
        { u"sr_RS"_s, tr("Serbian") },
        { u"sk"_s, tr("Slovak") },
        { u"sv"_s, tr("Swedish") },
        { u"es"_s, tr("Spanish") },
        { u"tr"_s, tr("Turkish") },
        { u"uk_UA"_s, tr("Ukrainian") },
        { u"sr_BA"_s, tr("Serbian (Ijekavian)") },
        { u"sr_RS"_s, tr("Serbian (Ekavian)") },
    };

    for(auto it = l.cbegin(); it != l.cend(); ++it)
    {
        QString title = it.key() != "auto"_L1 ? QStringLiteral("%1 (%2)").arg(it.value(), it.key()) : it.value();
        m_ui->langComboBox->addItem(title, it.key());
    }

    QString code = Qmmp::uiLanguageID();
    int index = m_ui->langComboBox->findData(code);
    if(index < 0)
        index = m_ui->langComboBox->findData(u"auto"_s);
    m_ui->langComboBox->setCurrentIndex(index);
}

void ConfigDialog::saveSettings()
{
    if(QmmpUiSettings *guis = QmmpUiSettings::instance())
    {
        guis->setGroupFormat(m_ui->groupLineEdit->text().trimmed());
        guis->setGroupExtraRowFormat(m_ui->extraRowFormatLineEdit->text().trimmed());
        guis->setLinesPerGroup(m_ui->linesPerGroupComboBox->currentData().toInt());
        guis->setGroupCoverVisible(m_ui->groupsShowCoverCheckBox->isChecked());
        guis->setGroupDividingLineVisible(m_ui->showDividingLineCheckBox->isChecked());
        guis->setGroupExtraRowVisible(m_ui->showExtraRowCheckBox->isChecked());
        guis->setUseMetaData(m_ui->metaDataCheckBox->isChecked());
        guis->setReadMetaDataForPlayLists(m_ui->plMetaDataCheckBox->isChecked());
        guis->setConvertUnderscore(m_ui->underscoresCheckBox->isChecked());
        guis->setConvertTwenty(m_ui->per20CheckBox->isChecked());
        guis->setClearPreviousPlayList(m_ui->clearPrevPLCheckBox->isChecked());
        guis->setSkipExistingTracks(m_ui->skipExistingTracksCheckBox->isChecked());
        guis->setStopAfterRemovingOfCurrentTrack(m_ui->stopAfterRemovingCheckBox->isChecked());
        guis->setResumeOnStartup(m_ui->continuePlaybackCheckBox->isChecked());
        guis->setRestrictFilters(m_ui->dirRestrictLineEdit->text());
        guis->setExcludeFilters(m_ui->dirExcludeLineEdit->text());
        guis->setDefaultPlayList(m_ui->defaultPlayListLineEdit->text(),
                                 m_ui->defaultPlayListCheckBox->isChecked());
        guis->setAutoSavePlayList(m_ui->autoSavePlayListCheckBox->isChecked());
        guis->setUseClipboard(m_ui->clipboardCheckBox->isChecked());
    }

    QmmpSettings *gs = QmmpSettings::instance();
    //proxy
    QUrl proxyUrl;
    proxyUrl.setHost(m_ui->hostLineEdit->text());
    proxyUrl.setPort(m_ui->portLineEdit->text().toInt());
    proxyUrl.setUserName(m_ui->proxyUserLineEdit->text());
    proxyUrl.setPassword(m_ui->proxyPasswLineEdit->text());
    gs->setNetworkSettings(m_ui->enableProxyCheckBox->isChecked(),
                           m_ui->authProxyCheckBox->isChecked(),
                           static_cast<QmmpSettings::ProxyType>(m_ui->proxyTypeComboBox->currentData().toInt()),
                           proxyUrl);

    gs->setCoverSettings(m_ui->coverIncludeLineEdit->text().split(QLatin1Char(',')),
                         m_ui->coverExcludeLineEdit->text().split(QLatin1Char(',')),
                         m_ui->coverDepthSpinBox->value(),
                         m_ui->useCoverFilesCheckBox->isChecked());
    int i = m_ui->replayGainModeComboBox->currentIndex();
    gs->setReplayGainSettings((QmmpSettings::ReplayGainMode)
                              m_ui->replayGainModeComboBox->itemData(i).toInt(),
                              m_ui->preampDoubleSpinBox->value(),
                              m_ui->defaultGainDoubleSpinBox->value(),
                              m_ui->clippingCheckBox->isChecked());
    i = m_ui->bitDepthComboBox->currentIndex();
    gs->setAudioSettings(m_ui->softVolumeCheckBox->isChecked(),
                         (Qmmp::AudioFormat)m_ui->bitDepthComboBox->itemData(i).toInt(),
                         m_ui->ditheringCheckBox->isChecked());
    gs->setAverageBitrate(m_ui->abrCheckBox->isChecked());
    gs->setBufferSize(m_ui->bufferSizeSpinBox->value());
    gs->setDetermineFileTypeByContent(m_ui->byContentCheckBox->isChecked());
    gs->setVolumeStep(m_ui->volumeStepSpinBox->value());
    EqSettings eqs = gs->eqSettings();
    eqs.setTwoPasses(m_ui->twoPassEqCheckBox->isChecked());
    gs->setEqSettings(eqs);
    QList<QVariant> var_sizes = { m_ui->splitter->sizes().constFirst(), m_ui->splitter->sizes().constLast() };
    QSettings settings;
    settings.setValue(u"ConfigDialog/splitter_sizes"_s, var_sizes);
    settings.setValue(u"ConfigDialog/window_size"_s, size());
    //User interface language
    int index = m_ui->langComboBox->currentIndex();
    if(index >= 0)
        Qmmp::setUiLanguageID(m_ui->langComboBox->itemData(index).toString());
    //fonts
    settings.setValue(u"CueEditor/font"_s, m_ui->cueFontLabel->font().toString());
    settings.setValue(u"CueEditor/use_system_font"_s, m_ui->cueSystemFontCheckBox->isChecked());
}

void ConfigDialog::updateGroupSettings()
{
    int linesPerGroup = m_ui->linesPerGroupComboBox->currentData().toInt();
    m_ui->groupsShowCoverCheckBox->setEnabled(linesPerGroup > 1);
    m_ui->showExtraRowCheckBox->setEnabled(linesPerGroup > 1);
    m_ui->extraRowFormatLineEdit->setEnabled(linesPerGroup > 1 && m_ui->showExtraRowCheckBox->isChecked());
    m_ui->extraRowFormatButton->setEnabled(linesPerGroup > 1 && m_ui->showExtraRowCheckBox->isChecked());
    m_ui->extraRowFormatLabel->setEnabled(linesPerGroup > 1 && m_ui->showExtraRowCheckBox->isChecked());
}

void ConfigDialog::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    if(column == 0 && item->type() >= PluginItem::TRANSPORT)
        dynamic_cast<PluginItem *>(item)->setEnabled(item->checkState(0) == Qt::Checked);
}

void ConfigDialog::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if(current->type() >= PluginItem::TRANSPORT)
    {
        m_ui->preferencesButton->setEnabled(dynamic_cast<PluginItem *>(current)->hasSettings());
        m_ui->informationButton->setEnabled(dynamic_cast<PluginItem *>(current)->hasAbout());
    }
    else
    {
        m_ui->preferencesButton->setEnabled(false);
        m_ui->informationButton->setEnabled(false);
    }
    m_preferencesAction->setEnabled(m_ui->preferencesButton->isEnabled());
    m_informationAction->setEnabled(m_ui->informationButton->isEnabled());
}

void ConfigDialog::on_cueFontButton_clicked()
{
    bool ok = false;
    QFont font = m_ui->cueFontLabel->font();
    font = QFontDialog::getFont (&ok, font, this);
    if(ok)
    {
        m_ui->cueFontLabel->setText(font.family () + QChar::Space + QString::number(font.pointSize ()));
        m_ui->cueFontLabel->setFont(font);
    }
}
