#ifndef COMICCONTROLLER_H
#define COMICCONTROLLER_H

#include <QObject>
#include <QQuickImageProvider>
#include <QImage>
#include <QNetworkAccessManager>
#include <QSignalMapper>
#include "comic.h"
#include "comicmodel.h"

#define baseurl "https://www.xkcd.com/"

class Comic;

class ComicController : public QObject, public QQuickImageProvider
{
    Q_OBJECT

    Q_PROPERTY(Comic* currentComic READ getCurrentComic NOTIFY currentComicChanged)
    /*Q_PROPERTY(QUrl currentComicUrl READ getCurrentComicUrl NOTIFY currentComicChanged)
    Q_PROPERTY(QString currentComicName READ getCurrentComicName NOTIFY currentComicChanged)
    Q_PROPERTY(QString currentComicAlt READ getCurrentComicAlt NOTIFY currentComicChanged)*/

    Q_PROPERTY(ComicModel* comicModel READ getModel)

    Comic* currentComic;
    QNetworkAccessManager nam;
    QSignalMapper mapper;
    ComicModel model;
public:
    explicit ComicController(QObject *parent = 0);
    Comic* getCurrentComic();

    ComicModel* getModel(){return &model;}

    void loadLatestComic();

    Q_INVOKABLE void loadComic(unsigned int id);
    Q_INVOKABLE void previousComic();
    Q_INVOKABLE void nextComic();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

signals:
    void currentComicChanged();

public slots:
    void onLatestComicJsonLoaded();
    void onComicJsonLoaded();
    void onComicImageLoaded(int id);
};

#endif // COMICCONTROLLER_H
