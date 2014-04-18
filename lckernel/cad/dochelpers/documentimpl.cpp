
#include <QHashIterator>
#include <QVector>
#include "qdebug.h"

#include "documentimpl.h"
#include "cad/geometry/geocoordinate.h"



using namespace lc;

DocumentImpl::DocumentImpl() : AbstractDocument() {
}

DocumentImpl::~DocumentImpl() {
    qDebug() << "DocumentImpl removed";
}

shared_ptr<LayerManager> DocumentImpl::layerManager() const {
    return _layerManager;
}

void DocumentImpl::setLayerManager(shared_ptr<LayerManager> layerManager) {
    _layerManager = layerManager;
}

void DocumentImpl::execute(shared_ptr<operation::Operation> operation) {
    std::lock_guard<std::mutex> lck (_documentMutex);
    begin(operation);
    this->operationProcess(operation);
    commit(operation);
}

void DocumentImpl::begin(shared_ptr<operation::Operation> operation) {
    this->operationStart(operation);
    BeginProcessEvent event;
    emit beginProcessEvent(event);
}

void DocumentImpl::commit(shared_ptr<operation::Operation> operation) {
    CommitProcessEvent event(operation);
    emit commitProcessEvent(event);
    this->operationFinnish(operation);
}

void DocumentImpl::addEntity(const QString& layerName, shared_ptr<const CADEntity> cadEntity) {
    AddEntityEvent event(layerName, cadEntity);
    emit addEntityEvent(event);
}

void DocumentImpl::replaceEntity(shared_ptr<const CADEntity> oldEntity, shared_ptr<const CADEntity> newEntity) {
    ReplaceEntityEvent event(oldEntity, newEntity);
    emit replaceEntityEvent(event);
}
void DocumentImpl::removeEntity(ID_DATATYPE id) {
    RemoveEntityEvent event(id);
    emit removeEntityEvent(event);
}
void DocumentImpl::absoleteEntity(shared_ptr<const CADEntity> entity) {
    AbsoluteEntityEvent event(entity);
    emit absoleteEntityEvent(event);
}

shared_ptr<const CADEntity> DocumentImpl::findEntityByID(ID_DATATYPE id) const {
    QHash <QString, shared_ptr<DocumentLayer> > allLayers = layerManager()->allLayers();
    QHashIterator<QString, shared_ptr<DocumentLayer> > li(allLayers);

    while (li.hasNext()) {
        li.next();
        shared_ptr<DocumentLayer> documentLayer = li.value();

        try {
            shared_ptr<const CADEntity> cip = documentLayer->findByID(id);
        } catch (QString error) {
            //
        }
    }

    return shared_ptr<const CADEntity>();
}

QString DocumentImpl::findEntityLayerByID(ID_DATATYPE id) const {
    QHash <QString, shared_ptr<DocumentLayer> > allLayers = layerManager()->allLayers();
    QHashIterator<QString, shared_ptr<DocumentLayer> > li(allLayers);

    while (li.hasNext()) {
        li.next();
        shared_ptr<DocumentLayer> documentLayer = li.value();

        try {
            shared_ptr<const CADEntity> cip = documentLayer->findByID(id);
            return documentLayer->layer()->name();
        } catch (QString error) {
            //
        }
    }

    throw "Entity not found";
    return "";
}


