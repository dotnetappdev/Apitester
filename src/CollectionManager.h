#ifndef COLLECTIONMANAGER_H
#define COLLECTIONMANAGER_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>

class CollectionManager : public QWidget
{
    Q_OBJECT
    
public:
    explicit CollectionManager(QWidget *parent = nullptr);
    
signals:
    void requestSelected(const QString &name, const QString &method, const QString &url,
                        const QString &headers, const QString &body, const QString &parameters);
    
private slots:
    void createNewCollection();
    void createNewRequest();
    void deleteSelected();
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
    
private:
    void setupUI();
    void loadCollections();
    void addCollectionToTree(int id, const QString &name, const QString &description);
    void addRequestToTree(QTreeWidgetItem *parent, int id, const QString &name, 
                         const QString &method, const QString &url);
    
    QTreeWidget *m_collectionsTree;
    QPushButton *m_newCollectionButton;
    QPushButton *m_newRequestButton;
    QPushButton *m_deleteButton;
    QLineEdit *m_searchEdit;
};

#endif // COLLECTIONMANAGER_H