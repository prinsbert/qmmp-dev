/***************************************************************************
 *   Copyright (C) 2009-2025 by Ilya Kotov                                 *
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
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPushButton>
#include <QStyle>
#include <qmmp/metadatamanager.h>
#include <qmmp/metadatamodel.h>
#include <qmmp/tagmodel.h>
#include <qmmp/soundcore.h>
#include "ui_detailsdialog.h"
#include "metadataformatter.h"
#include "playlisttrack.h"
#include "tageditor_p.h"
#include "covereditor_p.h"
#include "cueeditor_p.h"
#include "detailsdialog.h"

DetailsDialog::DetailsDialog(const QList<PlayListTrack *> &tracks, QWidget *parent)
        : QDialog(parent), m_tracks(tracks)
{
    m_ui = new Ui::DetailsDialog;
    m_ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    m_ui->directoryButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
    m_ui->nextButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight));
    m_ui->prevButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowLeft));
    updatePage();
    on_tabWidget_currentChanged(0);

    for(PlayListTrack *t : std::as_const(m_tracks))
        t->beginUsage();
}

DetailsDialog::DetailsDialog(PlayListTrack *track, QWidget *parent) : DetailsDialog(QList<PlayListTrack *>{ track }, parent)
{}

DetailsDialog::~DetailsDialog()
{
    for(PlayListTrack *t : std::as_const(m_tracks))
    {
        t->endUsage();
        if (!t->isUsed() && t->isSheduledForDeletion())
        {
            delete t;
            t = nullptr;
        }
    }

    if(!m_modifiedPaths.isEmpty())
        emit metaDataChanged(m_modifiedPaths.values());

    if(m_metaDataModel)
    {
        delete m_metaDataModel;
        m_metaDataModel = nullptr;
    }
    delete m_ui;
}

QStringList DetailsDialog::modifiedPaths() const
{
    return m_modifiedPaths.values();
}

void DetailsDialog:: on_directoryButton_clicked()
{
    QString dir_path;
    if(!m_info.path().contains(u"://"_s)) //local file
        dir_path = QFileInfo(m_info.path()).absolutePath();
    else if (m_info.path().contains(u":///"_s)) //pseudo-protocol
    {
        dir_path = QUrl(m_info.path()).path();
        dir_path.replace(QString::fromLatin1(QUrl::toPercentEncoding(u"#"_s)), u"#"_s);
        dir_path.replace(QString::fromLatin1(QUrl::toPercentEncoding(u"?"_s)), u"?"_s);
        dir_path.replace(QString::fromLatin1(QUrl::toPercentEncoding(u"%"_s)), u"%"_s);
        dir_path = QFileInfo(dir_path).absolutePath();
    }
    else
        return;

    QDesktopServices::openUrl(QUrl::fromLocalFile(dir_path));
}

void DetailsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(m_ui->buttonBox->standardButton(button) == QDialogButtonBox::Save)
    {
        TagEditor *tagEditor = nullptr;
        CoverEditor *coverEditor = nullptr;
        CueEditor *cueEditor = nullptr;

        if((tagEditor = qobject_cast<TagEditor *>(m_ui->tabWidget->currentWidget())))
        {
            tagEditor->save();
            m_modifiedPaths.insert(m_info.path());
        }
        else if((coverEditor = qobject_cast<CoverEditor *>(m_ui->tabWidget->currentWidget())))
        {
            coverEditor->save();
            m_modifiedPaths.insert(m_info.path());
            MetaDataManager::instance()->clearCoverCache();
        }
        else if((cueEditor = qobject_cast<CueEditor *>(m_ui->tabWidget->currentWidget())))
        {
            //update all cue tracks
            static const QRegularExpression trackNumber(u"#\\d+$"_s);
            int count = cueEditor->trackCount();
            QString path = m_info.path();
            path.remove(trackNumber);
            for(int i = 0; i < count; ++i)
                m_modifiedPaths.insert(QStringLiteral("%1#%2").arg(path).arg(i + 1));
            m_modifiedPaths.insert(m_info.path());

            cueEditor->save();
        }
    }
    else
    {
        //close all files before closing dialog
        if(m_metaDataModel)
        {
            delete m_metaDataModel;
            m_metaDataModel = nullptr;
        }
        reject();
    }
}

void DetailsDialog::on_tabWidget_currentChanged(int index)
{
    CoverEditor *coverEditor = nullptr;
    CueEditor *cueEditor = nullptr;
    if(qobject_cast<TagEditor *>(m_ui->tabWidget->widget(index)))
        m_ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(m_metaDataModel && !m_metaDataModel->isReadOnly());
    else if((coverEditor = qobject_cast<CoverEditor *>(m_ui->tabWidget->currentWidget())))
        m_ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(coverEditor->isEditable());
    else if((cueEditor = qobject_cast<CueEditor *>(m_ui->tabWidget->currentWidget())))
        m_ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(cueEditor->isEditable());
    else
        m_ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
}

void DetailsDialog::on_prevButton_clicked()
{
    if(m_page == 0)
        m_page = m_tracks.count() - 1;
    else
        m_page--;
    updatePage();
}

void DetailsDialog::on_nextButton_clicked()
{
    if(m_page >= m_tracks.count() - 1)
        m_page = 0;
    else
        m_page++;
    updatePage();
}

void DetailsDialog::closeEvent(QCloseEvent *)
{
    //close all files before closing dialog
    if(m_metaDataModel)
    {
        delete m_metaDataModel;
        m_metaDataModel = nullptr;
    }
}

void DetailsDialog::updatePage()
{
    if(m_metaDataModel)
    {
        delete m_metaDataModel;
        m_metaDataModel = nullptr;
    }

    while (m_ui->tabWidget->count() > 1)
    {
        int index = m_ui->tabWidget->count() - 1;
        QWidget *w = m_ui->tabWidget->widget(index);
        m_ui->tabWidget->removeTab(index);
        w->deleteLater();
    }

    m_ui->pageLabel->setText(tr("%1/%2").arg(m_page + 1).arg(m_tracks.count()));
    if(m_page >= 0 && m_page < m_tracks.count())
        m_info = *m_tracks.at(m_page);
    else
        m_info.clear();

    setWindowTitle(m_info.path().section(QLatin1Char('/'),-1));
    m_ui->pathEdit->setText(m_info.path());

    //load metadata and create metadata model
    QList<TrackInfo *> infoList = MetaDataManager::instance()->createPlayList(m_info.path());
    if(!infoList.isEmpty())
    {
        if(infoList.constFirst()->parts() & TrackInfo::MetaData)
            m_info.setValues(infoList.constFirst()->metaData());
        if(infoList.constFirst()->parts() & TrackInfo::Properties)
        {
            m_info.updateValues(infoList.constFirst()->properties());
            m_info.setDuration(infoList.constFirst()->duration());
        }
    }
    qDeleteAll(infoList);
    infoList.clear();

    QString coverPath;
    QImage coverImage;
    bool readOnly = false;

    if(m_info.path().contains(u"://"_s) && m_info.path().contains(QLatin1Char('#'))) //track of multi-track file
    {
        QString filePath = TrackInfo::pathFromUrl(m_info.path());
        if(QFileInfo(filePath).isFile())
            readOnly = !QFileInfo(filePath).isWritable() || !QFile::exists(filePath);
    }
    else if(!m_info.path().contains(u"://"_s)) //local file
    {
        coverPath = MetaDataManager::instance()->findCoverFile(m_info.path());
        readOnly = !QFileInfo(m_info.path()).isWritable() || !QFile::exists(m_info.path());
    }

    m_metaDataModel = MetaDataManager::instance()->createMetaDataModel(m_info.path(), readOnly);

    if(m_metaDataModel)
    {
        coverPath = coverPath.isEmpty() ? m_metaDataModel->coverPath() : coverPath;
        coverImage = m_metaDataModel->cover();
    }

    if((m_metaDataModel && (m_metaDataModel->dialogHints() & MetaDataModel::IsCoverEditable)) ||
            !coverPath.isEmpty() ||
            !coverImage.isNull())
    {
        CoverEditor *coverEditor = new CoverEditor(m_metaDataModel, coverPath, this);
        m_ui->tabWidget->addTab(coverEditor, tr("Cover"));
    }

    if(m_metaDataModel)
    {
        for(TagModel *tagModel : m_metaDataModel->tags())
        {
            TagEditor *editor = new TagEditor(tagModel, this);
            editor->setEnabled(!m_metaDataModel->isReadOnly());
            m_ui->tabWidget->addTab(editor, tagModel->name());
        }

        for(const MetaDataItem &item : m_metaDataModel->descriptions())
        {
            QTextEdit *textEdit = new QTextEdit(this);
            textEdit->setReadOnly(true);
            textEdit->setPlainText(item.value().toString());
            m_ui->tabWidget->addTab(textEdit, item.name());
        }

        QString lyrics = m_metaDataModel->lyrics();
        if(!lyrics.isEmpty())
        {
            QTextEdit *textEdit = new QTextEdit(this);
            textEdit->setReadOnly(true);
            textEdit->setPlainText(lyrics);
            m_ui->tabWidget->addTab(textEdit, tr("Lyrics"));
        }

        if(m_metaDataModel->dialogHints() & MetaDataModel::IsCueEditable)
        {
            CueEditor *cueEditor = new CueEditor(m_metaDataModel, m_info, this);
            m_ui->tabWidget->addTab(cueEditor, u"CUE"_s);
        }
    }

    printInfo();
}

void DetailsDialog::printInfo()
{
    SoundCore *core = SoundCore::instance();
    QString formattedText, metaDataRows, streamInfoRows, propertyRows;
    QStringList tableParts;

    //tags
    metaDataRows += formatRow(tr("Title"), m_info.value(Qmmp::TITLE));
    metaDataRows += formatRow(tr("Artist"), m_info.value (Qmmp::ARTIST));
    metaDataRows += formatRow(tr("Album artist"), m_info.value(Qmmp::ALBUMARTIST));
    metaDataRows += formatRow(tr("Album"), m_info.value(Qmmp::ALBUM));
    metaDataRows += formatRow(tr("Comment"), m_info.value(Qmmp::COMMENT));
    metaDataRows += formatRow(tr("Genre"), m_info.value(Qmmp::GENRE));
    metaDataRows += formatRow(tr("Composer"), m_info.value(Qmmp::COMPOSER));
    metaDataRows += formatRow(tr("Year"), m_info.value(Qmmp::YEAR));
    metaDataRows += formatRow(tr("Track"), m_info.value(Qmmp::TRACK));
    metaDataRows += formatRow(tr("Disc number"), m_info.value(Qmmp::DISCNUMBER));
    metaDataRows = metaDataRows.trimmed();
    if(!metaDataRows.isEmpty())
        tableParts << metaDataRows;

    //stream information
    if(core->state() == Qmmp::Playing && core->path() == m_info.path())
    {
        const QHash<QString, QString> &streamInfo = core->streamInfo();
        for(auto it = streamInfo.cbegin(); it != streamInfo.cend(); ++it)
            streamInfoRows += formatRow(it.key(), it.value());
    }
    streamInfoRows = streamInfoRows.trimmed();
    if(!streamInfoRows.isEmpty())
        tableParts << streamInfoRows;

    //properties
    QList<MetaDataItem> items;
    if(m_info.duration() > 0)
        items << MetaDataItem(tr("Duration"), MetaDataFormatter::formatDuration(m_info.duration()));
    if(!m_metaDataModel || !(m_metaDataModel->dialogHints() & MetaDataModel::CompletePropertyList))
    {
        items << MetaDataItem(tr("Bitrate"), m_info.value(Qmmp::BITRATE).toInt(), tr("kbps"));
        items << MetaDataItem(tr("Sample rate"), m_info.value(Qmmp::SAMPLERATE).toInt(), tr("Hz"));
        items << MetaDataItem(tr("Channels"), m_info.value(Qmmp::CHANNELS).toInt());
        items << MetaDataItem(tr("Sample size"), m_info.value(Qmmp::BITS_PER_SAMPLE).toInt(), tr("bits"));
        items << MetaDataItem(tr("Format name"), m_info.value(Qmmp::FORMAT_NAME));
        items << MetaDataItem(tr("File size"), m_info.value(Qmmp::FILE_SIZE).toInt() / 1024, tr("KiB"));
    }
    if(m_metaDataModel)
        items << m_metaDataModel->extraProperties();
    for(const MetaDataItem &item : std::as_const(items))
        propertyRows += formatRow(item);
    propertyRows = propertyRows.trimmed();
    if(!propertyRows.isEmpty())
        tableParts << propertyRows;

    //create table
    if(layoutDirection() == Qt::RightToLeft)
        formattedText.append(u"<DIV align=\"right\" dir=\"rtl\">"_s);
    else
        formattedText.append(u"<DIV>"_s);
    formattedText.append(u"<TABLE>"_s);

    formattedText += tableParts.join(u"<tr><td colspan=2><hr></td></tr>"_s);

    formattedText.append(u"</TABLE>"_s);
    formattedText.append(u"</DIV>"_s);
    m_ui->textEdit->setHtml(formattedText);
}

QString DetailsDialog::formatRow(const QString &key, const QString &value) const
{
    if(value.isEmpty() || key.isEmpty())
        return QString();
    QString str(u"<tr>"_s);
    if(layoutDirection() == Qt::RightToLeft)
        str.append(u"<td>"_s + value + u"</td> <td style=\"padding-left: 15px;\"><b>"_s + key + u"</b></td>"_s);
    else
        str.append(u"<td><b>"_s + key + u"</b></td> <td style=\"padding-left: 15px;\">"_s + value + u"</td>"_s);
    str.append(u"</tr>"_s);
    return str;
}

QString DetailsDialog::formatRow(const MetaDataItem &item) const
{
    if(item.value().isNull() || item.name().isEmpty() || !item.value().isValid())
        return QString();

    QString value;
    if(item.value().typeId() == QMetaType::Bool)
        value = item.value().toBool() ? tr("Yes") : tr("No");
    else if(item.value().typeId() == QMetaType::Double)
        value = QStringLiteral("%1").arg(item.value().toDouble(), 0, 'f', 4);
    else
        value = item.value().toString();

    if(value.isEmpty() || value == "0"_L1 || value == "0.0000"_L1)
        return QString();

    if(!item.suffix().isEmpty())
        value += QChar::Space + item.suffix();

    return formatRow(item.name(), value);
}
