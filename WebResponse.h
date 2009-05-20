#ifndef WEBRESPONSE_H
#define WEBRESPONSE_H

#include <QObject>
#include <QUrl>

class WebResponse : public QObject
{
public:
    WebResponse();

    bool isValid() const { return m_valid; }

private:
    QUrl m_url;
    QString m_type;
    int m_code;
    QByteArray m_data;
    qlonglong m_length;

    bool m_valid;
};

#endif // WEBRESPONSE_H
