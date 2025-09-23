#ifndef REQUESTMODEL_H
#define REQUESTMODEL_H

#include <QtCore/QObject>

class RequestModel : public QObject
{
    Q_OBJECT
    
public:
    explicit RequestModel(QObject *parent = nullptr);
    
    // Getters
    int id() const { return m_id; }
    int collectionId() const { return m_collectionId; }
    QString name() const { return m_name; }
    QString method() const { return m_method; }
    QString url() const { return m_url; }
    QString headers() const { return m_headers; }
    QString body() const { return m_body; }
    QString parameters() const { return m_parameters; }
    
    // Setters
    void setId(int id) { m_id = id; }
    void setCollectionId(int collectionId) { m_collectionId = collectionId; }
    void setName(const QString &name) { m_name = name; }
    void setMethod(const QString &method) { m_method = method; }
    void setUrl(const QString &url) { m_url = url; }
    void setHeaders(const QString &headers) { m_headers = headers; }
    void setBody(const QString &body) { m_body = body; }
    void setParameters(const QString &parameters) { m_parameters = parameters; }
    
private:
    int m_id;
    int m_collectionId;
    QString m_name;
    QString m_method;
    QString m_url;
    QString m_headers;
    QString m_body;
    QString m_parameters;
};

#endif // REQUESTMODEL_H