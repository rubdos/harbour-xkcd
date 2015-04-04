#ifndef COMIC_H
#define COMIC_H

#include <QObject>
#include <QImage>
#include <QUrl>

class ComicModel;

class Comic : public QObject
{
    friend class ComicModel;
    Q_OBJECT

    Q_PROPERTY(int id READ getId)
    Q_PROPERTY(QString name READ getName)
    Q_PROPERTY(QString alt READ getAlt)
    Q_PROPERTY(QUrl url READ getUrl)
    Q_PROPERTY(QImage image READ getImage)

    int id;
    QString name;
    QString alt;
    QImage image;
public:
    explicit Comic(QObject *parent = 0);

    int getId() const {return id;}
    QString getName() const {return name;}
    QString getAlt() const {return alt;}

    QUrl getUrl() const
    {
        return QUrl("image://comicProvider/comic" + QString::number(id));
    }

    QImage getImage() const {return image;}

    bool isComplete();

signals:

public slots:

};

Q_DECLARE_METATYPE(Comic*)

#endif // COMIC_H
