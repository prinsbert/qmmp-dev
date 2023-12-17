#ifndef GROUPEDCONTAINER2_H
#define GROUPEDCONTAINER2_H

#include <QList>
#include "playlistgroup.h"
#include "playlistcontainer2_p.h"
#include "playlisttrack.h"

class GroupedContainer2 : public PlayListContainer2
{
public:
    GroupedContainer2();

    void addTrack(PlayListTrack *track) override;
    void addTracks(const QList<PlayListTrack *> &tracks) override;
    int insertTrack(int index, PlayListTrack *track) override;
    void replaceTracks(const QList<PlayListTrack *> &tracks) override;
    QList<PlayListGroup *> groups() const override;
    QList<PlayListTrack *> tracks() const override;
    int groupCount() const override;
    int trackCount() const override;
    QList<PlayListTrack *> mid(int pos, int count) const override;
    bool isEmpty() const override;
    bool isSelected(int index) const override;
    void setSelected(int index, bool selected) override;
    void clearSelection() override;
    int indexOf(PlayListItem *item) const override;
    PlayListTrack *track(int index) const override;
    PlayListGroup *group(int index) const override;
    bool contains(PlayListItem *item) const override;
    void removeTrack(PlayListTrack *track) override;
    void removeTracks(QList<PlayListTrack *> tracks) override;
    bool move(const QList<int> &indexes, int from, int to) override;
    QList<PlayListTrack *> takeAllTracks() override;
    void clear() override;
    void reverseList() override;
    void randomizeList() override;

    //playlist view api
    int lineCount() const override;
    PlayListItem *itemAtLine(int lineIndex) const override;
    QList<PlayListItem *> itemsAtLines(int pos, int length = -1) const override;
    int subIndexOfLine(int lineIndex) const override;

private:
    void updateCache() const;

    struct PlayListLine
    {
      bool isGroup = false;
      int index = -1;
      int subindex = -1;
    };

    QList<PlayListTrack *> m_tracks;
    QList<PlayListGroup *> m_groups;
    mutable QList<PlayListLine> m_lines;
    mutable bool m_update = true;
};

#endif // GROUPEDCONTAINER2_H
