#ifndef WEBRESPONSE_H
#define WEBRESPONSE_H

#include <QUrl>

class WebResponse
{
public:
    WebResponse();
    WebResponse(int code, const QString & type, const QUrl & location, const QByteArray & data, qlonglong length);

    int code() const { return m_code; }
    const QString & type() const { return m_type; }
    const QUrl & location() const { return m_location; }
    const QByteArray & data() const { return m_data; }
    qlonglong length() const { return m_length; }

    bool isNull() const { return m_null; }
    void clear();

private:
    int m_code;
    QString m_type;
    QUrl m_location;
    QByteArray m_data;
    qlonglong m_length;

    bool m_null;
};

#endif // WEBRESPONSE_H
