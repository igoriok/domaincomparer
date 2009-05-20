#include "UrlInfo.h"

UrlInfo::UrlInfo()
{
    m_valid = false;
}

UrlInfo::UrlInfo(const UrlInfo & other)
{
    if (other.m_valid)
    {
        m_valid = true;
        m_url = other.m_url;
        m_parent = other.m_parent;
        m_code = other.m_code;
        m_type = other.m_type;
    }
    else
        m_valid = false;
}

UrlInfo & UrlInfo::operator =(const UrlInfo & other)
{
    if (other.m_valid)
    {
        m_valid = true;
        m_url = other.m_url;
        m_parent = other.m_parent;
        m_code = other.m_code;
        m_type = other.m_type;
    }
    else
        m_valid = false;
    return *this;
}

bool UrlInfo::operator ==(const UrlInfo & other)
{
    return (m_url == other.m_url);
}

UrlInfo UrlInfo::compare(const WebResponse & prev, const WebResponse & live)
{
    UrlInfo ui;

    // comparation code

    return ui;
}

uint qHash(const UrlInfo & ui)
{
    return qHash(ui.m_url.toString());
}

uint qHash(const QUrl & url)
{
    return qHash(url.toString());
}
