#ifndef COMICMODEL_H
#define COMICMODEL_H

#include <QAbstractListModel>
#include <QSqlDatabase>

#include <map>

#include "comic.h"

class ComicModel : public QAbstractListModel
{
    enum ComicRoles {
        IdRole = Qt::UserRole + 1,
        NameRole
    };
    Q_OBJECT

    QSqlDatabase db;

    std::map<int, Comic*>* cache;
    Comic* emptyComic;
public:
    explicit ComicModel(QObject *parent = 0);

    // Subclassed from QAbstractListModel
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[IdRole] = "id";
        roles[NameRole] = "name";
        return roles;
    }

    void storeComic(int id,
                    int day,
                    int month,
                    int year,
                    QString link,
                    QString img,
                    QString news,
                    QString alt,
                    QString title,
                    QString safe_title);

    void storeImage(int id,
                    QByteArray data);

    Comic* getComic(int id) const;

    ~ComicModel()
    {
        for(auto it : *cache)
        {
            delete it.second;
        }
        delete cache;
        delete emptyComic;
    }

signals:

public slots:

};

#endif // COMICMODEL_H
