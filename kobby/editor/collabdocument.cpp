#include "collabdocument.h"

namespace Kobby
{

CollabDocument::CollabDocument( QObject *parent )
    : QInfinity::Document()
    , KTextEditor::Document( parent )
{
}

}

