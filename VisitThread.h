#ifndef VisitThread_h
#define VisitThread_h

#include <QtCore>
#include "Path.h"
#include "Location.h"

struct Node;
struct Match
{
    Match(uint nodeTypes)
        : nodeTypes(nodeTypes)
    {}
    virtual ~Match() {}

    // path means e.g. namespace::class:: (including trailing double colons)
    virtual bool match(const QByteArray &path, const Node *node) = 0;

    const uint nodeTypes;
};

class VisitThread : public QThread
{
    Q_OBJECT
public:
    VisitThread();
    int lookup(Match *match);
    void printTree();
    QSet<Path> files() const;
protected:
    virtual void run();
public slots:
    void invalidate(const Path &path);
    void onFileParsed(const Path &path, void *unit);
private:
    Node *createOrGet(CXCursor cursor);
    static CXChildVisitResult buildTree(CXCursor cursor, CXCursor, CXClientData data);
    struct PendingReference {
        CXCursor cursor;
        CXCursor reference;
        Location location;
    };
    mutable QMutex mMutex;
    QSet<Path> mFiles;
    QHash<uint, PendingReference> mPendingReferences;
    Node *mRoot;
    QReadWriteLock mLock;
    QHash<unsigned, Node*> mNodes;
    int mBytes;
};

#endif
