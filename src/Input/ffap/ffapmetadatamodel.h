/***************************************************************************
 *   Copyright (C) 2011-2016 by Ilya Kotov                                 *
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

#ifndef FFAPMETADATAMODEL_H
#define FFAPMETADATAMODEL_H

#include <qmmp/metadatamodel.h>
#include <taglib/tag.h>
#include <taglib/apefile.h>
#include <taglib/tfilestream.h>

class QTextCodec;

/**
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class FFapMetaDataModel : public MetaDataModel
{
Q_OBJECT
public:
    FFapMetaDataModel(const QString &path, bool readOnly, QObject *parent);
    ~FFapMetaDataModel();
    QList<MetaDataItem> extraProperties() const override;
    QList<TagModel* > tags() const override;
    QString coverPath() const override;

private:
    QString m_path;
    QList<TagModel* > m_tags;
    TagLib::APE::File *m_file;
    TagLib::FileStream *m_stream;
};

class FFapFileTagModel : public TagModel
{
public:
    FFapFileTagModel(TagLib::APE::File *file, TagLib::APE::File::TagTypes tagType);
    ~FFapFileTagModel();
    QString name() const override;
    QList<Qmmp::MetaData> keys() const override;
    QString value(Qmmp::MetaData key) const override;
    void setValue(Qmmp::MetaData key, const QString &value) override;
    bool exists() const override;
    void create() override;
    void remove() override;
    void save() override;

private:
    QTextCodec *m_codec;
    TagLib::APE::File *m_file;
    TagLib::Tag *m_tag;
    TagLib::APE::File::TagTypes m_tagType;
};

#endif // FFapMETADATAMODEL_H
