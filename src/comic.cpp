#include "comic.h"

Comic::Comic(QObject *parent) :
    QObject(parent)
{
    id = -1;
    image.load(":/xkcd_logo.png");
    name = "Test";
    alt = "Alt";
}
bool Comic::isComplete()
{
    return (!image.isNull()) && (name.size() > 0); // TODO: add all the checks
}
