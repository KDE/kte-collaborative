#ifndef KOBBY_COLLABDOCUMENT_H
#define KOBBY_COLLABDOCUMENT_H

#include <libqinfinitymm/document.h>

#include <KTextEditor/Document>

namespace Kobby
{

class CollabDocument
    : public QInfinity::Document
    , public KTextEditor::Document
{

    public:
        CollabDocument( QObject *parent = 0 );

};

}

#endif

