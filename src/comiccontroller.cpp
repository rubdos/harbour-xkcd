#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSignalMapper>

#include "comiccontroller.h"
#include "comic.h"

ComicController::ComicController(QObject *parent) :
    QObject(parent),
    QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    currentComic = new Comic(this);
    qDebug() << "ComicController constructed";

    //Q_ASSERT(currentComic.load(":/xkcd_logo.png"));
    //currentComicId = 0;

    loadLatestComic();
}

void ComicController::loadLatestComic()
{
    QNetworkReply* reply = nam.get(QNetworkRequest(QUrl(baseurl "info.0.json")));
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(onLatestComicJsonLoaded()));
}

void ComicController::onLatestComicJsonLoaded()
{
    QIODevice * content = static_cast<QIODevice*>(QObject::sender());

    QJsonDocument doc = QJsonDocument::fromJson(content->readAll());
    content->deleteLater();

    QJsonObject obj = doc.object();
    loadComic(obj.value(QString("num")).toInt());
}
void ComicController::loadComic(unsigned int id)
{
    Comic* c = model.getComic(id);
    if((c->getId() == id) && (c->isComplete()))
    {
        currentComic = c;
        emit currentComicChanged();
        return;
    }
    QNetworkReply* reply = nam.get(QNetworkRequest(QUrl(QString(baseurl) + QString::number(id) + "/info.0.json")));
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(onComicJsonLoaded()));
}

void ComicController::onComicJsonLoaded()
{
    QIODevice * content = static_cast<QIODevice*>(QObject::sender());

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(content->readAll(), &err);
    if(err.error)
    {
        // Non existant comic. Todo: Warn the user
        return;
    }

    content->deleteLater();

    QJsonObject obj = doc.object();
    if(obj.value(QString("num")).isUndefined())
    {
        // Non existant comic. Todo: Warn the user
        return;
    }
    int id = obj.value(QString("num")).toInt();
    unsigned int day = obj.value(QString("day")).toInt();
    unsigned int month = obj.value(QString("month")).toInt();
    unsigned int year = obj.value(QString("year")).toInt();
    QString link = obj.value(QString("link")).toString();
    QString img = obj.value(QString("img")).toString();
    QString news = obj.value(QString("news")).toString();
    QString alt = obj.value(QString("alt")).toString();
    QString title = obj.value(QString("title")).toString();
    QString safe_title = obj.value(QString("safe_title")).toString();

    model.storeComic(id, day, month, year, link, img, news, alt, title, safe_title);

    QNetworkReply* reply = nam.get(QNetworkRequest(QUrl(img)));
    QObject::connect(reply, SIGNAL(finished()), &mapper, SLOT(map()));

    mapper.setMapping(reply, id);
    connect(&mapper, SIGNAL(mapped(int)), this, SLOT(onComicImageLoaded(int)));
}
void ComicController::onComicImageLoaded(int id)
{
    qDebug() << "Comic" << id << "downloaded";
    QIODevice * content = static_cast<QIODevice*>(mapper.mapping(id));

    model.storeImage(id, content->readAll());
    currentComic = model.getComic(id);
    //currentComic = QImage::fromData(content->readAll());
    //Q_ASSERT(!currentComic->image.isNull());
    content->deleteLater();
    emit currentComicChanged();
}

Comic* ComicController::getCurrentComic()
{
    return currentComic;
}

void ComicController::previousComic()
{
    loadComic(currentComic->getId() - 1);
}

void ComicController::nextComic()
{
    loadComic(currentComic->getId() + 1);
}

QPixmap ComicController::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    int width = currentComic->getImage().width();
    int height = currentComic->getImage().height();
    if(size)
        *size = currentComic->getImage().size();
    QPixmap pixmap(requestedSize.width() > 0? requestedSize.width() : width,
                   requestedSize.height() > 0? requestedSize.height() : height);
    pixmap = QPixmap::fromImage(currentComic->getImage());
    return pixmap;
}
