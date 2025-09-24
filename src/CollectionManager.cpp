#include "CollectionManager.h"
#include "DatabaseManager.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTreeWidgetItem>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

CollectionManager::CollectionManager(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadCollections();
}

void CollectionManager::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title and search
    QLabel *titleLabel = new QLabel("Collections");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    mainLayout->addWidget(titleLabel);
    
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Search collections...");
    mainLayout->addWidget(m_searchEdit);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_newCollectionButton = new QPushButton("New Collection");
    connect(m_newCollectionButton, &QPushButton::clicked, this, &CollectionManager::createNewCollection);
    buttonLayout->addWidget(m_newCollectionButton);
    
    m_newRequestButton = new QPushButton("New Request");
    connect(m_newRequestButton, &QPushButton::clicked, this, &CollectionManager::createNewRequest);
    buttonLayout->addWidget(m_newRequestButton);
    
    m_deleteButton = new QPushButton("Delete");
    connect(m_deleteButton, &QPushButton::clicked, this, &CollectionManager::deleteSelected);
    buttonLayout->addWidget(m_deleteButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Collections tree
    m_collectionsTree = new QTreeWidget();
    m_collectionsTree->setHeaderLabel("Name");
    connect(m_collectionsTree, &QTreeWidget::itemDoubleClicked, 
            this, &CollectionManager::onItemDoubleClicked);
    mainLayout->addWidget(m_collectionsTree);
}

void CollectionManager::loadCollections()
{
    m_collectionsTree->clear();
    
    QSqlQuery query(DatabaseManager::instance().database());
    if (!query.exec("SELECT id, name, description FROM collections ORDER BY name")) {
        return;
    }
    
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString description = query.value(2).toString();
        
        addCollectionToTree(id, name, description);
    }
}

void CollectionManager::addCollectionToTree(int id, const QString &name, const QString &description)
{
    QTreeWidgetItem *collectionItem = new QTreeWidgetItem(m_collectionsTree);
    collectionItem->setText(0, name);
    collectionItem->setData(0, Qt::UserRole, id);
    collectionItem->setData(0, Qt::UserRole + 1, "collection");
    collectionItem->setToolTip(0, description);
    
    // Load requests for this collection
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT id, name, method, url FROM requests WHERE collection_id = ? ORDER BY name");
    query.addBindValue(id);
    
    if (query.exec()) {
        while (query.next()) {
            int requestId = query.value(0).toInt();
            QString requestName = query.value(1).toString();
            QString method = query.value(2).toString();
            QString url = query.value(3).toString();
            
            addRequestToTree(collectionItem, requestId, requestName, method, url);
        }
    }
    
    collectionItem->setExpanded(true);
}

void CollectionManager::addRequestToTree(QTreeWidgetItem *parent, int id, const QString &name,
                                       const QString &method, const QString &url)
{
    QTreeWidgetItem *requestItem = new QTreeWidgetItem(parent);
    requestItem->setText(0, QString("%1 %2").arg(method, name));
    requestItem->setData(0, Qt::UserRole, id);
    requestItem->setData(0, Qt::UserRole + 1, "request");
    requestItem->setToolTip(0, url);
}

void CollectionManager::createNewCollection()
{
    bool ok;
    QString name = QInputDialog::getText(this, "New Collection", 
                                        "Collection name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) {
        return;
    }
    
    QString description = QInputDialog::getText(this, "New Collection", 
                                              "Collection description (optional):", 
                                              QLineEdit::Normal, "", &ok);
    
    if (DatabaseManager::instance().createCollection(name, description)) {
        loadCollections(); // Reload to show the new collection
    } else {
        QMessageBox::warning(this, "Error", "Failed to create collection.");
    }
}

void CollectionManager::createNewRequest()
{
    QTreeWidgetItem *currentItem = m_collectionsTree->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, "Info", "Please select a collection first.");
        return;
    }
    
    // Find the collection item (could be a request item selected)
    QTreeWidgetItem *collectionItem = currentItem;
    if (currentItem->data(0, Qt::UserRole + 1).toString() == "request") {
        collectionItem = currentItem->parent();
    }
    
    if (!collectionItem || collectionItem->data(0, Qt::UserRole + 1).toString() != "collection") {
        QMessageBox::information(this, "Info", "Please select a collection first.");
        return;
    }
    
    bool ok;
    QString name = QInputDialog::getText(this, "New Request", 
                                        "Request name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) {
        return;
    }
    
    int collectionId = collectionItem->data(0, Qt::UserRole).toInt();
    
    // Create a basic GET request
    if (DatabaseManager::instance().saveRequest(collectionId, name, "GET", 
                                              "https://api.example.com", "", "", "")) {
        loadCollections(); // Reload to show the new request
    } else {
        QMessageBox::warning(this, "Error", "Failed to create request.");
    }
}

void CollectionManager::deleteSelected()
{
    QTreeWidgetItem *currentItem = m_collectionsTree->currentItem();
    if (!currentItem) {
        return;
    }
    
    QString itemType = currentItem->data(0, Qt::UserRole + 1).toString();
    int id = currentItem->data(0, Qt::UserRole).toInt();
    
    int result = QMessageBox::question(this, "Confirm Delete", 
                                      QString("Are you sure you want to delete this %1?").arg(itemType),
                                      QMessageBox::Yes | QMessageBox::No);
    
    if (result != QMessageBox::Yes) {
        return;
    }
    
    bool success = false;
    if (itemType == "collection") {
        success = DatabaseManager::instance().deleteCollection(id);
    } else if (itemType == "request") {
        success = DatabaseManager::instance().deleteRequest(id);
    }
    
    if (success) {
        loadCollections(); // Reload to reflect changes
    } else {
        QMessageBox::warning(this, "Error", QString("Failed to delete %1.").arg(itemType));
    }
}

void CollectionManager::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    
    if (!item || item->data(0, Qt::UserRole + 1).toString() != "request") {
        return;
    }
    
    int requestId = item->data(0, Qt::UserRole).toInt();
    
    // Load request details from database
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT name, method, url, headers, body, parameters FROM requests WHERE id = ?");
    query.addBindValue(requestId);
    
    if (query.exec() && query.next()) {
        QString name = query.value(0).toString();
        QString method = query.value(1).toString();
        QString url = query.value(2).toString();
        QString headers = query.value(3).toString();
        QString body = query.value(4).toString();
        QString parameters = query.value(5).toString();
        
        emit requestSelected(name, method, url, headers, body, parameters);
    }
}