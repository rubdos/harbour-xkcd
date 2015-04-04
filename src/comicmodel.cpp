#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlResult>

#include "comicmodel.h"

ComicModel::ComicModel(QObject *parent) :
    QAbstractListModel(parent)
{
    emptyComic = new Comic();
    cache = new std::map<int, Comic*>();
    db = QSqlDatabase::addDatabase("QSQLITE");

    QString data_dir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QDir("/").mkpath(data_dir);

    db.setDatabaseName(data_dir + "/data.sqlite");
    db.open();
    qDebug() << db.lastError();
    QSqlQuery query(db);
    Q_ASSERT(query.exec("CREATE TABLE IF NOT EXISTS `xkcd_comic` ("
               "`id` INTEGER PRIMARY KEY AUTOINCREMENT, "
               "`month` INTEGER, "
               "`link` TEXT, "
               "`year` INTEGER, "
               "`news` TEXT, "
               "`safe_title` TEXT, "
               "`transscript` TEXT, "
               "`alt` TEXT, "
               "`img_url` TEXT, "
               "`img_data` BLOB, "
               "`title` TEXT, "
               "`day` INTEGER"
               ")"));
}

void ComicModel::storeComic(int id,
                            int day,
                            int month,
                            int year,
                            QString link,
                            QString img,
                            QString news,
                            QString alt,
                            QString title,
                            QString safe_title)
{
    beginResetModel(); // Performance?
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM `xkcd_comic` WHERE id=?");
    query.addBindValue(id);
    query.exec();
    query.next();
    if(query.value(0).toInt() > 0)
    {
        query.prepare("UPDATE `xkcd_comic` SET"
                "`day` = ?,"
                "`month` = ?,"
                "`year` = ?, "
                "`link` = ?,"
                "`news` = ?, "
                "`safe_title` = ?, "
                "`transscript` = ?, "
                "`alt` = ?,"
                "`img_url` = ?,"
                "`title` = ? WHERE `id` = ?");
    }
    else{
        if(!query.prepare("INSERT INTO `xkcd_comic` ("
                        "`id`,"
                        "`day`,"
                        "`month`,"
                        "`year`, "
                        "`link`,"
                        "`news`, "
                        "`safe_title`, "
                        "`transscript`, "
                        "`alt`,"
                        "`img_url`,"
                        "`title`"
                        ") VALUES (?,?,?,?,?,?,?,?,?,?,?)"))
        {
                qWarning() << query.lastError();
        }

        query.addBindValue(id);
    }
    query.addBindValue(day);
    query.addBindValue(month);
    query.addBindValue(year);
    query.addBindValue(link);
    query.addBindValue(news);
    query.addBindValue(safe_title);
    query.addBindValue("");
    query.addBindValue(alt);
    query.addBindValue(img);
    query.addBindValue(title);
    if(query.value(0).toInt() > 1)
        query.addBindValue(id);
    if(!query.exec())
    {
        qWarning() << query.lastError();
    }
    query.prepare("SELECT COUNT(*) FROM `xkcd_comic` WHERE id<=?");
    query.addBindValue(id);
    query.exec();
    query.next();
    endResetModel(); // Performance?
}

void ComicModel::storeImage(int id, QByteArray data)
{
    QSqlQuery query("UPDATE `xkcd_comic` SET img_data=? WHERE id=?");
    query.addBindValue(data);
    query.addBindValue(id);
    query.exec();
}

Comic* ComicModel::getComic(int id) const
{
    Comic* c;
    if(cache->find(id) != cache->end() && (*cache)[id]->isComplete())
    {
        return (*cache)[id];
    }
    else if(cache->find(id) != cache->end())
    {
        c = (*cache)[id];
    }
    else
    {
        c = new Comic();
    }
    QSqlQuery query(db);
    if(!query.prepare("SELECT "
                    "`day`,"
                    "`month`,"
                    "`year`, "
                    "`link`,"
                    "`news`, "
                    "`safe_title`, "
                    "`transscript`, "
                    "`alt`,"
                    "`img_url`,"
                    "`title`,"
                    "`img_data` FROM `xkcd_comic` WHERE `id`=?"))
    {
        qWarning() << query.lastError();
    }

    query.addBindValue(id);
    Q_ASSERT(query.exec());
    if(!query.next())
        return emptyComic;

    c->id = id;
    c->alt = query.value(7).toString();
    c->name = query.value(9).toString();
    c->image = QImage::fromData(query.value(10).toByteArray());
    (*cache)[id] = c;
    return c;
}

QVariant ComicModel::data(const QModelIndex &index, int role) const
{
    qDebug() << "data called, id="<<index.row();

    QSqlQuery query(db);
    if(!query.prepare("SELECT `id` FROM `xkcd_comic` ORDER BY `id` LIMIT 1 OFFSET ?"))
    {
        qDebug() << query.lastError();
    }
    query.addBindValue(index.row());
    Q_ASSERT(query.exec());
    Q_ASSERT(query.next());
    int id = query.value(0).toInt();

    QVariant res(QVariant::Invalid);
    Comic* c = getComic(id);
    if(c->id == -1) return res;
    switch(role)
    {
    case IdRole:
        return QString::number(c->id);
        break;
    case NameRole:
        return c->name;
        break;
    default:
        return res;
    }
}

int ComicModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    QSqlQuery query(db);
    query.exec("SELECT COUNT(*) FROM `xkcd_comic`");
    Q_ASSERT(query.next());

    int count = query.value(0).toInt();;
    qDebug() << "Database size:" << count << "comics";
    return count;
}
