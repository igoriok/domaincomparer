#ifndef WEBRESPONSE_H
#define WEBRESPONSE_H

#include <QUrl>

class WebResponse
{
public:
    WebResponse();
    WebResponse(const QString & type, int code, const QByteArray & data, qlonglong length);

    const QString & type() const { return m_type; }
    int code() const { return m_code; }
    const QByteArray & data() const { return m_data; }
    qlonglong length() const { return m_length; }

    bool isNull() const { return m_null; }
    void clear();

private:
    QString m_type;
    int m_code;
    QByteArray m_data;
    qlonglong m_length;

    bool m_null;
};

#endif // WEBRESPONSE_H
