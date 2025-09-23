#ifndef COLLECTIONMODEL_H
#define COLLECTIONMODEL_H

#include <QtCore/QObject>

class CollectionModel : public QObject
{
    Q_OBJECT
    
public:
    explicit CollectionModel(QObject *parent = nullptr);
    
    // Getters
    int id() const { return m_id; }
    QString name() const { return m_name; }
    QString description() const { return m_description; }
    
    // Setters
    void setId(int id) { m_id = id; }
    void setName(const QString &name) { m_name = name; }
    void setDescription(const QString &description) { m_description = description; }
    
private:
    int m_id;
    QString m_name;
    QString m_description;
};

#endif // COLLECTIONMODEL_H